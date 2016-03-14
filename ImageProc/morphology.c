#include "morphology.h"

void MORPHLoad(char** paths, int pathN)
{
	MarkPattern *Pat = NULL;
	uint8 i = 0, j = 0, k = 0, l = 0, size = 4;
	char *BUFF = NULL;
	FILE *f = NULL;
	memset(CSLUT, 0, sizeof(CSLUT));
	memset(CKLUT, 0, sizeof(CKLUT));
	memset(CTLUT, 0, sizeof(CTLUT));
	memset(USLUT, 0, sizeof(USLUT));
	memset(UKLUT, 0, sizeof(UKLUT));
	memset(UTLUT, 0, sizeof(UTLUT));
	for (k = 0; k < pathN; k++)
	{
		f = fopen(paths[k], "r");
		while (!feof(f))
		{
			if (fgetc(f) == '{')
			{
				l++;
				Pat = (MarkPattern*)calloc(1, sizeof(MarkPattern));
				if (!Pat)
				{
					errorCallB("Couldn't allocate memory");
					return;
				}
				BUFF = (char *)calloc(1, size);
				if (!BUFF)
				{
					errorCallB("Couldn't allocate memory");
					free(Pat);
					return;
				}
				while (BUFF[0] != '}')
				{
					fscanf(f, "%s", BUFF);
					ReadPattern(Pat, i, BUFF);
					i++;
				}
				i = 0;
				free(BUFF);
				BUFF = NULL;
				Pat->type = PT_CONDITIONAL;
				AddToLUT(Pat);
				if (Pat->charPos) free(Pat->charPos);
				if (Pat->dsPos) free(Pat->dsPos);
				if (Pat) free(Pat);
				Pat = NULL;
			}
		}
		fclose(f);
		f = NULL;
	}
}

void ReadPattern(MarkPattern* Pat, uint8 lineNumber, char *BUFF)
{
	switch (lineNumber)
	{
	case 0:
		Pat->table = ((uint8)BUFF[0]) + ((uint8)BUFF[1]) + ((uint8)BUFF[2]);
		break;
	case 1:
		Pat->x[4] = ((uint8)BUFF[0]);
		Pat->x[5] = ((uint8)BUFF[1]);
		Pat->x[6] = ((uint8)BUFF[2]);
		break;
	case 2:
		Pat->x[3] = ((uint8)BUFF[0]);
		Pat->x[8] = ((uint8)BUFF[1]);
		Pat->x[7] = ((uint8)BUFF[2]);
		break;
	case 3:
		Pat->x[2] = ((uint8)BUFF[0]);
		Pat->x[1] = ((uint8)BUFF[1]);
		Pat->x[0] = ((uint8)BUFF[2]);
		break;
	default:
		break;
	}

}

uint32 CountARR(uint8 values[512])
{
	uint32 i = 0, j = 0;
	for (i = 0; i < 512; i++)
	{
		if (values[i] == 1)
		{
			j++;
		}
	}
	return j;
}

void SaveToCondLUT(MarkPattern *Pat)
{
	uint32 l = 0;
	l = CalculateValue(Pat->x);
	switch (Pat->table)
	{
	case 83:
		CSLUT[l] = 1;
		break;
	case 84:
		CTLUT[l] = 1;
		break;
	case 75:
		CKLUT[l] = 1;
		break;
	case 167:
		CSLUT[l] = 1;
		CTLUT[l] = 1;
		break;
	case 158:
		CSLUT[l] = 1;
		CKLUT[l] = 1;
		break;
	case 159:
		CTLUT[l] = 1;
		CKLUT[l] = 1;
		break;
	case 242:
		CSLUT[l] = 1;
		CTLUT[l] = 1;
		CKLUT[l] = 1;
		break;
	default:
		break;

	}
}

void SaveToUncondLUT(MarkPattern *Pat)
{
	uint32 l = 0;
	l = CalculateValue(Pat->x);
	switch (Pat->table)
	{
	case 83:
		USLUT[l] = 1;
		break;
	case 84:
		UTLUT[l] = 1;
		break;
	case 75:
		UKLUT[l] = 1;
		break;
	case 167:
		USLUT[l] = 1;
		UTLUT[l] = 1;
		break;
	case 158:
		USLUT[l] = 1;
		UKLUT[l] = 1;
		break;
	case 159:
		UTLUT[l] = 1;
		UKLUT[l] = 1;
		break;
	case 242:
		USLUT[l] = 1;
		UTLUT[l] = 1;
		UKLUT[l] = 1;
		break;
	default:
		break;

	}
}

uint32 CalculateValue(uint8 *valARR)
{
	uint32 i = 0;
	int j = 0;
	for (j = 8; j >= 0; j = j - 1)
	{
		i = i << 1;
		i = i | valARR[j];
	}
	return i;

}

