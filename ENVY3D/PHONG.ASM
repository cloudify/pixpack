
.386
.MODEL FLAT
LOCALS

INCLUDE VBUFFER.INC
INCLUDE PHONG.INC

; ---------------- VARIABLES ----------------

;
; Map BYTE [256 x 256]	texture map, in (u, v) coord
;

.DATA	; ================ DATA SEGMENT ================

PhongMapPtr			DD		0	; 256x256 bytes + 768 (palette)
DivTablePtr			DD 		0	; 320x256 dword?!?

; ---------------- LOCAL VARIABLES ----------------

; -------- VERTEX DESCRIPIONS --------

A sPHONGV <>	; vertex A
B sPHONGV <>	; vertex B
C sPHONGV <>	; vertex C
D sPHONGV <>	; vertex D

; -------- Y DIFF --------

ABy	DW	?		; AB y-diff
ACy	DW	?		; AC y-diff
ADy	DW  ?       ; AD y-diff
BCy	DW	?       ; BC y-diff
BDy	DW	?       ; BD y-diff
CDy	DW	?       ; CD y-diff

; -------- DELTA --------

dAB	sDELTA  <>	; AB x, u, v delta
dAC sDELTA  <>  ; AC x, u, v delta
dAD sDELTA  <>  ; AD x, u, v delta
dBC sDELTA  <>  ; BC x, u, v delta
dBD sDELTA  <>  ; BD x, u, v delta
dCD sDELTA  <>  ; CD x, u, v delta

dTL sDELTA  <>	; top-left delta
dML sDELTA  <>	; mid-left delta
dBL sDELTA  <>	; bottom-left delta
dTR sDELTA  <>	; top-right delta
dMR sDELTA  <>	; mid-right delta
dBR sDELTA  <>	; bottom right Delta

; -------- SCANLINE --------

Lx				DD		?	; left-x for current scanline
Rx  			DD 	 	?	; right-x for current scanline
Ru				DD		?	; left-u for current scanline
Lu  			DD		?	; right-u for current scanline
Rv  			DD		?   ; left-v for current scanline
Lv  			DD		?   ; right-v for current scanline

y				DW		?	; y of current scanline
scanl_off		DD		?

dU				DD		?
dV				DD		?

U				DD		?
V				DD		?

; -------- TABLES --------

LABEL Mul50
I=0
REPT 200
	DW I
I=I+50
ENDM

.CODE	; ================ CODE SEGMENT ================

; ---------------- MACRO DEFINITION ----------------

_StoreV MACRO D, pS		; copy phong vertex D <- *pS
	mov eax, DWORD PTR [pS + 0]
	mov DWORD PTR D[0], eax
	mov eax, DWORD PTR [pS + 4]
	mov DWORD PTR D[4], eax
ENDM

_CopyV MACRO D, S		; copy phong vertex D <- S
	mov D, S
ENDM

_SwapV MACRO V0, V1		; swap two vertex V0 <-> V1
	xchg V0, V1
ENDM

_CmpVx MACRO V0, V1		; sort V0, V1 by x -> V0.x < V1.x
LOCAL @@ng
	mov ax, [V0].sP_x
	cmp ax, [V1].sP_x
	jng @@ng
	_SwapV V0, V1
@@ng:
ENDM

_CmpVy MACRO V0, V1		; sort V0, V1 by y -> V0.y < V1.y
LOCAL @@ng
	mov ax, [V0].sP_y
	cmp ax, [V1].sP_y
	jng @@ng
	_SwapV V0, V1
@@ng:
ENDM

_CopyD MACRO D, S       ; copy delta var D <- S
	push eax
	mov eax, S.sD_x
	mov D.sD_x, eax
	mov eax, S.sD_u
	mov D.sD_u, eax
	mov eax, S.sD_v
	mov D.sD_v, eax
	pop eax
ENDM

_CalcDiff MACRO _ABy, _A, _B	; _ABy <- _B.y - _A.y
	mov ax, _B.sP_y
	sub ax, _A.sP_y
	mov _ABy, ax
ENDM

_Divide MACRO _A, _B	; eax = ((B - A) << 16) / ebx
	mov ax, WORD PTR _B
	sub ax, WORD PTR _A					; ax = B - A

	movsx eax, ax						; eax = x
	add eax, 320						; eax = (x + 320)
	shl eax, 2							; eax = (x + 320)*4
	lea edx, [ebx*4 + ebx]				; edx = y*5
	shl edx, 9							; edx = y*5*512 = y*(320*2*4)
	add edx, eax						; edx = (x + 320)*4 + y*640*4
	add edx, DivTablePtr				; edx = &DivTable[(x + 320 + y*640)*4]
	mov eax, [edx]						; eax = DivTable[(x + 320 + y*640)*4]
