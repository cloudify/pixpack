/*
 * Hardware double scroller
 * code by pix - fferoldi@mbox.vol.it - federico@logicom.it
 */

#include <conio.h>
#include <dos.h>

#include <xlib.h>
#include <xpoint.h>

void	Split	( WORD pos );
void	interrupt far SplitIRQ	( void );

#define	sync()		while( !( inp( 0x3da ) & 8 ) )
#define nsync()		while( ( inp( 0x3da ) & 8 ) )
#define hsync() 	while( ( inp( 0x3da ) & 1 ) )
#define nhsync()	while( !( inp( 0x3da ) & 1 ) )

#define _border(x)	{outp(0x3c0, 0x11 | 32);outp(0x3c0, x);}

#define	DIVISOR		(1193180L/116L)

void	(interrupt far * OldTimer)( void );
WORD	SplitPos = 0, SplitVal=0x13;

void	main	( void )
{
	WORD	x, y;

	x_set_mode( X_MODE_320x240, 640 );

	for( y=0; y<240; y++ )
		for( x=0; x<640; x++ )
        	{
            		//x_put_pix( x, y, 0, (x*4)/5 );

            		if ( y<118 )
				x_put_pix( x, y, 0, (x*y&63) );
			else
            			x_put_pix( x, y, 160, (x*y&63) );
		}

    	getch();

        _disable();

        OldTimer=_dos_getvect( 0x8 );
        _dos_setvect( 0x8, SplitIRQ );
        outp( 0x43, 0x30 );
        outp( 0x40, 0 );
        outp( 0x40, 0 );

        _enable();

    	x=0;

	while( !kbhit() )
	{
                SplitPos = x;

		outpw( 0x3d4, 0xc | (SplitPos & 0xff00) );
    		outpw( 0x3d4, 0xd | (SplitPos << 8) );

                SplitVal = ( ( 40*3 - 1 - SplitPos ) << 8 ) | 0x13;

                //_border(1);

                sync();
    		nsync();
                nhsync();

                //outp( 0x43, 0x30 );
                outp( 0x40, DIVISOR & 0x00ff );
		outp( 0x40, DIVISOR >> 8 );

                x++;
                if( x == 80 )
                	x=0;
	}

        _disable();

        outp( 0x43, 0x36 );
        outp( 0x40, 0 );
        outp( 0x40, 0 );

        _dos_setvect( 0x8, OldTimer );

        _enable();

	getch();

    	x_text_mode();
}

void	interrupt far SplitIRQ	( void )
{
	nhsync();
    	hsync();

    	outpw( 0x3d4, SplitVal );

    	nhsync();
    	hsync();
    	nhsync();
    	hsync();

    	outpw( 0x3d4, 0x5013 );

        //_border(0);

	outp( 0x20, 0x20 );
}
