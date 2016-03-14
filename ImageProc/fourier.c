#include "fourier.h"

void FOURIERCalculate(IMAGE* imgPTR, uint8 type)
{
	uint8* bufPTR = imgPTR->imgBUFS[0];
	uint8* gray = NULL;
	uint32 i = 0, j = 0;
	double *magnitude = NULL, ampl_max = 0.0, ampl_min = 0.0;
	fftw_complex *in, *out;
	fftw_plan plan;

	magnitude = (double*)calloc(imgPTR->height*imgPTR->width, sizeof(double));
	if (!magnitude)
	{
		errorCallB("Can't allocate memory");
		return;
	}
	gray = (uint8*)calloc(imgPTR->height*imgPTR->width, sizeof(uint8));
	if (!gray)
	{
		errorCallB("Can't allocate memory");
		free(magnitude);
		return;
	}
	for (i = 0; i<imgPTR->height; i++)
	{
		for (j = 0; j<imgPTR->width; j++)
		{
			gray[i*imgPTR->width + j] = (uint8)(((float)(bufPTR[i*imgPTR->stride + j*3+0] + bufPTR[i*imgPTR->stride + j*3+1] + bufPTR[i*imgPTR->stride + j*3+2]))/3 + 0.5);
		}
	}

	in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)* imgPTR->height*imgPTR->width);
	if (!in)
	{
		errorCallB("Can't allocate memory");
		free(magnitude);
		fftw_free(in);
		return;
	}
	out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)* imgPTR->height*imgPTR->width);
	if (!out)
	{
		errorCallB("Can't allocate memory");
		free(magnitude);
		fftw_free(in);
		fftw_free(out);
		return;
	}
	for (i = 0; i<imgPTR->height; i++)
	{
		for (j = 0; j<imgPTR->width; j++)
		{
			in[i*imgPTR->width + j][0] = (double)gray[i*imgPTR->width + j];
			in[i*imgPTR->width + j][1] = 0.0;
		}
	}

	plan = fftw_plan_dft_2d(imgPTR->width, imgPTR->height, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(plan);

	for (i = 0; i<imgPTR->height; i++)
	{
		for (j = 0; j<imgPTR->width; j++)
		{
			if (type == 0)
			{
				magnitude[i*imgPTR->width + j] = atan2(out[i*imgPTR->width + j][1], out[i*imgPTR->width + j][0]);
			}
			else
			{
				if (sqrt(pow(out[i*imgPTR->width + j][0], 2) + pow(out[i*imgPTR->width + j][1], 2))>0)
				{
					magnitude[i*imgPTR->width + j] = log(sqrt(pow(out[i*imgPTR->width + j][0], 2) + pow(out[i*imgPTR->width + j][1], 2)));
				}
				else
				{
					magnitude[i*imgPTR->width + j] = 0.0;
				}
				
			}

			if (magnitude[i*imgPTR->width + j]<ampl_min)
			{
				ampl_min = magnitude[i*imgPTR->width + j];
			}
			if (magnitude[i*imgPTR->width + j]>ampl_max)
			{
				ampl_max = magnitude[i*imgPTR->width + j];
			}
		}
	}
	IMAGESaveChanges(imgPTR);
	for (i = 0; i<imgPTR->height; i++)
	{
		for (j = 0; j<imgPTR->width; j++)
		{
			magnitude[i*imgPTR->width + j] = ((magnitude[i*imgPTR->width + j] - ampl_min) / (ampl_max - ampl_min)) * 255 + 0.5;
			bufPTR[getPosY(imgPTR,i,imgPTR->height/2)*imgPTR->stride + getPosX(imgPTR,j,imgPTR->width/2) * 3 + 0] = (uint8)magnitude[i*imgPTR->width + j];
			bufPTR[getPosY(imgPTR,i,imgPTR->height/2)*imgPTR->stride + getPosX(imgPTR,j,imgPTR->width/2) * 3 + 1] = (uint8)magnitude[i*imgPTR->width + j];
			bufPTR[getPosY(imgPTR,i,imgPTR->height/2)*imgPTR->stride + getPosX(imgPTR,j,imgPTR->width/2) * 3 + 2] = (uint8)magnitude[i*imgPTR->width + j];
		}
	}
	fftw_free(in);
	fftw_free(out);
	IMAGEPaint(imgPTR);
	free(magnitude);
	free(gray);
	// Descrete Fourier Transform
	/*gray = (uint8*)calloc(imgPTR->height*imgPTR->width,sizeof(uint8));
	amplitude = (double*)calloc(imgPTR->height*imgPTR->width,sizeof(double));
	fourierARR = (FOURIER*) calloc(imgPTR->height*imgPTR->width,sizeof(FOURIER));

	for (i=0;i<imgPTR->height;i++)
	{
	for (j=0;j<imgPTR->width;j++)
	{
	gray[i*imgPTR->width + j] = (uint8)(bufPTR[i*imgPTR->stride + j*3 + 0] + bufPTR[i*imgPTR->stride + j*3 + 1] + bufPTR[i*imgPTR->stride + j*3 + 3])/3;
	}
	}

	for (k=0;k<imgPTR->height;k++)
	{
	for (l=0;l<imgPTR->width;l++)
	{
	sum_re = sum_im = 0;
	for (i=0;i<imgPTR->height;i++)
	{
	for (j=0;j<imgPTR->width;j++)
	{
	sum_re += gray[i*imgPTR->width + j]*cos(-2*M_PI*(((double)k*i)/imgPTR->height + ((double)l*j)/imgPTR->width));
	sum_im += gray[i*imgPTR->width + j]*sin(-2*M_PI*(((double)k*i)/imgPTR->height + ((double)l*j)/imgPTR->width));
	}
	}
	fourierARR[k*imgPTR->width + l].real = sum_re/imgPTR->dimensions;
	fourierARR[k*imgPTR->width + l].imag = sum_im/imgPTR->dimensions;
	}
	}

	for (k=0;k<imgPTR->height;k++)
	{
	for (l=0;l<imgPTR->width;l++)
	{
	amplitude[k*imgPTR->width+l] = log(sqrt(pow(fourierARR[k*imgPTR->width + l].real,2)+pow(fourierARR[k*imgPTR->width + l].imag,2)));
	if (amplitude[k*imgPTR->width+l]<ampl_min)
	{
	ampl_min = amplitude[k*imgPTR->width+l];
	}
	if (amplitude[k*imgPTR->width+l]>ampl_max)
	{
	ampl_max = amplitude[k*imgPTR->width+l];
	}
	}
	}

	for (k=0;k<imgPTR->height;k++)
	{
	for (l=0;l<imgPTR->width;l++)
	{
	amplitude[k*imgPTR->width+l] = ((amplitude[k*imgPTR->width+l]-ampl_min)/(ampl_max-ampl_min))*255+0.5;
	bufPTR[k*imgPTR->stride + l*3 + 0] = (uint8) amplitude[k*imgPTR->width+l];
	bufPTR[k*imgPTR->stride + l*3 + 1] = (uint8) amplitude[k*imgPTR->width+l];
	bufPTR[k*imgPTR->stride + l*3 + 2] = (uint8) amplitude[k*imgPTR->width+l];
	}
	}
	RefreshWriteableBitmap();
	RefreshOtherWindows();*/
}

uint32 getPosX(IMAGE* imgPTR, uint32 curPosX, uint32 offsetX)
{
	if (curPosX + offsetX < imgPTR->width) return curPosX + offsetX;
	return curPosX + offsetX - imgPTR->width;
}

uint32 getPosY(IMAGE* imgPTR, uint32 curPosY, uint32 offsetY)
{
	if (curPosY + offsetY < imgPTR->height) return curPosY + offsetY;
	return curPosY + offsetY - imgPTR->height;
}