void AddToLUT(MarkPattern *Pat)
{
	uint8* valARR = Pat->x;
	uint8 j = 0;
	uint32 i = 0, k = 0, l = 0;

	for (j = 0; j < 9; j++)
	{
		switch (valARR[j])
		{
		case 'M':
			valARR[j] = 1;
			Pat->type = PT_UNCONDITIONAL;
			break;
		case 'D':
			Pat->dsNumber += 1;
			break;
		case '0':
			valARR[j] = 0;
			break;
		case '1':
			valARR[j] = 1;
			break;
		case 'A':
			Pat->charNumber += 1;
			break;
		case 'B':
			Pat->charNumber += 1;
			break;
		case 'C':
			Pat->charNumber += 1;
			break;
		}
	}
	if (Pat->charNumber > 0)
	{
		getallCharPoss(Pat, (int)Pat->charNumber);
	}
	if (Pat->dsNumber > 0)
	{
		getallDsPoss(Pat, (int)Pat->dsNumber);
	}
	if ((Pat->dsNumber == 0) && (Pat->charNumber == 0))
	{
		if (Pat->type == PT_CONDITIONAL)
		{
			SaveToCondLUT(Pat);
		}
		else if (Pat->type == PT_UNCONDITIONAL)
		{
			SaveToUncondLUT(Pat);
		}
	}
}

void getallCharPoss(MarkPattern *Pat, int charN)
{
	uint32 permutations = 0, j = 0, l = 0, k = 0;
	uint8* charPosition = NULL;
	uint8* valARR = Pat->x;
	MarkPattern *tempPat = NULL;
	permutations = (uint32)pow(2.0f, charN);
	charPosition = (uint8*)calloc(charN, sizeof(uint8));
	if (!charPosition)
	{
		errorCallB("Couldn't allocate memory");
		free(charPosition);
		charPosition = NULL;
		return;
	}
	for (j = 0; j < 9; j++)
	{
		if ((valARR[j] == 'A') || (valARR[j] == 'B') || (valARR[j] == 'C'))
		{
			charPosition[l] = j;
			l++;
		}
	}
	for (j = 0; j<permutations; j++)
	{
		if (k>0)
		{
			tempPat = (MarkPattern*)calloc(1, sizeof(MarkPattern));
			if (!tempPat)
			{
				errorCallB("Couldn't allocate memory");
				free(charPosition);
				charPosition = NULL;
				free(tempPat);
				tempPat = NULL;
				return;
			}
			tempPat->charNumber = 0;
			tempPat->dsNumber = 0;
			for (l = 0; l < (uint8)charN; l++)
			{
				valARR[charPosition[l]] = ((k & (1 << l)) >> l);
			}
			memcpy(tempPat->x, valARR, 9 * sizeof(uint8));
			tempPat->type = PT_UNCONDITIONAL;
			tempPat->table = Pat->table;
			AddToLUT(tempPat);
			free(tempPat);
			tempPat = NULL;
		}
		k++;
	}
	free(charPosition);
	charPosition = NULL;
}

void getallDsPoss(MarkPattern *Pat, int dsN)
{
	uint32 permutations = 0, j = 0, l = 0, k = 0;
	uint8* dsPosition = NULL;
	uint8* valARR = Pat->x;
	MarkPattern *tempPat = NULL;
	permutations = (uint32)pow(2.0f, dsN);
	dsPosition = (uint8*)calloc(dsN, sizeof(uint8));
	if (!dsPosition)
	{
		errorCallB("Couldn't allocate memory");
		free(dsPosition);
		dsPosition = NULL;
		return;
	}
	for (j = 0; j < 9; j++)
	{
		if (valARR[j] == 'D')
		{
			dsPosition[l] = j;
			l++;
		}
	}
	for (j = 0; j < permutations; j++)
	{
		tempPat = (MarkPattern*)calloc(1, sizeof(MarkPattern));
		if (!tempPat)
		{
			errorCallB("Couldn't allocate memory");
			free(tempPat);
			tempPat = NULL;
			free(dsPosition);
			dsPosition = NULL;
			return;
		}
		tempPat->charNumber = 0;
		tempPat->dsNumber = 0;
		for (l = 0; l < (uint8)dsN; l++)
		{
			valARR[dsPosition[l]] = ((k & (1 << l)) >> l);
		}
		memcpy(tempPat->x, valARR, 9 * sizeof(uint8));
		tempPat->type = PT_UNCONDITIONAL;
		tempPat->table = Pat->table;
		AddToLUT(tempPat);
		free(tempPat);
		tempPat = NULL;
		k++;
	}
	free(dsPosition);
	dsPosition = NULL;
}

uint32 getPixelPattern(uint8 *valARR, uint32 x, uint32 y, uint32 lineOFF)
{
	uint8 tempARR[9];
	memset(tempARR, 0, sizeof(tempARR));
	tempARR[8] = valARR[(x + 0)*lineOFF + (y + 0)];
	tempARR[7] = valARR[(x + 0)*lineOFF + (y + 1)];
	tempARR[6] = valARR[(x - 1)*lineOFF + (y + 1)];
	tempARR[5] = valARR[(x - 1)*lineOFF + (y + 0)];
	tempARR[4] = valARR[(x - 1)*lineOFF + (y - 1)];
	tempARR[3] = valARR[(x + 0)*lineOFF + (y - 1)];
	tempARR[2] = valARR[(x + 1)*lineOFF + (y - 1)];
	tempARR[1] = valARR[(x + 1)*lineOFF + (y + 0)];
	tempARR[0] = valARR[(x + 1)*lineOFF + (y + 1)];
	return CalculateValue(tempARR);
}

