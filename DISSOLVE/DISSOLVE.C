
#include <math.h>
#include <stdio.h>
#include <io.h>
#include <conio.h>

typedef unsigned char byte;

byte Palette[768];
byte huge Mix[256][256];
byte far Image[64000];

int SearchColor(int r, int g, int b);
void MakeMix(void);

void main(void)
{
	int hFile, i;
	unsigned x, y;
	byte far *Video=(byte far *)0xA0000000L;

	printf(" - loading image...\n");

	hFile=_open("liquid.raw", 0);
	_read(hFile, Palette, 32);
	_read(hFile, Palette, 768);
	_read(hFile, Image, 64000);
	_close(hFile);

	asm mov ax, 13h
	asm int 10h

	outp(0x3c8, 0);
	for(i=0; i < 768; i++)
		outp(0x3c9, Palette[i]>>=2);

	MakeMix();

	while(kbhit()) getch();

	for(x=0; x < 64000; x++)
			Video[x]=Image[x];

	getch();

	while(!kbhit())
	{
		for(x=320; x < 64000 - 320; x++)
			Video[x]=Image[x]=Mix[Image[x]][Image[x + 1]];
	}

	while(kbhit()) getch();

	getch();

	asm mov ax, 3h
	asm int 10h
}

int SearchColor(int r, int g, int b)
{
	int bestC=0, C, I;
	int bestD=32767, D, Dr, Dg, Db;

	for(C=0; C < 256; C++)
	{
		I=C * 3;
		//D=sqrt(pow(r - Palette[I], 2) + pow(g - Palette[I + 1], 2) + pow(b - Palette[I + 3], 2));
		Dr=r - Palette[I];
		Dg=g - Palette[I + 1];
		Db=b - Palette[I + 2];
		//D=Dr*Dr + Dg*Dg + Db*Db;
		D=abs(Dr)+abs(Dg)+abs(Db);

		if(D < bestD)
		{
			bestD=D;
			bestC=C;
		}
	}
	return bestC;
}

void MakeMix(void)
{
	int c1, c2, i1, i2;
	byte far *Video=(byte far *)0xA0000000L;

	for(i1=0; i1 < 256; i1++)
		Video[i1]=i1;

	for(c1=0; (c1 < 256) && (!kbhit()); c1++)
	{
		//printf(" - making mixtable row %d (%d%%)...      \r", c1, (c1 * 100) / 256);
		Video[c1 + 320] = 255;
		i1=c1 * 3;
		for(c2=0; c2 <= c1; c2++)
		{
			i2=c2 * 3;
			//Video[320*c1 + c2]=
			Mix[c2][c1]=Mix[c1][c2]=SearchColor(	(int)((double)(Palette[i1] + Palette[i2]) / 2.1),
													(int)((double)(Palette[i1 + 1] + Palette[i2 + 1]) / 2.1),
													(int)((double)(Palette[i1 + 2] + Palette[i2 + 2]) / 2.1)
												);


		}
	}
}