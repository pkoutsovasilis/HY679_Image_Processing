#ifndef _MORPHOLOGY_
#define _MORPHOLOGY_

#include "image.h"
#include "globals.h"
#include <math.h>

typedef enum _pat_typ
{
	PT_CONDITIONAL,
	PT_UNCONDITIONAL
} Pattern_Type;

typedef struct _un_pat
{
	uint32 table;
	uint8 x[9];
	uint8* charPos;
	uint8 charNumber;
	uint8* dsPos;
	uint8 dsNumber;
	Pattern_Type type;
} MarkPattern;

typedef enum _proc_typ
{
	PR_TYPE_SHRINK,
	PR_TYPE_THIN,
	PR_TYPE_SKELET
} Process_Type;

static uint8 CSLUT[512];
static uint8 CKLUT[512];
static uint8 CTLUT[512];

static uint8 USLUT[512];
static uint8 UKLUT[512];
static uint8 UTLUT[512];

DLL void MORPHLoad(char** paths, int pathN);
DLL void MORPHProcess(IMAGE* imgPTR, uint32 iterations, Process_Type type);
static uint8 checkUnconditionalHit(uint32 index, Process_Type type);
static uint8 checkConditionalHit(uint32 index, Process_Type type);
static void ReadPattern(MarkPattern* Pat, uint8 lineNumber, char *BUFF);
static void getallCharPoss(MarkPattern *Pat, int charN);
static void getallDsPoss(MarkPattern *Pat, int dsN);
static void AddToLUT(MarkPattern *Pat);
static void SaveToCondLUT(MarkPattern *Pat);
static void SaveToUncondLUT(MarkPattern *Pat);
static uint32 CalculateValue(uint8 valARR[8]);
static uint32 CountARR(uint8 values[512]);
static uint32 getPixelPattern(uint8 *valARR, uint32 x, uint32 y, uint32 lineOFF);
#endif