; $Id: log10l.s,v 1.2 2012/05/08 20:15:14 wilkiec Exp $

; Long double functions jump through to double (64 bit) functions

        .section .libfd.log10l, code

        .global _log10l
_log10l:        
        bra    _log10
        
        .end
