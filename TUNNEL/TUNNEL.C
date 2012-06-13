/*
 * plasma tunnel
 * code by pix - fferoldi@mbox.vol.it - federico@logicom.it
 */
 
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <math.h>
#include <alloc.h>
#include <mem.h>
#include <time.h>
#include <io.h>

#include "tunnel.h"

#define _pAlignPara(ptr) ((void*)(((unsigned long)ptr & 0xffff0000L) + (((unsigned long)ptr & 0xfff0L) << 12)+ (((unsigned long)ptr & 0xfL) ? 0x10000L : 0)))

void Fire(void);
void BFire(void);
void CalcRotMap(int h);
void CalcAvTable(void);

extern ubyte far RotList[256];
extern ubyte far AvTable[256];

ubyte ubLine[256];
uword *pOffsetMap, *pAOffsetMap;
ubyte *pTexture, *pATexture;
int Trig[1024];

void main(void)
{
	ubyte far *pVideo=(ubyte far *)0xA0000000L;
	int i, y;
	uword off;
	long time, frames;

	pOffsetMap=farmalloc(160 * 100 * 2 + 16);
	pAOffsetMap=_pAlignPara(pOffsetMap);

	printf(" - OffsetMap: %Fp -> %Fp\n", pOffsetMap, pAOffsetMap);

	pTexture=farmalloc(256L * 256L + 16L);
	pATexture=_pAlignPara(pTexture);

	printf(" - Texture: %Fp -> %Fp\n", pTexture, pATexture);

	MakeOffsetMap();
	CalcAvTable();

	for(i=0; i < 768; i++)
		Trig[i]=(int)((double)127.0 * sin((i * 3.14159)/128.0));

	for(i=0; i < 256; i++)
		ubLine[i]=rand() & 255;

	_fmemset(&pATexture[0], 0, 256L*128L);
	_fmemset(&pATexture[256L*128L], 0, 256L*128L);

	//for(y=0; y < 256; y++) MakeLine(&pATexture[256U * y]);
	//for(y=0; y < 256; y++) pATexture[200*256L + y/2]=pATexture[201*256L + y]=pATexture[202*256L + y]=255;
	//for(y=0; y < 256; y++) pATexture[256L*y]=pATexture[256L*y + 128]=255;

	asm mov ax, 13h
	asm int 10h

	outp(0x3d4, 9);	outp(0x3d5, inp(0x3d5) + 2);

	SetPalette();

	time=clock();
	frames=0;
	y=0;
	while(!kbhit())
	{
		//while(!(inp(0x3da)&8));
		CalcRotMap(Trig[(y) & 255]);
		Fire();
		DrawScreen(0, 0);

		// for(y=0; y < 200; y++) _fmemcpy((void far *)(0xA0000000L + 320L*y), &pATexture[256L*y], 256);
		// while((inp(0x3da)&8));
		y+=2;
		frames++;
	}
	getch();
	/*
	while(!kbhit())
	{
		CalcRotMap(Trig[(y) & 255]);
		BFire();
		DrawScreen(0, 0);
		y+=2;
		frames++;
	}
	*/

	time=clock() - time;

	//getch();

	asm mov ax, 3h
	asm int 10h

	farfree(pOffsetMap);
	farfree(pTexture);

	printf(" - TuNNeL / RaDiAL-FiRE - code/copyright 1995 pix/N!P\n");
	printf(" fps: %lf\n", (double)frames * CLK_TCK / time);
}

ubyte LogI[256];
ubyte Palette[768];

#define _OutDAC(r, g, b) {outp(0x3c9, r); outp(0x3c9, g); outp(0x3c9, b);}
#define _K 8

void SetPalette(void)
{
	int i;

	for(i=0; i < 256; i++)
	{
		double v;
		v=log(1 + i*(exp(_K)-1)/256)*256/_K;

		if(v > 255) v=255; else if(v < 0) v=0;

		LogI[i]=(unsigned char)v;
	}

	outp(0x3c8, 0);

	//for(i=0; i < 256; i++) _OutDAC(LogI[i << 1] >> 2, 0, 0);
	for(i=0; i < 32; i++)
		_OutDAC(i << 1, 0, 0);
	for(i=0; i < 32; i++)
		_OutDAC(63, i << 1, 0);

}

void DrawLine(int y)
{
	ubyte far *pVideo=(ubyte far *)0xA0000000L + 320L * y;
	int x;
	for(x=0; x < 256; x++)
		pVideo[x]=ubLine[x];
}

void MakeLine(ubyte *pBuffer)
{
	int x;
	for(x=0; x < 256; x++)
		pBuffer[x]=(pBuffer[x] + (rand() & 255)) >> 1;
		//pBuffer[x]=(pBuffer[x] + pBuffer[x + 1]) >> 2;
}

void MakeOffsetMap(void)
{
	double fMaxR=sqrt(80.0 * 80.0 + 50.0 * 50.0);
	int x, y;

	for(y=0; y < 100; y++)
	{
		if((y & 31) == 0)
			printf(" - making offset-map %d%%...    \r", y);
		for(x=0; x < 160; x++)
		{
			int xcent=x - 80;
			int ycent=y - 50;
			uword tx;
			double r;
			uword ty;

			r=sqrt((double)xcent * xcent + (double)ycent * ycent);
			r/=fMaxR + 10;

			r=pow((sin(r * 3.14159 / 2)*.8+.2), 0.35);

			ty=240 - (240.0 * r);

			if((xcent == 0) && (ycent == 0))
				xcent++;

			tx=(uword)((double)(128 + atan2(xcent, ycent) * 128.0 / 3.14));

			tx=tx & 255;
			ty=(ty & 255);

			pAOffsetMap[x + 160*y]=tx + 256*ty;
		}
	}
}

void Fire(void)
{
	int i;

	for(i=0; i < 200; i++)
	{
		pATexture[65535L - (rand() + rand())]=rand() & 63; //63;  // 128 + (inp(0x40) & 127);
	}
	for(i=0; i < 200; i++)
	{
		pATexture[65535L - rand()]=rand() & 31;
	}


	Smooth();
}

void BFire(void)
{
	int i;

	for(i=0; i < 200; i++)
		pATexture[65535L ^ (inp(0x40) * inp(0x40))]=0;

	Smooth();
}

int rot=0;

void CalcRotMap(int h)
{
	int i;

	for(i=0; i < 256; i++)
		RotList[i]=((int)(h * i) >> 7) + rot;
	rot++;
}

void CalcAvTable(void)
{
	int i, v;

	for(i=0; i < 256; i++)
	{
		v=(i + 1) >> 2;
		AvTable[i]=v;
	}
}
