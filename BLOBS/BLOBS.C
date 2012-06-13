/*
 PRJ: blobs experiments
CODE: pix - fferoldi@mbox.vol.it - federico@logicom.it
 MOD: blobs.c / main
*/

#include <conio.h>
#include <stdio.h>
#include <memory.h>

#include "blobs.h"

/* 768 (palette) + 768 (fade-buffer) */
uchar   Palette[ 768 * 2 ];

/* 2point coords */
point_  point[2];

void    main    ( void )
{
	int     x,y;
	uint    page=0;

	/* inizialize POINT[ 0 ] */

	point[0].x=80;
	point[0].y=50;

	point[0].sx=P0_MX;
	point[0].sy=P0_MY;
	point[0].ax=1;
	point[0].ay=1;

	/* ...and POINT[ 1 ] */

	point[1].x=80;
	point[1].y=50;

	point[1].sx=P1_MX;
	point[1].sy=P1_MY;
	point[1].ax=1;
	point[1].ay=1;

	/* Going in 320x200x256... */

	_asm    mov ax,13h
	_asm    int 10h

	/* 4x SCAN */

	outp( 0x3d4, 9 );
	outp( 0x3d5, ( inp( 0x3d5 ) & ( 255 ^ 31 ) ) | 3 );

	/* compute Palette */

	y=0;

	Palette[ y++ ]= Palette[ y++ ]= Palette[ y++ ]= 0;

	for(x=1;x<64;x++)
	{
		Palette[ y++ ]= ( uchar ) 63;
		Palette[ y++ ]= ( uchar ) 63 - x;
		Palette[ y++ ]= ( uchar ) 63 - x;
	}
	for(x=0;x<64;x++)
	{
		Palette[ y++ ]= ( uchar ) 63 - x;
		Palette[ y++ ]= ( uchar ) 0;
		Palette[ y++ ]= ( uchar ) x;
	}
	for(x=0;x<128;x++)
	{
		Palette[ y++ ]= ( uchar ) 0;
		Palette[ y++ ]= ( uchar ) 0;
		Palette[ y++ ]= ( uchar ) ( 63 - ( x >> 1 ) );
	}

	/* FADE-IN loop */

	x=0;
	page=0;

	while( x <= 64 )
	{
		Fade( ( uchar )x++ );
		while( !( inp( 0x3da ) & 8 ) );
		Remap();
		page^= 8000;
		do_blob(    ( page ) >> 2,
					point[0].x,
					point[0].y,
					point[1].x,
					point[1].y  );

		Move_points();
		video_index( page );
	}

	/* MAIN loop */

	while(!kbhit())
	{
		while( !( inp( 0x3da ) & 8 ) );

		do_blob(    ( page ^= 8000 ) >> 2,
					point[0].x,
					point[0].y,
					point[1].x,
					point[1].y  );

		outp( 0x3c0, 0x11 | 32 );
		outp( 0x3c0, 0 );


		Move_points();
		video_index( page );
	}

	/* FADE-OUT loop */

	x=32;

	while( x-- >= 0 )
	{
		for( y=0; y < 768; y++ )
			if( Palette[ 768 + y ] < 62 )
				Palette[ 768 + y ]+=2;
			else
				Palette[ 768 + y ]= 63;

		while( !( inp( 0x3da ) & 8 ) );

		Remap();

		do_blob(    ( page ^= 8000 ) >> 2,
					point[0].x,
					point[0].y,
					point[1].x,
					point[1].y  );

		Move_points();

		video_index( page );
	}

	Cut_out();

	getch();

	/* ..back in text */

	_asm    mov ax,3h
	_asm    int 10h

	puts( " - ReAL BLoBs - a very mini TeST - CoDeD by PiX / NeMeSiS " );
}

void    Move_points     ( void )
{
	/* Move POINT[0] */

	point[ 0 ].x+= point[ 0 ].sx;
	point[ 0 ].y+= point[ 0 ].sy;

	if( point[ 0 ].sx == P0_MX || point[ 0 ].sx == -P0_MX )
		point[ 0 ].ax = - point[ 0 ].ax;
	if( point[ 0 ].sy == P0_MY || point[ 0 ].sy == -P0_MY )
		point[ 0 ].ay = - point[ 0 ].ay;

	point[ 0 ].sx+= point[ 0 ].ax;
	point[ 0 ].sy+= point[ 0 ].ay;

	/* ... and POINT[1] */

	point[ 1 ].x+= point[ 1 ].sx;
	point[ 1 ].y+= point[ 1 ].sy;

	if( point[ 1 ].sx == P1_MX || point[ 1 ].sx == -P1_MX )
		point[ 1 ].ax = - point[ 1 ].ax;
	if( point[ 1 ].sy == P1_MY || point[ 1 ].sy == -P1_MY )
		point[ 1 ].ay = - point[ 1 ].ay;

	point[ 1 ].sx+= point[ 1 ].ax;
	point[ 1 ].sy+= point[ 1 ].ay;
}

/* Thiz performs TRUE fade */

void    Fade        ( int grade )
{
	int c;
	for( c= 0; c < 768; c++ )
		Palette[ c + 768 ]= ( uchar )( ( int ) Palette[ c ] * ( int ) grade >> 6 );
}

void    Remap       ( void )
{
	int c;
	outp( 0x3c8, 0 );
	for( c= 0; c < 768; c++ )
		outp( 0x3c9, Palette[ c + 768 ] );
}

void    Cut_out     ( void )
{
	uchar   far *video=( uchar* ) 0xA0000000L;
	uint    x,y;

	video_index( 0 );

	_fmemset( video, 63 ,32000 );
	border( 63 );

	outp( 0x3c8, 0 );
	for( x=0; x<32; x++ )
	{
		outp( 0x3c9, x << 1 );
		outp( 0x3c9, x << 1 );
		outp( 0x3c9, x << 1);
	}

	for( x=0; x<32; x++ )
	{
		while( ( inp( 0x3da ) & 8 ) );
		while( !( inp( 0x3da ) & 8 ) );
		border( 31 - x );
	}


	for( y=0; y < 50; y++ )
	{
		while( !( inp( 0x3da ) & 8 ) );
		_fmemset( &video[ 320 * ( y * 2 ) ], 0, 320 );
		_fmemset( &video[ 320 * ( 99 - y * 2 ) ], 0, 320 );
	}

}
