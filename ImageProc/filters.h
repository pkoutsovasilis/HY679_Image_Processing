#ifndef _FILTERS_
#define _FILTERS_

#include "globals.h"
#include "image.h"

//maximum filter 20x20
#define MAXBUFF 400

typedef struct _filt
{
	int *coefficients;
	uint8 lines, columns;
	uint8 centerX, centerY;
} FILTER;

typedef void(__stdcall * FiltersAdd)(FILTER* filtPTR, char* name);
typedef void(__stdcall * FiltersClear)();



DLL void FILTERSInitialise(FiltersAdd Add, FiltersClear Clear);
DLL void FILTERSLoad(char** paths, int pathN);
DLL void FILTERSApply();
DLL void FILTERSFree(FILTER* filtPTR);

static uint32 getPosX(int posX, uint32 Width);
static uint32 getPosY(int posY, uint32 Height);
static FiltersAdd filtAdd;
static FiltersClear filtClear;

#endif