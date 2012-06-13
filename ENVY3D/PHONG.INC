
; ---------------- PROTOTYPES ----------------

GLOBAL PhongMapPtr:DWORD
GLOBAL DivTablePtr:DWORD

GLOBAL DrawFace:PROC
GLOBAL MakeDivTable:PROC
GLOBAL LoadPhongMap:PROC

; ---------------- DATA STRUCTURE ----------------

sPHONGV STRUC							; phong vertex structure
	sP_x	DW	?	; projected x
	sP_y	DW	?	; projected y
	sP_u	DW	?	; u coord in map
	sP_v	DW	?	; v coord in map
ENDS

sDELTA  STRUC							; delta stepping structure
	sD_x	DD	?	; x/y step
	sD_u	DD	?	; u/y step
	sD_v	DD	?	; v/y step
ENDS

; ---------------- MACRO ----------------

_SetPhongV MACRO Vect, x, y, u, v
	mov Vect.sP_x, x
	mov Vect.sP_y, y	
	mov Vect.sP_u, u	
	mov Vect.sP_v, v	
ENDM

_TableDivide MACRO den	; eax / den (den must be a 32 bit reg)
	movsx eax, ax						; eax = x
	add eax, 320						; eax = (x + 320)
	shl eax, 2							; eax = (x + 320)*4
	lea edx, [den*4 + den]				; edx = y*5
	shl edx, 9							; edx = y*5*512 = y*(320*2*4)
	add edx, eax						; edx = (x + 320)*4 + y*640*4
	add edx, DivTablePtr				; edx = &DivTable[(x + 320 + y*640)*4]
	mov eax, [edx]						; eax = DivTable[(x + 320 + y*640)*4]
ENDM
