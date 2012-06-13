
#include <conio.h>

#include "main.h"
#include "vga.h"

volatile long frameCount;
uword vgaStart;
ushort tmrSync200;

#include "globals.h"

void vgaInit(void)
{
	frameCount=0;
	vgaStart=0;
}

void TimerSync(void)
{
	asm mov ax, 13h
	asm int 10h

	tmrGetScrSync(&tmrSync200);

	asm mov ax, 3h
	asm int 10h
}

/*
void frameWait(int count)
{
	ulong frameOldCount=frameCount;
	while(frameCount < (frameOldCount + count));
}
*/

/*
void frameWaitOne(void)
{
	ulong frameOldCount=frameCount;
	while(frameCount == frameOldCount);
}
*/

/*
void preVR(void)
{
	outpw(0x3d4, 0xc | (vgaStart & 0xFF00U));
	outpw(0x3d4, 0xd | (vgaStart << 8));
	frameCount++;
}
*/

/*
void vgaDupeScan(uword val)
{
	outp(0x3d4, 9);
	outp(0x3d5,(inp(0x3d5) & (255 ^ 31)) | val);
}
*/

/*
void FadePalFromBlack(char *pDst, char *pSrc, int Pos)
{
	int i;
	if(Pos > 64)
		Pos=64;
	else if(Pos < 0)
		Pos=0;

	for(i=0; i < 768; i++)
		pDst[i]=(pSrc[i] * Pos) >> 6;
}
*/

/*
void MakeBlackToWhitePal(char *pPalette, int pos)
{
	int i, val=(63 * pos) >> 6;
	for(i=0; i < 768; i++)
		pPalette[i]=val;
}
*/

/*
void FadePalFromWhite(char *pDst, char *pSrc, int Pos)
{
	int i, aPos=63 * (64 - Pos);
	for(i=0; i < 768; i++)
		pDst[i]=(aPos + pSrc[i] * Pos) >> 6;
}
*/