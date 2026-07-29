#ifndef __FREQ_DETECTOR_H
#define __FREQ_DETECTOR_H

#include "main.h"

enum {
    DATA_STREAM_BAT_CUR_em = 0,
    DATA_STREAM_RADAR_POS_em,
    DATA_STREAM_RADAR_SPEED_em,
    Data_stream_Radar_qua_em,
};

typedef struct {
    uint32_t dummy;
} FreqDetector;

extern FreqDetector *freq_detector_pst[];

double FreqDetector_GetFreq_db(FreqDetector *det);

#endif
