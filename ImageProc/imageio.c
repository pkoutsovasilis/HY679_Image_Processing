#include "imageio.h"

int LoadImage(IMAGE* imgPTR, char* path)
{
	TIFF *in = NULL;
	uint32* raster = NULL;
	uint32 i = 0, j = 0, k = 0;
	in = TIFFOpen(path, "r");
	if (!in)
	{
		errorCallB("Can't open image");
		return -1;
	}

	/*imgPTR->changeImageProperty("YCbCrAvailable","true");*/
	TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &imgPTR->width);
	TIFFGetField(in, TIFFTAG_IMAGELENGTH, &imgPTR->height);
	imgPTR->changeImageProperty("ImagePath", path);
	imgPTR->dimensions = imgPTR->width*imgPTR->height;
	imgPTR->encoding[0] = PHOTOMETRIC_RGB;
	imgPTR->encoding[1] = PHOTOMETRIC_RGB;
	raster = (uint32*)_TIFFmalloc(imgPTR->dimensions*sizeof(uint32));
	if (!raster)
	{
		errorCallB("Can't allocate memory");
		return -1;
	}
	imgPTR->imgBUFS[3] = imgPTR->createImage(imgPTR->width, imgPTR->height, &imgPTR->stride); // displaying image buffer
	for (k = 0; k < 3; k++)
	{
		imgPTR->imgBUFS[k] = (uint8*)calloc(imgPTR->height*imgPTR->stride, sizeof(uint8)); // original image buffer
		if (!imgPTR->imgBUFS[k])
		{
			errorCallB("Can't allocate Memory");
			IMAGEFree(imgPTR);
			return -1;
		}
	}

	if (!TIFFReadRGBAImageOriented(in, imgPTR->width, imgPTR->height, raster, ORIENTATION_TOPLEFT, 0))
	{
		errorCallB("Can't read image");
		IMAGEFree(imgPTR);
		return -1;
	}
	for (i = 0; i < imgPTR->height; i++)
	{
		for (j = 0; j < imgPTR->width; j++)
		{
			imgPTR->imgBUFS[0][i*imgPTR->stride + j * 3 + 0] = (raster[i*imgPTR->width + j] & 0xff);
			imgPTR->imgBUFS[0][i*imgPTR->stride + j * 3 + 1] = ((raster[i*imgPTR->width + j] >> 8) & 0xff);
			imgPTR->imgBUFS[0][i*imgPTR->stride + j * 3 + 2] = ((raster[i*imgPTR->width + j] >> 16) & 0xff);
		}
	}
	for (k = 0; k < 3; k++)
	{
		memcpy(imgPTR->imgBUFS[k], imgPTR->imgBUFS[0], imgPTR->height*imgPTR->stride);
	}
	IMAGEPaint(imgPTR);
	TIFFClose(in);
	in = NULL;
	return 1;
}

void LoadRAWImage(IMAGE* imgPTR, char* paths[], uint16 channels, uint16 bitpsam, uint32 width, uint32 height)
{
	FILE *inraw[3] = {NULL,NULL,NULL};
	uint8 chbytes = 0;
	TIFF *out = NULL;
	uint8 ibuf[2] = {0,0};
	uint32 i= 0,j=0,k=0,l=0;
	uint32 max = 0, min = 0;
	uint32 tmp = 0;
	imgPTR->dimensions = height*width;
	imgPTR->width = width;
	imgPTR->height = height;
	imgPTR->changeImageProperty("ImagePath", paths[0]);
	imgPTR->encoding[0] = PHOTOMETRIC_RGB;
	imgPTR->encoding[1] = PHOTOMETRIC_RGB;
	imgPTR->imgBUFS[3] = imgPTR->createImage(imgPTR->width, imgPTR->height, &imgPTR->stride); // displaying image buffer
	for (k = 0; k < 3; k++)
	{
		imgPTR->imgBUFS[k] = (uint8*)calloc(imgPTR->height*imgPTR->stride, sizeof(uint8)); // original image buffer
		if (!imgPTR->imgBUFS[k])
		{
			errorCallB("Can't allocate Memory");
			IMAGEFree(imgPTR);
		}
	}
	chbytes = (uint16)(bitpsam+7)/8;
	for (i=0;i<chbytes;i++)
	{
		max <<= 8;
		max |= 0xFF;
	}
	for (i = 0; i<channels; i++)
	{
		inraw[i] = fopen(paths[i],"rb");
		if (!inraw[i])
		{
			errorCallB("Cant open input raw file");
		}
	}
	for (i = 0 ; i<height;i++)
	{
		for (j = 0; j < width;  j++)
		{
			fread(ibuf,sizeof(uint8),chbytes,inraw[0]);
			tmp = 0;
			tmp = (ibuf[1]<<8)|ibuf[0];
			tmp =(uint32)((float)tmp/max * 255.0f + 0.5f);
			imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 + 0] = tmp;
			imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 + 1] = tmp;
			imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 + 2] = tmp;
		}
	}
	IMAGEPaint(imgPTR);
}

void SaveImage(IMAGE* imgPTR, char* path, bool openSavedIMG)
{
	TIFF *out = NULL;
	uint32 i = 0, j = 0;
	uint8* linePTR = NULL;
	uint16 tmp = 0;
	out = TIFFOpen(path, "w");
	if (!out)
	{
		errorCallB("Can't open output image");
		IMAGEFree(imgPTR);
		return;
	}
	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, imgPTR->width);
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, imgPTR->height);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8, 8, 8);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 3);
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(out, TIFFTAG_PHOTOMETRIC, imgPTR->encoding[0]);
	linePTR = imgPTR->imgBUFS[3];
	if (imgPTR->encoding[0] == PHOTOMETRIC_YCBCR)
	{
		TIFFSetField(out, TIFFTAG_YCBCRSUBSAMPLING, 1, 1);
		TIFFSetField(out, TIFFTAG_MINSAMPLEVALUE, 0, 0, 0);
		TIFFSetField(out, TIFFTAG_MAXSAMPLEVALUE, 255, 255, 255);
		TIFFSetField(out, TIFFTAG_COMPRESSION, COMPRESSION_JPEG);
		linePTR = imgPTR->imgBUFS[0];
	}

	for (i = 0; i < imgPTR->height; i++)
	{
		if (TIFFWriteScanline(out, linePTR, i, 0) < 0)
		{
			break;
		}
		linePTR += imgPTR->stride;
	}

	TIFFClose(out);
	out = NULL;		
}