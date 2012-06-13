
GLOBAL TrigTable:DWORD
GLOBAL ACosTable:WORD

;
; MACRO _Sin dst, src		src=sin(dst)
; src and dst must be 32 bit regs  
; src must be in the range 0-511 (511 = 2pi)
;

_Sin MACRO dst, src
	mov dst, TrigTable[src * 4]
ENDM

;
; MACRO _Cos dst, src		src=sin(dst)
; src and dst must be 32 bit regs  
; src must be in the range 0-511 (511 = 2pi)
;

_Cos MACRO dst, src
	mov dst, TrigTable[src * 4 + 128*4]
ENDM

