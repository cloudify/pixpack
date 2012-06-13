.386
.MODEL FLAT
LOCALS

INCLUDE MAIN.INC
INCLUDE 3DO.INC
INCLUDE DOSFUNC.INC
INCLUDE 3D.INC
INCLUDE MEM.INC

.DATA

FileHandle		DW	?		; handle of current file

LABEL Header				; 3DO header data
HeaderID		DD	?		; : ID must be 3DOB
HeaderLen		DD	?		; : file length (currently ignored)
HeaderVert		DD	?		; : # of vertices
HeaderFace		DD	?		; : # of faces

LABEL Chunk
ChunkID			DD	?		; current chunk ID
ChunkLen		DD	?		; current chunk length

; -- strings --

strNVertex		DB	" - loading 3DO", 10, 13
				DB	" : # of vertices: $"
strNFace		DB	10, 13, " : # of faces: $"
strA3DO			DB	10, 13, " > allocating memory, ret: $"
strVPtr			DB  10, 13, " : vertex list ptr: $"
strFPtr			DB	10, 13, " : face list ptr: $"
strRChunk		DB	10, 13, " > reading chunks", 10, 13, "$"
strChunkFound	DB	" : chunk  len:$"
strChunkName	DB	" id:'xxxx' ", 10, 13, "$"
strRVList		DB	" : > reading vertex list", 10, 13, "$"
strRFList		DB	" : > reading face list", 10, 13, "$"
strRVNor		DB	" : > reading vertex normal list", 10, 13, "$"

.CODE

;
; load a 3do object, and allocate memory
; edx = offset ASCIZ filename
; return:	ax = error code
;				00xxH : xx = dos error code
;				0101H : bad 3DO ID
;				0102H : mem alloc error
;

Load3DO PROC
	mov ax, 3D00H
	int 21H
	jc @@return
	mov FileHandle, ax
	mov bx, ax

	mov ah, 3FH	
	mov edx, OFFSET Header				; read 3do header
	mov ecx, 4 * 4
	int 21H
	cmp HeaderID, CHUNK_3DO_3DOB		; check header ID
	jne @@bad_id

; write some information
	
	_dosWriteString strNVertex	
	mov edx, HeaderVert
	call PrintHexDD
	_dosWriteString strNFace
	mov edx, HeaderFace
	call PrintHexDD
	_dosWriteString strA3DO
	call Alloc3DO						; allocate object memory
	mov edx, eax
	call PrintHexDD
	jc @@return
	_dosWriteString strVPtr
	mov edx, VertexPtr
	call PrintHexDD
	_dosWriteString strFPtr
	mov edx, FacePtr
	call PrintHexDD
	_dosWriteString strRChunk

@@readchunkheader:
	mov ah, 3FH
	mov edx, OFFSET Chunk				; read current chunk header
	mov ecx, 4 * 2
	int 21H
	and eax, eax						; jmp if eof
	jz @@eof

	_dosWriteString strChunkFound
	mov edx, ChunkLen
	call PrintHexDD
	mov eax, ChunkID
	mov DWORD PTR strChunkName[5], eax
	_dosWriteString strChunkName

; [ check chunk id ]
	cmp ChunkID, CHUNK_3DO_VLST
	je @@read_vlst
	cmp ChunkID, CHUNK_3DO_FLST
	je @@read_flst
	cmp ChunkID, CHUNK_3DO_VNOR
	je @@read_vnor

@@jumpnext:
	mov ax, 4201H						; set relative file position
	mov edx, ChunkLen					; : move forward by ChunkLen bytes
	int 21H
	jmp @@readchunkheader

; [ read vertex list ]
@@read_vlst:
	call ReadVList
	jmp @@readchunkheader
@@read_flst:
	call ReadFList
	jmp @@readchunkheader
@@read_vnor:
	call ReadVNor
	jmp @@readchunkheader

@@eof:
	mov ah, 3EH							; close file
	int 21H
	xor ax, ax							; return code = 0
@@return:								; general return
	ret
@@bad_id:
	mov ax, 0101H						; bad 3DO ID
	ret
ENDP

;
; allocate memory for 3DO object
; return:	ax = error code
;				0000 : ok
;				0102 : alloc error
;

