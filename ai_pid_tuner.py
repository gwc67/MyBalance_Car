#!/usr/bin/env python3
"""
平衡小车 Angle PID 远程调参工具
通过蓝牙串口读取小车状态 → DeepSeek API 分析 → 自动下发新PID参数
"""

import serial
import json
import requests
import time
import sys
import re
import argparse

# ============================================================
# 配置
# ============================================================
DEEPSEEK_API_KEY = "sk-your-api-key-here"  # 替换为你的 DeepSeek API Key
DEEPSEEK_API_URL = "sk-98810a25befc46e2b7bb6556ebaa0fd7"
DEEPSEEK_MODEL = "deepseek-chat"

SERIAL_PORT = "COM6"       # 蓝牙串口号 (Windows: COMx, Linux: /dev/rfcomm0)
SERIAL_BAUD = 9600
SERIAL_TIMEOUT = 2.0       # 读取超时(秒)

# 调参间隔(秒)，给小车运动稳定时间
TUNE_INTERVAL = 8.0

# ============================================================
# PID 系统描述 (来自 TIM1_UP_IRQHandler)
# ============================================================
SYSTEM_PROMPT = """你是一位拥有10年经验的PID算法工程师，精通平衡小车（两轮自平衡车）的PID调参。

## 平衡小车 Angle 环 PID 系统描述

**控制架构:**
- Angle环(角度环): PD控制器，10ms控制周期
- 角度来源: MPU6050互补滤波 (0.01*加速度计角度 + 0.99*陀螺仪积分角度)
- 角度范围: -70° ~ +70° (超出范围自动停车)

**AnglePID 计算公式 (PD控制器，注意没有使用Ki):**
```
AnglePID.Out = Kp * (Target - Actual) - Kd * GyroY_Actual
```

- Actual: 当前互补滤波后的角度 (单位: 度)
- Target: 目标角度，由速度环输出决定 (静止时接近0)
- GyroY_Actual: Y轴角速度 (单位: 度/秒，陀螺仪原始值/32768*2000)
- Out: PID输出，限幅到 ±60，转为PWM驱动电机

**重要说明:**
- Ki 参数虽然存在但没有参与计算 (纯PD控制)
- Kd 作用于角速度而非角度误差微分 (相当于速度前馈)
- 输出直接驱动电机: AvePwm = -AnglePID.Out
- 平衡的目标是让 Angle 稳定在 Target 附近 (振荡尽量小)

**数据字段含义:**
- Angle: 当前互补滤波角度(°) — 正值前倾，负值后仰
- GyroY: Y轴角速度(°/s) — 正值向前转，负值向后转
- Target: 目标角度(°) — 静止时为0或接近0
- Out: PID输出 (-60 ~ +60)
- AvePwm: 平均PWM占空比
- Kp/Ki/Kd: 当前的PID参数
- RunFlag: 小车运行状态 (1=运行, 0=停车)

**调参目标:**
1. 小车能够稳定站立，无明显振荡
2. 受到轻微推动后能快速恢复平衡
3. 角度误差尽量小 (|Angle - Target| 尽量接近0)
4. 电机输出不过度饱和 (避免频繁达到±60限幅)

**调参策略:**
- Kp 过小: 小车太软，无法维持站立，容易倒
- Kp 过大: 小车高频振荡，电机抖动
- Kd 过小: 阻尼不足，小车持续晃动
- Kd 过大: 阻尼过大，小车反应迟钝，修正缓慢
- 典型起点: Kp=0.3, Kd=0.1 (Ki保持0)
- Kp 调节步进: 0.03~0.05
- Kd 调节步进: 0.02~0.05

## 输出格式

请在分析后给出新的PID参数，严格使用以下格式:
```
ANALYSIS: <对当前数据的简要分析，用中文，50字以内>
PID: Kp Ki Kd
```

例如:
```
ANALYSIS: 角度振荡明显，Kd偏小导致阻尼不足，需增大Kd
PID: 0.30 0.00 0.15
```
"""


