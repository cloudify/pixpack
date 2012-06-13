/*
 * bmode test
 * code by pix - fferoldi@mbox.vol.it - federico@logicom.it
 */

#include <stdlib.h>
#include <conio.h>

#include "bmode.h"
#include "test.h"

void    set_pixel   ( int x, int y, int c );
void    draw_list   ( POINT *plist, uint len );
void    draw_random     ( uint len );

POINT   list[ _POINTS ];

void    main    ( void )
{
	int     pos;

	for( pos=0; pos<_POINTS; pos++ )
	{
		list[ pos ].x= ( uchar ) rand() % 256;
		list[ pos ].y= ( uchar ) rand() % 200;
		list[ pos ].c= ( uchar ) rand() % 256;
	}

	set_bmode();

	while( !kbhit() )
	{
		wvbl();
		border( 1 );
		/* draw_list( list, _POINTS ); */
		draw_random ( 6000 );
		border( 0 );

	}

	getch();

	_asm    mov ax,3h
	_asm    int 10h
}

void    set_pixel   ( int x, int y, int c )
{
	_asm
	{
		push    es
		mov     ax,0a000h
		mov     es,ax
		mov     bl,x
		mov     bh,y
		mov     al,c
		mov     es:[ bx ],al
		pop     es
	}
}

void    draw_list   ( POINT *plist, uint len )
{
	_asm    {
		push    es
		push    ds
		push    si

		mov     ax,0A000h
		mov     es,ax

		lds     si,plist
		mov     cx,len
	draw_loop:
		mov     bx, ds:[ si ]
		mov     al, ds:[ si + 2 ]
		mov     es:[ bx ], al
		add     si, 4
		loop    draw_loop

		pop     si
		pop     ds
		pop     es
	}
}

void    draw_random     ( uint len )
{
	_asm    {
		push    es
		push    ds
		push    si

		mov     ax,0A000h
		mov     es,ax

		in      al,40h
		mov     bl,al
		in      al,40h
		mov     bh,al

		mov     ds,bx

		mov     cx,len
	draw_loop:
		mov     al,[bx]
		mov     es:[ bx ], al
		add     bx,0x0305
		loop    draw_loop

		pop     si
		pop     ds
		pop     es
	}
}

