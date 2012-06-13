
#include <conio.h>

#include "midas.h"

#include "main.h"
#include "memory.h"
#include "fileio.h"
#include "vga.h"
#include "slimea.h"
#include "slime.h"

#include "globals.h"

static void inVR(void);

uchar *pPicture, *pAPicture;
long *pSlimeTable, *pASlimeTable;
static uchar *pPalette, *pTempPal;
long *pTrig;

long StartX, StartY, dX, dY, Zoom;

int SlimePos=0;
int ang=0, count=0, pos=0;

int PatternOldRow=-1;
int ZoomSide=1;

void DoSlime(void)
{
	long frameOld;

	pPicture=pAlloc(32768L + 16L);
	pAPicture=_pAlignPara(pPicture);

	pSlimeTable=pAlloc(MAXTRIG * sizeof(long) + 16L);
	pASlimeTable=_pAlignPara(pSlimeTable);

	pPalette=pAlloc(768);
	pTempPal=pAlloc(768);
	pTrig=pAlloc(MAXTRIG * sizeof(long));

	pLoadFile("slime.bin", pAPicture, 32768L);
	pLoadFile("slime.tab", pASlimeTable, MAXTRIG * sizeof(long));
	pLoadFile("slime.pal", pPalette, 768);
	pLoadFile("slime.trg", pTrig, MAXTRIG * sizeof(long));

	FadePalFromBlack(pTempPal, pPalette, 0);
	RemapPal(pTempPal, 0, 256);

	tmrSyncScr(tmrSync200, preVR, NULL, inVR);

	frameCount=0;
	frameWaitOne();

	while(pos != 255) {
		frameOld=frameCount;
		DrawPic(StartX, StartY, dX, dY, (ZoomSide) ? Zoom : -Zoom);
		while(frameCount == frameOld);
	}
	frameCount=0;
	while(frameCount < 128) {
		frameOld=frameCount;
		FadePalFromBlack(pTempPal, pPalette, 64 - (frameOld >> 1));
		RemapPal(pTempPal, 0, 256);
		DrawPic(StartX, StartY, dX, dY, (ZoomSide) ? Zoom : -Zoom);
		while(frameCount == frameOld);
	}

	FadePalFromBlack(pTempPal, pPalette, 0);
	RemapPal(pTempPal, 0, 256);

	tmrSyncScr(tmrSync200, preVR, NULL, NULL);

	Free(pPicture);
	Free(pPalette);
	Free(pTrig);
	Free(pSlimeTable);
}

static void inVR(void) {
	SlimePos=(count * 2) & 511;
	ang=(count & 511);

	StartX=(pTrig[ang + 128] << 7) + (128L << 16);
	StartY=(pTrig[ang] << 7) + (100L << 16);
	dX=pTrig[ang + 128];
	dY=pTrig[ang];

	//if((pMpInfo->row !=PatternOldRow) && (pMpInfo->row & 7) == 0)
	if(pMpInfo->chans[1].volumebar > 100)
	{
		ZoomSide^=1;
		//PatternOldRow=pMpInfo->row;
	}

	switch(pos)	{
		case 0:
			Zoom=pTrig[ang] + 20000L;

			FadePalFromBlack(pTempPal, pPalette, ang >> 1);
			RemapPal(pTempPal, 0, 256);
			if(ang >= 128)
				pos=1;
			break;

		case 1:
			Zoom=65536L + (pTrig[ang + 128] >> 1) + 20000L;

			if((pMpInfo->pos >= 7) && (pMpInfo->row >= 28))
				pos=255;

		#ifdef DEBUG
			if(kbhit())
				pos=255;
		#endif
			break;

		case 255:
			Zoom+=600L;
			break;
	}

	if(++count == 4096)
		count=0;

	pInfoUpdate();
}
