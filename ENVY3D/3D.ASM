
.386
.MODEL FLAT
LOCALS
JUMPS

INCLUDE MISC.INC
INCLUDE 3D.INC
INCLUDE PHONG.INC
INCLUDE TRIG.INC

.DATA

VertexTot		DD	?				; # of vertices
VertexPtr		DD	?				; ptr to vertex list
FaceTot			DD	?				; # of faces
FacePtr			DD	?				; ptr to face list
VisFacePtrPtr	DD	?				; ptr to sorted list of ptr to faces
VisFacePtrTot	DD	?				; # of ptr to faces

.CODE

;
; project vertex list
; (update sV_px, sV_py)
;

ProjectVertices PROC
	mov edi, VertexPtr					; edi points to vertex list
	mov ecx, VertexTot					; ecx = # of vertex
@@loop:
	mov ebx, [edi].sV_z					; ebx = z
	add ebx, 256 * 65536				; ebx = (z + 256F)
	jle @@next							; skip if ebx <= 0

	mov eax, [edi].sV_x					; eax = x
	cdq									; edx:eax = x
	shld edx, eax, 8					; edx:eax = x * 256
	shl eax, 8 	
	idiv ebx							; eax = (x * 256) / (x + 256F)
	add ax, 160
	mov [edi].sV_px, ax					; save projected x

	mov eax, [edi].sV_y					; eax = y                      
	cdq									; edx:eax = y                  
	shld edx, eax, 8					; edx:eax = y * 256            
	shl eax, 8	
	idiv ebx							; eax = (y * 256) / (y + 256F) 
	neg ax
	add ax, 100
	mov [edi].sV_py, ax					; save projected y             
@@next:
	add edi, SIZE sVertex
	dec ecx
	jnz @@loop
	ret
ENDP

;
; backface culling
; calculate face normal_z and
; fill VisFacePtr list with visible faces
;

.DATA

v0x		DD	?
v0y		DD	?
v1x		DD	?
v1y		DD	?
v2x		DD	?
v2y		DD	?

.CODE

BackFaceCulling PROC
	mov esi, FacePtr					; esi = ptr to face
	mov edi, VisFacePtrPtr				; edi = ptr to face ptr list
	mov VisFacePtrTot, 0				; face ptr list len = 0
	mov ecx, FaceTot					; ecx = face counter
@@loop:
	mov edx, [esi].sF_vlist[0*4]		; cal face center
	mov eax, [edx].sV_z
	mov edx, [esi].sF_vlist[1*4]
	add eax, [edx].sV_z
	mov edx, [esi].sF_vlist[2*4]
	add eax, [edx].sV_z
	mov edx, [esi].sF_vlist[3*4]
	add eax, [edx].sV_z
	sar eax, 2
	mov [esi].sF_cz, eax

	mov ebx, [esi].sF_vlist[4 * 0]
	mov ax, [ebx].sV_py
	shl eax, 16
	mov v0x, eax					
	mov ax, [ebx].sV_px
	shl eax, 16
	mov v0y, eax
	
	mov ebx, [esi].sF_vlist[4 * 1]
	mov ax, [ebx].sV_py
	shl eax, 16
	mov v1x, eax
	mov ax, [ebx].sV_px
	shl eax, 16
	mov v1y, eax

	mov ebx, [esi].sF_vlist[4 * 2]
	mov ax, [ebx].sV_py
	shl eax, 16
	mov v2x, eax
	mov ax, [ebx].sV_px
	shl eax, 16
	mov v2y, eax
	
	mov eax, v0y					
	sub v1y, eax					; _v1y = v1y - v0y
	mov eax, v0x
	sub v2x, eax					; _v2x = v2x - v0x
	mov eax, v0x
	sub v1x, eax					; _v1x = v1x - v0x
	mov eax, v0y
	sub v2y, eax					; _v2y = v2y - v0y

	mov eax, v1y
	mov edx, v2x
	imul edx
	shrd eax, edx, 16
	mov v1y, eax					; _v1y = (v1y - v0y) * (v2x - v0x)

	mov eax, v1x
	mov edx, v2y
	imul edx
	shrd eax, edx, 16
	mov v1x, eax					; _v1x = (v1x - v0x) * (v2y - v0y)

	mov eax, v1y
	sub eax, v1x					; eax = face normal z

	mov [esi].sF_nz, eax			; save in face structure

	and eax, eax					; check if visible face
	jns @@next						; if not jmp to next face

	mov [edi], esi					; save visible face ptr in

	add edi, 4
	inc VisFacePtrTot

