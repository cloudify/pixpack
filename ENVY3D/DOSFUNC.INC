
; ******** DOS SERVICES MACRO ********

;
; write a String to video, via int 21H service 09H
; String must terminate with '$'
; modifies EDX AH
;

_dosWriteString MACRO String
	mov edx, OFFSET String
	mov ah, 9
	int 21H
ENDM

;
; define and write a String to video, via int 21H service 09H
; a '$' will be appended to the string
; modifies EDX AH
;

_DefWriteString MACRO String
LOCAL @@lstring, @@write
	jmp @@write
@@lstring:
	DB	String, '$'
@@write:
	mov edx, OFFSET @@lstring
	mov ah, 9
	int 21H
ENDM

_Write MACRO String
LOCAL @@lstring, @@write
	jmp @@write
@@lstring:
	DB	String, '$'
@@write:
	mov edx, OFFSET @@lstring
	mov ah, 9
	int 21H
ENDM

_WriteLn MACRO String
LOCAL @@lstring, @@write
	jmp @@write
@@lstring:
	DB	String, 10, 13, '$'
@@write:
	mov edx, OFFSET @@lstring
	mov ah, 9
	int 21H
ENDM

;
; terminate with return code (int 21H, 4CH)
;

_Terminate MACRO RetCode
	mov al, RetCode
	mov ah, 4CH
	int 21H
ENDM

