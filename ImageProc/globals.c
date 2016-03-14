#include "globals.h"

const char* RGB_CHANNELS[] = { "R", "G", "B" };
const char* YCBCR_CHANNELS[] = { "Y", "Cb", "Cr" };

void SetErrorCallback(ErrorCallback errCB)
{
	errorCallB = errCB;
}