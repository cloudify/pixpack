
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>

#include "hilist.h"

HIPOINT     point_list[ _MAX_POINT ];
COORD       morph_list[ _MAX_POINT ];

void    main    ( void )
{
	uint    pos,count;

	for( pos=0; pos<_MAX_POINT; pos++ )
	{
		point_list[ pos ].x= 320L << _FP;
		point_list[ pos ].y= 240L << _FP;
		point_list[ pos ].sx= ( rand() - rand() /2  ) * 16L;
		point_list[ pos ].sy= ( rand() - rand() /2  ) * 16L;
		point_list[ pos ].old_offset=0;
		point_list[ pos ].old_mask=255;
	}

	videomode(0x12);

	outpw( 0x3c4, 0x0f02 );

	while( !kbhit() )
	{
		wvbl();
		draw( point_list, _MAX_POINT, 0 );
		move( point_list, _MAX_POINT );
	}

	getch();

	morph_ifs();

	for( pos=0; pos<_MAX_POINT; pos++ )
	{
		point_list[ pos ].sx= ( ( ( long ) ( 65536.0 * ( 320.0 + 220 * sin ( ( double ) pos * 6.28 / _MAX_POINT ) ) ) ) - point_list[ pos ].x ) / ( 70 * 1 );
		point_list[ pos ].sy= ( ( ( long ) ( 65536.0 * ( 240.0 + 220 * cos ( ( double ) pos * 6.28 / _MAX_POINT ) ) ) ) - point_list[ pos ].y ) / ( 70 * 1 );
	}

	chaos();

	wait_count( 70 * 1 );

	for( pos=0; pos<_MAX_POINT; pos++ )
	{
		point_list[ pos ].sx= ( ( ( long ) ( 65536.0 * ( 320.0 + ( 220.0 * pos / _MAX_POINT ) * sin ( ( double ) pos * 6.28 * 8 / _MAX_POINT ) ) ) ) - point_list[ pos ].x ) / ( 70 * 1 );
		point_list[ pos ].sy= ( ( ( long ) ( 65536.0 * ( 240.0 + ( 220.0 * pos / _MAX_POINT ) * cos ( ( double ) pos * 6.28 * 8 / _MAX_POINT ) ) ) ) - point_list[ pos ].y ) / ( 70 * 1 );
	}

	chaos();

	wait_count( 70 * 1 );

	for( pos=0; pos<_MAX_POINT; pos++ )
	{
		point_list[ pos ].sx= ( ( ( long ) ( 65536.0 * ( 320.0 + ( 220.0 * pos / _MAX_POINT ) * cos ( ( double ) pos * 6.28 * 8 / _MAX_POINT ) ) ) ) - point_list[ pos ].x ) / ( 70 * 1 );
		point_list[ pos ].sy= ( ( ( long ) ( 65536.0 * ( 240.0 + ( 220.0 * pos / _MAX_POINT ) * sin ( ( double ) pos * 6.28 * 8 / _MAX_POINT ) ) ) ) - point_list[ pos ].y ) / ( 70 * 1 );
	}

	chaos();

	wait_count( 70 * 1 );

	while( !kbhit() )
	{
		wvbl();
		draw( point_list, _MAX_POINT, 0 );
		move_gravity ( point_list, _MAX_POINT, 65536L );
	}

	getch();

	while( !kbhit() )
	{
		wvbl();
		border( 1 );
		draw( point_list, _MAX_POINT, 0 );
		border( 2 );
		move( point_list, _MAX_POINT );
		border( 0 );
	}

	videomode(3);
}

void    chaos   ( void )
{
	uint    pos;
	HIPOINT tmp;
	uint    ridx;

	for( pos=0; pos<_MAX_POINT; pos++ )
	{
		ridx = rand() % _MAX_POINT;
		tmp= point_list[ ridx ];
		point_list[ ridx ]= point_list[ pos ];
		point_list[ pos ]= tmp;
	}
}

void    wait_count  ( int count )
{
	while( !kbhit() && count-- )
	{
		wvbl();
		move( point_list, _MAX_POINT );
		draw( point_list, _MAX_POINT, 0 );
	}
}

void    morph   ( void )
{
	register int    pos;
	for( pos=0; pos<_MAX_POINT; pos++ )
	{
		point_list[ pos ].sx= ( ( ( long ) morph_list[ pos ].x << _FP ) - point_list[ pos ].x ) / ( 70 * 1 );
		point_list[ pos ].sy= ( ( ( long ) morph_list[ pos ].y << _FP ) - point_list[ pos ].y ) / ( 70 * 1 );
	}
}

void    morph_ifs   ( void )
{
	FILE    *stream;
	stream= fopen( "hilist.dat", "rb" );
	while( fread( morph_list, 4, _MAX_POINT, stream ) == _MAX_POINT )
	{
		morph();
		chaos();
		wait_count( 70 * 1 );
	}

	fclose( stream );
}

void    gravity     ( long val )
{
	int pos;
	for( pos=0; pos<_MAX_POINT; pos++ )
		point_list[ pos ].sy += val;
}
