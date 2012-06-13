
.model compact,c

;x		SDWORD		0
;y		SDWORD		4
;sx		SDWORD		8
;sy		SDWORD		12
;old_offset	uint		16
;old_mask	uchar		18
;flags		uchar		19

.data

.code

locals

LABEL   voff_tab
I=0
REPT    480
        DW      I
        I= I + 80
ENDM

mask_tab        DB      128, 64, 32, 16, 8, 4, 2, 1

.386

public  draw

draw    proc    uses ds si es di, list:dword, len:word, video:word

        mov     ax,video
        or      ax,0A000h
        mov     es,ax
        
        lds     si,list
        
        mov     cx,len
@@loop:
        mov     bx,[ si + 6 ]           ;bx = Y (MSB)
        add     bx,bx                   ;bx *= 2
        mov     di,voff_tab[ bx ]       ;di = offset(y)
        mov     ax,[ si + 2 ]           ;ax = X (MSB)
        mov     bx,ax                   ;bx = ax
        shr     ax,3                    ;ax /= 8
        add     di,ax                   ;di += offset(x)
        and     bx,7                    ;bx &= 7
        mov     al,mask_tab[ bx ]       ;al = mask(x)
        
        mov     bx,[ si + 16 ]          ;bx = old_offset
        mov     ah,[ si + 18 ]          ;ah = old_mask
                          
        and     es:[ bx ],ah            ;clear old
        or      es:[ di ],al            ;draw current

        not     al                      ;invert mask                   
        mov     [ si + 16 ],di          ;save offset
        mov     [ si + 18 ],al          ;save mask

        add     si,20                   ;next point

        dec     cx                      ;continue?
        jnz     @@loop

        ret
draw    endp

public  move

move    proc    uses ds si, list:dword, len:word

        lds     si,list
        mov     cx,len
@@loop:
        mov     eax,[ si ]              ;eax = X
        add     eax,[ si + 8 ]          ;eax += SX
        js      @@x_under               ;jmp if ( X < 0 )
        cmp     eax,640 SHL 16          
        jb      @@restore_x             ;jmp if ( X < 640 )
        mov     eax,639 SHL 16          ;X = 639
        neg     dword ptr [ si + 8 ]
        jmp     short @@restore_x
@@x_under:
        xor     eax,eax                 ;X = 0
        neg     dword ptr [ si + 8 ]    ;SX = -SX
@@restore_x:
        mov     [ si ],eax              ;restore X

        mov     eax,[ si + 4 ]          ;eax = Y
        add     eax,[ si + 12 ]         ;eax += SY
        js      @@y_under               ;jmp if ( Y < 0 )
        cmp     eax,480 SHL 16          
        jb      @@restore_y             ;jmp if ( Y < 480 )
        mov     eax,479 SHL 16          ;Y = 479
        neg     dword ptr [ si + 12 ]
        jmp     short @@restore_y
@@y_under:
        xor     eax,eax                 ;Y = 0
        neg     dword ptr [ si + 12 ]   ;SY = -SY
@@restore_y:
        mov     [ si + 4 ],eax          ;restore Y

        add     si,20
        dec     cx
        jnz     @@loop

        ret
move    endp


public  move_gravity

move_gravity    proc    uses ds si, list:dword, len:word, grav:dword

        lds     si,list
        mov     ebx,grav
        mov     cx,len
@@loop:
        mov     eax,[ si ]              ;eax = X
        add     eax,[ si + 8 ]          ;eax += SX
        js      @@x_under               ;jmp if ( X < 0 )
        cmp     eax,640 SHL 16          
        jb      @@restore_x             ;jmp if ( X < 640 )
        mov     eax,639 SHL 16          ;X = 639
        neg     dword ptr [ si + 8 ]
        jmp     short @@restore_x
@@x_under:
        xor     eax,eax                 ;X = 0
        neg     dword ptr [ si + 8 ]    ;SX = -SX
@@restore_x:
        mov     [ si ],eax              ;restore X

        mov     eax,[ si + 4 ]          ;eax = Y
        add     eax,[ si + 12 ]         ;eax += SY
        js      @@y_under               ;jmp if ( Y < 0 )
        cmp     eax,480 SHL 16          
        jb      @@restore_y             ;jmp if ( Y < 480 )
        mov     eax,479 SHL 16          ;Y = 479
        neg     dword ptr [ si + 12 ]   ;SY = -SY
        jmp     short @@restore_y
@@y_under:
        xor     eax,eax                 ;Y = 0
        neg     dword ptr [ si + 12 ]   ;SY = -SY
@@restore_y:
        mov     [ si + 4 ],eax          ;restore Y
        add     [ si + 12 ],ebx
        
        add     si,20
        dec     cx
        jnz     @@loop

        ret
move_gravity    endp

end
