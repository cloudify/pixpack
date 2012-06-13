// - -- --=-- -- - //
// 3D BAR Scroller //
// - -- --=-- -- - //
// WiLd :) version //

// Copyright 1995 NeMeSiS production
// Code: pix - fferoldi@mbox.vol.it - federico@logicom.it
// Note: Low optimized! :) It's just something (e` gia` qualcosa... :))

//uncomment this for a "fps" benchmark
#define BENCH

//uncomment this if you like the border_line
//#define BORDER

//BAR RADIUS
#define RAD	40

#include <math.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>

#define _border(x) {outp(0x3c0, 0x11 | 32); outp(0x3c0, x);}
typedef unsigned char uchar;

typedef struct
{
	int y, z;
	uchar a;
	uchar vnum;
}   sVertex;

#define PI 256
int Trig[( PI * 5 ) / 4];

#define _sin(x)	Trig[(x)&(PI-1)]
#define _cos(x)	Trig[((x)&(PI-1))+(PI/4)]

uchar AngList[640];
uchar Palette[768];
uchar Map[32 * 320];

uchar _seg *video = 0xA000;

int visible[8][3] = {{3, 0, 1}, {2, 3, 0}, {2, 3, 0}, {1, 2, 3},
{1, 2, 3}, {0, 1, 2}, {0, 1, 2}, {3, 0, 1}};

int barcol[4] = {1, 2, 1, 0};

