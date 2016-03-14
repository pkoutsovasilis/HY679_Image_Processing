#include "sinogram.h"

void SINOGRAMNormalize(IMAGE* imgPTR, float* tempBUF)
{
	int i = 0, j = 0, k = 0, l = 0;
	float max = 0.0, min = 0.0;
	uint8* imgBUF = imgPTR->imgBUFS[0];

	for (i = 0; i < (int)(imgPTR->height); i++)
	{
		for (j = 0; j < (int)(imgPTR->width); j++)
		{
			if (max < tempBUF[i*imgPTR->width + j])
			{
				max = tempBUF[i*imgPTR->width + j];
			}
			else if (min > tempBUF[i*imgPTR->width + j])
			{
				min = tempBUF[i*imgPTR->width + j];
			}
		}
	}


	for (i = 0; i < (int)(imgPTR->height); i++)
	{
		for (j = 0; j < (int)(imgPTR->width); j++)
		{
			tempBUF[i*imgPTR->width + j] = (float)(((tempBUF[i*imgPTR->width + j] - min) / (max - min)) * 255 + 0.5);
			imgBUF[i*imgPTR->stride + j * 3 + 0] = (uint8)tempBUF[i*imgPTR->width + j];
			imgBUF[i*imgPTR->stride + j * 3 + 1] = (uint8)tempBUF[i*imgPTR->width + j];
			imgBUF[i*imgPTR->stride + j * 3 + 2] = (uint8)tempBUF[i*imgPTR->width + j];
		}
	}

	free(tempBUF);
	tempBUF = NULL;

	IMAGEPaint(imgPTR);
}

void SINOGRAMBackprojection(IMAGE* imgPTR)
{
	uint8* imgBUF = imgPTR->imgBUFS[0];
	int i = 0, j = 0, k = 0, l = 0;
	float* tempBUF = NULL;
	float angleCenter = 0.0;
	float angle = ((float)M_PI) / 512.0f, filter = 0.0;
	int tmp = 0;
	
	tempBUF = (float*)calloc(imgPTR->height*imgPTR->width, sizeof(float));

	for (i = 0; i < (int)(imgPTR->height); i++)
	{
		angleCenter = i*angle;

		for (k=-(int)(imgPTR->height*0.5);k<(int)(imgPTR->height*0.5);k++)
		{
			for (j=-(int)(imgPTR->width*0.5);j<(int)(imgPTR->width*0.5);j++)
			{
				tmp = (int)(((double)j*cos(angleCenter) + k*sin(angleCenter)+0.5));
				if ((tmp>=-(int)(imgPTR->width*0.5))&&(tmp<(int)(imgPTR->width*0.5)))
				{
					tempBUF[(int)((k+(imgPTR->height*0.5))*imgPTR->width + (j+(imgPTR->width*0.5)))] += (float)imgBUF[i*imgPTR->stride + (tmp+(int)(imgPTR->width*0.5))*3];
				}
			}
		}
	}
	SINOGRAMNormalize(imgPTR, tempBUF);
}

