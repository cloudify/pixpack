;
; plasma tunnel
; code by pix - fferoldi@mbox.vol.it - federico@logicom.it
;

.model large, c
locals

.data

extrn pAOffsetMap:dword
extrn pATexture:dword

.code

.386

global DrawScreen:proc

DrawScreen proc uses ds es fs si di bx dx, OffX:word, OffY:word
	mov	ax, 0A000h
	mov	es, ax
	xor	di, di

	mov fs, word ptr pATexture[2]
	lds si, dword ptr pAOffsetMap

	mov cx, (160*100/2) / 2
	xor bx, bx
@@loop:
REPT 2
	mov	ebx, [si]
	add si, 4

	mov dx, bx
	shr bx, 8
	add dl, RotList[bx]
	mov bx, dx
	mov al, fs:[bx]
	mov ah, al

	ror eax, 16
	shr ebx, 16

	mov dx, bx
	shr bx, 8
	add dl, RotList[bx]
	mov bx, dx
	mov al, fs:[bx]
	mov ah, al

	ror eax, 16

	mov es:[di], eax
	add di, 4

ENDM
	dec cx
	jnz @@loop

	ret
endp

global Smooth:proc

Smooth proc uses ds bx si di
	lds di, pATexture
	xor ax, ax
	xor dx, dx
	mov bx, OFFSET AvTable

	mov ch, 252; vcount = 256 rows
@@vloop:

	; bound check
;	mov si, bx
;	mov al, [bx]
;	inc bh
;	mov dl, [bx]
;	add ax, dx
;	dec bl
;	mov dl, [bx]
;	add ax, dx
;	inc bl
;	inc bl
;	mov dl, [bx]
;	add ax, dx
;	mov bx, si
;	shr ax, 2

	mov ax, [di + 256]
	add al, ah
	add al, [di + 256 + 255]
	add al, [di]

	xlat AvTable

	mov [di - 256], al
	inc di

	; 254 with no bound check
	mov cl, 254/8

	;xchg bx, di

@@hloop:
REPT 8
LOCAL @@nodec

;	mov al, [bx]			; AL = [+0.+0]
;	mov edx, [bx + 256 - 1]	; 3.2.1.0
;	rol edx, 16				; 1.0.3.2
;	and dx, 255				; 1.0.-.2
;	add ax, dx				; +=2
;	rol edx, 8 				; 0.-.2.1
;	and dx, 255				; 0.-.-.1
;	add ax, dx				; +=1
;	shr edx, 24				; -.-.-.0
;	add ax, dx				; +=0
;	shr ax, 2

	mov ax, [di + 256 - 1]
	add al, ah
	add al, [di + 256 + 1]
	add al, [di]

	xlat AvTable

	mov [di - 256], al
	inc di
ENDM

	dec cl
	jnz @@hloop

	;xchg di, bx

	; bound check
;	mov si, bx
;	mov al, [bx]
;	inc bh
;	mov dl, [bx]
;	add ax, dx
;	dec bl
;	mov dl, [bx]
;	add ax, dx
;	inc bl
;	inc bl
;	mov dl, [bx]
;	add ax, dx
;	mov bx, si
;	shr ax, 2
;	mov [bx - 256], al
;	inc bx

	mov ax, [di + 256 - 1]
	add ax, [di]
	add al, ah

	xlat AvTable

	mov [di - 256], al
	inc di

	; next roe
	dec ch
	jnz @@vloop

	ret
endp

global RotList:byte

RotList		db	256 dup(0)

global AvTable:byte

AvTable		db	256	dup(0)

end


; ROBA VECCHIA

Smooth proc uses ds bx si
	lds bx, pATexture
	mov cx, 256 * 250
	xor ax, ax
	xor dx, dx
@@loop:
	mov si, bx

	;inc bh
	mov al, [bx]
	inc bh
	mov dl, [bx]
	add ax, dx
	dec bl
	mov dl, [bx]
	add ax, dx
	inc bl
	inc bl
	mov dl, [bx]
	add ax, dx

	mov bx, si
	shr ax, 2
	mov [bx - 256], al
	inc bx
	dec cx
	jnz @@loop
	ret
endp
