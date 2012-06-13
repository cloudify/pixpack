/*
	- FaST!!! PLaSMa - Copyright (c) 1994 NeMeSiS Producion -
	CoDe: PiX / NeMeSiS
*/

#include "midas.h"
#include "main.h"
#include "vga.h"
#include "memory.h"
#include "fileio.h"
#include "plasmaa.h"
#include "plasma.h"

#include "globals.h"

void Move(void);
void inVR(void);

#define box_w 160
#define box_h 100
#define pi 3.1415926L

ubyte *pPalette;
ubyte *pTempPal;
ubyte *pTable1, *pATable1;
ubyte *pTable2, *pATable2;
ubyte *pMask, *pAMask;
int	*pTrigTable;

uword	circle1=0, circle2=0, circle3=0, circle4=0;
uword	circle5=0, circle6=0, circle7=0, circle8=0;
int 	x1, y1, x2, y2, x3, y3, x4, y4;

void DoPlasma(void)
{
	uword vgaBank;
	long frameOld;

	pTempPal=pAlloc(768);

	pPalette=pAlloc(768);
	pLoadFile("plasma.pal", pPalette, 768);

	pTable1=pAlloc(64000L + 16L);
	pATable1=_pAlignPara(pTable1);
	pLoadFile("plasma.tb1", pATable1, 64000L);

	pTable2=pAlloc(64000L + 16L);
	pATable2=_pAlignPara(pTable2);
	pLoadFile("plasma.tb2", pATable2, 64000L);

	pMask=pAlloc(32000L + 16L);
	pAMask=_pAlignPara(pMask);
	pLoadFile("plasma.msk", pAMask, 32000L);

	pTrigTable=pAlloc(320 * 2);
	pLoadFile("plasma.trg", pTrigTable, 320 * 2);

	vgaStart=0;
	vgaBank=0;

	vgaDupeScan(3);
	tmrSyncScr(tmrSync200, preVR, NULL, inVR);

	FadePalFromBlack(pTempPal, pPalette, 0);

	do
	{
		pInfoUpdate();
	}
	while((pMpInfo->pos < 1) || (pMpInfo->row < 32));

	frameCount=0;
	while((frameOld=frameCount) < 64)
	{
		vgaBank^=100 * 80;
		CalculateBody(x1, y1, x2, y2, x3, y3, vgaBank << 2);
		vgaStart=vgaBank;
		FadePalFromBlack(pTempPal, pPalette, frameCount);
		while(frameCount == frameOld);
	}

	FadePalFromBlack(pTempPal, pPalette, 64);
	while((pMpInfo->pos < 2) || (pMpInfo->row < 46))
	{
		frameOld=frameCount;
		vgaBank^=100 * 80;
		CalculateBody(x1, y1, x2, y2, x3, y3, vgaBank << 2);
		vgaStart=vgaBank;
		while(frameCount == frameOld);
	}

	frameCount=0;
	while((frameOld=frameCount) < 64)
	{
		vgaBank^=100 * 80;
		CalculateBody(x1, y1, x2, y2, x3, y3, vgaBank << 2);
		vgaStart=vgaBank;
		FadePalFromBlack(pTempPal, pPalette, 64 - frameCount);
		while(frameCount == frameOld);
	}

	vgaStart=0;
	FadePalFromBlack(pTempPal, pPalette, 0);
	frameWait(2);

	tmrSyncScr(tmrSync200, preVR, NULL, NULL);
	vgaDupeScan(1);

	Free(pTable1);
	Free(pTable2);
	Free(pMask);
	Free(pTrigTable);
	Free(pPalette);
	Free(pTempPal);
}

#define TGTBFP 1

void Move(void)
{
	x2=(box_w / 2) + (((box_w / 2) * pTrigTable[((circle1 += 3) >> TGTBFP) & 255]) >> 8); // 0.085/6*128/pi
	y2=(box_h / 2) + (((box_h / 2) * pTrigTable[(64 + ((circle2 -= 2) >> TGTBFP)) & 255]) >> 8); //.1/6*128/pi
	x1=(box_w / 2) + (((box_w / 2) * pTrigTable[(64 + ((circle3 += 3) >> TGTBFP)) & 255]) >> 8); //.3/6*128/pi
	y1=(box_h / 2) + (((box_h / 2) * pTrigTable[((circle4 -=  5) >> TGTBFP) & 255 ] ) >> 8); //.2/6*128/pi
	x3=(box_w / 2) + (((box_w / 2) * pTrigTable[(64 + ((circle5 +=  4) >> TGTBFP)) & 255]) >> 8); //.4/6*128/pi
	y3=(box_h / 2) + (((box_h / 2) * pTrigTable[((circle6 -= 3) >> TGTBFP) & 255]) >> 8); //.15/6*128/pi
}

void inVR(void)
{
	RemapPal(pTempPal, 0, 256);
	Move();
	pInfoUpdate();
}

