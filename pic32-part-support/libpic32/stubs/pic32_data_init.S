/*-------------------------------------------------------------------------
 *
 * Copyright (c) 2017, Microchip Technology Inc. and its subsidiaries ("Microchip")
 * All rights reserved.
 * 
 * This software is developed by Microchip Technology Inc. and its
 * subsidiaries ("Microchip").
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1.      Redistributions of source code must retain the above copyright
 *         notice, this list of conditions and the following disclaimer.
 * 2.      Redistributions in binary form must reproduce the above
 *         copyright notice, this list of conditions and the following
 *         disclaimer in the documentation and/or other materials provided
 *         with the distribution.
 * 3.      Microchip's name may not be used to endorse or promote products
 *         derived from this software without specific prior written
 *         permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MICROCHIP "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL MICROCHIP BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING BUT NOT LIMITED TO
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWSOEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *-------------------------------------------------------------------------*/        
#include <xc.h>

#define save_ra v1

	.section	.text,code
	.align	2
	.globl	__pic32_data_init

	.set noreorder
	.set noat
	.set nomips16
#if defined(__mips_micromips) || defined(__USE_MICROMIPS)
	.set micromips
#else
	.set nomicromips
#endif

	.ent	__pic32_data_init
	.type	__pic32_data_init, @function

__pic32_data_init:
	move save_ra,ra

#define INIT_DATA
#if defined(INIT_DATA)
        ##################################################################
        # Initialize data using the linker-generated .dinit table
        ##################################################################
        .equiv FMT_CLEAR,0
        .equiv FMT_COPY,1
_dinit_init:
        la      t0,_dinit_addr

#define SRC t0
#define DST t1
#define LEN t2
#define FMT t3

0:      lw      DST,0(SRC)
        beqz    DST,9f
        addu    SRC,4
        lw      LEN,0(SRC)
        addu    SRC,4
        lw      FMT,0(SRC)
        beq     FMT,$0,_dinit_clear
        addu    SRC,4

_dinit_copy:
        lbu     t4,0(SRC)
        subu    LEN,1
        addu    SRC,1
        sb      t4,0(DST)
        bne     LEN,$0,_dinit_copy
        addu    DST,1

        b       _dinit_end
        nop

_dinit_clear:
        sb      $0,(DST)
        subu    LEN,1
        bne     LEN,$0,_dinit_clear
        addu    DST,1

_dinit_end:
        addu    SRC,3
        addiu   LEN,$0,-4
        and     SRC,LEN,SRC
        lw      DST,0(SRC)
        bne     DST,$0,0b
        nop
9:
#endif
	.set	macro
	.set	reorder
	move ra, save_ra
    jr ra
    nop
	.end	__pic32_data_init
	.size	__pic32_data_init, .-__pic32_data_init
