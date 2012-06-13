.model large, c
locals

.386

.data

extrn frameCount:dword				; frame counter
extrn vgaStart:word					; vga start address

.code


global vgaSet200c:proc				; init 320x200x256 chained

vgaSet200c proc uses es di
	mov	ax, 13h
	int	10h
	mov	ax, 0a000h
	mov	es, ax
	xor di, di
	mov	cx, 64000 / 4
	xor ax, ax
	rep stosd
	ret
endp

global vgaSet3:proc                 ; init 80x25 text mode

vgaSet3 proc
	mov ax, 3
	int 10h
	ret
endp

global RemapPal:proc				; remap palette

; in -	pPalette	:ptr to palette array (rgb)
;		Index		:start index
;		Count		:colors to map
; out -	none

RemapPal proc uses ds si, pPalette:PTR, Index:WORD, Count:WORD
		lds		si, pPalette
		mov		dx, 3C8h
		mov		al, byte ptr Index
		out		dx, al
		inc		dx
		mov		cx, Count
		mov		ax, cx
		add		cx, cx
		add		cx, ax
		rep     outsb
		ret
endp

global frameWait:proc				; wait a number of frames

; in -	Count	:frames to wait for
; out - none

frameWait proc Count:dword
	mov	eax, frameCount
	add eax, Count
@@test:
	cmp eax, frameCount
	jb @@test
	ret
endp


global frameWaitOne:proc			; wait a single frame

frameWaitOne proc
	mov	eax, frameCount
@@test:
	cmp eax, frameCount
	jb @@test
	ret
endp


global preVR:proc					; pre-vertical-retrace proc
									; called every time before VR
preVR proc uses dx ax
	mov dx, 3d4h					; set vga-start-address to vgaStart
	mov ah, byte ptr vgaStart[1]
	mov al, 0ch
	out dx, ax
	inc al
	mov ah, byte ptr vgaStart[0]
	out dx, ax
	inc frameCount					; inc frame counter
	ret
endp


global vgaDupeScan:proc				; dupes scan lines
									; used to set ?x100 mode

; in -	Value	:hres = 400 / (Value + 1) lines
; out - none

vgaDupeScan proc Value:word
	mov	dx, 3d4h
	mov al, 9
	out dx, al
	inc dx
	in 	al, dx
	and al, 255 XOR 31
	or 	al, byte ptr Value[0]
	out dx, al
	ret
endp


global FadePalFromBlack:proc		; calc 256cols-palette faded from black

; in -	pDst	:ptr to dest 768bytes-array (calculated palette)
;		pSrc	:ptr to src 768bytes-array (source palette)
;		Pos		:fading position 0=black 64=src pal

FadePalFromBlack proc uses ds si es di, pDst:ptr, pSrc:ptr, Pos:word
	mov	dl, byte ptr Pos[0]
	cmp dl, 64
	jb	@@make_pal
	mov dl, 64
@@make_pal:
	lds si, pSrc
	les di, pDst
	mov cx, 768
@@loop:
	mov al, [si]
	mul	dl
	shr ax, 6
	mov es:[di], al
	inc si
	inc di
	dec cx
	jne @@loop
	ret
endp


global MakeBlackToWhitePal:proc		; make black2white pal

; in -	pDst	:ptr to dest 768bytes-array (calculated palette)
;		Pos		:fading position 0=black 64=white

MakeBlackToWhitePal proc uses es di, pDst:ptr, Pos:word
	mov	al, byte ptr Pos[0]
	cmp al, 64
	jb	@@make_pal
	mov al, 64
@@make_pal:
	mov ah, al
	mov dx, ax
	shl eax, 16
	mov ax, dx
	les di, pDst
	mov cx, 768/4
	rep stosd
	ret
endp

global FadePalFromWhite:proc		; calc 256cols-palette faded from white

; in -	pDst	:ptr to dest 768bytes-array (calculated palette)
;		pSrc	:ptr to src 768bytes-array (source palette)
;		Pos		:fading position 0=white 64=src pal

FadePalFromWhite proc uses ds si es di, pDst:ptr, pSrc:ptr, Pos:word
	mov	dx, 64						; dx = 64
	sub dx, Pos						; dx = 64 - Pos
	mov ax, 63
	mul dx
	mov dx, ax						; dx = 63 * (64 - Pos)
	shr dx, 6						; dl = 63 * (64 - Pos) / 64
	mov dh, byte ptr Pos
	lds si, pSrc
	les di, pDst
	mov cx, 768
@@loop:
	mov al, [si]					; al= pal
	mul	dh							; ax= pal * pos
	shr ax, 6						; al= pal * pos / 64
	neg al							; al= - pal * pos / 64
	add al, dl						; al= 63 - pos * 63 / 64 - pal * pos / 64
	mov es:[di], al
	inc si
	inc di
	dec cx
	jnz @@loop
	ret
endp

end