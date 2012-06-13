;
;Pix's BMode ( byte mode )
;
; PRJ: BMODE routines ( 256 x 200 x 256 )
;CODE: pix - fferoldi@mbox.vol.it - federico@logicom.it
; MOD: bmode.asm 

.model  compact,c
locals

.code

.386

;
;       crt reg list    DB      idx, val
;       ends with 0,0
;

bmode_crt       db      13h, 32
                db      0, 64 + 16
                db      1, 63
                db      2, 69
                db      3, 113
                db      4, 72
                db      5, 112
                dw      0

;
; C near callable as
; void set_bmode ( void )
;

public  set_bmode

set_bmode       proc    uses dx bx ax

        mov     ax,13h          ;set 320 x 200 x 256
        int     10h

        mov     dx, 3d4h        ;CRT register
                
        mov     al, 11h         ;CRT protect OFF
        out     dx, al
        inc     dx
        in      al, dx
        and     al, 7fh
        out     dx, al
        dec     dx        

        xor     bx, bx          ;bx = 0
        
@@loop:
        mov     ax, word ptr bmode_crt[ bx ]    ;load idx and value
        and     ax, ax                  ;end of list?
        jz      @@exit                  ;yes -> exit
        out     dx, ax                  ;no -> outs
        inc     bx
        inc     bx
        jmp     @@loop                  ;next
        
@@exit:
        mov     al, 11h         ;CRT protect ON
        out     dx, al
        inc     dx
        in      al, dx
        or      al, 128
        out     dx, al
        
        ret                     ;exit
set_bmode       endp

delta_x dw      ?
delta_y dw      ?
inc_x   dd      ?
inc_y   dd      ?

public bline

bline   proc    uses es si di, x0:word, y0:word, x1:word, y1:word, col:word

        mov     bx, x1
        sub     bx, x0          ;bx = Dx
        
        mov     delta_x, bx     ;save Dx
        
        mov     cx, y1
        sub     cx, y0          ;cx = Dy

        mov     delta_y, cx     ;save Dy

                                ;cx = Dy ; bx = Dx
        and     bx,bx
        jns     @@bx_pos
        neg     bx
@@bx_pos:                       ;bx = abs( Dx )
        and     cx,cx
        jns     @@cx_pos
        neg     cx
@@cx_pos:                       ;cx = abs( Dy )

        cmp     cx, bx          
        jnb     @@cx_max        ;jmp if cx >= bx
        mov     cx, bx
@@cx_max:                       ;cx = max( cx, bx )
        and     ecx, 0ffh       ;ecx = Count 
        jz      @@exit          ;exit if linelen = 0
        
        movsx   eax, delta_x    ;eax = Dx
        cdq                     ;edx:eax = Dx
        shl     eax, 8          ;edx:eax = Dx * 2^8
        idiv    ecx             ;eax = Dx * 2^8 / Count
        ;mov     inc_x, eax      ;save inc_x

        ;mov     esi, inc_x      ;movzx
        mov     esi, eax
        ror     esi, 8
        
        movsx   eax, delta_y    ;eax = Dy
        cdq                     ;edx:eax = Dy
        shl     eax, 8          ;edx:eax = Dy * 2^8
        idiv    ecx             ;eax = Dy * 2^8 / Count
        ;mov     inc_y, eax      ;save inc_y
        
        ;mov     edx, inc_y
        mov     edx, eax
        ror     edx, 8 
                
        
        xor     ebx, ebx
        mov     bl, byte ptr x0 ;ebx = |      x.l      |   0    |   x.h  |
                
        xor     eax, eax
        mov     al, byte ptr y0 ;eax = |      y.l      |   0    |   y.h  |

        mov     ch, byte ptr col        ;cx = |   col  |  count |

        mov     di, 0A000h
        mov     es, di

        align   2
@@inner:
        mov     bh, al
        mov     es:[ bx ], ch
        add     ebx, esi
        adc     bl, 0
        add     eax, edx
        adc     al, 0
        dec     cl
        jnz     @@inner
@@exit:
        ret
bline   endp

end
                   