@@next:
	add esi, SIZE sFace
	dec ecx
	jnz @@loop
	ret
ENDP

;
; draw object
;

.DATA

PhV0	sPhongV		<>
PhV1	sPhongV		<>
PhV2	sPhongV		<>
PhV3	sPhongV		<>

.CODE

_GetEnvyCoord MACRO
	mov edx, [edi].sV_ny
	sar edx, 8
	add edx, 256
	mov ax, ACosTable[edx*2]
	shl eax, 16
	mov edx, [edi].sV_nx
	sar edx, 8
	add edx, 256
	mov ax, ACosTable[edx*2]
ENDM

DrawFaceList PROC
	mov esi, VisFacePtrPtr				; ptr to face ptr
	mov ecx, VisFacePtrTot				; # of ptr to face

@@loop:

	mov ebx, [esi]						; ebx = ptr to face

	mov edi, [ebx].sF_vlist[4 * 0]		; edi = ptr to vertex 0
	mov eax, dword ptr [edi].sV_px		; eax = vertex y:x
	mov dword ptr PhV0.sP_x, eax		
	_GetEnvyCoord
	mov dword ptr PhV0.sP_u, eax

	mov edi, [ebx].sF_vlist[4 * 1]
	mov eax, dword ptr [edi].sV_px
	mov dword ptr PhV1.sP_x, eax
	_GetEnvyCoord
	mov dword ptr PhV1.sP_u, eax

	mov edi, [ebx].sF_vlist[4 * 2]
	mov eax, dword ptr [edi].sV_px
	mov dword ptr PhV2.sP_x, eax
	_GetEnvyCoord
	mov dword ptr PhV2.sP_u, eax

	mov edi, [ebx].sF_vlist[4 * 3]
	mov eax, dword ptr [edi].sV_px
	mov dword ptr PhV3.sP_x, eax
	_GetEnvyCoord
	mov dword ptr PhV3.sP_u, eax

	push esi ecx
	
	mov esi, OFFSET PhV0
	mov edi, OFFSET PhV1
	mov ebx, OFFSET PhV2
	mov ebp, OFFSET PhV3
	call DrawFace

	pop ecx esi

	add esi, 4
	
	dec ecx
	jnz @@loop

	ret
ENDP

.DATA

sinx		DD		?
siny		DD		?
sinz		DD		?
cosx		DD		?
cosy		DD		?
cosz		DD		?
tmatrix		DD		3*3 DUP(?)
tx			DD		?
ty			DD		?
tz			DD		?

.CODE

;
; rotate object
; eax = x angle
; ebx = y angle
; ecx = z angle
;

RotateObject PROC
	_Sin edx, eax
	mov sinx, edx
	_Sin edx, ebx
	mov siny, edx
	_Sin edx, ecx
	mov sinz, edx

	_Cos edx, eax
	mov cosx, edx
	_Cos edx, ebx
	mov cosy, edx
	_Cos edx, ecx
	mov cosz, edx

	mov eax, cosy
	mov edx, cosz
	imul edx
	shrd eax, edx, 16
	mov tmatrix[4*(3*0 + 0)], eax	; matrix[0][0]=cy * cz

	mov eax, sinz
	mov edx, cosy
	imul edx
	shrd eax, edx, 16
	mov tmatrix[4*(3*0 + 1)], eax	; matrix[0][1]=sz * cy

	mov eax, siny
	neg eax
	mov tmatrix[4*(3*0 + 2)], eax	; matrix[0][1]=-sy

	mov eax, sinx
	mov edx, siny
	imul edx
	shrd eax, edx, 16
	mov edx, cosz
	imul edx
	shrd eax, edx, 16
	mov ebx, eax
	mov eax, sinz
	mov edx, cosx
	imul edx
	shrd eax, edx, 16
	sub ebx, eax
	mov tmatrix[4*(3*1 + 0)], ebx	; matrix[1][0]=sx*sy*cz - sz*cx

	mov eax, sinx
	mov edx, siny
	imul edx
	shrd eax, edx, 16
	mov edx, sinz
	imul edx
	shrd eax, edx, 16
	mov ebx, eax
	mov eax, cosx
	mov edx, cosz
	imul edx
	shrd eax, edx, 16
	add ebx, eax
	mov tmatrix[4*(3*1 + 1)], ebx	; matrix[1][1]=sx*sy*sz + cx*cz

	mov eax, sinx
	mov edx, cosy
	imul edx
	shrd eax, edx, 16
	mov tmatrix[4*(3*1 + 2)], eax	; matrix[1][2]=sx * cy
	
	mov eax, siny
	mov edx, cosx
	imul edx
	shrd eax, edx, 16
	mov edx, cosz
	imul edx
	shrd eax, edx, 16
	mov ebx, eax
	mov eax, sinx
	mov edx, sinz
	imul edx
	shrd eax, edx, 16
	add ebx, eax
	mov tmatrix[4*(3*2 + 0)], ebx	; matrix[2][0]=sy*cx*cz + sx*sz

	mov eax, siny
	mov edx, sinz
	imul edx
	shrd eax, edx, 16
	mov edx, cosx
	imul edx
	shrd eax, edx, 16
	mov ebx, eax
	mov eax, sinx
	mov edx, cosz
	imul edx
	shrd eax, edx, 16
	sub ebx, eax
	mov tmatrix[4*(3*2 + 1)], ebx	; matrix[2][1]=sy*sz*cx - sx*cz

	mov eax, cosx
	mov edx, cosy
	imul edx
	shrd eax, edx, 16
	mov tmatrix[4*(3*2 + 2)], eax	; matrix[2][2]=cx*cy

	mov edi, VertexPtr
	mov ecx, VertexTot