ENDM

_CalcStepX MACRO _dAB, _A, _B   
	_Divide _A.sP_x, _B.sP_x
	mov _dAB.sD_x, eax
ENDM

_CalcStepU MACRO _dAB, _A, _B	
	_Divide _A.sP_u, _B.sP_u
	mov _dAB.sD_u, eax
ENDM

_CalcStepV MACRO _dAB, _A, _B	
	_Divide _A.sP_v, _B.sP_v
	mov _dAB.sD_v, eax
ENDM

;
; MACRO: draw a textured scan line
; from Lx to Rx, scanline y
;

DrawScanLine MACRO 
LOCAL @@return, @@hloop, @@delta_zero
	movsx eax, word ptr Lx[2]
	mov edi, scanl_off
	add edi, eax

	movsx ebx, word ptr Rx[2]			; ebx = scanline end x
	sub ebx, eax						; ebx = x1 - x0
	js @@return							; if x1 < x0 exit
	jz @@delta_zero						; if x1 == x0 don't calc deltas

	_Divide Lu[2], Ru[2]
	mov dU, eax

	_Divide Lv[2], Rv[2]
	mov dV, eax

@@delta_zero:
	mov cx, bx
	inc cx

	mov eax, Lu
	mov U, eax
	mov eax, Lv
	mov V, eax

	xor ebx, ebx

	mov esi, PhongMapPtr

@@hloop:
	mov bh, BYTE PTR V[2]
	mov bl, BYTE PTR U[2]
	mov al, [esi + ebx]
	mov [edi], al
	inc edi
	mov eax, dU
	add U, eax
	mov eax, dV
	add V, eax
	dec cx
	jnz @@hloop
@@return:
ENDM


; ---------------- PROCEDURES ----------------

;
; draw a face
; esi = pA
; edi = pB
; ebx = pC
; ebp = pD

DrawFace PROC 
	; ordina bubble sort x/y
	_CmpVx esi, edi
	_CmpVx esi, ebx
	_CmpVx esi, ebp
	_CmpVx edi, ebx
	_CmpVx edi, ebp
	_CmpVx ebx, ebp
	_CmpVy esi, edi
	_CmpVy esi, ebx
	_CmpVy esi, ebp
	_CmpVy edi, ebx
	_CmpVy edi, ebp
	_CmpVy ebx, ebp

	_StoreV A, esi
	_StoreV B, edi
	_StoreV C, ebx
	_StoreV D, ebp

	_CalcDiff ABy, A, B		; ax = ABy = B.y - A.y
	movsx ebx, ax
	jz @@ABy_z
	_CalcStepX dAB, A, B
	_CalcStepU dAB, A, B
	_CalcStepV dAB, A, B
	jmp @@Calc_dAC
@@ABy_z:
	mov dAB.sD_x, 0fffffffh;

@@Calc_dAC:
	_CalcDiff ACy, A, C
	movsx ebx, ax
	jz @@ACy_z
	_CalcStepX dAC, A, C
	jmp @@Calc_dAD
@@ACy_z:
	mov dAC.sD_x, 0fffffffh;

@@Calc_dAD:
	_CalcDiff ADy, A, D
	movsx ebx, ax
	jz @@ADy_z
	_CalcStepX dAD, A, D
	jmp @@Calc_dCD
@@ADy_z:
	mov dAD.sD_x, 0fffffffh;

@@Calc_dCD:
	_CalcDiff CDy, C, D
	movsx ebx, ax
	jz @@CDy_z
	_CalcStepX dCD, C, D
	_CalcStepU dCD, C, D
	_CalcStepV dCD, C, D
	jmp @@check_delta1
@@CDy_z:
	mov dCD.sD_x, 0fffffffh;

@@check_delta1:
	mov	eax, dAD.sD_x	; eax = dAD.x
	cmp dAB.sD_x, eax	; dAB.x < dAD.x ?
	jnl @@check_delta2	; jmp if not
	cmp dAC.sD_x, eax	; dAC.x < dAD.x ?
	jnl @@check_delta2	; jmp if not

	_CalcDiff BCy, B, C
	movsx ebx, ax
	jz @@BCy_z1
	_CalcStepX dBC, B, C
	_CalcStepU dBC, B, C
	_CalcStepV dBC, B, C
