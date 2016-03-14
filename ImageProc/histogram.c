#include "histogram.h"

HISTOGRAM* HISTInitialise(IMAGE* imgPTR, IMAGE* histimgPTR, HistogramAdd histAdd, HistogramClear histClear)
{
	uint8 i = 0;
	HISTOGRAM* histPTR = (HISTOGRAM*)malloc(sizeof(HISTOGRAM));
	if (!histPTR)
	{
		errorCallB("Couldn't allocate memory");
		return NULL;
	}
	histPTR->histogramAdd = histAdd;
	histPTR->histogramClear = histClear;
	histPTR->imgPTR = imgPTR;
	histPTR->histImgPTR = histimgPTR;
	histPTR->histImgPTR->width = 256;
	histPTR->histImgPTR->height = 256;
	histPTR->histImgPTR->encoding[0] = PHOTOMETRIC_RGB;
	for (i = 0; i < 3; i++)
		histPTR->histogram[i] = NULL;
	histPTR->originalImage = (uint8*)calloc(imgPTR->height*imgPTR->stride, sizeof(uint8));
	if (!histPTR->originalImage)
	{
		errorCallB("Couldn't allocate memory");
		HISTFree(histPTR);
		return NULL;
	}
	memcpy(histPTR->originalImage, imgPTR->imgBUFS[0], imgPTR->height*imgPTR->stride);
	histPTR->histImgPTR->imgBUFS[3] = histPTR->histImgPTR->createImage(256, 256, &histPTR->histImgPTR->stride); // displaying image buffer
	return histPTR;
}

void HISTCalculate(HISTOGRAM* histPTR)
{
	uint32 hist_max[3] = { 0, 0, 0 }, i = 0, j = 0, k = 0;
	uint64  hist_sum = 0, hist_var = 0;
	IMAGE* imgPTR = histPTR->imgPTR;
	IMAGE* histimgPTR = histPTR->histImgPTR;
	float hist_aver = 0.0, hist_stan_dev = 0.0, scale[3] = { 0.0, 0.0, 0.0 };
	
	histPTR->histogramClear();

	for (i = 0; i < 3; i++)
	{
		histPTR->histogram[i] = (uint32*)calloc(256, sizeof(uint32));
		if (!histPTR->histogram[i])
		{
			errorCallB("Couldn't allocate memory");
			HISTFree(histPTR);
			return;
		}
	}

	for (i = 0; i < imgPTR->height; i++)
	{
		for (j = 0; j < imgPTR->width ; j++)
		{
			for (k = 0; k < 3; k++)
			{
				histPTR->histogram[k][imgPTR->imgBUFS[0][i*imgPTR->stride + j*3 + k]] += 1;
			}
		}
	}

	for (i = 0; i < 3; i++)
	{
		hist_sum = 0;
		hist_var = 0;
		for (j = 0; j < 256; j++)
		{

			if (hist_max[i] < histPTR->histogram[i][j])
			{
				hist_max[i] = histPTR->histogram[i][j];
			}

			hist_sum += histPTR->histogram[i][j] * j;
			hist_var += histPTR->histogram[i][j] * j*j;
		}
		hist_aver = ((float)hist_sum) / (imgPTR->dimensions);
		
		hist_stan_dev = (float)sqrt(((float)hist_var / (imgPTR->dimensions)) - pow((double)hist_aver, 2));

		if (imgPTR->encoding[0] == PHOTOMETRIC_RGB)
		{
			histPTR->histogramAdd(hist_aver, hist_stan_dev, hist_max[i], RGB_CHANNELS[i]);
		}
		else if (imgPTR->encoding[0] == PHOTOMETRIC_YCBCR)
		{
			histPTR->histogramAdd(hist_aver, hist_stan_dev, hist_max[i], YCBCR_CHANNELS[i]);
		}
		scale[i] = (float)(hist_max[i])*((float)1. / 256);
	}
	
	histimgPTR->lockImage();

	for (i = 0; i < 256; i++)
	{
		for (j = 0; j < 256; j++)
		{
			if ((hist_max[2] - histPTR->histogram[2][j]) / scale[2] + 0.5f <= i)
			{
				histimgPTR->imgBUFS[3][i*histimgPTR->stride + j * 3 + 0] = 53;
				histimgPTR->imgBUFS[3][i*histimgPTR->stride + j * 3 + 1] = 53;
				histimgPTR->imgBUFS[3][i*histimgPTR->stride + j * 3 + 2] = 228;
			}
			else if ((hist_max[1] - histPTR->histogram[1][j]) / scale[1] + 0.5f <= i)
			{
				histimgPTR->imgBUFS[3][i*histimgPTR->stride + j * 3 + 0] = 53;
				histimgPTR->imgBUFS[3][i*histimgPTR->stride + j * 3 + 1] = 228;
				histimgPTR->imgBUFS[3][i*histimgPTR->stride + j * 3 + 2] = 53;
			}
			else if ((hist_max[0] - histPTR->histogram[0][j]) / scale[0] + 0.5f <= i)
			{
				histimgPTR->imgBUFS[3][i*histimgPTR->stride + j * 3 + 0] = 228;
				histimgPTR->imgBUFS[3][i*histimgPTR->stride + j * 3 + 1] = 53;
				histimgPTR->imgBUFS[3][i*histimgPTR->stride + j * 3 + 2] = 53;
			}
			else {
				histimgPTR->imgBUFS[3][i*histimgPTR->stride + j * 3 + 0] = 208;
				histimgPTR->imgBUFS[3][i*histimgPTR->stride + j * 3 + 1] = 209;
				histimgPTR->imgBUFS[3][i*histimgPTR->stride + j * 3 + 2] = 215;
			}

		}
	}
	histimgPTR->unlockImage(256,256);
}

