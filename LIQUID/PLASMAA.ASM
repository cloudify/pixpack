
.model large, c

.data

extrn pATable1:ptr
extrn pATable2:ptr
extrn pAMask:ptr

.code
.386

tb1		dd	?
tb2		dd	?
tb3		dd	?
tb4		dd	?
rollw	dw	?

global CalculateBody:proc

CalculateBody	proc	uses ds es di si bp fs
						arg x1:word, y1:word, x2:word, y2:word, \
							x3:word, y3:word, pgoff:word

		mov		di, pgoff
		mov		ax, 0A000h
		mov		fs, ax

		mov		bx, 320

		mov		ax, y1
		mul		bx
		add		ax, x1
		push	ax

		mov		ax, y2
		mul		bx
		add		ax, x2
		push	ax

		mov		ax, y3
		mul		bx
		add		ax, x3
		push	ax

		pop		si	;tb3
		pop		bx	;tb2
		pop		bp	;tb1

		mov		ax, word ptr pATable2[2]
		mov		es, ax

		ror		esi, 16
		mov		si, word ptr pAMask[2]
		mov		gs, si
		xor		si, si
		ror		esi, 16

		mov		ax, word ptr pATable1[2]
		mov		ds, ax

		mov		cx, 100
@@inner:
I = 0
rept 160/4
		mov		eax, ds:[ bp + I ]
		add		eax, es:[ bx + I ]
		and		eax, 7f7f7f7fh
		add		eax, es:[ si + I ]
		and		eax, 7f7f7f7fh

		ror		esi, 16

		mov		dx, ax
		mov		ah, al
		mov		dl, dh

		or		ax, gs:[ si + (I+0)*2 ]
		or		dx, gs:[ si + (I+1)*2 ]

		mov		fs:[ di + (I+0)*2 ], ax
		mov		fs:[ di + (I+1)*2 ], dx

		shr		eax, 16
		mov		dx, ax
		mov		ah, al
		mov		dl, dh

		or		ax, gs:[ si + (I+2)*2 ]
		or		dx, gs:[ si + (I+3)*2 ]

		ror 	esi, 16

		mov		fs:[ di + (I+2)*2 ], ax
		mov		fs:[ di + (I+3)*2 ], dx


I = I + 4
endm
		lea		bp, [ bp + 320 ]
		lea		bx, [ bx + 320 ]
		lea		si, [ si + 320 ]
		lea		di, [ di + 320 ]
		ror		esi, 16
		lea		si, [ si + 320 ]
		ror		esi, 16

		dec		cx
		jnz		@@inner

		ret
CalculateBody	endp

end