@@BCy_z1:

	movsx ebx, ADy
	and ebx, ebx
	jz @@ADy_z2
	_CalcStepU dAD, A, D
	_CalcStepV dAD, A, D
@@ADy_z2:

	_CopyD dTL, dAB
	_CopyD dBL, dCD
	_CopyD dML, dBC
	_CopyD dTR, dAD
	_CopyD dMR, dAD
	_CopyD dBR, dAD
	jmp @@draw

@@check_delta2:
	mov	eax, dAD.sD_x	; eax = dAD.x
	cmp dAB.sD_x, eax	; dAB.x > dAD.x ?
	jng @@check_delta3	; jmp if not
	cmp dAC.sD_x, eax	; dAC.x > dAD.x ?
	jng @@check_delta3	; jmp if not

	_CalcDiff BCy, B, C
	movsx ebx, ax
	jz @@BCy_z2
	_CalcStepX dBC, B, C
	_CalcStepU dBC, B, C
	_CalcStepV dBC, B, C
@@BCy_z2:

	movsx ebx, ADy
	and ebx, ebx
	jz @@ADy_z3
	_CalcStepU dAD, A, D
	_CalcStepV dAD, A, D
@@ADy_z3:

	_CopyD dTR, dAB
	_CopyD dBR, dCD
	_CopyD dMR, dBC
	_CopyD dTL, dAD
	_CopyD dML, dAD
	_CopyD dBL, dAD
	jmp @@draw

@@check_delta3:
	mov eax, dAB.sD_x	; eax = dAB.x
	cmp eax, dAC.sD_x	; dAB.x < dAC.x ?
	jnl @@delta4		; jmp if not

	_CalcDiff BDy, B, D
	movsx ebx, ax
	jz @@BDy_z1
	_CalcStepX dBD, B, D
	_CalcStepU dBD, B, D
	_CalcStepV dBD, B, D
@@BDy_z1:

	movsx ebx, ACy
	and ebx ,ebx
	jz @@ACy_z2
	_CalcStepU dAC, A, C
	_CalcStepV dAC, A, C
@@ACy_z2:

	_CopyD dTL, dAB
	_CopyD dBR, dCD
	_CopyD dML, dBD
	_CopyD dBL, dBD
	_CopyD dTR, dAC
	_CopyD dMR, dAC
	jmp @@draw

@@delta4:

	_CalcDiff BDy, B, D
	movsx ebx, ax
	jz @@BDy_z3
	_CalcStepX dBD, B, D
	_CalcStepU dBD, B, D
	_CalcStepV dBD, B, D
@@BDy_z3:

	movsx ebx, ACy
	and ebx ,ebx
	jz @@ACy_z3
	_CalcStepU dAC, A, C
	_CalcStepV dAC, A, C
@@ACy_z3:

	_CopyD dTR, dAB
	_CopyD dBL, dCD
	_CopyD dMR, dBD
	_CopyD dBR, dBD
	_CopyD dTL, dAC
	_CopyD dML, dAC
	jmp @@draw

@@draw:

	movsx eax, A.sP_y
	mov y, ax
	lea edx, [eax + eax * 4]			; edx = y * 5
	shl edx, 6							; edx = y * 320
	add edx, VBufferPtr					; edx = &VBuffer[320 * y]
	mov scanl_off, edx

	cmp ax, B.sP_y
	jnb @@else1

	mov ax, A.sP_x
	shl eax, 16
	mov Rx, eax
	mov Lx, eax
	mov ax, A.sP_u
	shl eax, 16
	mov Ru, eax
	mov Lu, eax
	mov ax, A.sP_v
	shl eax, 16
	mov Rv, eax
	mov Lv, eax

@@draw_top_loop:
	DrawScanLine
    mov eax, dTR.sD_x
	add Rx, eax
	mov eax, dTL.sD_x
	add Lx, eax
	mov eax, dTR.sD_u
	add Ru, eax
	mov eax, dTL.sD_u
	add Lu, eax
	mov eax, dTR.sD_v
	add Rv, eax
	mov eax, dTL.sD_v
	add Lv, eax
	inc y
	add scanl_off, 320

	mov ax, y
	cmp ax, B.sP_y
	jb @@draw_top_loop
	jmp @@draw_mid

