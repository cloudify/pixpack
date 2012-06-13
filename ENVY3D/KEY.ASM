;
; **** KEYBOARD HANDLER ****
; based on code by James Ketrenos (ketrenoj@cs.pdx.edu)
; modified by Federico Feroldi - Pix / NeMeSiS
;

.386
.MODEL FLAT

PIC_CMD			EQU	20H
NONSPEC_EOI		EQU 20H

INCLUDE KEY.INC

.DATA

kbKeys				DB		256 DUP (0)
kbKeyNumPress		DB		0
kbKeyLast			DB		0
kbOldKeyLast		DB		0

toASCII				DB		0,27,'1234567890-=',14,15
					DB		'qwertyuiop',0,0,13,0,'as'
					DB		'dfghjkl',0,0,'''',0,0,'zxcv'
					DB		'bnm',0,'./',0,0,0,' ',0,1,2,3,4,5
					DB		6,7,8,9,10,0,0,24,25,26,'-',21,22,23,0,18
					DB		19,20,16,17,0,0,0,11,12,0,0,0,0,0,0,0
					DB		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
					DB		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
					DB		0,27,'!',0,'#',0,'%',0,0,0,0,0,0,0,14,15
					DB		'QWERTYUIOP',0,0,13,0,'AS'
					DB		'DFGHJKL:',0,'"',0,0,'ZXCV'
					DB		'BNM',0,0,'?',0,0,0,' ',0,1,2,3,4,5
					DB		6,7,8,9,10,0,0,24,25,26,'-',21,22,23,0,18
					DB		19,20,16,17,0,0,0,11,12,0,0,0,0,0,0,0
					DB		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
					DB		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
e0flag				DB		0
oldint9off			DD		?
oldint9sel			DW		?
keyinst				DB		0
keyhead				DB		0
keytail				DB		0
keybuffer			DB		256 DUP(0)
keyrepeat			DB		256 DUP(0)

.CODE

kbClearKeys PROC
	push	bx
    mov		bl, keyhead
    mov		keytail, bl
    mov		[kbKeyLast], 0
    pop		bx
    ret
ENDP

kbGetKey PROC
	push	eax ebx
	xor		eax, eax
	movzx	ebx, keytail				; point to the end of the buffer
	cmp		bl, keyhead					; : and if it is equal to the front,
	jz		GKDone						; : then the buffer is empty, so wait

	mov		al, keybuffer[ebx]			; fetch the ASCII Code
	inc		[keytail]					; increment the KeyTail
										; : NOTE that it loops at bl==256
	cmp		al, 1Bh						; esc
	jne		GKDone
	cmp		keyrepeat[1], 2				; dont want Esc repeating
	jb		GKDone
	xor		al, al
GKDone:
	mov		[kbKeyLast],al
	pop		ebx eax
	ret									; return to caller and all that stuff
ENDP

kbSetNewHandler PROC
	cmp 	[keyinst], 1
	je  	exitnew9

	mov 	[keyinst], 1
	cli
	pushad

	mov 	ax, 0204H
	mov 	bl, 09H
	int 	31H

	mov 	[oldint9off], edx
	mov 	[oldint9sel], cx

	mov 	ax, 0205H
	mov 	bl, 09H
	mov 	edx, offset kbNewHandler
	mov 	cx, cs
	int		31H

	popad
	sti
exitnew9:
	ret
ENDP

kbSetOldHandler PROC
	cmp		[keyinst], 0
	je		exitold9

	mov		[keyinst], 0
	cli
	pushad

	mov		ax, 0205H
	mov		bl, 09H
    mov		edx, [oldint9off]
    mov		cx, [oldint9sel]
	int		31H

	popad
	sti
exitold9:
	ret
ENDP

kbNewHandler PROC
	push	eax ebx ecx ds
	mov		ax, @data
	mov		ds, ax

	in		al,60h
	mov		ah,al

	cmp		ah,0E0h
	jae		e0flagset

	movzx	ebx,ah
	and		bl,01111111b
	add		bl,[e0flag]
	mov		[e0flag],0

	rol		ah,1
	jc		keyrelease

keypress:
	mov     ah,1
	sub     ah,kbKeys[ebx]           ; old key status
	add     [kbKeyNumPress],ah
	mov     kbKeys[ebx],1            ; key pressed
	mov     bh,kbKeys[2ah]           ; get left shift status
	or      bh,kbKeys[36h]           ; get right shift status
	ror     bh,1                    ; put in bit 7
	add     bl,bh                   ; final key value
	xor     bh,bh                   ; clear for index use
	mov     al,toASCII[ebx]         ; get translated value

	movzx   ecx,keyhead             ;; Point to the front of the buffer
	inc     cl                      ;; : and if the next space places
	cmp     cl,keytail              ;; : us on the end of the buffer,
	jz      int9_done               ;; : then the buffer is full ...
	dec     cl                      ;; : otherwise, we help fill it.

	mov     keybuffer[ecx],al       ;; Save the ASCII Code into buffer
	inc     cl                      ;; Increment the KeyHead
	mov     keyhead,cl              ;; : NOTE that it loops at bl==256
	cmp     keyrepeat[ebx],2
	je      int9_done
	inc     keyrepeat[ebx]
	jmp     int9_done

keyrelease:
	dec     [kbKeyNumPress]
	mov     kbKeys[ebx],0            ; key released
	mov     keyrepeat[ebx],0
	jmp     int9_done

e0flagset:      
	mov     [e0flag],128
int9_done:      
	mov     al,NONSPEC_EOI
	out     PIC_CMD,al
	pop     ds ecx ebx eax
	iretd
ENDP

END
