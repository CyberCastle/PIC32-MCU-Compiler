; $Id: cosl.s,v 1.2 2012/05/08 20:15:14 wilkiec Exp $

; Long double functions jump through to double (64 bit) functions

        .section .libfd.cosl, code

        .global _cosl
_cosl:  
        bra    _cos
        
        .end