@@else1:
	mov ax, A.sP_x
	shl eax, 16
	mov Lx, eax
	mov ax, B.sP_x
	shl eax, 16
	mov Rx, eax
	mov ax, A.sP_u
	shl eax, 16
	mov Lu, eax
    mov ax, B.sP_u
	shl eax, 16
	mov Ru, eax
	mov ax, A.sP_v
	shl eax, 16
	mov Lv, eax
    mov ax, B.sP_v
	shl eax, 16
	mov Rv, eax

@@draw_mid:

	DrawScanLine 

@@draw_mid_loop:
	
	mov ax, y
	cmp ax, C.sP_y
	jnb @@draw_bot
	
	mov eax, dMR.sD_x
	add Rx, eax
	mov eax, dML.sD_x
	add Lx, eax
	mov eax, dMR.sD_u
	add Ru, eax
	mov eax, dML.sD_u
	add Lu, eax
	mov eax, dMR.sD_v
	add Rv, eax
	mov eax, dML.sD_v
	add Lv, eax
	inc y
	add scanl_off, 320

	DrawScanLine 
	jmp @@draw_mid_loop

@@draw_bot:

@@draw_bot_loop:
	
	mov ax, y
	cmp ax, D.sP_y
	jnb @@draw_end
	
	mov eax, dBR.sD_x
	add Rx, eax
	mov eax, dBL.sD_x
	add Lx, eax
	mov eax, dBR.sD_u
	add Ru, eax
	mov eax, dBL.sD_u
	add Lu, eax
	mov eax, dBR.sD_v
	add Rv, eax
	mov eax, dBL.sD_v
	add Lv, eax
	inc y
	add scanl_off, 320
		
	DrawScanLine 
	jmp @@draw_bot_loop
@@draw_end:
	ret
ENDP

;
; make the division table like this macro:
; 
; A=-320
; REPT 640
; 	B=0
;	REPT 200
;		DD		A IDIV B
;	B = B + 1
;	ENDM
; A = A + 1
; ENDM
;
; div-by-zero will result with +/- infinite (very big) value
;

MakeDivTable PROC
	push edi eax ebx ecx edx
	mov edi, DivTablePtr				; edi = [DivTable]
	mov ecx, 0							; B=0
@@divide0:
	mov ebx, -320						; A=-320
@@divide1:	
	and ecx, ecx						; B == 0 ?
	jz @@dzero							; : then jmp to @@dzero
	mov eax, ebx						; eax = A 
	shl eax, 16							; eax = A << 16
	cdq									; edx:eax = A << 16
	idiv ecx							; eax = (A << 16) / B
	jmp @@store
@@dzero:								; if div-by-zero
	mov eax, 0FFFFFFFH					; set eax to infinite
	and ebx, ebx						; test for sign
	jns @@store							; if negative
	neg eax								; negate eax
@@store:
	mov [edi], eax						; store value
	add edi, 4							; next dword
	inc ebx								; inc A
	cmp ebx, 320						; if A < 320
	jl @@divide1						; : loop to @@divide1
	inc ecx								; inc B
	cmp ecx, 320						; cx < 320 ?
	jb @@divide0						; : then loop to @@divide0
	pop edx ecx ebx eax edi
	ret
ENDP

.DATA

szPhongMapName		DB		"phongmap.bin", 0
szPhongPalName		DB		"phongpal.bin", 0

.CODE

;
; load phong_map and phong_map_palette
;

LoadPhongMap PROC
	mov ax, 3D00H							; open file
	mov edx, OFFSET szPhongMapName		; : phong_map 
	int 21H
	jc @@return
	mov bx, ax							; bx = ax = handle
	mov ah, 3FH							; read from file
	mov ecx, 65536						; : 256*256 bytes
	mov edx, PhongMapPtr				; : to phong_map_ptr
	int 21H
	jc @@return
	mov ah, 3EH							; close file
	int 21H
	
	mov ax, 3D00H						; open file
	mov edx, OFFSET szPhongPalName		; : phong_pal
	int 21H
	jc @@return
	mov bx, ax							; bx = ax = handle
	mov ah, 3FH							; read from file
	mov ecx, 768						; : 768 bytes
	mov edx, PhongMapPtr				; : to phong_map_ptr + 65536
	add edx, 65536
	int 21H
	jc @@return
	mov ah, 3EH							; close file
	int 21H
@@return:
	ret
ENDP

END
