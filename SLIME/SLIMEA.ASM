;
; slime rotozoom
; code by pix - fferoldi@mbox.vol.it -federico@logicom.it
;

.model small, c
locals
.386

.data

extrn Picture:byte
extrn Trig:dword
extrn SlimeTable:dword
extrn SlimePos:word

.code

PosX		dw	?
PosY		dw	?

StartX		dd	?
StartY		dd	?

DeltaX		dd	?
DeltaY		dd	?

OldDeltaX	dd	?
OldDeltaY	dd	?

CountY		db	?

global DrawPic:proc

proc DrawPic uses es ds si di bp, StX:dword, StY:dword, DlX:dword, DlY:dword, Zoom:dword
	mov		ax, 0A000h
	mov		es, ax
	xor		di, di
	mov		ax, seg Picture
	mov		ds, ax
	mov		si, SlimePos
	shl		si, 2

	mov		eax, StX
	mov		StartX, eax
	mov		eax, StY
	mov		StartY, eax

	mov		ebx, Zoom

	mov		eax, DlX
	imul	ebx
	shrd	eax, edx, 16
	mov		DeltaX, eax
	mov		OldDeltaX, eax

	mov		eax, DlY
	imul	ebx
	shrd	eax, edx, 16
	mov		DeltaY, eax
	mov		OldDeltaY, eax

	mov		ax, word ptr DeltaX
	shl		eax, 16

	mov		cx, word ptr StartX
	shl		ecx, 16

	xor		edx, edx
	mov		dl, byte ptr DeltaY[2]

	mov		bx, word ptr DeltaY
	shl		ebx, 16
	mov		bx, word ptr DeltaX[2]
	mov		ebp, ebx

	mov		ebx, StartY
	shl		ebx, 16

UNROLLED=16

	mov		cx, (320 / UNROLLED) OR (200 SHL 8)
@@loopY:
	mov		bh, byte ptr StartY[2]
	mov		bl, byte ptr StartX[2]
	and		ecx, 0FFFFh
	and		ebx, 0FFFFh

@@loopX:
I=0
rept UNROLLED
	and		bh, 7fh	;and		bx, 7FFFh
	mov		dh, ds:Picture[bx]
	mov		es:[di], dh
	add		ecx, eax
	adc		ebx, ebp
	adc		bh, dl
	inc		di
I=I+1
endm
	dec		cl
	jnz		@@loopX
	mov		cl, 320/UNROLLED

	;mov		esi, DeltaX
	;add		StartY, esi
	;mov		esi, DeltaY
	;sub		StartX, esi
	mov		bx, word ptr DeltaX[0]
	add		word ptr StartY, bx
	mov		bx, word ptr DeltaX[2]
	adc		word ptr StartY[2], bx

	mov		bx, word ptr DeltaY[0]
	sub		word ptr StartX, bx
	mov		bx, word ptr DeltaY[2]
	sbb		word ptr StartX[2], bx

	;mov		bx, word ptr Trig[si + 0]
	;add		bx, word ptr OldDeltaX
	;mov		word ptr DeltaX, bx
	;mov		bx, word ptr Trig[si + 2]
	;adc		bx, word ptr OldDeltaX[2]
	;mov		word ptr DeltaX[2], bx

	mov		bx, word ptr SlimeTable[si + 0 + 64*4]
	add		bx, word ptr OldDeltaY
	mov		word ptr DeltaY, bx
	mov		bx, word ptr SlimeTable[si + 2 + 64*4]
	adc		bx, word ptr OldDeltaY[2]
	mov		word ptr DeltaY[2], bx

	add		si, 4

	dec		ch
	jnz		@@loopY

	ret
endp DrawPic

end
