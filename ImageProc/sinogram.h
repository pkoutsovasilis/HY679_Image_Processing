#ifndef _SINO_
#define _SINO_

#define M_PI 3.14159265358979323846

#include "image.h"
#include <fftw3.h>
#include <math.h>

typedef enum {
	TYPE_HANNING,
	TYPE_HAMMING
} WINDOW_TYPE;

static void SINOGRAMNormalize(IMAGE* imgPTR, float* tempBUF);
DLL void SINOGRAMFilteredBackprojection(IMAGE* imgPTR, WINDOW_TYPE type);
DLL void SINOGRAMBackprojection(IMAGE* imgPTR);
DLL void SINOGRAMFilt(IMAGE* imgPTR, double const* filter);
static int getpos(int number, IMAGE* imgPTR);
#endif