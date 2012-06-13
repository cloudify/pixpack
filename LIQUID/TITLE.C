
#include <memory.h>

#include "midas.h"
#include "main.h"
#include "fileio.h"
#include "memory.h"
#include "vga.h"
#include "title.h"

#include "globals.h"

static void inVR(void);

static ubyte *pPalette;
static ubyte *pTitleImg;
static ubyte *pTempPal;

void DoTitle(void)
{
	long frameOld;

	pPalette=pAlloc(64768L);
	pTitleImg=&pPalette[768];
	pTempPal=pAlloc(768);
	pLoadFile("liquid.img", pPalette, 64768L);

	tmrSyncScr(tmrSync200, preVR, NULL, inVR);

	FadePalFromBlack(pTempPal, pPalette, 0);

	memcpy((void*)0xA0000000L, pTitleImg, 64000);

	do { pInfoUpdate(); } while(pMpInfo->pos < 3);

	frameCount=0;
	while((frameOld=frameCount) < 16)
	{
		MakeBlackToWhitePal(pTempPal, (int)frameOld << 2);
		while(frameCount == frameOld);
	}

	frameCount=0;
	while((frameOld=frameCount) < 64)
	{
		FadePalFromWhite(pTempPal, pPalette, frameOld);
		while(frameCount == frameOld);
	}

	do { pInfoUpdate(); } while(pMpInfo->row < 40);

	frameCount=0;
	while((frameOld=frameCount) < 64)
	{
		FadePalFromBlack(pTempPal, pPalette, 64 - frameOld);
		while(frameCount == frameOld);
	}

	FadePalFromBlack(pTempPal, pPalette, 0);
	frameWaitOne();

	tmrSyncScr(tmrSync200, preVR, NULL, NULL);

	Free(pPalette);
	Free(pTempPal);
}

static void inVR(void)
{
	RemapPal(pTempPal, 0, 256);
}
