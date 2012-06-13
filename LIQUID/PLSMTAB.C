
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>

void CalcTab1(void);
void CalcTab2(void);
void MakePal(void);

#define box_w 160
#define box_h 100
#define pi 3.1415926L

unsigned char	palette[768];
unsigned char 	far pTable1[64000];
unsigned char 	far pTable2[64000];
unsigned char	far pMask[32000];
int		TrigTable[320];

unsigned int	circle1=0, circle2=0, circle3=0, circle4=0;
unsigned int	circle5=0, circle6=0, circle7=0, circle8=0;
int 	x1, y1, x2, y2, x3, y3, x4, y4;

void main(void)
{
	int i;
	FILE *file;

	printf(" - trig table: making...");
	for(i=0; i<320; i++)
		TrigTable[i] = (int)(256.0 * sin(((double)i * 3.14159) / 128.0));
	printf(" saving...\n");
	file=fopen("plasma.trg", "wb");
	fwrite(TrigTable, 320 * 2, 1, file);
	fclose(file);

	/*
	printf(" - palette: making...");
	MakePal();
	printf(" saving...\n");
	file=fopen("plasma.pal", "wb");
	fwrite(palette, 768, 1, file);
	fclose(file);
	*/

	printf(" - table 1st: making...");
	CalcTab1();
	printf(" saving...\n");
	file=fopen("plasma.tb1", "wb");
	fwrite(pTable1, 64000, 1, file);
	fclose(file);

	printf(" - table 2st: making...");
	CalcTab2();
	printf(" saving...\n");
	file=fopen("plasma.tb2", "wb");
	fwrite(pTable2, 64000, 1, file);
	fclose(file);

	printf(" - mask map: making...");
	file=fopen("img\\pixofn.raw", "rb");
	fread(pMask, 32 + 768, 1, file);
	fread(pMask, 32000, 1, file);
	fclose(file);

	for(i=0; i < 32000; i++)
		pMask[i]=(pMask[i] != 0) ? 128 : 0;

	printf(" saving...\n");
	file=fopen("plasma.msk", "wb");
	fwrite(pMask, 32000, 1, file);
	fclose(file);

	printf("\n");
}

void	CalcTab1	( void )
{
	unsigned int	i, j;

	for(i=0; i < (box_h * 2); i++)
	{
		for(j=0; j < (box_w * 2); j++)
			pTable1[(i * box_w * 2) + j]=((unsigned char)((sqrt(16.0 + (box_h - i) * (box_h-i) + (box_w - j) * (box_w - j)) - 4) * 5)) & 127;

	#ifdef DEBUG
		if( (i&15) == 0 )
			printf( " Processing 1st table: %u%%...  \r", (i*100)/(box_h*2) );
	#endif
	}
}

void 	CalcTab2	( void )
{
	unsigned int	i, j, tabpos=0;
	double 	temp;

	for(i=0; i < (box_h * 2); i++)
	{
		for(j=0; j < (box_w * 2); j++)
		{
			temp=sqrt(16.0 + (box_h - i) * (box_h - i) + (box_w - j) * (box_w - j)) - 4;
			pTable2[tabpos]=(sin(temp / 9.5) + 1) * 90;
			pTable2[tabpos]&= 127;
			tabpos++;
		}
	#ifdef DEBUG
		if( (i&15) == 0 )
			printf( " Processing 2nd table: %u%%...  \r", (i*100)/(box_h*2) );
	#endif
	}
}

#define mycol(u, a) ((TrigTable[(unsigned int)((u) + (a) + 64) & 255] + 256)) >> 3

void MakePal(void)
{
	int i=0;
	unsigned int r=(128 * 1) / 6, g=(128 * 3) / 6,	b=(128 * 5) / 6;

	for(i=0; i < 128; i++)
	{
		palette[i * 3 + 0]=mycol(i * 2, r >> 3);
		palette[i * 3 + 1]=mycol(i * 2, g >> 4);
		palette[i * 3 + 2]=mycol(i * 2, b >> 5);
	}

	r+=4;
	g+=3;
	b+=5;

	for(i=0; i < 128; i++)
	{
		palette[i * 3 + 384]=mycol(i * 2, r >> 3);
		palette[i * 3 + 384]=mycol(i * 2, g >> 4);
		palette[i * 3 + 384]=mycol(i * 2, b >> 5);
	}

	for(i=0; i < 384; i++)
	{
		palette[i]>>=2;
		palette[i + 384]=(palette[i + 384] >> 1) + 32;
	}
}



