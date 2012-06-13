/*
 * slime rotozoom
 * code by pix - fferoldi@mbox.vol.it -federico@logicom.it
 */
 
#include <stdio.h>
#include <math.h>
#include <conio.h>
#include <time.h>

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

void DrawPic(long StartX, long StartY, long dX, long dY, long Zoom);

uchar Picture[256U * 128U];
uchar Palette[768];

#define MAXTRIG	(512 + 200 + 128)

long Trig[MAXTRIG];
long SlimeTable[MAXTRIG];
int SlimePos=0;

void main(void)
{
	int i;
	FILE* Stream;
	int ang=0, count=0, ang2;
	long timer, frames;

	Stream=fopen("SLIME.RAW", "rb");

	if(Stream == NULL)
		return;

	fread(Palette, 32, 1, Stream);
	fread(Palette, 768, 1, Stream);
	for(i=0; i<128; i++)
	{
		fread(&Picture[256L*i], 256L, 1, Stream);
	}
	fclose(Stream);

	for(i=0; i<MAXTRIG; i++)
	{
		Trig[i]=(long)(65536.0 * sin(3.14159 * 2.0 * i / 512.0));
	}

	for(i=0; i<MAXTRIG; i++)
	{
		SlimeTable[i]=(Trig[(i * 2) & 511]) / 2;
	}

	asm mov ax, 13h
	asm int 10h

	outp(0x3c8, 0);
	for(i=0; i<768; i++)
	{
		outp(0x3c9, Palette[i] >> 2);
	}

	count=0;

	frames=0;
	timer=clock();

	while(!kbhit())
	{
		SlimePos= (count*2) & 511;
		ang=count & 511;
		DrawPic(Trig[ang + 128] << 7, Trig[ang] << 7,
				Trig[ang + 128], Trig[ang],
				Trig[ang + 64] + 65536L + 40000L);
		if(++count == 1024)
			count=0;
		frames++;
	}

	timer=clock() - timer;

	getch();

	asm mov ax, 3h
	asm int 10h

	printf(	" .oO TeXtUrE-SLiME Oo. Copyright 1995 NeMeSiS Production - CoDe: PiX\n\n");

	printf( "%lf sec/frame\n"
			"%lf frames/sec\n",
			(double)timer/(CLK_TCK * frames),
			(double)(CLK_TCK * frames)/timer);
}

