#ifndef _IMAGEIO_
#define _IMAGEIO_

#include "globals.h"
#include "image.h"

DLL int LoadImage(IMAGE* imgPTR, char* path);
DLL void SaveImage(IMAGE* imgPTR, char* path, bool openSavedIMG);
DLL void LoadRAWImage(IMAGE* imgPTR, char* paths[], uint16 channels, uint16 bitpsam, uint32 width, uint32 height);
#endif