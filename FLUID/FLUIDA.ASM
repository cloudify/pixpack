.model large, c
LOCALS

extrn back

.data

extrn Height

.code

.386

GLOBAL Calculate:PROC

Calculate PROC
	ARG npage:WORD, density:WORD
	USES DS, SI, ES, DI

	mov dx, @data
	mov ds, dx

	cmp npage, 0
	jne @@page1
	les di, DWORD PTR ds:height[4]
	lds si, DWORD PTR ds:height[0]
	jmp @@draw
@@page1:
	les di, DWORD PTR ds:height[0]
	lds si, DWORD PTR ds:height[4]
@@draw:								; ds:si = npage
									; es:di = hpage
	add si, 101*2					; start with offset 101
	add di, 101*2
	mov cx, 158						; x count
@@loop0:
	push cx							; push x
	mov cx, 98						; y count
@@loop1:
	push cx							; push y
	mov ax, es:[di + 100*2]			; ax = hpage[count + 100]
	add ax, es:[di - 100*2]			; ax += hpage[count - 100]
	add ax, es:[di + 1*2]			; ax += hpage[count + 1]
	add ax, es:[di - 1*2]			; ax += hpage[count - 1]
	sar ax, 1						; ax /= 2
	sub ax, ds:[si]					; ax -= npage[count]
	mov dx, ax
	mov cx, density
	sar dx, cl
	sub ax, dx						; ax -= ax >> density
	mov ds:[si], ax					; npage[count] = ax
	add si, 2						; count++
	add di, 2
	pop cx							; pop y
	dec cx
	jnz @@loop1
	add si, 4						; count += 2
	add di, 4
	pop cx							; pop x
	dec cx
	jnz @@loop0
	ret
ENDP

.data

.code

x	DW	?
y	DW	?
ox	DW	?
oy	DW	?
c	DW	?

LABEL mul100
I=0
REPT 160
	DW I
I=I+100
ENDM

GLOBAL Draw:PROC

Draw PROC
	ARG hpage:WORD
	USES DS, SI, ES, DI

	mov bx, SEG Back
	mov fs, bx
	mov bx, OFFSET Back			; fs:bx = back

	mov di, 0A000H
	mov es, di
	xor di, di					; es:di = vidmem

	mov dx, @data
	mov ds, dx

	cmp hpage, 0
	jne @@page1
	lds si, DWORD PTR ds:height[0]
	jmp @@draw
@@page1:
	lds si, DWORD PTR ds:height[4]
@@draw:								; ds:si = hpage
	mov y, 0
	mov cx, 100
@@loop0:
	push cx si
	mov cx, 159
	mov x, 0
@@loop1:
	mov ax, ds:[si]				; ax = hpage[o]
	mov dx, ax					; dx = hpage[o]
	sub ax, ds:[si + 100*2]		; ax = hpage[o] - hpage[o + 100]
	sub dx, ds:[si + 1*2]		; dx = hpage[o] - hpage[o + 1]
	mov ox, ax					; ox = hpage[o] - hpage[o + 100]
	mov oy, dx					; oy = hpage[o] - hpage[o + 1]

	mov dx, 128					; dx = 128
	sub dx, ax					; dx = 128 - ox
	mov ax, dx 					; ax = 128 - dx
	cmp ax, 0					; bound ax (0, 255)
	jnl @@chk255
	xor ax, ax
	jmp @@draw2
@@chk255:
	cmp ax, 255
	jle @@draw2
	mov ax, 255
@@draw2:
	mov c, ax					; c = 128-ox in (0, 255)

	mov dx, bx					; dx = bx
	mov bx, ox					; bx = ox
	sar bx, 3					; bx = (ox >> 3)
	add bx, x					; bx = (ox >> 3) + x
	add bx, bx
	mov bx, mul100[bx]			; bx = ((ox >> 3) + x) * 100
	xchg bx, dx					; dx = ((ox >> 3) + x) * 100

	mov ax, oy					; ax = oy
	sar ax, 3					; ax = (oy >> 3)
	add ax, y					; ax = (oy >> 3) + y
	add dx, ax					; dx = ((ox >> 3) + x) * 100 + (oy >> 3) + y

	push bx
	add bx, dx
	mov al, fs:[bx]
	pop bx						; al = back[dx]

	mov ah, BYTE PTR c
	mul ah
	sar ax, 8					; ax = (al * c) >> 8

	;mov ah, al
	;mov es:[di], ax				; draw pixel
	;mov es:[di + 320], ax
	;inc di

	mov es:[di + 320], al	;;;

	inc di

	add si, 100*2

	inc x

	dec cx
	jnz @@loop1

	pop si cx

	add di, 161	;*2

	add si, 2

	inc y

	dec cx
	jnz @@loop0

	ret
ENDP

end