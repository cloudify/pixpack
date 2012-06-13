
; ******** INT 10H SERVICES ********

; 
; set video-mode to Mode
; modifies AX
;

_SetVideoMode MACRO Mode
	mov ax, Mode
	int 10H
ENDM
