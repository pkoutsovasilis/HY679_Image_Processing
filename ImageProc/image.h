#ifndef _IMAGE_
#define _IMAGE_

#include "globals.h"

typedef uint8* (__stdcall * CreateImage)(uint32 Width, uint32 Height, uint32* stride);
typedef void (__stdcall * LockImage)();
typedef void (__stdcall * UnlockImage)(uint32 width, uint32 height);
typedef void (__stdcall * ChangeImageProperty)(char *propertyName, void *propertyValue);

typedef enum {
	TYPE_VISUAL,
	TYPE_FOURIER,
	TYPE_HISTOGRAM
} IMAGE_TYPE;

typedef struct IMAGE IMAGE;

struct IMAGE{
	uint32 width;
	uint32 height;
	uint32 stride;
	uint32 dimensions;
	ttag_t encoding[2];
	uint8 *imgBUFS[4];			// 3 WiteableBitmap Buffer, 1 Undo Buffer, 2 Restore Buffer, 0 Working Buffer
	IMAGE_TYPE type;			// type of image for appropriate actions
	CreateImage createImage;	// c# delegate cannot be freed from c!
	LockImage lockImage;		// c# delegate cannot be freed from c!
	UnlockImage unlockImage;	// c# delegate cannot be freed from c!
	ChangeImageProperty changeImageProperty; // c# delegate cannot be freed from c!
} ;


DLL IMAGE* InitialiseImage(IMAGE_TYPE type, CreateImage cre, LockImage lockIMG, UnlockImage unlockIMG, ChangeImageProperty chgProp);
DLL void IMAGERestore(IMAGE* imgPTR);
DLL void IMAGEUndo(IMAGE* imgPTR);
DLL void IMAGERedo(IMAGE* imgPTR);
DLL void IMAGEToYCBCR(IMAGE* imgPTR);
DLL void IMAGEToRGB(IMAGE* imgPTR);
DLL void IMAGEFree(IMAGE* imgPTR);
DLL uint32 IMAGEGetPixelValue(IMAGE* imgPTR, uint32 x, uint32 y);
DLL void negative(IMAGE* imgPTR);
DLL void grayscale(IMAGE* imgPTR);

void IMAGEPaint(IMAGE* imgPTR);
void IMAGESaveChanges(IMAGE* imgPTR);

void IMAGEPaint(IMAGE* imgPTR);
static void VisualRGB(IMAGE* imgPTR);
#endif