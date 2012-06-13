/*
 PRJ: BMODE
CODE: PiX / -=NeMeSiS=-
 MOD: test.h / bmode test program
*/

#ifndef _TEST_H_
#define _TEST_H_

typedef unsigned char   uchar;
typedef unsigned int    uint;
typedef unsigned long   ulong;

typedef struct
{
	uchar   x;
	uchar   y;
	uchar   c;
	uchar   dummy;
}   POINT;

#define _POINTS 15000

#define border(x)   { outp( 0x3c0, 0x11 | 32 ); outp( 0x3c0, x ); }
#define wvbl()      while( !( inp( 0x3da ) & 8 ) );
#define nwvbl()     while( inp( 0x3da ) & 8 );

#endif
