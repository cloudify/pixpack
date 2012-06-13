
#include <math.h>
#include <mem.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>

#include "fixed.h"
#include "caleid.h"

#define	_border(x)	//{ outp( 0x3c0, 32 | 0x11 ); outp( 0x3c0, x ); }

sPoint	PointList[ MAXPOINT ];

long	TrigTable[ 320 ];
uchar	Buffer[ 40*200 ];
uchar	Palette[ 16*3 ];

void	main	( void )
{
		int		x,i;
        uint	plane = 0x0100;
        int		Angle=0, AngleSpd=0, AngleAcc=1;

        // Make trigtable
        for( x=0; x<320; x++ )
        	TrigTable[ x ] = ( long )( (double)65536.0 * sin( ( ( double) x*3.14159 )/128.0 ) );

        // Clear lists
        for( x=0; x<MAXPOINT; x++ )
        {
        	PointList[ x ].x = PointList[ x ].y = 0;
            //PointList[ x ].x = ( long )( ( rand() % 140 ) - 70 ) << 16;
            //PointList[ x ].y = ( long )( ( rand() % 140 ) - 70 ) << 16;
        }

        _asm	mov	ax, 0Dh
        _asm	int	10h

        // Remap palette
        MakePalette( 63, 63, 63 );

        inp( 0x3da );
        for( x=0; x<16; x++ )
        {
        	outp( 0x3c0, x );
            outp( 0x3c0, x );
        }
        outp( 0x3c0, 32 );

        Remap();

		// Clear buffer
        memset( Buffer, 0, 40*200 );

        while( !kbhit() )
        {
        	while( !( inp( 0x3da ) & 8 ) );
            _border( 15 );

            outpw( 0x3c4, plane | 2 );
			if( plane == 0x0400 )
            	plane = 0x0100;
            else
            	plane <<= 1;

            _border( 7 );

            CopyAndClear();

            _border( 3 );

            RotateAndMakeBuffer( ( (Angle>>4) + 128 ) & 255 );
            Angle += AngleSpd;
            AngleSpd += AngleAcc;
            if( ( AngleSpd >= ( 50 + (rand()&31) ) ) || ( AngleSpd <= ( -50 - (rand()&31) ) ) )
            {
            	AngleAcc = -AngleAcc;
                memset( Palette, 63, 16*3 );

                /*
                while( !( inp( 0x3da ) & 8 ) );
                Remap();

                MakePalette( (rand()&31) + 31, (rand()&31) + 31, (rand()&31) + 1 );
                for( i=0; i<2; i++ )
                {
                	while( inp( 0x3da ) & 8 );
                	while( !( inp( 0x3da ) & 8 ) );
                }
                Remap();
                */
            }

            _border( 1 );

            UpdateList();
            UpdateList();
            UpdateList();

            _border( 0 );

        }

        getch();

        _asm	mov	ax, 3h
        _asm	int	10h

		puts( " - CaLEiDoSCoPE - CoDe: PiX / NeMeSiS - (c) 1994 NeMeSiS production\n" );
}

/*
void	CopyAndClear	( void )
{
	_fmemcpy( ( void far * )0xA0000000L, Buffer, 40*200 );
    memset( Buffer, 0, 40*200 );
}
*/

uchar	mask[ 8 ] = { 128, 64, 32, 16, 8, 4, 2, 1 };

void	MakeBuffer		( void )
{
	int		x;
    uint	offset;

    for( x=0; x<MAXPOINT; x++ )
    {
    	offset = ( ( PointList[ x ].y >> 16 ) + 100 ) * 320 + ( ( PointList[ x ].x >> 16 ) + 160 );
        Buffer[ offset >> 3 ] |= mask[ offset & 7 ];
    }
}

/*
void	RotateAndMakeBuffer		( int Angle )
{
	int		i;
    long	xt, yt;
    uint	offset;

    for( i=0; i<MAXPOINT; i++ )
    {
    	xt = fixed_mul( TrigTable[ Angle+64 ] , PointList[ i ].x ) -
        	 fixed_mul( TrigTable[ Angle ] , PointList[ i ].y );

        yt = fixed_mul( TrigTable[ Angle+64 ] , PointList[ i ].y ) +
        	 fixed_mul( TrigTable[ Angle ] , PointList[ i ].x );

    	offset = ( ( yt >> 16 ) + 100 ) * 320 + ( ( xt >> 16 ) + 160 );
        Buffer[ offset >> 3 ] |= mask[ offset & 7 ];
    }
}
*/

void	RotateList		( int angle )
{
	int		i;
    long	xt, yt;

    for( i=0; i<MAXPOINT; i++ )
    {
        xt = fixed_mul( TrigTable[ angle+64 ] , PointList[ i ].x ) -
        	 fixed_mul( TrigTable[ angle ] , PointList[ i ].y );

        yt = fixed_mul( TrigTable[ angle+64 ] , PointList[ i ].y ) +
        	 fixed_mul( TrigTable[ angle ] , PointList[ i ].x );

		PointList[ i ].x = xt;
        PointList[ i ].y = yt;
    }
}

int		UpIndex = 0;
long	UpAng = 0, UpAngSpd = 254;
long	UpRad = 0, UpRadSpd = 1, UpRadAcc = 1;

void	UpdateList		( void )
{
	PointList[ UpIndex ].x = ( TrigTable[ ( ( UpAng >> 3 ) & 255 ) ] * UpRad ) >> 4;
    PointList[ UpIndex ].y = ( TrigTable[ ( ( UpAng >> 3 ) & 255 ) + 64 ] * UpRad ) >> 4;

    if( ++UpIndex == MAXPOINT )
    {
    	UpIndex = 0;
        UpAngSpd = rand() ;
        UpRad = 0;
        UpRadAcc = rand()&3;
        UpRadSpd = rand()&7;
    }

    UpAng += UpAngSpd;
    UpRad += UpRadSpd;

	UpRadSpd += UpRadAcc;

    if( ( UpRadSpd <= -40 ) || ( UpRadSpd >= 39 ) )
    	UpRadAcc = -UpRadAcc;

    if( UpRad > (100<<4) )
    {
    	UpRad = (100<<4);
        UpRadSpd = -UpRadSpd;
    }
    else
    if( UpRad < (-100<<4) )
    {
    	UpRad = (-100<<4);
        UpRadSpd = -UpRadSpd;
    }

}

void	MakePalette		( int r, int g, int b )
{
	int		x;
    int		lev;

    for( x=0; x<16; x++ )
    {
    	lev=0;

        if( x & 1 )
        	lev++;
        if( x & 2 )
        	lev++;
        if( x & 4 )
        	lev++;
        /*
		if( x & 8 )
        	lev++;
        */

        Palette[ x*3 + 0 ] = (lev*r)/3;
        Palette[ x*3 + 1 ] = (lev*g)/3;
        Palette[ x*3 + 2 ] = (lev*b)/3;
    }
}