@@loop:
	mov eax, [edi].sV_x					; eax = x
	mov edx, tmatrix[4*(3*0 + 0)]
	imul edx
	shrd eax, edx, 16					; eax = x*[0][0]
	mov ebx, eax						; ebx = x*[0][0]
	mov eax, [edi].sV_y					; eax = y
	mov edx, tmatrix[4*(3*1 + 0)]	
	imul edx
	shrd eax, edx, 16					; eax = y*[1][0]
	add ebx, eax						; ebx = x*[0][0]+y*[1][0]
	mov eax, [edi].sV_z					; eax = z
	mov edx, tmatrix[4*(3*2 + 0)]
	imul edx
	shrd eax, edx, 16					; eax = z*[2][0]
	add ebx, eax						; ebx = x*[0][0]+y*[1][0]+z*[2][0]
	mov tx, ebx

	mov eax, [edi].sV_x
	mov edx, tmatrix[4*(3*0 + 1)]
	imul edx
	shrd eax, edx, 16
	mov ebx, eax
	mov eax, [edi].sV_y
	mov edx, tmatrix[4*(3*1 + 1)]
	imul edx
	shrd eax, edx, 16
	add ebx, eax
	mov eax, [edi].sV_z
	mov edx, tmatrix[4*(3*2 + 1)]
	imul edx
	shrd eax, edx, 16
	add ebx, eax
	mov ty, ebx

	mov eax, [edi].sV_x
	mov edx, tmatrix[4*(3*0 + 2)]
	imul edx
	shrd eax, edx, 16
	mov ebx, eax
	mov eax, [edi].sV_y
	mov edx, tmatrix[4*(3*1 + 2)]
	imul edx
	shrd eax, edx, 16
	add ebx, eax
	mov eax, [edi].sV_z
	mov edx, tmatrix[4*(3*2 + 2)]
	imul edx
	shrd eax, edx, 16
	add ebx, eax
	mov tz, ebx

	mov eax, tx
	mov [edi].sV_x, eax
	mov eax, ty
	mov [edi].sV_y, eax
	mov eax, tz
	mov [edi].sV_z, eax

; normals
	mov eax, [edi].sV_nx				; eax = x
	mov edx, tmatrix[4*(3*0 + 0)]
	imul edx
	shrd eax, edx, 16					; eax = x*[0][0]
	mov ebx, eax						; ebx = x*[0][0]
	mov eax, [edi].sV_ny					; eax = y
	mov edx, tmatrix[4*(3*1 + 0)]	
	imul edx
	shrd eax, edx, 16					; eax = y*[1][0]
	add ebx, eax						; ebx = x*[0][0]+y*[1][0]
	mov eax, [edi].sV_nz					; eax = z
	mov edx, tmatrix[4*(3*2 + 0)]
	imul edx
	shrd eax, edx, 16					; eax = z*[2][0]
	add ebx, eax						; ebx = x*[0][0]+y*[1][0]+z*[2][0]
	mov tx, ebx

	mov eax, [edi].sV_nx
	mov edx, tmatrix[4*(3*0 + 1)]
	imul edx
	shrd eax, edx, 16
	mov ebx, eax
	mov eax, [edi].sV_ny
	mov edx, tmatrix[4*(3*1 + 1)]
	imul edx
	shrd eax, edx, 16
	add ebx, eax
	mov eax, [edi].sV_nz
	mov edx, tmatrix[4*(3*2 + 1)]
	imul edx
	shrd eax, edx, 16
	add ebx, eax
	mov ty, ebx

	mov eax, [edi].sV_nx
	mov edx, tmatrix[4*(3*0 + 2)]
	imul edx
	shrd eax, edx, 16
	mov ebx, eax
	mov eax, [edi].sV_ny
	mov edx, tmatrix[4*(3*1 + 2)]
	imul edx
	shrd eax, edx, 16
	add ebx, eax
	mov eax, [edi].sV_nz
	mov edx, tmatrix[4*(3*2 + 2)]
	imul edx
	shrd eax, edx, 16
	add ebx, eax
	mov tz, ebx

	mov eax, tx
	mov [edi].sV_nx, eax
	mov eax, ty
	mov [edi].sV_ny, eax
	mov eax, tz
	mov [edi].sV_nz, eax
				  

	add edi, SIZE sVertex
	dec ecx
	jnz @@loop
	ret
