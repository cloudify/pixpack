/*
 * 256x256 mode
 * code by pix - fferoldi@mbox.vol.it - federico@logicom.it
 */

#include <conio.h>

typedef unsigned char   uchar;
typedef unsigned int    uint;
typedef unsigned long   ulong;

#define _outcrt(x,y)    outpw( 0x3d4, ( unsigned ) (x) | ( ( unsigned ) (y) << 8 ) );

void    main    ( void )
{
	uchar   *video=( uchar * )0xA0000000L;
	uint    pos,x;

	_asm    mov     ax,13h
	_asm    int     10h

	outp( 0x3c8,0 );
	for( x=0; x<256; x++ )
	{
		outp( 0x3c9,x >> 2 );
		outp( 0x3c9,x >> 2 );
		outp( 0x3c9,63 );
	}


	/* sprot. CRT */
	outp( 0x3d4, 0x11 );
	outp( 0x3d5, inp( 0x3d5 ) & 0x7f );

	/* 256 */

	/* ampiezza logica */
	_outcrt( 0x13, 32 );

	/* tot hor */
	_outcrt( 0, 64 + 16 );

	/* fine vis. hor */
	_outcrt( 1, 63 );

	/* iniz blank hor */
	_outcrt( 2, 69 );

	/* fin blank hor */
	_outcrt( 3, 113 );

	/* iniz. ritr. oriz */
	_outcrt( 4, 72 );

	/* fine ritr. oriz */
	_outcrt( 5, 112 );

	/* 240 */

	/*
	_outcrt( 6, 13 );
	_outcrt( 7, 62 );
	_outcrt( 9, 65 );
	_outcrt( 0x10, 234 );
	_outcrt( 0x11, 172 );
	_outcrt( 0x12, 223 );
	_outcrt( 0x14, 0 );
	_outcrt( 0x15, 231 );
	_outcrt( 0x16, 6 );
	_outcrt( 0x17, 227 );

	outpw( 0x3c4, 0x0100 );
	outp( 0x3c2, 0xe3 );
	outpw( 0x3c4, 0x0300 );
	*/

	/* prot. CRT */
	outp( 0x3d4, 0x11 );
	outp( 0x3d5, inp( 0x3d5 ) | 256 );

	for( pos=0; pos<256; pos++ )
		for( x=0; x<256; x++ )
		video[ ( pos << 8 ) | x ]= ( unsigned char ) ( pos + x );

	getch();

	_asm    mov     ax,3h
	_asm    int     10h
}