void HISTEqualize(HISTOGRAM* histPTR, bool RefreshHIST)
{
	uint32 i = 0, j = 0, k = 0;
	uint64  cdf[3][256];
	IMAGE* imgPTR = histPTR->imgPTR;

	float norm = 255.0f / imgPTR->dimensions;

	memset(cdf, 0, sizeof(cdf));

	for (i = 1; i < 256; i++)
	{
		cdf[0][i] = histPTR->histogram[0][i] + cdf[0][i - 1];
		cdf[1][i] = histPTR->histogram[1][i] + cdf[1][i - 1];
		cdf[2][i] = histPTR->histogram[2][i] + cdf[2][i - 1];
	}

	IMAGESaveChanges(imgPTR);

	for (i = 0; i<imgPTR->height; i++)
	{
		for (j = 0; j<imgPTR->width; j++)
		{
			imgPTR->imgBUFS[0][i*imgPTR->stride + j * 3 + 0] = (uint8)(cdf[0][imgPTR->imgBUFS[0][i*imgPTR->stride + j * 3 + 0]] * norm + 0.5f);
			imgPTR->imgBUFS[0][i*imgPTR->stride + j * 3 + 1] = (uint8)(cdf[1][imgPTR->imgBUFS[0][i*imgPTR->stride + j * 3 + 1]] * norm + 0.5f);
			imgPTR->imgBUFS[0][i*imgPTR->stride + j * 3 + 2] = (uint8)(cdf[2][imgPTR->imgBUFS[0][i*imgPTR->stride + j * 3 + 2]] * norm + 0.5f);
		}
	}
	IMAGEPaint(imgPTR);
	if (RefreshHIST)
	{
		HISTCalculate(histPTR);
	}
}

void HISTReset(HISTOGRAM* histPTR)
{
	IMAGE* imgPTR = histPTR->imgPTR;
	memcpy(imgPTR->imgBUFS[0], histPTR->originalImage, imgPTR->height*imgPTR->stride);
	IMAGEPaint(imgPTR);
	HISTCalculate(histPTR);
}

void HISTFree(HISTOGRAM* histPTR)
{
	uint8 i = 0;
	for (i = 0; i < 3; i++)
	{
		if (histPTR->histogram[i])
		{
			free(histPTR->histogram[i]);
			histPTR->histogram[i] = NULL;
		}
	}
	
	if (histPTR)
	{
		free(histPTR);
		histPTR = NULL;
	}
}