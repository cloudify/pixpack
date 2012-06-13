/*
 PRJ: blobs experiments
CODE: PiX /NeMeSiS
 MOD: blobs.h / main include
*/

#ifndef _BLOBS_H_
#define _BLOBS_H_

	/* **** CONSTs **** */

#define _DEBUG_

#define P0_MX   7
#define P0_MY   5
#define P1_MX   10
#define P1_MY   6

	/* **** MACROs **** */

#define video_index(x)  { outpw( 0x3d4, 0xc | ( x & 0xff00 ) ); outpw( 0x3d4, 0xd | ( x << 8 ) ); }
#define border(x)       { outp( 0x3c0, 0x11 | 32 ); outp( 0x3c0, x ); }

	/* **** TYPEDEFs **** */

typedef unsigned char   uchar;
typedef unsigned int    uint;

typedef struct
{
	int x;
	int y;
	int sx;
	int sy;
	int ax;
	int ay;
} point_;

/* **** PROTOs **** */

extern void do_blob     ( uint page, uint x0, uint y0, uint x1, uint y1 );
void    Fade            ( int grade );
void    Remap           ( void );
void    Move_points     ( void );
void    Cut_out         ( void );

#endif /* _BLOBS_H_ */
