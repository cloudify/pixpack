
; PRJ: blobs experiments
;CODE: pix - fferoldi@mbox.vol.it - federico@logicom.it
; MOD: blobs.asm / asm core

.model small,c
locals

.data

extrn   Palette:BYTE

.code

.386

;같같같같 QUAD_LIST: a list of 256 DWs, list[x]=x*x 같같같같 

LABEL   Quad_list
I=0
REPT    128
        DW      I*I     ;0..127
        I=I+1
ENDM
I=-128
REPT    128
        DW      I*I     ;-128..0
        I=I+1
ENDM

;같같같같 void DO_BLOB( Page:DW, x0:DW, y0:DW, x1:DW, y1:DW ) 같같같같 

public do_blob

do_blob proc    uses es di, pg:word, x0:word, y0:word, x1:word, y1:word
        
        mov     ax,pg
        add     ax,0A000h
        mov     es,ax
        xor     di,di

        mov     ch,100
        mov     cl,160
@loop:
                
        xor     bx,bx   ;
        mov     bl,cl   ;
        sub     bl,BYTE PTR x0  
        add     bx,bx   ;
        mov     si,Quad_list[bx]        ;si= (x-x0)^2

        xor     bx,bx        
        mov     bl,ch   
        sub     bl,BYTE PTR y0  
        add     bx,bx 
        add     si,Quad_list[bx]        ;si+= (y-y0)^2

        ;si= ( x - x0 ) ^ 2 + ( y - y0 ) ^2

        xor     bx,bx
        mov     bl,cl   ;al= x
        sub     bl,BYTE PTR x1   ;al= x-x1
        add     bx,bx
        mov     ax,Quad_list[bx]        ;ax= (x-x1)^2

        xor     bx,bx
        mov     bl,ch   
        sub     bl,BYTE PTR y1
        add     bx,bx
        add     ax,Quad_list[bx]        ;ax+= (y-y0)^2

        mov     bx,ax
        add     bx,si
        cmp     bx,9000
        jnb     @black_pixel

        mul     si

        shl     edx,16
        mov     dx,ax
        shr     edx,12

        inc     edx
        cmp     edx,254
        jb      @@1
@black_pixel:
        xor     dx,dx
@@1:
        mov     dh,dl
        ;stosw
        mov     es:[di],dx
        inc     di
        inc     di
        
        dec     cl
        jnz     @loop
@dec_ch:
        mov     cl,160
        dec     ch
        jnz     @loop
        ret
do_blob endp

end

