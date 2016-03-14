#include "image.h"

IMAGE* InitialiseImage(IMAGE_TYPE type, CreateImage cre, LockImage lockIMG, UnlockImage unlockIMG, ChangeImageProperty chgProp)
{
	uint8 i = 0;
	IMAGE* img = (IMAGE*)calloc(1,sizeof(IMAGE));
	img->width=0;
	img->height=0;
	img->type = type; 
	img->createImage = cre; 
	img->lockImage = lockIMG;
	img->unlockImage = unlockIMG;
	img->changeImageProperty = chgProp;
	for(i=0;i<4;i++)
		img->imgBUFS[i] = NULL;
	return img;
}

void IMAGEPaint(IMAGE* imgPTR)
{	
	imgPTR->lockImage();
	if (imgPTR->encoding[0] == PHOTOMETRIC_RGB)
	{
		memcpy(imgPTR->imgBUFS[3],imgPTR->imgBUFS[0],imgPTR->height*imgPTR->stride);
		imgPTR->changeImageProperty("ImageEncoding","RGB");
	}
	else
	{
		VisualRGB(imgPTR);
		imgPTR->changeImageProperty("ImageEncoding", "YCbCr");
	}
	imgPTR->unlockImage(imgPTR->width,imgPTR->height);	
	imgPTR->changeImageProperty("ImageWidth",&imgPTR->width);
	imgPTR->changeImageProperty("ImageHeight",&imgPTR->height);
}

void IMAGERestore(IMAGE* imgPTR)
{
	bool tmp = false;
	memcpy(imgPTR->imgBUFS[0],imgPTR->imgBUFS[2],imgPTR->height*imgPTR->stride);
	imgPTR->encoding[0] = PHOTOMETRIC_RGB;
	imgPTR->changeImageProperty("ImageUndo", &tmp);
	IMAGEPaint(imgPTR);
}

void IMAGESaveChanges(IMAGE* imgPTR)
{
	bool tmp = true;
	imgPTR->encoding[1] = imgPTR->encoding[0];
	memcpy(imgPTR->imgBUFS[1],imgPTR->imgBUFS[0],imgPTR->height*imgPTR->stride);
	imgPTR->changeImageProperty("ImageUndo", &tmp);
}

void IMAGEUndo(IMAGE* imgPTR)
{
	uint32 i=0,j=0;
	bool tmp = false;
	uint8* tempBUF = (uint8*)calloc(imgPTR->height*imgPTR->stride, sizeof(uint8));
	if (!tempBUF)
	{
		errorCallB("Couldn't allocate memory");
		return;
	}
	memcpy(tempBUF, imgPTR->imgBUFS[0], imgPTR->height*imgPTR->stride);
	memcpy(imgPTR->imgBUFS[0],imgPTR->imgBUFS[1],imgPTR->height*imgPTR->stride);
	memcpy(imgPTR->imgBUFS[1], tempBUF, imgPTR->height*imgPTR->stride);
	free(tempBUF);
	tempBUF = NULL;
	imgPTR->encoding[0] = imgPTR->encoding[1];
	IMAGEPaint(imgPTR);
	imgPTR->changeImageProperty("ImageUndo", &tmp);
}

void IMAGERedo(IMAGE* imgPTR)
{
	uint32 i = 0, j = 0;
	bool tmp = true;
	uint8* tempBUF = (uint8*)calloc(imgPTR->height*imgPTR->stride, sizeof(uint8));
	if (!tempBUF)
	{
		errorCallB("Couldn't allocate memory");
		return;
	}
	memcpy(tempBUF, imgPTR->imgBUFS[0], imgPTR->height*imgPTR->stride);
	memcpy(imgPTR->imgBUFS[0], imgPTR->imgBUFS[1], imgPTR->height*imgPTR->stride);
	memcpy(imgPTR->imgBUFS[1], tempBUF, imgPTR->height*imgPTR->stride);
	imgPTR->encoding[1] = imgPTR->encoding[0];
	IMAGEPaint(imgPTR);
	imgPTR->changeImageProperty("ImageUndo", &tmp);
}

