#include "geometry.h"

void GEOMETRYFlipHorizontal(IMAGE* imgPTR)
{
	uint8* tempBUF = NULL;
	uint32 i = 0, j = 0;
	tempBUF = (uint8*)calloc(imgPTR->height*imgPTR->stride,sizeof(uint8));
	for (i=0;i<imgPTR->height;i++)
	{
		for(j=0;j<imgPTR->width;j++)
		{
			tempBUF[i*imgPTR->stride + j*3 + 0] = imgPTR->imgBUFS[0][i * imgPTR->stride + (imgPTR->width - 1 - j)*3 + 0];
			tempBUF[i*imgPTR->stride + j*3 + 1] = imgPTR->imgBUFS[0][i * imgPTR->stride + (imgPTR->width - 1 - j)*3 + 1];
			tempBUF[i*imgPTR->stride + j*3 + 2] = imgPTR->imgBUFS[0][i * imgPTR->stride + (imgPTR->width - 1 - j)*3 + 2];
		}
	}
	IMAGESaveChanges(imgPTR);
	memcpy(imgPTR->imgBUFS[0],tempBUF,imgPTR->height*imgPTR->stride);
	IMAGEPaint(imgPTR);
	free(tempBUF);
}

void GEOMETRYFlipVertical(IMAGE* imgPTR)
{
	uint8* tempBUF = NULL;
	uint32 i = 0, j = 0;
	tempBUF = (uint8*)calloc(imgPTR->height*imgPTR->stride,sizeof(uint8));
	for (i=0;i<imgPTR->height;i++)
	{
		for(j=0;j<imgPTR->width;j++)
		{
			tempBUF[i*imgPTR->stride + j*3 + 0] = imgPTR->imgBUFS[0][(imgPTR->height - 1 - i) * imgPTR->stride + j*3 + 0];
			tempBUF[i*imgPTR->stride + j*3 + 1] = imgPTR->imgBUFS[0][(imgPTR->height - 1 - i) * imgPTR->stride + j*3 + 1];
			tempBUF[i*imgPTR->stride + j*3 + 2] = imgPTR->imgBUFS[0][(imgPTR->height - 1 - i) * imgPTR->stride + j*3 + 2];
		}
	}
	IMAGESaveChanges(imgPTR);
	memcpy(imgPTR->imgBUFS[0],tempBUF,imgPTR->height*imgPTR->stride);
	IMAGEPaint(imgPTR);
	free(tempBUF);
}

void GEOMETRYRotate(IMAGE* imgPTR)
{
	uint8* tempBUF = NULL;
	uint32 i = 0, j = 0;
	uint32 tempWidth = 0, tempHeight = 0;


	tempBUF = (uint8*)calloc(imgPTR->height*imgPTR->stride,sizeof(uint8));

	for (i=0;i<imgPTR->height;i++)
	{
		for(j=0;j<imgPTR->width*3;j += 3)
		{
			tempBUF[j*imgPTR->height+i*3 + 0] = imgPTR->imgBUFS[0][i*imgPTR->stride+j + 0];
			tempBUF[j*imgPTR->height+i*3 + 1] = imgPTR->imgBUFS[0][i*imgPTR->stride+j + 1];
			tempBUF[j*imgPTR->height+i*3 + 2] = imgPTR->imgBUFS[0][i*imgPTR->stride+j + 2];
		}
	}
	if ( imgPTR->height != imgPTR->width)
	{
		imgPTR->imgBUFS[3] = imgPTR->createImage(imgPTR->height,imgPTR->width, &imgPTR->stride);
		tempWidth = imgPTR->width;
		tempHeight = imgPTR->height;
		imgPTR->width = tempHeight;
		imgPTR->height = tempWidth;
	}
	IMAGESaveChanges(imgPTR);
	memcpy(imgPTR->imgBUFS[0],tempBUF,imgPTR->height*imgPTR->stride);
	IMAGEPaint(imgPTR);
	free(tempBUF);
}