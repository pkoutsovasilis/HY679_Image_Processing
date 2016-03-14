#include "statistics.h"

void STATSCalculate(IMAGE* imgPTR,StatisticsAdd staAdd, StatisticsClear staClear)
{
	uint32 i = 0, j = 0, k = 0;
	uint64 sum[3] = { 0, 0, 0 }, sumP2[3] = { 0, 0, 0 }, mul_hor_f[3] = { 0, 0, 0 }, mul_hor_s[3] = { 0, 0, 0 }, mul_ver_f[3] = { 0, 0, 0 }, mul_ver_s[3] = { 0, 0, 0 };
	float average = 0.0, averageP2 = 0.0, var = 0.0, std_dev = 0.0, cor_hor_f = 0.0, cor_hor_s = 0.0, cor_ver_f = 0.0, cor_ver_s = 0.0;

	if (!imgPTR)
	{
		errorCallB("Internal Error");
		return;
	}

	staClear();
	for (i = 0; i<imgPTR->height; i++)
	{
		for (j = 0; j<imgPTR->width; j++)
		{
			for (k = 0; k<3; k++)
			{
				sum[k] += imgPTR->imgBUFS[0][i*(imgPTR->stride) + j * 3 + k];
				sumP2[k] += imgPTR->imgBUFS[0][i*(imgPTR->stride) + j * 3 + k] * imgPTR->imgBUFS[0][i*(imgPTR->stride) + j * 3 + k];
				if (j + 1<imgPTR->width)
				{
					mul_hor_f[k] += imgPTR->imgBUFS[0][i*(imgPTR->stride) + j * 3 + k + 3] * imgPTR->imgBUFS[0][i*(imgPTR->stride) + j * 3 + k];
				}
				if (j + 2<imgPTR->width)
				{
					mul_hor_s[k] += imgPTR->imgBUFS[0][i*(imgPTR->stride) + j * 3 + k + 6] * imgPTR->imgBUFS[0][i*(imgPTR->stride) + j * 3 + k];
				}
				if (i + 1<imgPTR->height)
				{
					mul_ver_f[k] += imgPTR->imgBUFS[0][(i + 1)*(imgPTR->stride) + j * 3 + k] * imgPTR->imgBUFS[0][i*(imgPTR->stride) + j * 3 + k];
				}
				if (i + 2<imgPTR->height)
				{
					mul_ver_s[k] += imgPTR->imgBUFS[0][(i + 2)*(imgPTR->stride) + j * 3 + k] * imgPTR->imgBUFS[0][i*(imgPTR->stride) + j * 3 + k];
				}
			}
		}
	}

	for (k = 0; k<3; k++)
	{
		average = ((float)sum[k]) / (imgPTR->height*imgPTR->width);
		averageP2 = average*average;
		var = ((float)sumP2[k]) / (imgPTR->height*imgPTR->width) - averageP2;
		std_dev = (float)sqrt(var);
		cor_hor_f = (((float)mul_hor_f[k]) / (imgPTR->height*(imgPTR->width - 1)) - averageP2) / var;
		cor_hor_s = (((float)mul_hor_s[k]) / (imgPTR->height*(imgPTR->width - 2)) - averageP2) / var;
		cor_ver_f = (((float)mul_ver_f[k]) / ((imgPTR->height - 1)*imgPTR->width) - averageP2) / var;
		cor_ver_s = (((float)mul_ver_s[k]) / ((imgPTR->height - 2)*imgPTR->width) - averageP2) / var;
		if (imgPTR->encoding[0] == PHOTOMETRIC_RGB)
		{
			staAdd(average, std_dev, cor_hor_f, cor_hor_s, cor_ver_f, cor_ver_s, RGB_CHANNELS[k]);
		}
		else
		{
			staAdd(average, std_dev, cor_hor_f, cor_hor_s, cor_ver_f, cor_ver_s, YCBCR_CHANNELS[k]);
		}

	}
}