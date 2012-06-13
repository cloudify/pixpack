
.386
.MODEL FLAT
LOCALS

INCLUDE MEM.INC

.CODE

;
; allocate a memory block (int 31H, 0EE42H)
; edx = size
; return:	edx = ptr to memory block
;			eax = size of allocated memory block
;			CF set in case of error
;

Alloc PROC 
	mov ax, 0EE42H
	int 31H
	jnc @@ok
	and eax, eax
	jz @@ok
	mov ax, 0EE40H
	int 31H
	stc
@@ok:
	ret
ENDP

END