def parse_args():
    parser = argparse.ArgumentParser(description="平衡小车 Angle PID AI调参工具")
    parser.add_argument("--port", default=SERIAL_PORT, help=f"串口号 (默认: {SERIAL_PORT})")
    parser.add_argument("--baud", type=int, default=SERIAL_BAUD, help=f"波特率 (默认: {SERIAL_BAUD})")
    parser.add_argument("--key", default=DEEPSEEK_API_KEY, help="DeepSeek API Key")
    parser.add_argument("--interval", type=float, default=TUNE_INTERVAL, help="调参间隔秒数 (默认: 8)")
    parser.add_argument("--dry-run", action="store_true", help="仅读取和分析，不实际修改PID")
    parser.add_argument("--auto", action="store_true", help="全自动模式，不需要确认直接下发")
    return parser.parse_args()


def connect_serial(port, baud):
    """连接蓝牙串口"""
    print(f"[串口] 连接 {port} @ {baud} baud ...")
    try:
        ser = serial.Serial(port, baud, timeout=SERIAL_TIMEOUT)
        ser.flushInput()
        ser.flushOutput()
        time.sleep(0.5)
        print(f"[串口] 已连接")
        return ser
    except serial.SerialException as e:
        print(f"[错误] 无法打开串口 {port}: {e}")
        sys.exit(1)


def request_report(ser):
    """向小车发送 {Report} 命令获取当前状态"""
    ser.flushInput()
    cmd = b"{Report}\n"
    ser.write(cmd)
    ser.flush()
    print(f"[发送] {{Report}}")

    start = time.time()
    while time.time() - start < 3.0:
        line = ser.readline()
        if not line:
            continue
        try:
            text = line.decode("utf-8", errors="ignore").strip()
        except Exception:
            continue
        # Report 格式: R,Angle,GyroY,Target,Out,AvePwm,Kp,Ki,Kd,RunFlag
        if text.startswith("R,"):
            return text
        # 其他输出也打印出来
        if text:
            print(f"[串口] {text}")
    return None


def parse_report(line):
    """解析 Report 数据行
    返回 dict 或 None
    格式: R,Angle,GyroY,Target,Out,AvePwm,Kp,Ki,Kd,RunFlag
    """
    try:
        parts = line.strip().split(",")
        if len(parts) != 10 or parts[0] != "R":
            return None
        return {
            "Angle": float(parts[1]),
            "GyroY": float(parts[2]),
            "Target": float(parts[3]),
            "Out": float(parts[4]),
            "AvePwm": float(parts[5]),
            "Kp": float(parts[6]),
            "Ki": float(parts[7]),
            "Kd": float(parts[8]),
            "RunFlag": int(parts[9]),
        }
    except (ValueError, IndexError):
        return None


def call_deepseek(api_key, data, history=None):
    """调用 DeepSeek API 获取PID调参建议"""
    if history is None:
        history = []

    # 构建当前状态描述
    status_text = f"""当前状态数据:
- Angle: {data['Angle']:.2f}° (互补滤波角度)
- GyroY: {data['GyroY']:.2f}°/s (Y轴角速度)
- Target: {data['Target']:.2f}° (目标角度)
- Out: {data['Out']:.1f} (PID输出, 范围±60)
- AvePwm: {data['AvePwm']:.1f} (平均PWM)
- 当前Kp: {data['Kp']:.3f}, Ki: {data['Ki']:.3f}, Kd: {data['Kd']:.3f}
- 运行状态: {"运行中" if data['RunFlag'] == 1 else "已停车"}

请分析上述数据，给出AnglePID的调参建议 (Kp Ki Kd)。"""

    messages = [
        {"role": "system", "content": SYSTEM_PROMPT},
    ]
    # 保留最近3轮历史
    for h in history[-3:]:
        messages.append({"role": "user", "content": h["user"]})
        messages.append({"role": "assistant", "content": h["assistant"]})
    messages.append({"role": "user", "content": status_text})

    headers = {
        "Authorization": f"Bearer {api_key}",
        "Content-Type": "application/json",
    }
    payload = {
        "model": DEEPSEEK_MODEL,
        "messages": messages,
        "temperature": 0.3,
        "max_tokens": 512,
    }

    print(f"[API] 请求 DeepSeek ...")
    try:
        resp = requests.post(DEEPSEEK_API_URL, headers=headers, json=payload, timeout=30)
        resp.raise_for_status()
        result = resp.json()
        content = result["choices"][0]["message"]["content"]
        return content
    except requests.exceptions.RequestException as e:
        print(f"[错误] API请求失败: {e}")
        if hasattr(e, 'response') and e.response is not None:
            print(f"  {e.response.text[:500]}")
        return None