ENDP

;
; sort faces (bubble)
;

SortVisFaceList PROC
   	mov esi, VisFacePtrPtr				; esi = &&aface
	mov ecx, VisFacePtrTot				; ecx = acount
@@ext_loop:
	mov eax, [esi]						; eax = &aface
	lea edi, [esi + 4] 					; edi = &&bface = &&aface + 1
	mov ebx, ecx	 					; ebx = bcount = acount - 1
@@in_loop:
	mov edx, [edi]						; edx = &bface
	mov edx, [edx].sF_cz				; edx = &bface->cz
	cmp edx, [eax].sF_cz				; &bface->cz < &aface->cz
	jnl @@in_next						; if true don't swap
	mov edx, [edi]						;\ 
	xchg edx, [esi]						; > xchg [edi]<->[esi]
	mov [edi], edx						;/
@@in_next:
	add edi, 4							; &&bface++
	dec ebx
	jnz @@in_loop
	add esi, 4							; &&aface++
	dec ecx
	jnz @@ext_loop
	ret
ENDP

.DATA

RSortBlockPtr		DD	?
RSortPtrList		DD 	16 DUP(?)
RSortCountList		DD	16 DUP(?)

.CODE

;
; sort visible faces (radix)
;

RSortVisFaceList PROC
NIBBLE=0
REPT 8
LOCAL @@init_loop, @@sort_loop
	mov ecx, 16							; ecx = bucket counter
	mov ebx, RSortBlockPtr				; ebx = ptr to bucket-block
	xor edi, edi						; edi = 0
	xor eax, eax						; eax = 0
	mov edx, FaceTot					; edx = dwords per bucket
	shl edx, 2							; edx = bytes per bucket
@@init_loop:
	mov RSortCountList[edi], eax		; zero ecx.bucket counter 
	mov RSortPtrList[edi], ebx			; set ecx.bucket ptr to ebx
	add edi, 4							; next bucket
	add ebx, edx						; ptr to next bucket
	dec ecx								; do loop
	jnz @@init_loop

	mov ecx, VisFacePtrTot				; ecx = # of visible faces
	mov esi, VisFacePtrPtr				; esi = &&vface
@@sort_loop:
	mov edx, [esi]						; edx = &vface
	mov ebx, [edx].sF_cz				; ebx = &vface->cz
	xor ebx, 7FFFFFFFH
	shr ebx, NIBBLE * 4					; ebx >>= NIBBLE*4
	and ebx, 15							; ebx &= 15
	shl ebx, 2							; ebx = bucket# * 4
	mov eax, RSortPtrList[ebx]			; eax = &((cz>>(NIBBLE*4))&15).bucket
	mov [eax], edx						; *eax = &vface
	add eax, 4							; eax += 4
	mov RSortPtrList[ebx], eax			; &((cz>>(NIBBLE*4))&15).bucket = eax
	inc RSortCountList[ebx]				; ((cz>>(NIBBLE*4))&15).bucket.len++
	add esi, 4							; &&vface++
	dec ecx								; do loop
	jnz @@sort_loop

	mov edi, VisFacePtrPtr				; edi = &&vface
	mov ebx, RSortBlockPtr				; ebx = ptr to bucket block
	mov edx, FaceTot					; edx = bucket len (dwords)
	shl edx, 2							; edx = bucket len (bytes)
I=0
REPT 16
	mov esi, ebx						; src = I.bucket
	mov ecx, RSortCountList[I*4]		; count = I.bucket.len (dws)
	rep movsd							; copy
	add ebx, edx						; add offset
I=I+1
ENDM
NIBBLE=NIBBLE+1	
ENDM
	ret
ENDP


END
