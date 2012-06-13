model small, c
locals

sPoint struc
	x	dd	?
    y	dd	?
sPoint ends

MAXPOINT = 1000

.data

extrn	TrigTable:dword
extrn	PointList:sPoint
extrn	Buffer:byte

.code
.386

global CopyAndClear:proc

CopyAndClear	proc	uses es
        mov		ax, 0A000h
        mov		es, ax
        xor		bx, bx
        mov		cx, 40*200/4
@@loop:
        xor		eax, eax
		xchg	eax, dword ptr Buffer[ bx ]
        mov		es:[ bx ], eax
        add		bx, 4
        dec		cx
        jnz		@@loop

		ret
CopyAndClear	endp

global fixed_mul:proc

fixed_mul	proc	a:dword, b:dword
		mov		eax, a
        mov		edx, b
        imul	edx
        ;shrd	eax, edx, 16
		shr		eax, 16
		ret
fixed_mul	endp

label	yoffset word
I = 0
rept 200
	dw	I
I = I + 40
endm

pixmask	db	128, 64, 32, 16, 8, 4, 2, 1

seno	dd	?
coseno	dd	?
xt		dd	?
yt		dd	?

global RotateAndMakeBuffer:proc

RotateAndMakeBuffer	proc	uses es di si bp, Angle:word
		mov		ax, 0A000h
        mov		es, ax

        mov		bx, Angle
        shl		bx, 2

        mov		esi, TrigTable[ bx ]
        mov		edi, TrigTable[ bx + 64*4 ]

        xor		bx, bx
        mov		cx, MAXPOINT
@@loop:
		mov		eax, PointList[ bx ].x
        push	eax
        imul	edi
        shrd	eax, edx, 16
        mov		ebp, eax

        mov		eax, PointList[ bx ].y
        push	eax
        imul	esi
        shrd	eax, edx, 16
        sub		ebp, eax
        mov		xt, ebp

        pop		eax
        imul	edi
        shrd	eax, edx, 16
        mov		ebp, eax

        pop		eax
        imul	esi
        shrd	eax, edx, 16

	    add		ebp, eax
    	mov		yt, ebp

        mov		bp, word ptr yt[ 2 ]
        ;add		bp, 100
        ;add		bp, bp
        lea		bp, [ebp*2+200]
        mov		dx, yoffset[ bp ]

        mov		ax, word ptr xt[ 2 ]
        add		ax, 160
        mov		bp, ax
        shr		ax, 3
        add		dx, ax
        and		bp, 7
        mov		al, pixmask[ bp ]
        mov		bp, dx
        or		es:[ bp ], al

        add		bx, size sPoint

        dec		cx
        jnz		@@loop

		ret
RotateAndMakeBuffer	endp

.data

extrn	Palette:byte

.code

global Remap:proc

Remap	proc	uses si
	mov		dx, 3c8h
    xor		al, al
    out		dx, al
    inc		dx
    mov		si, offset Palette
    mov		cx, 16*3
    rep		outsb
    ret
Remap	endp

end