void main( void )
{
	FILE *stream;
	long count, timer, scount = 0;
	int i, x, amul, mul;
	sVertex Vert[4];

	stream = fopen( "BARSRCL.RAW", "rb" );
	fread( Palette, 32, 1, stream );
	fread( Palette, 768, 1, stream );
	fread( Map, 32 * 320, 1, stream );
	fclose( stream );

	for ( i = 0; i < 32 * 320; i++ )
	{
		if ( Map[i] == 0xbf )
			Map[i] = 64;
		else
			Map[i] = 0;
	}

	for ( x = 0; x < 320; x++ )
	{
		for ( i = 0; i < 16; i++ )
		{
			uchar t = Map[32 * x + i];

			Map[32 * x + i] = Map[32 * x + 31 - i];
			Map[32 * x + 31 - i] = t;
		}
	}

	// Trig table
	for ( i = 0; i < PI + PI / 4; i++ )
	{
		Trig[i] = ( int ) ( 255.0 * sin( ( i * 6.283185307 ) / ( double ) PI ) );
	}

	asm mov ax, 0x13
	asm int 0x10

	outp( 0x3c8, 0 );
	for ( i = 0; i < 64; i++ )
	{
		outp( 0x3c9, 0 );
		outp( 0x3c9, i );
		outp( 0x3c9, 0 );
	}

	for ( i = 0; i < 64; i++ )
	{
		outp( 0x3c9, i );
		outp( 0x3c9, i );
		outp( 0x3c9, i );
	}

	for ( i = 0; i < 64; i++ )
	{
		outp( 0x3c9, i );
		outp( 0x3c9, 0 );
		outp( 0x3c9, 0 );
	}

	for ( i = 0; i < 64; i++ )
	{
		outp( 0x3c9, i );
		outp( 0x3c9, i );
		outp( 0x3c9, i );
	}

	/* for ( i = 0; i < 768; i++ ) outp( 0x3c9, Palette[i] >> 2 ); */

	for ( i = 0; i < 320; i++ )
		AngList[i] = 32;

	amul = 0;
	count = 0;
	timer = clock(  );
	while ( !kbhit(  ) )
	{
		mul = _sin( amul );
		amul++;

		if ( scount < 64 * 1 )
		{
		} else if ( scount < 256 * 2 )
		{
			for ( i = 0; i < 320; i++ )
				AngList[i] = 32 + (amul << 1);
		} else if ( scount < 256 * 3 )
		{
			for ( i = 0; i < 320; i++ )
				AngList[i] = amul * 2 + ( ( 16 * _cos( amul ) ) >> 8 ) + ( ( ( ( 40 * _sin( ( i >> 2 ) + ( amul << 3 ) ) ) >> 8 ) * _cos( mul >> 1 ) ) >> 8 );
		} else if ( scount < 256 * 4 )
		{
			for ( i = 0; i < 320; i++ )
				AngList[i] = amul * 2 + ( ( 16 * _cos( amul ) ) >> 8 ) + ( ( ( ( 64 * _sin( ( i ) + ( amul << 3 ) ) ) >> 8 ) * _cos( mul >> 1 ) ) >> 8 );
		} else if ( scount < 256 * 5 )
		{
			for ( i = 0; i < 320; i++ )
				AngList[i] = amul * 2 - i - ( ( 40 * _sin( scount + i ) ) >> 8 );
		} else
			scount = 256 * 2;

#ifndef BENCH
		while ( !( inp( 0x3da ) & 8 ) );
#endif

#ifdef BORDER
		_border( 32 );
#endif

		for ( x = 0; x < 320; x++ )
		{
			int py, c1, c2, c3, c4, dc1, dc2, dh1, dh2;
			int pm, dm1, dm2, om;
			int vs;
			unsigned int voff;
			sVertex t;

			vs = AngList[x] >> 5;

			om = ( ( x + ( amul << 1 ) ) % 320 ) << 5;

			voff = x + 320 * ( 199 - RAD * 2 );

#define _setvert(c, col)	{ 	Vert[c].a=AngList[x] + (c << 6); \
						Vert[c].y=((RAD * _sin(Vert[c].a)) >> 8); \
						Vert[c].z=((RAD * _cos(Vert[c].a)) >> 8); \
						Vert[c].vnum=64*col; \
						}

			_setvert( visible[vs][0], barcol[visible[vs][0]] );
			_setvert( visible[vs][1], barcol[visible[vs][1]] );
			_setvert( visible[vs][2], barcol[visible[vs][2]] );

#define ZRATE	20
			c1 = ( Vert[visible[vs][0]].vnum + ( ZRATE * 63 ) / ( RAD - Vert[visible[vs][0]].z + ZRATE ) ) << 8;
			c2 = ( Vert[visible[vs][0]].vnum + ( ZRATE * 63 ) / ( RAD - Vert[visible[vs][1]].z + ZRATE ) ) << 8;
			c3 = ( Vert[visible[vs][1]].vnum + ( ZRATE * 63 ) / ( RAD - Vert[visible[vs][1]].z + ZRATE ) ) << 8;
			c4 = ( Vert[visible[vs][1]].vnum + ( ZRATE * 63 ) / ( RAD - Vert[visible[vs][2]].z + ZRATE ) ) << 8;

			if ( ( dh1 = ( Vert[visible[vs][1]].y - Vert[visible[vs][0]].y ) ) != 0 )
			{
				dm1 = ( 32 << 8 ) / dh1;
				dc1 = ( c2 - c1 ) / dh1;
			} else
				dm1 = dc1 = 0;

			if ( ( dh2 = ( Vert[visible[vs][2]].y - Vert[visible[vs][1]].y ) ) != 0 )
			{
				dm2 = ( 32 << 8 ) / dh2;
				dc2 = ( c4 - c3 ) / dh2;
			} else
				dm2 = dc2 = 0;

			for ( py = -RAD; py < Vert[visible[vs][0]].y; py++ )
			{
				video[voff] = 0;
				voff += 320;
			}

			pm = 0;

			for ( ; py < Vert[visible[vs][1]].y; py++ )
			{
				video[voff] = ( c1 >> 8 ) + Map[( pm >> 8 ) + om];
				c1 += dc1;
				pm += dm1;
				voff += 320;
			}

			pm = 0;

			for ( ; py < Vert[visible[vs][2]].y; py++ )
			{
				video[voff] = ( c3 >> 8 ) + Map[( pm >> 8 ) + om];
				c3 += dc2;
				pm += dm2;
				voff += 320;
			}

			for ( ; py < RAD; py++ )
			{
				video[voff] = 0;
				voff += 320;
			}
		}
		count++;
		scount++;

#ifdef BORDER
		_border( 0 );
#endif

	}
	timer = clock(  ) - timer;

	getch(  );

	asm mov ax, 0x3
	asm int 0x10

	    printf( " -=[!3D-BAR! ScRoLLeR]=- Copyright 1995 NeMeSiS production\n"
				    " Code: Pix / NeMeSiS\n\n" );


#ifdef BENCH
	printf( " count: %ld frames\n"
			" timer: %.2lf sec\n"
			"   fps: %.2lf frames/sec\n",
			( long ) count,
			( double ) timer / CLK_TCK,
			( double ) count * CLK_TCK / timer );
#endif
}
