
#include <stdio.h>
#include <math.h>
#include <conio.h>
#include <time.h>

#include "slime.h"

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

uchar Picture[256U * 128U];
uchar Palette[768];

long Trig[MAXTRIG];
long SlimeTable[MAXTRIG];
int SlimePos=0;

void main(void)
{
	int i;
	FILE* Stream;
	int ang=0, count=0, ang2;
	long timer, frames;

	printf(" - image: loading...");
	Stream=fopen("img\\SLIME.RAW", "rb");
	if(Stream == NULL)
		return;
	fread(Palette, 32, 1, Stream);
	fread(Palette, 768, 1, Stream);
	for(i=0; i < 768; i++)
		Palette[i]>>=2;
	fread(Picture, 256L*128L, 1, Stream);
	fclose(Stream);

	printf(" saving...\n");
	Stream=fopen("slime.pal", "wb");
	fwrite(Palette, 768, 1, Stream);
	fclose(Stream);
	Stream=fopen("slime.bin", "wb");
	fwrite(Picture, 256L*128L, 1, Stream);
	fclose(Stream);

	printf(" - misc tables: making...");

	for(i=0; i<MAXTRIG; i++)
		Trig[i]=(long)(65536.0 * sin(3.14159 * 2.0 * i / 512.0));

	for(i=0; i<MAXTRIG; i++)
		SlimeTable[i]=(Trig[(i * 2) & 511]) / 2;

	printf(" saving...\n");
	Stream=fopen("slime.trg", "wb");
	fwrite(Trig, MAXTRIG * sizeof(long), 1, Stream);
	fclose(Stream);
	Stream=fopen("slime.tab", "wb");
	fwrite(SlimeTable, MAXTRIG * sizeof(long), 1, Stream);
	fclose(Stream);
}

