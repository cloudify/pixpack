
#include <stdio.h>
#include <math.h>
#include <conio.h>

typedef unsigned char	uchar;
typedef unsigned int 	uint;
typedef unsigned long	ulong;

#define _border(x)	{ outp( 0x3c0, 32 | 0x11 ); outp( 0x3c0, x ); }

uchar	far Palette[ 768 ];
uchar	far	Picture[ 256U*100U ];
uchar	far Plasma[ 256U*100U ];
uchar	far	Radial[ 64000U ];

void	MakePlasma	( uint x1, uint y1, uint x2, uint y2 );
void	MakeDeform	( void );

void	main	( void )
{
	FILE	*stream;
	int		x, y;
	double	ang;

	printf("\n - DeFoRmEr - CoDe: PiX - (c) 1995 NeMeSiS Production\n" );

	for( y=0; y<200; y++ )
	{
		for( x=0; x<320; x++ )
		{
			double	r;

			r= (double)( x-160 )*( x-160 ) + ((double)( y-100 )*( y-100 ))*2;
			r= ( sin( sqrt( r ) * 6.0 * 6.28 / 190.0 ) + 1 ) * (15.0/2.0);
			Radial[ 320U*y + x ]= (uchar)r;
		}

		if( ( y & 7 ) == 0 )
		{
			printf(" þ Radial map %u%%...\r", y>>1 );
		}
	}

	printf("\n þ Loading picture...");

	stream=fopen( "DEFORM.RAW", "rb" );
	fread( Palette, 32L, 1, stream );
	fread( Palette, 768L, 1, stream );
	fread( Picture, 256L*100L, 1, stream );
	fclose( stream );

	_asm	mov		ax, 13h
	_asm	int		10h

	outp( 0x3d4, 9 );
	outp( 0x3d5, ( inp( 0x3d5 ) & (255^31) ) | 3 );

	outp( 0x3c8, 0 );
	for( x=0; x<768; x++ )
		outp( 0x3c9, Palette[ x ]>>2 );

	x=y=0;
	ang=0;

	while( !kbhit() )
	{
		MakePlasma( 32+30*sin( ang ),  50+40*cos( ang*1.4 ),
					32+30*sin( ang*1.8 ),  50+40*cos( ang*2.2 ) );

		ang+=.03;

		while( !( inp( 0x3da ) & 8 ) );
		_border( 20 );
		MakeDeform();
		_border( 100 );

	}

	getch();

	_asm	mov		ax, 3h
	_asm	int		10h

	printf("\n - DeFoRMeR - code/copyright 1995 pix/N!P\n" );
}

