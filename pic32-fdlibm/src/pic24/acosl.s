; $Id: acosl.s,v 1.2 2012/05/08 20:15:13 wilkiec Exp $

; Long double functions jump through to double (64 bit) functions

	.section .libfd.acosl, code

        .global _acosl
_acosl: 
        bra    _acos
        
        .end