void MORPHProcess(IMAGE* imgPTR, uint32 iterations, Process_Type type)
{
	uint8 *gray = NULL, *mPixelStack = NULL, *output = NULL, *bufPTR = NULL, *CondHit = NULL, *UncondHit = NULL;
	uint32 i = 0, j = 0, k = 0, l = 0, debug = 0;
	FILE *gray_out = NULL, *mpixels_out = NULL, *output_file = NULL;

	//gray_out = fopen("C:\\Users\\Panos\\Desktop\\gray.txt", "w");
	//mpixels_out = fopen("C:\\Users\\Panos\\Desktop\\mpixels.txt", "w");
	//output_file = fopen("C:\\Users\\Panos\\Desktop\\output.txt", "w");

	bufPTR = imgPTR->imgBUFS[0];

	if (type == PR_TYPE_SHRINK)
	{
		CondHit = CSLUT;
		UncondHit = USLUT;
	}
	else if (type == PR_TYPE_THIN)
	{
		CondHit = CTLUT;
		UncondHit = UTLUT;
	}
	else
	{
		CondHit = CKLUT;
		UncondHit = UKLUT;
	}

	IMAGESaveChanges(imgPTR);
	for (k = 0; k < iterations; k++)
	{
		gray = (uint8*)calloc(imgPTR->height*imgPTR->width, sizeof(uint8));
		if (!gray)
		{
			errorCallB("Couldn't allocate memory");
			free(gray);
			gray = NULL;
			return;
		}
		mPixelStack = (uint8*)calloc(imgPTR->height*imgPTR->width, sizeof(uint8));
		if (!mPixelStack)
		{
			errorCallB("Couldn't allocate memory");
			free(gray);
			gray = NULL;
			free(mPixelStack);
			mPixelStack = NULL;
			return;
		}
		output = (uint8*)calloc(imgPTR->height*imgPTR->width, sizeof(uint8));
		if (!output)
		{
			errorCallB("Couldn't allocate memory");
			free(gray);
			gray = NULL;
			free(mPixelStack);
			mPixelStack = NULL;
			free(output);
			output = NULL;
			return;
		}
		for (i = 0; i < imgPTR->height; i++)
		{
			for (j = 0; j < imgPTR->width; j++)
			{
				gray[i*imgPTR->width + j] = bufPTR[i*imgPTR->stride + j * 3 + 0];
				if (gray[i*imgPTR->width + j] != 255)
				{
					gray[i*imgPTR->width + j] = 1;
				}
				else
				{
					gray[i*imgPTR->width + j] = 0;
				}

			/*	fputc(gray[i*imgPTR->width + j] + 48, gray_out);
				fputc(' ', gray_out);*/
			}
			//fputc('\n', gray_out);
		}

		//fclose(gray_out);


		for (i = 1; i < imgPTR->height - 1; i++)
		{
			for (j = 1; j < imgPTR->width - 1; j++)
			{
				l = getPixelPattern(gray, i, j, imgPTR->width);

				if (CondHit[l] == 1)
				{
					mPixelStack[i*imgPTR->width + j] = 1;
				}

			/*	fputc(mPixelStack[i*imgPTR->width + j] + 48, mpixels_out);
				fputc(' ', mpixels_out);*/
			}
			//fputc('\n', mpixels_out);
		}
		//fclose(mpixels_out);

		for (i = 1; i < imgPTR->height - 1; i++)
		{
			for (j = 1; j<imgPTR->width - 1; j++)
			{

				if (gray[i*imgPTR->width + j]>0)
				{

					l = getPixelPattern(mPixelStack, i, j, imgPTR->width);
					if (mPixelStack[i*imgPTR->width + j] == 0)
					{
						output[i*imgPTR->width + j] = 1;
					}
					else if ((UncondHit[l] == 1))
					{
						output[i*imgPTR->width + j] = 1;
					}
				}
				else {
					output[i*imgPTR->width + j] = 0;
				}
				
				//fputc(output[i*imgPTR->width + j] + 48, output_file);
				//fputc(' ', output_file);
			}
			//fputc('\n', output_file);
		}
		//fclose(output_file);


		for (i = 0; i < imgPTR->height; i++)
		{
			for (j = 0; j < imgPTR->width; j++)
			{
				bufPTR[i*imgPTR->stride + j * 3 + 0] = 255 - 255 * output[i*imgPTR->width + j];
				bufPTR[i*imgPTR->stride + j * 3 + 1] = 255 - 255 * output[i*imgPTR->width + j];
				bufPTR[i*imgPTR->stride + j * 3 + 2] = 255 - 255 * output[i*imgPTR->width + j];
			}
		}
	}
	free(gray);
	gray = NULL;
	free(mPixelStack);
	mPixelStack = NULL;
	free(output);
	output = NULL;
	IMAGEPaint(imgPTR);
}