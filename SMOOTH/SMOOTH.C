
#include <dos.h>
#include <io.h>
#include <fcntl.h>
#include <conio.h>

#include "smooth.h"

int bLoadImage(void);
void ColorAdjust(void);
void CalcContour(void);
void CalcSmooth(uint pos);
void Draw(void);

uchar acPalette[768];
uchar acImage[_IMG_SIZE];
uchar acContour[_IMG_SIZE];
uchar acSmooth[_IMG_SIZE];

void main(void)
{
	int iPos=0;

	if(!bLoadImage())
		return;

	ColorAdjust();
	CalcContour();

	_asm mov ax,13h
	_asm int 10h

	outp(0x3c8, 0);
	for(iPos=0; iPos < 768; iPos++)
		outp(0x3c9, acPalette[iPos]);

	iPos=0;

	while(!kbhit())
	{
		for(iPos=0; (iPos < 128) && !kbhit(); iPos++)
		{
			CalcSmooth(iPos);
			Draw();
		}

		for(iPos=128; (iPos > 0) && !kbhit(); iPos--)
		{
			CalcSmooth(iPos);
			Draw();
		}
	}

	getch();

	_asm mov ax, 3h
	_asm int 10h

}

int bLoadImage(void)
{
	int hFile=_open("nemesis.raw", O_RDONLY);
	if(hFile == -1)
		return 0;
	_read(hFile, acPalette, 32);
	_read(hFile, acPalette, 768);
	_read(hFile, acImage, _IMG_SIZE);
	_close(hFile);
	return -1;
}

void ColorAdjust(void)
{
	uint i;

	for(i=0; i < 768; i++)
		acPalette[i]>>=2;

	for(i=0; i < 64; i++)
	{
		uint uPalOff=i * 3;
		acPalette[uPalOff]=acPalette[uPalOff + 192 * 3];
		acPalette[uPalOff + 1]=acPalette[uPalOff + 192 * 3 + 1];
		acPalette[uPalOff + 2]=acPalette[uPalOff + 192 * 3 + 2];
	}
	for(i=64; i < 256; i++)
	{
		uint uPalOff=i * 3;
		acPalette[uPalOff]=acPalette[63 * 3];
		acPalette[uPalOff + 1]=acPalette[63 * 3 + 1];
		acPalette[uPalOff + 2]=acPalette[63 * 3 + 2];
	}
	for(i=0; i < _IMG_SIZE; i++)
	{
		acImage[i]=(acImage[i] >= 192) ? (acImage[i] - 192) : 0;
	}
}

#define _Image(x, y) acImage[(x) + _IMG_WIDTH * (y)]
#define _D 6

uint Median(int cx, int cy, int d)
{
	uint Sum=0, Count=0;
	int x, y;
	for(y=cy - d; y <= cy + d; y++)
		for(x=cx - d; x <= cx + d; x++)
		{
			Sum+=_Image(x, y);
			Count++;
		}
	return Sum/Count;
}

void CalcContour(void)
{
	uint x, y, col;

	for(y=0; y < _IMG_HEIGHT; y++)
	{
		for(x=0; x < _IMG_WIDTH; x++)
		{
			if((x < _D) || (y < _D) || (x >= (_IMG_WIDTH - _D)) || (y >= (_IMG_HEIGHT - _D)))
				col=0;
			else col=Median(x, y, _D);

			acContour[x + _IMG_WIDTH * y]=col;
		}
	}
}

void CalcSmooth(uint pos)
{
	uint i, npos=128 - pos;

	for(i=0; i < _IMG_SIZE; i++)
	{
		acSmooth[i]=((uint)acContour[i] * pos + (uint)acImage[i] * npos) >> 7;
	}
}

#define _VID_GAP (320 - _IMG_WIDTH);

void Draw(void)
{
	static uchar _seg* pcVideo=(uchar _seg*)0xA000;
	uint x, y, uOff=0, vOff=0;

	while(!(inp(0x3da) & 8));

	for(y=0; y < _IMG_HEIGHT; y++)
	{
		for(x=0; x < _IMG_WIDTH; x++)
			pcVideo[vOff++]=acSmooth[uOff++];
		vOff+=_VID_GAP;
	}
}