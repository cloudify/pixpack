
#ifndef _HILIST_H_
#define _HILIST_H_

/*
 * CONSTs
 */

#define _MAX_POINT  3000
#define _FP         16

/*
 * MACROs
 */

#define border(x)   { outp( 0x3c0, 0x11 | 32 ); outp( 0x3c0, x ); }
#define wvbl()      while( !( inp( 0x3da ) & 8 ) );
#define nwvbl()     while( inp( 0x3da ) & 8 );
#define videomode(x)    _asm    mov     ax,x \
						_asm    int     10h

/*
 * TYPEDEFs
 */

typedef unsigned char   uchar;
typedef unsigned int    uint;
typedef unsigned long   ulong;

typedef struct
{
	long    x;
	long    y;
	long    sx;
	long    sy;
	uint    old_offset;
	uchar   old_mask;
	uchar   flags;
}   HIPOINT;

typedef struct
{
	uint    x;
	uint    y;
}   COORD;

/*
 * PROTOs
 */

extern void draw    ( HIPOINT *list, uint len, uint page );
extern void move    ( HIPOINT *list, uint len );
extern void move_gravity    ( HIPOINT *list, uint len, long grav );

void    chaos       ( void );
void    wait_count  ( int count );
void    morph       ( void );
void    morph_ifs   ( void );
void    gravity     ( long val );

#endif
