.386
.MODEL FLAT
LOCALS

INCLUDE PHONG.INC
INCLUDE VGA.INC

.DATA

.CODE

;
; remap palette
; al = start index
; ecx = # of colors to remap
; esi = palette adr
;

vgaRemapPal PROC
	mov dx, 3C8H						; pal index port
	out dx, al							; set start index
	inc dx								; pal write-data port
	lea ecx, [ecx * 2 + ecx]			; count = colors * 3
	rep outsb							; remap
	ret	
ENDP

END
