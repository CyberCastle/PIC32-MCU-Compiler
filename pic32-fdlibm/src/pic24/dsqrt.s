; $Id: dsqrt.s,v 1.3.2.2 2012/07/23 18:10:03 johnsonl Exp $

; Square root of IEEE754 double dst = sqrt(dst)

        .section .libfd.sqrt, code

        .global _sqrt, _sqrtl

        .equ    EDOM,33                 ; Domain error

; Register use:
; w0-w3 dst
; w4-w7 temp
; w8    dexp
; w9    temp
; w10   dsgn
; w11   temp
; w12   temp
; w13   temp

_sqrt:
_sqrtl:
; Save registers
        push.d  w12
        push.d  w10
        push.d  w8

; Unpack operand
        mov     #0x8000,w12
        and     w3,w12,w10              ; dsgn
        lsr     w3,#4,w8
        bclr    w8,#11                  ; dexp
        and     w3,#0x0f,w3
        cp0     w8
        bra     z,$5
        bset    w3,#4                   ; dman

; NaN check
$5:     mov     #0x7ff,w12
        cp      w8,w12                  ; infinity or NaN ?
        bra     neq,$10
        and     w3,#0x0f,w13
        ior     w13,w2,w13
        ior     w13,w1,w13
        ior     w13,w0,w13
        bra     z,$10                   ; infinity ?
        bra     $85                     ; return the NaN

; Denormal check
$10:    btst    w3,#4
        bra     nz,$15                  ; normal ?
        ior     w0,w1,w13
        ior     w13,w2,w13
        ior     w13,w3,w13
        bra     z,$15                   ; zero ?
        inc     w8,w8                   ; no, minimum exponent

; Special operand check
$15:    cp0     w8
        bra     z,$85                   ; zero --> zero
        cp0     w10
        bra     z,$20
        bclr    w10,#15
        mov     w12,w8
        mov     #0x1f,w3
        mov     #0xffff,w2
        mul.su  w2,#1,w0
        mov     #EDOM,w13
        mov     w13,_errno
        bra     $85                     ; < 0 ? QNaN
$20:    cp      w8,w12
        bra     eq,$85                  ; +infinity --> +infinity
        
; Normalize
$25:    btst    w3,#4
        bra     nz,$30
        add     w0,w0,w0
        addc    w1,w1,w1
        addc    w2,w2,w2
        addc    w3,w3,w3
        dec     w8,w8
        bra     $25

; Compute exponent, align, set up for loop
$30:    mov     #0x3ff,w13
        sub     w8,w13,w12
        lsr     w12,w8
        add     w8,w13,w8               ; result exponent

        add     w0,w0,w4
        addc    w1,w1,w5
        addc    w2,w2,w6
        addc    w3,w3,w7                ; remainder

        mul.uu  w0,#0,w0
        mul.uu  w2,#0,w2                ; root

; Square root loop(s)
        mov     #0x20,w13               ; test bit
        btst    w12,#0
        bra     z,$40                   ; even exponent ?
        
$35:    add     w4,w4,w4
        addc    w5,w5,w5
        addc    w6,w6,w6
        addc    w7,w7,w7                ; shift remainder left
        
$40:    ior     w3,w13,w12              ; or in test bit
        sub     w7,w12,w7               ; subtract q + test
        bra     ltu,$45                 ; too much ?

        add     w12,w13,w3              ; add in q bit
        rrnc    w13,w13                 ; shift test bit right
        bra     nn,$35
        bra     $50

$45:    add     w7,w12,w7               ; restore remainder
        rrnc    w13,w13                 ; shift test bit right
        bra     nn,$35

$50:    add     w4,w4,w4
        addc    w5,w5,w5
        addc    w6,w6,w6
        addc    w7,w7,w7                ; shift remainder left
        
        ior     w2,w13,w12              ; or in test bit
        sub     w6,w12,w6
        subb    w7,w3,w7                ; subtract q + test
        bra     ltu,$55                 ; too much ?

        add     w12,w13,w2
        addc    w3,#0,w3                ; add in q bit
        rrnc    w13,w13                 ; shift test bit right
        bra     nn,$50
        bra     $60

$55:    add     w6,w12,w6
        addc    w7,w3,w7                ; restore remainder
        rrnc    w13,w13                 ; shift test bit right
        bra     nn,$50

$60:    add     w4,w4,w4
        addc    w5,w5,w5
        addc    w6,w6,w6
        addc    w7,w7,w7                ; shift remainder left
        
        ior     w1,w13,w12              ; or in test bit
        sub     w5,w12,w5
        subb    w6,w2,w6
        subb    w7,w3,w7                ; subtract q + test
        bra     ltu,$65                 ; too much ?

        add     w12,w13,w1
        addc    w2,#0,w2                ; add in q bit
        rrnc    w13,w13                 ; shift test bit right
        bra     nn,$60
        bra     $70

$65:    add     w5,w12,w5
        addc    w6,w2,w6
        addc    w7,w3,w7                ; restore remainder
        rrnc    w13,w13                 ; shift test bit right
        bra     nn,$60

$70:    add     w4,w4,w4
        addc    w5,w5,w5
        addc    w6,w6,w6
        addc    w7,w7,w7                ; shift remainder left
        
        ior     w0,w13,w12              ; or in test bit
        sub     w4,w12,w4
        subb    w5,w1,w5
        subb    w6,w2,w6
        subb    w7,w3,w7                ; subtract q + test
        bra     ltu,$75                 ; too much ?

        add     w12,w13,w0
        addc    w1,#0,w1                ; add in q bit
        rrnc    w13,w13                 ; shift test bit right
        bra     nn,$70
        bra     $80

$75:    add     w4,w12,w4
        addc    w5,w1,w5
        addc    w6,w2,w6
        addc    w7,w3,w7                ; restore remainder
        rrnc    w13,w13                 ; shift test bit right
        bra     nn,$70

; Guard/round is low bit
$80:    lsr     w3,w3
        rrc     w2,w2
        rrc     w1,w1
        rrc     w0,w0
        lsr     w3,w3
        rrc     w2,w2
        rrc     w1,w1
        rrc     w0,w0
        bra     nc,$85

; Round
        addc    w0,#0,w0
        addc    w1,#0,w1
        addc    w2,#0,w2
        addc    w3,#0,w3
        btst    w3,#5
        bra     z,$85
        lsr     w3,w3
        inc     w8,w8
        
; Pack result
$85:    sl      w8,#4,w8                ; dexp
        and.b   w3,#0x0f,w10            ; dsgn + dman
        ior     w10,w8,w3

; Restore registers
        pop.d   w8
        pop.d   w10
        pop.d   w12
        return  

        .end
