/*
 * image warp
 * code by pix - fferoldi@mbox.vol.it - federico@logicom.it
 */

#include <stdio.h>
#include <malloc.h>
#include <conio.h>
#include <dos.h>
#include <fcntl.h>
#include <io.h>

typedef unsigned char	uchar;
typedef unsigned int	uint;
typedef unsigned long	ulong;

uchar	Palette[768];
uchar	far *Picture[2];
uint	far *PlaneData[4];

uint	PalSeg, PicSeg[2], DataSeg;

int		main	( void )
{
	int		Handle;
	uchar	far *Video=(uchar far*)0xA0000000L;
	uint	x, pos, page, npage, plane, voff, pos2;

	if( ((Picture[0]=malloc(64001)) == NULL) ||
		((Picture[1]=malloc(64001)) == NULL) ||
		((PlaneData[0]=malloc(32000)) == NULL) ||
		((PlaneData[1]=malloc(32000)) == NULL) ||
		((PlaneData[2]=malloc(32000)) == NULL) ||
		((PlaneData[3]=malloc(32000)) == NULL) )
	{
		puts(" No mem!\n");
		return 1;
	}
	Picture[0][64000]=Picture[1][64000]=0;

	puts("WARP.DAT...");
	Handle=_open("WARP.DAT", O_BINARY );
	if(Handle == -1)
	{
		puts("WARP.DAT not found");
		return 2;
	}
	_read( Handle, PlaneData[0], 32000 );
	_read( Handle, PlaneData[1], 32000 );
	_read( Handle, PlaneData[2], 32000 );
	_read( Handle, PlaneData[3], 32000 );
	_close( Handle );

	puts("WARP.RAW...");
	Handle=_open("WARP.RAW", O_BINARY);
	if(Handle == -1)
	{
		puts("WARP.RAW not found");
		return 2;
	}

	_read(Handle, Palette, 32);
	_read(Handle, Palette, 768);
	_read(Handle, Picture[0], 64000);
	_close(Handle);

	//puts("Press a key...");
	//getch();

	_asm {
		mov	ax, 13h
		int	10h
		}

	outp(0x3c4, 4);
	outp(0x3c5, inp(0x3c5) & 0xf7);

	outp(0x3c8, 0);
	for( x=0; x<768; x++ )
		outp( 0x3c9, Palette[x]>>2 );

	page=0;
	npage=1;
	voff=0;

	do
	{
		outpw(0x3d4,0xc|(voff&0xff00));
		outpw(0x3d4,0xd|(voff<<8));
		while( !( inp( 0x3da ) & 8 ) );

		voff^=16000;

		for(plane=0; plane<4; plane++)
		{
			outp(0x3c4,2);
			outp(0x3c5,1<<plane);
			pos2=plane;
			for( pos=0; pos<16000; pos++ )
			{
				Video[pos+voff]=
				Picture[npage][pos2]=
				Picture[page][ PlaneData[plane][pos] ];
				pos2+=4;
			}
		}
		page^=1;
		npage^=1;

	}
	while( !kbhit() );

	_asm {
		mov	ax, 3h
		int	10h
		}

	printf(" - WARPed - code/copyright 1994 pix/nemesis\n");

	return 0;
}
