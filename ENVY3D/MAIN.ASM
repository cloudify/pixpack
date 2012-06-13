
LOCALS
.386
.MODEL FLAT
.STACK 1000H

; ======== DEFINITIONS ========

;_DEBUG = 1
;_KBHANDLER = 1

; ======== INCLUDES ========

INCLUDE DOSFUNC.INC
INCLUDE VBUFFER.INC
INCLUDE VIDFUNC.INC
INCLUDE KEY.INC
INCLUDE MEM.INC
INCLUDE MAIN.INC
INCLUDE PHONG.INC
INCLUDE VGA.INC
INCLUDE 3DO.INC
INCLUDE 3D.INC

; ======== WRITEABLE NOT-EXECUTABLE SEGMENT ========

.DATA

; ---- EXTERNAL ----

; ---- DATA ----

; -- phisical addresses --

BasePtr         DD              ?
VideoPtr        DD              0A0000H
TextVideoPtr	DD				0B8000H

; -- strings --

szObjectName	DB				"object.3do", 0

strOk			DB				" ok"
strCR			DB				10, 13, "$"

; -- general --

PV0		sPhongV		<>
PV1		sPhongV		<>
PV2		sPhongV		<>
PV3		sPhongV		<>

FrameCount		DD	?
TimerCount		DD	?

; ======== READONLY EXECUTABLE SEGMENT ========

.CODE

IFDEF _DEBUG
	EXTRN Debug:NEAR
ENDIF

start:
IFDEF _DEBUG
	call Debug
ENDIF
	mov ax, 0EE02H
	int 31H
	mov BasePtr, ebx					; get prg base ptr
	sub VideoPtr, ebx					; adjust phisical address
	sub TextVideoPtr, eax				; : for video-memory access

	_WriteLn " - 3D TeSTiNG UnDeR P.MoDE - copyright 1995 Pix / N!P"

IFDEF _KBHANDLER
	_WriteLn " - installing keyboard handler"
	call kbSetNewHandler				; set new keyboard handler
ENDIF

	_Write " - alloc > virtual video buffer (64k): "
	mov edx, 64000						; 320x200 bytes
	call Alloc
	jc @@exit
	mov VBufferPtr, edx					; clear allocated buffer
	mov edi, edx
	mov ecx, 64000 / 4
	xor eax, eax
	rep stosd
	call PrintHexDD						; print address
	_dosWriteString strOk

	_Write " - alloc > phong map (64k): "
	mov edx, 65536 + 768				; 256x256 + 768
	call Alloc
	jc @@exit
	mov PhongMapPtr, edx
	call PrintHexDD
	_dosWriteString strOk

	_Write " - alloc > div table (820k): "
	mov edx, 819200						; 320x2x320 dword
	call Alloc
	jc @@exit
	mov DivTablePtr, edx
	call PrintHexDD
	_dosWriteString strOk

	_WriteLn " - making > div-table"
	call MakeDivTable

	_Write " - load > phong map/palette: "
	call LoadPhongMap					; : and palette
	_dosWriteString strOk

	mov edx, OFFSET szObjectName
	call Load3DO

	; jmp @@exit

	_Write " - alloc RSortBlock: "
	mov edx, FaceTot
	shl edx, 2 + 4
	call Alloc
	jc @@exit
	mov RSortBlockPtr, edx
	call PrintHexDD
	_dosWriteString strOk

	mov ax, 13H							; set video mode 13H
	int 10H								; : 320x200x256 color, chain

	mov esi, PhongMapPtr				; remap phong map
	add esi, 65536						; : palette
	mov ecx, 256						; 256 colors
	mov al, 0							; from index 0
	call vgaRemapPal

	mov ah, 0
	int 1AH
	shl ecx, 16
	mov cx, dx
	mov TimerCount, ecx					; read system timer count 18.2Hz

	mov FrameCount, 0					; reset frame counter

@@main_loop:

	call ProjectVertices
	call BackFaceCulling
	call RSortVisFaceList
	call DrawFaceList

	mov VBufferBound[0], 0
	mov VBufferBound[2], 0
	mov VBufferBound[4], 319
	mov VBufferBound[6], 199
	Call VBufferUpdate	

	mov eax, 1
	mov ebx, 2
	mov ecx, 3
	call RotateObject

	inc FrameCount

IFDEF _KBHANDLER
	cmp kbkeyNumPress, 0
ELSE
	mov ah, 1
	int 16H
ENDIF
je @@main_loop

@@exit:
	mov ax, 3H							; back to text mode
	int 10H

	_WriteLn " * DOS32 + 3D + .r.o.u.t.i.n.e.s. * CoDe: pix - nemesis"

	_Write " - counter ticks: "
	mov ah, 0
	int 1AH
	shl ecx, 16
	mov cx, dx
	sub ecx, TimerCount					; ecx = timer ticks
	mov edx, ecx
	call PrintHexDD
	_dosWriteString strCR

	_Write " - total frames: "
	mov edx, FrameCount
	call PrintHexDD
	_dosWriteString strCR	

IFDEF _KBHANDLER
	_WriteLn " - resetting old keyboard handler"
	call kbSetOldHandler				; reset old keyboard handler
ENDIF

	_Terminate 0						; terminate with ret code=0

.DATA

strHexDD	DB	"00000000H$"			; 32bit-hex string
Dec2Hex		DB	"0123456789ABCDEF"		; hex digits

.CODE

;
; print a dword in hex format
; edx = value 
;

PrintHexDD PROC							
	push eax ecx edx
	xor ecx, ecx
@@loop:
	rol edx, 4
	movzx eax, dl
	and al, 0FH
	mov al, Dec2Hex[eax]
	mov strHexDD[ecx], al
	inc ecx
	cmp ecx, 8
	jb @@loop
	_dosWriteString strHexDD
	pop edx ecx eax
	ret
ENDP

END start
