/*
 * bline test
 * code by pix - fferoldi@mbox.vol.it - federico@logicom.it
 */

#include <conio.h>
#include <stdlib.h>
#include <stdio.h>

#include "bmode.h"

long    bench   ( void );
void    move_it_up      ( void );

int     x[2], y[2], sx[2], sy[2];

void    main    ( void )
{
	long    count;

	x[0]=1;
	y[0]=1;
	sx[0]=7;
	sy[0]=2;

	x[1]=10;
	y[1]=15;
	sx[1]=3;
	sy[1]=5;

	set_bmode();

	/*
	while( !kbhit() )
		move_it_up();
	*/

	count= bench();

	/*
	do
	{
		while( !( inp( 0x3da ) & 8 ) );
		bline( 128, 100, x1 + 128 , y1 + 100, 0 );
		x1= rand() % 128;
		y1= rand() % 100;
		c= rand() % 256;
		bline( 128, 100, x1 + 128 , y1 + 100, 15 );
	}
	while( !kbhit() );
	*/
	/*
	while( getch() != 27 );
	*/

	_asm    mov ax,3h
	_asm    int 10h

	printf( "bench: %ld == %ld / scan\n", count, count / ( 70 ) );
}

long    bench   ( void )
{
	int x0, y0, c;
	long    count=0;

	while( !( inp( 0x3da ) & 8 ) );

	for( c=0; c< 70; c++)
	{
		while( ( inp( 0x3da ) & 8 ) );
		while( !( inp( 0x3da ) & 8 ) )
		{
			bline(  rand()&255, rand()%200,
					rand()&255, rand()%200,
					rand()&255 );
			count++;
		}
	}

	count=0;

	for( c=0; c< 70; c++)
	{
		while( ( inp( 0x3da ) & 8 ) );
		while( !( inp( 0x3da ) & 8 ) )
		{
			bline(  rand()&255, rand()%200,
					rand()&255, rand()%200,
					rand()&255 );
			count++;
		}
	}

	return count;
}

void    move_it_up      ( void )
{
		while( !( inp( 0x3da ) & 8 ) );

		bline( x[0], y[0], x[1], y[1], 0 );

		x[0]+=sx[0];
		y[0]+=sy[0];

		x[1]+=sx[1];
		y[1]+=sy[1];

		if( x[0] < 0 )
		{
			x[0]=0;
			sx[0]=-sx[0];
		}
		else
		if( x[0] > 255 )
		{
			x[0]=255;
			sx[0]=-sx[0];
		}

		if( y[0] < 0 )
		{
			y[0]=0;
			sy[0]=-sy[0];
		}
		else
		if( y[0] > 199 )
		{
			y[0]=199;
			sy[0]=-sy[0];
		}

		if( x[1] < 0 )
		{
			x[1]=0;
			sx[1]=-sx[1];
		}
		else
		if( x[1] > 255 )
		{
			x[1]=255;
			sx[1]=-sx[1];
		}

		if( y[1] < 0 )
		{
			y[1]=0;
			sy[1]=-sy[1];
		}
		else
		if( y[1] > 199 )
		{
			y[1]=199;
			sy[1]=-sy[1];
		}

		bline( x[0], y[0], x[1], y[1], 15 );
}

