
#include <stdio.h>
#include <malloc.h>
#include <conio.h>
#include <math.h>
#include <memory.h>
#include <stdlib.h>

typedef unsigned char uchar;
typedef unsigned int uint;

extern void Calculate(unsigned npage, unsigned density);
extern void Draw(unsigned hpage);

int far *Height[2];

uchar	far* video=(uchar far*)0xA0000000L;

uchar back[160*100];

int main(void)
{
	int	x, y, z;
	int	hpage=0;
	double ang=0;
	int key;
	int mode=0;
	int density=4, pheight=400, radius=5;
	FILE *fp;

	printf("\n - FLUiD - NeMeSiS production\n");

	Height[0]=malloc(32000L);
	Height[1]=malloc(32000L);

	if(	(Height[0] == NULL) ||
		(Height[1] == NULL)
		)
	{
		printf(" No mem!\n");
		return 1;
	}

	memset(Height[0], 0, 32000);
	memset(Height[1], 0, 32000);

	printf(	" code & copyright pix/NeMeSiS\n\n"
			" c = clear                 q/w = -/+ liquid density\n"
			" a/s = -/+ blup height     z/x = -/+ blup radius\n"
			" 1 = surfer                2 = rain\n"
			" 3 = quite                 4 = blast\n"
			" 5 = quad blup             6 = circ blup\n"
			" 7 = blast2                8 = blast3\n"
			"\n - press a key -\n");
	getch();

	_asm mov	ax, 13h
	_asm int	10h

	outp(0x3c8,0);

	for(x=0;x<256;x++) {
		outp(0x3c9,63 - (255 - x)/4);
		outp(0x3c9,63 - (255 - x)/4);
		outp(0x3c9,63 - (255 - x)/8);
	}

	memset((void far*)0xA0000000L, 128, 64000);

	fp=fopen("CHESSP.RAW", "rb");
	fseek(fp, 768 + 32, 0);
	fread(back, 160*100, 1, fp);
	fclose(fp);

	z=0;
do
{
	int	ccl, chg, posy;

	if( kbhit() )
		switch(key=getch())
		{
			case 'c':
				memset(Height[0], 0, 32000);
				memset(Height[1], 0, 32000);
				break;
			case 'q':
				density--;
				break;
			case 'w':
				density++;
				break;
			case 'a':
				pheight-=20;
				break;
			case 's':
				pheight+=20;
				break;
			case 'z':
				radius--;
				break;
			case 'x':
				radius++;
				break;
			case '4':
				{
					int x, y, v=0;
					FILE *file;
					file=fopen("pix.raw", "rb");
					fseek(file, 768+32, 0);
					for(y=0; y < 100; y++) {
						for(x=0; x < 160; x++) {
							fread(&v, 1, 1, file);
							Height[hpage][100*x+y]+=v;
						}
					}
					fclose(file);
				}
				break;

			case '5':
				{
					int px=(1+(rand()%(159-10)));
					int py=(1+(rand()%(99-10)));
					int cx, cy;
					for(cy=py; cy<(py+10); cy++)
						for(cx=px;cx<(px+10);cx++)
							Height[hpage][100*cx+cy]=pheight;
				}
				break;
			case '6':
				{
					int rquad=radius*radius;
					int px=(2+radius+(rand()%(154-2*radius)));
					int py=(2+radius+(rand()%(94-2*radius)));
					int cx, cy, cyq;
					for(cy=-radius; cy<radius; cy++) {
						for(cx=-radius;cx<radius;cx++) {
							cyq=cy*cy;
							if((cx*cx + cyq) < rquad)
								Height[hpage][100*(cx+px)+(cy+py)]=pheight;
						}
					}
				}
				break;
			case '7':
				{
					int rquad=radius*radius;
					int px=(2+radius+(rand()%(154-2*radius)));
					int py=(2+radius+(rand()%(94-2*radius)));
					int cx, cy, cyq;
					for(cy=-radius; cy<radius; cy++) {
						for(cx=-radius;cx<radius;cx++) {
							cyq=cy*cy;
							if( ((cx*cx + cyq) <= rquad) && ((cx*cx + cyq) >= rquad-1) )
								Height[hpage][100*(cx+px)+(cy+py)]=pheight;
						}
					}
				}
				break;
			case '8':
				{
					int o;
					for(o=0; o < 160*100; o++)
						Height[hpage][o++]+=Height[hpage][o] + rand()%101 - 50;
				}
				break;
			default:
				mode=key-'0';

		};

	/*
	for(y=0;y<100;y++) {
		video=(uchar far*)(0xA0000000L+320L*y);
		for(x=0;x<159;x++) {
			int o=100*x+y;
			int ox=(Height[hpage][o] - Height[hpage][o + 100]);
			int oy=(Height[hpage][o] - Height[hpage][o + 1]);
			int c=128 - ox;
			if(c < 0)
				c=0;
			else if(c > 255)
				c=255;
			*(video++)=(back[100*((ox >> 3) + x) + ((oy >> 3) + y)] * c) >> 8;
		}
	}
	*/
	Draw(hpage);

	/*
	for(x=0;x<160;x++)
	{
		video=(uchar far*)(0xA0000000L+320L*199+x);
		posy=0;

		for(y=0;y<100;y++)
		{
			chg=(int)y+((Height[hpage][100*x + y])>>3);
			//ccl=(Height[hpage][100*x + y]>>3)+128;
			ccl=y;
			while( (chg > posy) && (posy < 200)) {
				*video=ccl;
				video-=320;
				posy++;
			}
		}
		while(posy<200)
		{
			*video=0;
			video-=320;
			posy++;
		}
	}
	*/

	switch(mode)
	{
		case 1:
			x=(int)( 80 + 50.0*cos(ang));
			y=(int)( 50 + 40.0*sin(ang*1.2+1));
			ang+=0.05;
			Height[hpage][100*x + y]=pheight;
			Height[hpage][100*(x-1) + y]=
			Height[hpage][100*(x+1) + y]=
			Height[hpage][100*x + y+100]=
			Height[hpage][100*x + y-100]=pheight >> 1;
			break;
		case 2:
			{
				int x=1 + rand()%(160-2);
				int y=1 + rand()%(100-2);
				Height[hpage][100*x + y]=rand()%pheight;
			}
			break;
	}


	{
		int		newh;
		int		npage=hpage^1;
		uint	count;

		/*
		count=101;
		for(x=1; x<159; x++) {
			for(y=1; y < 99; y++) {
				Height[npage][count]=((Height[hpage][count + 100] +
										Height[hpage][count - 100] +
										Height[hpage][count + 1] +
										Height[hpage][count - 1] ) >> 1) -
										Height[npage][count];
				Height[npage][count]=Height[npage][count] - (Height[npage][count] >> density);
				count++;
			}
			count+=2;
		}
		*/
		Calculate(npage, density);
	}
	hpage^=1;
}
while(key != 27);

	_asm mov	ax, 3h
	_asm int	10h

	return 0;
}