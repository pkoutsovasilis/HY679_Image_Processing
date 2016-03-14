#ifndef _STATS_
#define STATS

#include <math.h>
#include "image.h"

typedef void(__stdcall * StatisticsAdd)(float average, float standard_dev, float correlation_hor_f, float correlation_hor_s, float correlation_ver_f, float correlation_ver_s, const char* channel);
typedef void(__stdcall * StatisticsClear)();

DLL void STATSCalculate(IMAGE* imgPTR, StatisticsAdd staAdd, StatisticsClear staClear);

#endif