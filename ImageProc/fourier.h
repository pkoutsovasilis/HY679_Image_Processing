#ifndef _FOURIER_
#define _FOURIER_

#include "globals.h"
#include "image.h"
#include <math.h>
#include <fftw3.h>

#define M_PI 3.14159265358979323846

DLL void FOURIERCalculate(IMAGE* imgPTR, uint8 type);

static uint32 getPosX(IMAGE* imgPTR, uint32 curPosX, uint32 offsetX);

static uint32 getPosY(IMAGE* imgPTR, uint32 curPosY, uint32 offsetY);

typedef struct
{
	double real;
	double imag;
} FOURIER;

#endif