#ifndef _GLOBALS_
#define _GLOBALS_

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <tiffio.h>
#include <memory.h>

#define DLL __declspec(dllexport)

typedef unsigned long long uint64;
typedef uint8 bool;
#define true 1
#define false 0

typedef void(__stdcall * ErrorCallback)(char* errormessage);

DLL void SetErrorCallback(ErrorCallback errCB);

ErrorCallback errorCallB;

extern const char* RGB_CHANNELS[];
extern const char* YCBCR_CHANNELS[];
#endif