
.386
.MODEL FLAT

INCLUDE TRIG.INC

.DATA

;
; trig table made like this macro:
;
; I=0
; REPT 640
;	DD	65536 * SIN(I * 3.14159 / 256)
;	I = I + 1
; ENDM
;

LABEL TrigTable DWORD
INCLUDE SINTAB.INC

;
; acos table made like this macro:
;
; I = 0
; REPT 513
; 	DW	K * (ACOS(I / 256 - 1) / 3.14159 - 0.5)
; I = I + 1
; ENDM
;

LABEL ACosTable WORD
INCLUDE ACOSTAB.INC

.CODE

END
