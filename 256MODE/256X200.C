/*
 * 256x200 mode
 * code by pix - fferoldi@mbox.vol.it - federico@logicom.it
 */

#include <conio.h>

typedef unsigned char   uchar;
typedef unsigned int    uint;
typedef unsigned long   ulong;

#define _outcrt(x,y)    outpw( 0x3d4, ( unsigned ) (x) | ( ( unsigned ) (y) << 8 ) );

void    main    ( void )
{
	uchar   far *video=( uchar far * )0xA0000000L;
	uint    pos,x;

	_asm    mov     ax,13h
	_asm    int     10h

	outp( 0x3c8,0 );
	for( x=0; x<256; x++ )
	{
		outp( 0x3c9, 0 );
		outp( 0x3c9, 0 );
		outp( 0x3c9, x^63 );
	}

    /* set seq_mode & reset seq */

    //outpw( 0x3c4, 0x0604 );
    outpw( 0x3c4, 0x0100 );

    outpw( 0x3c2, ( inp( 0x3cc ) & 0x3f ) | 128 | 64 );

	/* sprot. CRT */
	outp( 0x3d4, 0x11 );
	outp( 0x3d5, inp( 0x3d5 ) & 0x7f );

	/* 256 */

	/* ampiezza logica */
	_outcrt( 0x13, 32 );

	/* tot hor */
	_outcrt( 0, 0x5f );	// 0x50

	/* fine vis. hor */
	_outcrt( 1, 0x3f );

	/* iniz blank hor */
	_outcrt( 2, 0x42 );	// 0x45

	/* fin blank hor */
	_outcrt( 3, 0x9f );	// 0x71

	/* iniz. ritr. oriz */
	_outcrt( 4, 0x4c );	// 0x48

	/* fine ritr. oriz */
	_outcrt( 5, 0 );	// 0x70

    //_outcrt( 0x17, 0xe3 );

    //outpw( 0x3d4, 0x0108 );

	/* prot. CRT */
	outp( 0x3d4, 0x11 );
	outp( 0x3d5, inp( 0x3d5 ) | 128 );

	for( pos=0; pos<256; pos++ )
		for( x=0; x<256; x++ )
			video[ ( pos << 8 ) | x ]= ( unsigned char ) ( pos + x ) ^ 255;

	getch();

	_asm    mov     ax,3h
	_asm    int     10h
}
