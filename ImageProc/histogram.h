#ifndef _HIST_
#define _HIST_

#include <math.h>
#include "image.h"
#include "globals.h"

typedef void(__stdcall * HistogramAdd)(float average, float standard_dev, uint64 max_val, const char* channel);
typedef void(__stdcall * HistogramClear)();

typedef struct _hist
{
	HistogramAdd histogramAdd;
	HistogramClear histogramClear;
	IMAGE* imgPTR;
	IMAGE* histImgPTR;
	uint8* originalImage;
	uint32 *histogram[3]; //only freeable from c
} HISTOGRAM;

DLL HISTOGRAM* HISTInitialise(IMAGE* imgPTR, IMAGE* histimgPTR, HistogramAdd histAdd, HistogramClear histClear);
DLL void HISTCalculate(HISTOGRAM* histPTR);
DLL void HISTFree(HISTOGRAM* histPTR);
DLL void HISTEqualize(HISTOGRAM* histPTR, bool RefreshHIST);
DLL void HISTReset(HISTOGRAM* histPTR);

#endif