/*
 * bmode access test
 * code by pix - fferoldi@mbox.vol.it - federico@logicom.it
 */

#include <stdio.h>
#include <conio.h>

unsigned long   bench   ( void );

#include "bmode.h"

void    main    ( void )
{
	unsigned long   count_mode13=0, count_bmode=0;
	int             x;

	_asm    mov ax,13h
	_asm    int 10h

	count_mode13= bench();

	set_bmode();

	outp( 0x3c0, 0x11 | 32 );
	outp( 0x3c0, 1 );

	count_bmode= bench();

	_asm    mov ax,3h
	_asm    int 10h

	if( count_bmode > count_mode13 )
		printf( " mode13: %lu     bmode: %lu  1:%lf\n",
				count_mode13, count_bmode, ( double )count_bmode / count_mode13 );
	else
		printf( " mode13: %lu     bmode: %lu  %lf:1\n",
				count_mode13, count_bmode, ( double )count_mode13 / count_bmode );
}

unsigned long   bench   ( void )
{
	unsigned long   count=0;
	_asm
	{
		push    es
		mov     ax, 0a000h
		mov     es, ax
		xor     bx, bx
		mov     cx,20
	_main_loop:
	_bench_wcz:
		in      al, 40h
		mov     ah,al
		in      al, 40h
		and     ax,ax
		jnz     _bench_wcz
	_bench_loop:
		mov     es:[ bx ], al
		inc     word ptr count
		adc     word ptr count[ 2 ], 0
		in      al, 40h
		mov     ah,al
		in      al, 40h
		and     ax,ax
		jnz     _bench_loop
		inc     bx
		dec     cx
		jnz     _main_loop
		pop     es
	}
	return count;
}

/*
unsigned long   bench   ( void )
{
	unsigned long   count=0;
	_asm
	{
		push    es
		mov     ax, 0a000h
		mov     es, ax
		xor     bx, bx
		mov     dx, 3dah
	_bench_wvbl:
		in      al, dx
		test    al, 8
		jz      _bench_wvbl
	_bench_wvrt:
		in      al, dx
		test    al, 8
		jnz     _bench_wvrt
	_bench_loop:
		mov     es:[ bx ], al
		inc     word ptr count
		adc     word ptr count[ 2 ], 0
		in      al, dx
		test    al, 8
		jz      _bench_loop
		pop     es
	}
	return count;
}
*/
