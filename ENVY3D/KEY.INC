;
; **** KEYBOARD HANDLER ****
; based on code by James Ketrenos (ketrenoj@cs.pdx.edu)
; modified by Federico Feroldi - Pix / NeMeSiS
;

GLOBAL kbKeys:BYTE
GLOBAL kbKeyNumPress:BYTE
GLOBAL kbKeyLast:BYTE
GLOBAL kbOldKeyLast:BYTE

GLOBAL kbClearKeys:PROC
GLOBAL kbGetKey:PROC
GLOBAL kbSetNewHandler:PROC
GLOBAL kbSetOldHandler:PROC

_WaitKeyPress MACRO
LOCAL @@loop
@@loop:
	cmp kbKeyNumPress, 0
	je @@loop
ENDM

