.model compact, c
locals

extrn	Plasma:byte
extrn	Picture:byte
extrn	Radial:byte

.data

.code
.386

label Mul320 word

I=0
rept 200
	dw	I
I=I+320
endm

global MakeDeform:proc

MakeDeform	proc	uses ds es si di
	mov		ax, 0A000h+20+2
	mov		es, ax

	mov		ax, offset Plasma
	shr		ax, 4
	add     ax, seg Plasma
	mov		ds, ax
	xor		si, si

	mov		ax, offset Picture
	shr		ax, 4
	add		ax, seg Picture
	mov		fs, ax
	xor		bx, bx

	mov		cx, 100*256

	push	bp
	xor		bp, bp

@@loop:

	mov		ax, ds:[ si ]
	sub		ah, al
	sub		al, ds:[ si+256 ]
	mov		di, bx
	add		bl, ah
	add		bh, al
	mov		al, fs:[ bx ]
	mov		bx, di
	inc		bx
	mov		es:[ bp ], al
	inc		bp
	inc		si
	dec		cl
	jnz		@@loop

	mov		cl, 0
	add		bp, 320-256
	dec		ch
	jnz		@@loop

	pop		bp
	ret

MakeDeform	endp

global MakePlasma:proc

MakePlasma	proc	uses ds es si di, x1:word, y1:word, x2:word, y2:word
	mov		ax, seg Radial
	mov		ds, ax

	mov		si, y1
	add		si, si
	mov		si, Mul320[ si ]
	add		si, x1

	mov		bx, y2
	add		bx, bx
	mov		bx, Mul320[ bx ]
	add		bx, x2

	mov		ax, seg Plasma
	mov		es, ax
	xor		di, di

	mov		ch, 100
	mov		cl, 256/4
@@loop:

I=0
rept 256/4

	mov		eax, ds:[ si + I ]
	add		eax, ds:[ bx + I ]
	mov		es:[ di + I ], eax
	;add		si, 4
	;add		bx, 4
	;add		di, 4
	;dec		cl
	;jnz		@@loop
I=I+4
endm
	;mov		cl, 256/4
	add		si, 320 ;-256
	add		bx, 320 ;-256
	add		di, 256 ;
	dec		ch
	jnz		@@loop

	ret
MakePlasma	endp

end