void SINOGRAMFilteredBackprojection(IMAGE* imgPTR, WINDOW_TYPE type)
{
	uint8* imgBUF = imgPTR->imgBUFS[0];
	int i = 0, j = 0, k = 0, l = 0;
	float* tempBUF = NULL;
	float angleCenter = 0.0;
	float angle = ((float)M_PI) / 512.0f, *filter = NULL;
	int tmp = 0;
	fftw_complex *in = NULL, *out = NULL, *fnl = NULL;
	fftw_plan plan, plan2;

	filter = (float*)calloc(imgPTR->width, sizeof(float));

	if (type == TYPE_HAMMING)
	{
		for (l = 0; l < (int)(imgPTR->width); l++)
		{
			filter[l] = (float)(0.54 + 0.46*cos(2 * M_PI*(l - (int)(imgPTR->width*0.5)) / (imgPTR->width - 1)))*abs(l - (int)(imgPTR->width*0.5));
		}
	}
	else if (type == TYPE_HANNING)
	{
		for (l = 0; l < (int)(imgPTR->width); l++)
		{
			filter[l] = (float)(0.5 + 0.5*cos(2 * M_PI*(l - (int)(imgPTR->width*0.5)) / (imgPTR->width - 1)))*abs(l - (int)(imgPTR->width*0.5));
		}
	}

	tempBUF = (float*)calloc(imgPTR->height*imgPTR->width, sizeof(float));

	for (i = 0; i < (int)(imgPTR->height); i++)
	{
		in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*imgPTR->width);
		out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*imgPTR->width);
		fnl = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*imgPTR->width);

		plan = fftw_plan_dft_1d(imgPTR->width, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
		plan2 = fftw_plan_dft_1d(imgPTR->width, out, fnl, FFTW_BACKWARD, FFTW_ESTIMATE);

		for (l = 0; l < (int)(imgPTR->width); l++)
		{
			in[l][0] = (float)(imgBUF[i*imgPTR->stride + l * 3 + 0]);
			in[l][1] = 0.0;
		}

		fftw_execute(plan);

		for (l = -(int)(imgPTR->width*0.5); l < (int)(imgPTR->width*0.5); l++)
		{
			out[getpos(l, imgPTR)][0] *= filter[l + (int)(imgPTR->width*0.5)];
			out[getpos(l, imgPTR)][1] *= filter[l + (int)(imgPTR->width*0.5)];
		}

		fftw_execute(plan2);

		angleCenter = i*angle;
		
		for (k = -(int)(imgPTR->height*0.5); k<(int)(imgPTR->height*0.5); k++)
		{
			for (j = -(int)(imgPTR->width*0.5); j<(int)(imgPTR->width*0.5); j++)
			{
				tmp = (int)(((double)j*cos(angleCenter) + k*sin(angleCenter) + 0.5));
				if ((tmp >= -(int)(imgPTR->width*0.5)) && (tmp<(int)(imgPTR->width*0.5)))
				{
					tempBUF[(int)((k + (imgPTR->height*0.5))*imgPTR->width + (j + (imgPTR->width*0.5)))] += (float)fnl[(int)(tmp + imgPTR->width*0.5)][0];//(float)imgBUF[i*imgPTR->stride + (tmp+(int)(imgPTR->width*0.5))*3];
				}
			}
		}

		fftw_free(in);
		in = NULL;

		fftw_free(out);
		out = NULL;

		fftw_free(fnl);
		fnl = NULL;
	}

	SINOGRAMNormalize(imgPTR, tempBUF);
}

void SINOGRAMFilt(IMAGE* imgPTR, double const* filter)
{
	uint8* imgBUF = imgPTR->imgBUFS[0];
	int i = 0, j = 0, k = 0, l = 0;
	float* tempBUF = NULL;
	float angleCenter = 0.0;
	float angle = ((float)M_PI) / 512.0f;
	int tmp = 0;
	fftw_complex *in = NULL, *out = NULL, *fnl = NULL;
	fftw_plan plan, plan2;

	tempBUF = (float*)calloc(imgPTR->height*imgPTR->width, sizeof(float));

	for (i = 0; i < (int)(imgPTR->height); i++)
	{
		in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*imgPTR->width);
		out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*imgPTR->width);
		fnl = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*imgPTR->width);

		plan = fftw_plan_dft_1d(imgPTR->width, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
		plan2 = fftw_plan_dft_1d(imgPTR->width, out, fnl, FFTW_BACKWARD, FFTW_ESTIMATE);

		for (l = 0; l < (int)(imgPTR->width); l++)
		{
			in[l][0] = (float)(imgBUF[i*imgPTR->stride + l * 3 + 0]);
			in[l][1] = 0.0;
		}

		fftw_execute(plan);

		for (l = -(int)(imgPTR->width*0.5); l < (int)(imgPTR->width*0.5); l++)
		{
			out[getpos(l, imgPTR)][0] *= filter[l + (int)(imgPTR->width*0.5)];
			out[getpos(l, imgPTR)][1] *= filter[l + (int)(imgPTR->width*0.5)];
		}

		fftw_execute(plan2);

		angleCenter = i*angle;
		
		for (k = -(int)(imgPTR->height*0.5); k<(int)(imgPTR->height*0.5); k++)
		{
			for (j = -(int)(imgPTR->width*0.5); j<(int)(imgPTR->width*0.5); j++)
			{
				tmp = (int)(((double)j*cos(angleCenter) + k*sin(angleCenter) + 0.5));
				if ((tmp >= -(int)(imgPTR->width*0.5)) && (tmp<(int)(imgPTR->width*0.5)))
				{
					tempBUF[(int)((k + (imgPTR->height*0.5))*imgPTR->width + (j + (imgPTR->width*0.5)))] += (float)fnl[(int)(tmp + imgPTR->width*0.5)][0];//(float)imgBUF[i*imgPTR->stride + (tmp+(int)(imgPTR->width*0.5))*3];
				}
			}
		}

		fftw_free(in);
		in = NULL;

		fftw_free(out);
		out = NULL;

		fftw_free(fnl);
		fnl = NULL;
	}

	SINOGRAMNormalize(imgPTR, tempBUF);
}

int getpos(int number, IMAGE* imgPTR)
{
	if (number < 0)
	{
		return (int)(imgPTR->width) + number;
	}
	else
	{
		return number;
	}
}