def parse_pid_response(text):
    """从 DeepSeek 回复中提取ANALYSIS文本和PID数值
    期望格式: ANALYSIS: xxx\nPID: Kp Ki Kd
    返回 (analysis, (kp, ki, kd)) 或 (text, None)
    """
    analysis = ""
    kp = ki = kd = None

    for line in text.split("\n"):
        line_stripped = line.strip()
        if line_stripped.upper().startswith("ANALYSIS"):
            analysis = line_stripped.split(":", 1)[-1].strip()
        elif line_stripped.upper().startswith("PID"):
            parts = line_stripped.split(":", 1)[-1].strip().split()
            if len(parts) >= 3:
                try:
                    kp = float(parts[0])
                    ki = float(parts[1])
                    kd = float(parts[2])
                except ValueError:
                    pass
            # 也尝试匹配 {AnglePID kp ki kd} 格式
            m = re.search(r"\{AnglePID\s+([\d.+\-eE]+)\s+([\d.+\-eE]+)\s+([\d.+\-eE]+)\}", text)
            if m:
                try:
                    kp = float(m.group(1))
                    ki = float(m.group(2))
                    kd = float(m.group(3))
                except ValueError:
                    pass

    if kp is None:
        # 尝试从任意行解析3个float
        for line in text.split("\n"):
            nums = re.findall(r"([\d.+\-eE]+)", line.strip())
            if len(nums) >= 3:
                try:
                    kp = float(nums[0])
                    ki = float(nums[1])
                    kd = float(nums[2])
                    if analysis == "":
                        analysis = line.strip()
                    break
                except ValueError:
                    continue

    return (analysis, (kp, ki, kd) if kp is not None else None)


def send_pid(ser, kp, ki, kd):
    """向小车发送 {AnglePID kp ki kd} 命令"""
    cmd = f"{{AnglePID {kp:.3f} {ki:.3f} {kd:.3f}}}\n"
    ser.write(cmd.encode("utf-8"))
    ser.flush()
    print(f"[发送] {cmd.strip()}")

    # 等待 OK 回显
    start = time.time()
    while time.time() - start < 2.0:
        line = ser.readline()
        if not line:
            continue
        text = line.decode("utf-8", errors="ignore").strip()
        if text:
            print(f"[串口] {text}")
        if "OK" in text:
            print(f"[结果] PID参数已更新 ✓")
            return True
    return False


def safety_check(data, new_kp, new_ki, new_kd):
    """安全检查: 防止极端危险的PID值"""
    warnings = []
    if new_kp < 0 or new_kd < 0:
        warnings.append("Kp或Kd为负值，小车将失控!")
    if new_kp > 2.0:
        warnings.append(f"Kp={new_kp} 过大，可能导致剧烈振荡")
    if new_kd > 1.0:
        warnings.append(f"Kd={new_kd} 过大，小车反应会非常迟钝")
    if new_ki > 0.5:
        warnings.append(f"Ki={new_ki} 过大，积分饱和风险 (注意角度环不使用Ki)")
    return warnings


