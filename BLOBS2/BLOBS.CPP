/*
 * fast blobs with log tables
 * pix - fferoldi@mbox.vol.it - federico@logicom.it
 */

#include <stdlib.h>
#include <alloc.h>
#include <conio.h>
#include <stdio.h>
#include <math.h>

unsigned char far *LogTable;
unsigned char Palette[256*3];

void CalcLogTable(void);
void CalcPalette(void);

void main(void)
{
	LogTable=(unsigned char far *)malloc(64000U);
	if(LogTable == NULL)
		return;

	CalcLogTable();
	CalcPalette();

	asm mov ax,13h
	asm int 10h

	outp(0x3c8, 0);
	for(int i=0; i<768; i++)
		outp(0x3c9, Palette[i]);

	unsigned char _seg *VidMem=(unsigned char _seg *)0xA000U;

	for(i=0; i<256; i++)
		VidMem[i+320*190]=i;

#	define POINTS 4

	struct points {
		int x, y, sx, sy;
	} plist[POINTS];

#	define FIXED 4

	for(i=0; i < POINTS; i++) {
		plist[i].x=(rand() % 160) << FIXED;
		plist[i].y=(rand() % 100) << FIXED;
		plist[i].sx=(rand() % FIXED) - (FIXED / 2);
		plist[i].sy=(rand() % FIXED) - (FIXED / 2);
	}

	while(!kbhit())
	{
		while(!(inp(0x3da)&8));

		unsigned int I0;
		unsigned Ilist[POINTS];

		Ilist[0]=(plist[0].x >> FIXED) + 320*(plist[0].y >> FIXED);
		Ilist[1]=(plist[1].x >> FIXED) + 320*(plist[1].y >> FIXED);
		Ilist[2]=(plist[2].x >> FIXED) + 320*(plist[2].y >> FIXED);
		Ilist[3]=(plist[3].x >> FIXED) + 320*(plist[3].y >> FIXED);

		I0=0;

		for(int y=0; y < 100; y++)
		{
			for(int x=0; x < 160; x++) {
				VidMem[I0++]= (
					LogTable[Ilist[0]++] +
					LogTable[Ilist[1]++] +
					LogTable[Ilist[2]++] +
					LogTable[Ilist[3]++]
					) >> 2;
			}

			I0+=160;

			Ilist[0]+=160;
			Ilist[1]+=160;
			Ilist[2]+=160;
			Ilist[3]+=160;
		}


		for(i=0; i < POINTS; i++) {
			plist[i].x+=plist[i].sx;
			plist[i].y+=plist[i].sy;

			if(plist[i].x < 0) {
				plist[i].x=0;
				plist[i].sx=-plist[i].sx;
			} else if(plist[i].x > (159 << FIXED)) {
				plist[i].x=(159 << FIXED);
				plist[i].sx=-plist[i].sx;
			}
			if(plist[i].y < 0) {
				plist[i].y=0;
				plist[i].sy=-plist[i].sy;
			} else if(plist[i].y > (99 << FIXED)) {
				plist[i].y=(99 << FIXED);
				plist[i].sy=-plist[i].sy;
			}
		}

	}

	getch();

	asm mov ax,3h
	asm int 10h
}

#define A	51664.41661
#define LOGA	0.18467725

void CalcLogTable(void)
{
	unsigned int index=0;
	int x, y;
	double quadx, quady, quadsum;
	double logval;
	double k1=210.0 / log10(35600);

	for(y=-100; y<100; y++)
	{
		quady=((double)y * y);

		if((y & 15) == 0)
			printf(" > PRE.CALC[%u%%]...\r", (y + 100)/2);

		for(x=-160; x<160; x++)
		{
			quadsum=((double)x * x) + quady;
			if(quadsum > 0)
				logval=log10(35600/quadsum) * k1;
			else
				logval=255;

			if(logval > 255)
				logval=255;
			else if(logval < 0)
				logval=0;

			LogTable[index++]=(unsigned char)logval;
		}
	}
}

void CalcPalette(void)
{
	int index=0, color;

	for(color=0; color<64; color++)
	{
		Palette[index++]=0;
		Palette[index++]=0;
		Palette[index++]=0;
	}
	for(color=0; color<32; color++)
	{
		Palette[index++]=color*2;
		Palette[index++]=0;
		Palette[index++]=color*2;
	}
	for(color=0; color<32; color++)
	{
		Palette[index++]=63;
		Palette[index++]=color*2;
		Palette[index++]=63;
	}
	for(color=0; color<128; color++)
	{
		Palette[index++]=0;
		Palette[index++]=0;
		Palette[index++]=0;
	}

	/*
	for(color=0; color<256; color++)
	{
		#define BASE 1.019 //1.016380297
		double value=63 - pow(BASE, color);

		if(value < 0)
			value=0;
		else if(value > 63)
			value=63;

		unsigned char color=(unsigned char)value;
		Palette[index++]=color>>1;
		Palette[index+]=color;
		Palette[index+]=color;
	}
	*/
}
