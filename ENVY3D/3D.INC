
sVertex STRUC
	sV_x		DD	?				; vertex x
	sV_y		DD	?				; : y
	sV_z		DD	?				; : z
	sV_nx		DD	?				; vertex normal x
	sV_ny		DD	?				; : y
	sV_nz		DD	?				; : z
	sV_px		DW	?				; projected x
	sV_py		DW	?				; : y
ENDS

sFace STRUC
	sF_vlist	DD	4 DUP(?)		; face vertex list
	sF_nz		DD	?				; normal z
	sF_cz		DD	?				; center z
ENDS

GLOBAL VertexTot:DWORD
GLOBAL VertexPtr:DWORD
GLOBAL FaceTot:DWORD
GLOBAL FacePtr:DWORD
GLOBAL VisFacePtrPtr:DWORD
GLOBAL VisFacePtrTot:DWORD
GLOBAL RSortBlockPtr:DWORD

GLOBAL ProjectVertices:PROC
GLOBAL BackFaceCulling:PROC
GLOBAL DrawFaceList:PROC
GLOBAL RotateObject:PROC
GLOBAL SortVisFaceList:PROC
GLOBAL RSortVisFaceList:PROC