def main():
    args = parse_args()

    # 检查 API Key
    if args.key == "sk-your-api-key-here":
        print("[警告] 请先设置 DeepSeek API Key!")
        print("  方式1: 命令行参数 --key sk-xxx")
        print("  方式2: 修改脚本中的 DEEPSEEK_API_KEY")
        sys.exit(1)

    ser = connect_serial(args.port, args.baud)

    print()
    print("=" * 55)
    print("  平衡小车 Angle PID AI 调参工具")
    print(f"  串口: {args.port} | 调参间隔: {args.interval}s")
    print(f"  {'仅分析模式 (不修改PID)' if args.dry_run else '正常模式'}")
    print(f"  {'全自动模式' if args.auto else '确认模式'}")
    print("=" * 55)
    print()

    history = []
    iteration = 0

    try:
        while True:
            iteration += 1
            print(f"\n{'─' * 55}")
            print(f"第 {iteration} 轮调参")
            print(f"{'─' * 55}")

            # 1. 请求状态报告
            line = request_report(ser)
            if line is None:
                print("[警告] 未收到 Report 响应，重试...")
                time.sleep(2)
                continue

            data = parse_report(line)
            if data is None:
                print(f"[警告] 解析失败: {line}")
                time.sleep(2)
                continue

            print(f"[数据] Angle={data['Angle']:.2f}° GyroY={data['GyroY']:.2f}°/s "
                  f"Target={data['Target']:.2f}° Out={data['Out']:.1f} "
                  f"Kp={data['Kp']:.3f} Kd={data['Kd']:.3f}")

            if data["RunFlag"] == 0:
                print("[提示] 小车未运行，建议先启动小车再调参")

            # 2. 调用 DeepSeek
            response = call_deepseek(args.key, data, history)
            if response is None:
                print("[错误] API调用失败，跳过本轮")
                time.sleep(args.interval)
                continue

            print(f"\n[AI分析]")
            print(response)
            print()

            analysis, pid_vals = parse_pid_response(response)
            if pid_vals is None:
                print("[警告] 无法从AI回复中解析PID值")
                print("  AI回复格式应为: PID: Kp Ki Kd")
                time.sleep(args.interval)
                continue

            new_kp, new_ki, new_kd = pid_vals

            # 安全检查
            warnings = safety_check(data, new_kp, new_ki, new_kd)
            if warnings:
                print("\n[安全检查] ⚠ 警告:")
                for w in warnings:
                    print(f"  ! {w}")
                if not args.auto:
                    ok = input("\n参数可能有风险，是否继续? (y/N): ").strip().lower()
                    if ok != "y":
                        print("[跳过] 本轮不修改")
                        time.sleep(args.interval)
                        continue

            print(f"\n[建议] Kp={new_kp:.3f} Ki={new_ki:.3f} Kd={new_kd:.3f}")
            print(f"[当前] Kp={data['Kp']:.3f} Ki={data['Ki']:.3f} Kd={data['Kd']:.3f}")

            if abs(new_kp - data['Kp']) < 0.001 and abs(new_kd - data['Kd']) < 0.001:
                print("[跳过] PID值未变化，无需更新")
            else:
                if args.dry_run:
                    print("[仅分析] 不发送PID命令")
                elif args.auto:
                    send_pid(ser, new_kp, new_ki, new_kd)
                else:
                    ok = input("\n确认下发? (y/N): ").strip().lower()
                    if ok == "y":
                        send_pid(ser, new_kp, new_ki, new_kd)
                    else:
                        print("[跳过] 用户取消")

            # 保存历史
            history.append({
                "user": f"数据: Angle={data['Angle']:.2f} GyroY={data['GyroY']:.2f} "
                        f"Kp={data['Kp']:.3f} Kd={data['Kd']:.3f}",
                "assistant": response,
            })

            print(f"\n等待 {args.interval:.0f} 秒后进入下一轮...")
            time.sleep(args.interval)

    except KeyboardInterrupt:
        print("\n\n[退出] 用户中断")
    finally:
        ser.close()
        print("[串口] 已关闭")


if __name__ == "__main__":
    main()
