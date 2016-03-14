#include "filters.h"

void FILTERSInitialise(FiltersAdd Add, FiltersClear Clear)
{
	filtAdd = Add;
	filtClear = Clear;
}

void FILTERSLoad(char** paths, int pathN)
{
	FILTER *tmpFILTER = NULL;
	FILE *f = NULL;
	uint8 i = 0, j = 0, k = 0;
	int x = 0, y = 0;
	int BUFF1[MAXBUFF];
	char BUFF2[10];

	filtClear();
	for (k = 0; k < pathN; k++)
	{
		f = fopen(paths[k], "r");
		if (!f)
		{
			printf("");
		}
		while (!feof(f))
		{
			memset(BUFF2, 0, sizeof(BUFF2));
			if (fgetc(f) == '{')
			{
				i = 0;
				j = 0;
				memset(BUFF1, 0, sizeof(BUFF1));
				tmpFILTER = (FILTER*)calloc(1, sizeof(FILTER));
				tmpFILTER->coefficients = NULL;
				if (!tmpFILTER)
				{
					errorCallB("Couldn't allocate memory");
					return;
				}
				if (!BUFF1)
				{
					return;
				}
				while (fscanf(f, "%i", &BUFF1[i]) > 0)
				{
					if (fgetc(f) == '\n')
					{
						j++;
					}
					i++;
				}

				fscanf(f, "c: %d,%d", &x, &y);
				tmpFILTER->centerX = x;
				tmpFILTER->centerY = y;
				while (fgetc(f) != '\n');
				fscanf(f, "n: %s", BUFF2);
				tmpFILTER->lines = j;
				tmpFILTER->columns = i / j;
				tmpFILTER->coefficients = (int*)calloc(i, sizeof(int));
				memcpy(tmpFILTER->coefficients, BUFF1, i*sizeof(int));
				filtAdd(tmpFILTER, BUFF2);
			}
		}
		fclose(f);
	}
}

uint32 getPosY(int posY, uint32 Height)
{
	if (posY < 0)
	{
		return 0;
	}
	else if (posY > (int)Height - 1)
	{
		return Height - 1;
	}
	return posY;
}

uint32 getPosX(int posX, uint32 Width)
{
	if (posX < 0)
	{
		return 0;
	}
	else if (posX > (int)Width - 1)
	{
		return Width - 1;
	}
	return posX;
}

void FILTERSApply(IMAGE* imgPTR, FILTER* filter)
{
	uint8* imgBUF = imgPTR->imgBUFS[0], *tempBUF = NULL;
	int *coefficients = filter->coefficients, filter_sum = 0, pixsum[3] = { 0, 0, 0 }, k = 0, l = 0;
	uint32 i = 0, j = 0, a = 0, b = 0, offsets[4] = { 0, 0, 0, 0 };

	tempBUF = (uint8*)calloc(imgPTR->height*imgPTR->stride, sizeof(uint8));
	if (!tempBUF)
	{
		errorCallB("Can't open image");
		return;
	}

	for (i = 0; i < filter->lines; i++)
		for (j = 0; j < filter->columns; j++)
			filter_sum += coefficients[i*filter->columns + j];

	if (filter_sum < 0)
	{
		errorCallB("Can't open image");
		free(tempBUF);
		return;
	}

	offsets[0] = filter->centerX;
	offsets[1] = (filter->columns - 1) - filter->centerX;
	offsets[2] = filter->centerY;
	offsets[3] = (filter->lines - 1) - filter->centerX;

	for (i = 0; i < imgPTR->height; i++)
	{
		for (j = 0; j < imgPTR->width; j++)
		{
			memset(pixsum, 0, sizeof(pixsum));
			for (k = i - offsets[2]; k <= (int)(i + offsets[3]); k++)
			{
				for (l = j - offsets[0]; l <= (int)(j + offsets[1]); l++)
				{
					pixsum[0] += ((int)imgBUF[getPosY(k, imgPTR->height)*imgPTR->stride + 3 * getPosX(l, imgPTR->width) + 0]) * coefficients[(k - i + offsets[2])*filter->columns + (l - j + offsets[0])];
					pixsum[1] += ((int)imgBUF[getPosY(k, imgPTR->height)*imgPTR->stride + 3 * getPosX(l, imgPTR->width) + 1]) * coefficients[(k - i + offsets[2])*filter->columns + (l - j + offsets[0])];
					pixsum[2] += ((int)imgBUF[getPosY(k, imgPTR->height)*imgPTR->stride + 3 * getPosX(l, imgPTR->width) + 2]) * coefficients[(k - i + offsets[2])*filter->columns + (l - j + offsets[0])];

				}
			}
			if (filter_sum > 0) //Low Pass Filter
			{
				tempBUF[i*imgPTR->stride + 3 * j + 0] = (uint8)(pixsum[0] / filter_sum);
				tempBUF[i*imgPTR->stride + 3 * j + 1] = (uint8)(pixsum[1] / filter_sum);
				tempBUF[i*imgPTR->stride + 3 * j + 2] = (uint8)(pixsum[2] / filter_sum);
			}
			else if (filter_sum == 0)  //High Pass Filter
			{
				if (coefficients[filter->centerY*filter->columns + filter->centerX] >= 0)
				{
					tempBUF[i*imgPTR->stride + 3 * j + 0] = (imgBUF[i*imgPTR->stride + 3 * j + 0] + abs(pixsum[0]) > 255) ? 255 : imgBUF[i*imgPTR->stride + 3 * j + 0] + abs(pixsum[0]);
					tempBUF[i*imgPTR->stride + 3 * j + 1] = (imgBUF[i*imgPTR->stride + 3 * j + 1] + abs(pixsum[1]) > 255) ? 255 : imgBUF[i*imgPTR->stride + 3 * j + 1] + abs(pixsum[1]);
					tempBUF[i*imgPTR->stride + 3 * j + 2] = (imgBUF[i*imgPTR->stride + 3 * j + 2] + abs(pixsum[2]) > 255) ? 255 : imgBUF[i*imgPTR->stride + 3 * j + 2] + abs(pixsum[2]);

				}
				else
				{
					tempBUF[i*imgPTR->stride + 3 * j + 0] = (imgBUF[i*imgPTR->stride + 3 * j + 0] - abs(pixsum[0]) < 0) ? 0 : imgBUF[i*imgPTR->stride + 3 * j + 0] - abs(pixsum[0]);
					tempBUF[i*imgPTR->stride + 3 * j + 1] = (imgBUF[i*imgPTR->stride + 3 * j + 1] - abs(pixsum[1]) < 0) ? 0 : imgBUF[i*imgPTR->stride + 3 * j + 1] - abs(pixsum[1]);
					tempBUF[i*imgPTR->stride + 3 * j + 2] = (imgBUF[i*imgPTR->stride + 3 * j + 2] - abs(pixsum[2]) < 0) ? 0 : imgBUF[i*imgPTR->stride + 3 * j + 2] - abs(pixsum[2]);
				}
			}
		}
	}
	IMAGESaveChanges(imgPTR);
	memcpy(imgBUF, tempBUF, imgPTR->height*imgPTR->stride);
	free(tempBUF);
	tempBUF = NULL;
	IMAGEPaint(imgPTR);


}

void FILTERSFree(FILTER* filtPTR)
{
	if (filtPTR->coefficients) free(filtPTR->coefficients);
	if (filtPTR) free(filtPTR);

}