; $Id: frexpl.s,v 1.2 2012/05/08 20:15:14 wilkiec Exp $

; Long double functions jump through to double (64 bit) functions

        .section .libfd.frexpl, code

        .global _frexpl
_frexpl:        
        bra    _frexp
        
        .end