Alloc3DO PROC
	mov eax, HeaderVert					; eax = # of vertices
	mov VertexTot, eax					; save in VertexTot
	mov edx, SIZE sVertex				; edx = size of vertex structure
	mul edx								; eax = necessary memory
	mov edx, eax						
	call Alloc							; allocate memory block
	jc @@error							; if cf set jmp error
	mov VertexPtr, edx					; : else save ptr in VertexPtr

	mov eax, HeaderFace					; eax = # of faces
	mov FaceTot, eax					; save in FaceTot             
	mov edx, SIZE sFace					; edx = size of face structure
	mul edx								; eax = necessary memory        
	mov edx, eax						                                
	call Alloc							; allocate memory block         
	jc @@error							; if cf set jmp error           
	mov FacePtr, edx					; : else save ptr in FacePtr  

	mov edx, FaceTot					; edx = # of faces
	shl edx, 2							; edx = # of faces * 4
	call Alloc							; alloc
	jc @@error
	mov VisFacePtrPtr, edx

	xor ax, ax							; clear error code
	ret			
@@error:
	mov ax, 0102H						; alloc error
	ret
ENDP

;
; read vertex list
; bx = file handle
;

ReadVList PROC
	push eax ebx ecx edx
	_dosWriteString strRVList 
	mov ecx, VertexTot
	mov edx, VertexPtr
@@loop:
	and ecx, ecx
	jz @@return
	dec ecx
	push ecx

	mov ah, 3FH
	mov ecx, 4 * 3
	int 21H
	add edx, SIZE sVertex

	pop ecx
	jmp @@loop
@@return:
	pop edx ecx ebx eax
	ret
ENDP

;
; read face list
; bx = file handle
;

.DATA

FaceSideTot		DD	?

.CODE

ReadFList PROC
	push eax ebx ecx edx edi
   	_dosWriteString strRFList 
	mov ecx, FaceTot					; # of faces
	mov edx, FacePtr					; ptr to facelist
@@loop:
	and ecx, ecx						; check if no more faces
	jz @@return
	dec ecx
	push ecx							; save face counter

	push edx							; save face ptr
	mov ah, 3FH
	mov ecx, 4							; read # of sides in
	mov edx, OFFSET FaceSideTot			; : next face
	int 21H
	pop edx								; restore face ptr
	
	;cmp FaceSideTot, 4
	;jne @@next

	mov ah, 3FH							; read face data
	mov ecx, 4 * 4						; : at current face ptr
	int 21H
	
	push edx							; save face ptr

	mov edi, edx						; edi = ptr to face

	mov eax, [edi].sF_vlist[0 * 4]		; eax = index of vertex 0
	mov edx, SIZE sVertex				; edx = size of each vertex
	mul edx								; edx:eax = vertex offset in vlist
	add eax, VertexPtr					; eax = vertex offset
	mov [edi].sF_vlist[0 * 4], eax		; save vertex offset

	mov eax, [edi].sF_vlist[1 * 4]
	mov edx, SIZE sVertex
	mul edx
	add eax, VertexPtr
	mov [edi].sF_vlist[1 * 4], eax

	mov eax, [edi].sF_vlist[2 * 4]
	mov edx, SIZE sVertex
	mul edx
	add eax, VertexPtr
	mov [edi].sF_vlist[2 * 4], eax

	mov eax, [edi].sF_vlist[3 * 4]
	mov edx, SIZE sVertex
	mul edx
	add eax, VertexPtr
	mov [edi].sF_vlist[3 * 4], eax
	
	pop edx								; restore face ptr
	add edx, SIZE sFace					; ptr to next face
	
	push edx							; save face ptr
	mov ax, 4201H						
	mov edx, 4 * 8 - 4 * 4				; skip unreaded data
	int 21H
	pop edx								; restore face ptr

@@next:
	pop ecx								; restore face counter
	jmp @@loop							; loop
@@return:
	pop edi edx ecx ebx eax
	ret
ENDP

;
; read vertex normal list
; bx = file handle
;

ReadVNor PROC
	push eax ebx ecx edx
	_dosWriteString strRVNor
	mov ecx, VertexTot
	mov edx, VertexPtr
	add edx, sVertex.sV_nx
@@loop:
	and ecx, ecx
	jz @@return
	dec ecx
	push ecx

	mov ah, 3FH
	mov ecx, 4 * 3
	int 21H
	add edx, SIZE sVertex

	pop ecx
	jmp @@loop
@@return:
	pop edx ecx ebx eax
	ret
ENDP

END
