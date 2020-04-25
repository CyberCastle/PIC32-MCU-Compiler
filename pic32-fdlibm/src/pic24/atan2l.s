; $Id: atan2l.s,v 1.2 2012/05/08 20:15:13 wilkiec Exp $

; Long double functions jump through to double (64 bit) functions

        .section .libfd.atan2l, code

        .global _atan2l
_atan2l:        
        bra    _atan2
        
        .end