void IMAGEToYCBCR(IMAGE* imgPTR)
{
	uint32 i=0,j=0;
	int Y = 0, Cb = 0, Cr = 0;
	IMAGESaveChanges(imgPTR);
	for (i = 0 ; i<imgPTR->height;i++)
	{
		for (j=0;j<imgPTR->width;j++)
		{
			Y = (uint8) ((float)(+0.299f*imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 + 0]+0.587f*imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 +1]+0.114f*imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 +2]))+0;
			Cb = (uint8)((float)(-0.169f*imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 + 0]-0.331f*imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 +1]+0.500f*imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 +2]))+128;
			Cr = (uint8)((float)(+0.500f*imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 + 0]-0.419f*imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 +1]-0.081f*imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 +2]))+128;

			imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 + 0] = (uint8)Y;
			imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 + 1] = (uint8)Cb;
			imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 + 2] = (uint8)Cr;
		}
	}
	imgPTR->encoding[0] = PHOTOMETRIC_YCBCR;
	IMAGEPaint(imgPTR);
}

void IMAGEToRGB(IMAGE* imgPTR)
{
	IMAGESaveChanges(imgPTR);
	memcpy(imgPTR->imgBUFS[0],imgPTR->imgBUFS[3],imgPTR->height*imgPTR->stride);
	imgPTR->encoding[0] = PHOTOMETRIC_RGB;
	IMAGEPaint(imgPTR);
}

void VisualRGB(IMAGE* imgPTR)
{
	uint32 i=0,j=0;
	for (i = 0 ; i<imgPTR->height;i++)
		for (j=0;j<imgPTR->width;j++)
		{
			imgPTR->imgBUFS[3][i*imgPTR->stride + j*3 +0]  = (uint8) ( 1.000f*(imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 + 0] - 0 )+0.000f*(imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 +1] - 128)+1.400f*(imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 +2] - 128));
			imgPTR->imgBUFS[3][i*imgPTR->stride + j*3 +1]  = (uint8) ( 1.000f*(imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 + 0] - 0 )-0.343f*(imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 +1] - 128)-0.711f*(imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 +2] - 128));
			imgPTR->imgBUFS[3][i*imgPTR->stride + j*3 +2]  = (uint8) ( 1.000f*(imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 + 0] - 0 )+1.765f*(imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 +1] - 128)+0.000f*(imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 +2] - 128));
		}
}

void IMAGEFree(IMAGE* imgPTR)
{
	uint8 i = 0;
	for (i=0;i<3;i++)
	{
		if(imgPTR->imgBUFS[i])
		{
			free(imgPTR->imgBUFS[i]);
			imgPTR->imgBUFS[i] = NULL;
		}
	}

	if (imgPTR)
	{
		free(imgPTR);
	}
}

uint32 IMAGEGetPixelValue(IMAGE* imgPTR, uint32 x, uint32 y)
{
	uint32 val = 0;
	uint8* imgBUF = imgPTR->imgBUFS[0];
	val = imgBUF[y*imgPTR->stride + x*3 + 0];
	val = val << 8;
	val |= imgBUF[y*imgPTR->stride + x * 3 + 1];
	val = val << 8;
	val |= imgBUF[y*imgPTR->stride + x * 3 + 2];
	return val;
}

void negative(IMAGE* imgPTR)
{
	uint32 i=0,j=0;
	for (i = 0 ; i<imgPTR->height;i++)
	{
		for (j=0;j<imgPTR->width;j++)
		{
			imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 +0]  = 255 - imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 +0];
			imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 +1]  = 255 - imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 +1];
			imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 +2]  = 255 - imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 +2];
		}
	}
	IMAGESaveChanges(imgPTR);
	IMAGEPaint(imgPTR);
}

void grayscale(IMAGE* imgPTR)
{
	uint32 i=0,j=0;
	uint8 k=0;
	for (i = 0 ; i<imgPTR->height;i++)
	{
		for (j=0;j<imgPTR->width;j++)
		{
			k = (uint8)(((uint32)(imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 +0] + imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 +1]+imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 +2]))/3);
			imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 +0]  = k;
			imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 +1]  = k;
			imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 +2]  = k;
		}
	}
	IMAGESaveChanges(imgPTR);
	IMAGEPaint(imgPTR);
}