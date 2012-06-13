
.386
.MODEL FLAT
LOCALS

.DATA

; ---- INCLUDE ----

INCLUDE MAIN.INC
INCLUDE VBUFFER.INC

; ---- DATA ----

VBufferPtr		DD		?	;64000 DUP(15)
VBufferBound	DW		4 DUP(0)

CountX			DB		0
CountY			DB		0
DeltaX			DW		0

.CODE

VBufferUpdate PROC 
	push esi edi
	mov ax, VBufferBound[2 * 2]			; ax = x1
	sub ax, VBufferBound[0 * 2]			; ax = x1 - x0
	add ax, 3
	shr ax, 2							; ax = (x1 - x0) / 4
	mov CountX, al						; CountX = (x1 - x0) / 4
	neg ax								; ax = -ax
	add ax, 80							; ax = 80 - (x1 - x0) / 4
	shl ax, 2							; ax = 320 - 4 * ((x1 - x0) / 4)
	mov DeltaX, ax						; DeltaX = 320 - 4 * ((x1 - x0) / 4)

	mov ax, VBufferBound[3 * 2]			; ax = y1
	sub ax, VBufferBound[1 * 2]			; ax = y1 - y0
	mov CountY, al						; CountY = y1 - y0

	movzx esi, VBufferBound[1 * 2]		; esi = y0
	lea esi, [esi * 4 + esi]			; esi = esi * 5
	shl esi, 6							; esi = esi * 5 * 64 = esi * 320
	movsx eax, VBufferBound[0]			; eax = x0
	add esi, eax						; esi = 320 * y0 + x0
	mov edi, esi						; edi = 320 * y0 + x0
	add esi, VBufferPtr					; esi = VBuffer[320 * y0 + x0]
	add edi, VideoPtr					; edi = vid_mem[320 * y0 + x0]

	mov ch, CountY
	movsx edx, DeltaX
	mov bl, CountX
	mov cl, bl
@@loop:

	mov eax, [esi]
	mov [edi], eax
	add edi, 4
	xor eax, eax
	mov [esi], eax
	add esi, 4
	dec cl
	jnz @@loop
	mov cl, bl
	add esi, edx
	dec ch
	jnz @@loop
	pop edi esi
	ret
ENDP

END
