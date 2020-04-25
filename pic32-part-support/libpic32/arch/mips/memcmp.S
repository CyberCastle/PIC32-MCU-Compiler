.ident "libc/arch/mips/memcmp.S"
.file 1 "Microchip PIC32 Library"
.loc 1 0

/*
 * Originally written by Kjeld Borch Egevang <kjelde@mips.com>, 2002.
 */

/*
 * Copyright (c) 2012-2015
 *      MIPS Technologies, Inc., California.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the MIPS Technologies, Inc., nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE MIPS TECHNOLOGIES, INC. ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE MIPS TECHNOLOGIES, INC. BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <xc.h>
#include <sys/asm.h>

	.set	nomips16

/* int memcmp(const void *s1, const void *s2, size_t n); */

#if __mips >= 3 && __mips != 32
#define MIPS64 1
#undef MIPS32
#else
#undef MIPS64
#define MIPS32 1
#endif

#if __mips > 1 && __mips != 32 && __mips != 64
#define BRANCHLIKELY 1
#endif
#if __mips == 1
#define LOADDELAY 1
#endif

#ifdef MIPS64
.set gp64
#define	L	ld
#define	LL	ldl
#define	LR	ldr
#define word_bits 0x3
#define word_mask 0x7
#define word_size 0x8
#define	SUBU	dsubu
#define	ADDIU	daddiu
#endif


#ifdef MIPS32
#define	L	lw
#define	LL	lwl
#define	LR	lwr
#define word_bits 0x2
#define word_mask 0x3
#define word_size 0x4
#define	SUBU	subu
#define	ADDIU	addiu
#endif


LEAF(memcmp)
	.set noreorder

	slti	t1, a2, word_size
	beq	t1, zero, 1f
	ADDU	a3, a0, a2
#ifndef BRANCHLIKELY
	bgtz	a2, ..memcmp_byte
	nop
#else
	bgtzl	a2, ..memcmp_byte
	lbu	t0, 0(a0)
#endif
	jr	ra
	move	v0, zero
1:	or	t0, a0, a1		/* Check for unaligned strings */
	andi	t0, word_mask
#ifndef BRANCHLIKELY
	beqz	t0, 2f
	nop
#else
	beqzl	t0, 2f
	L	t0, 0x00(a0)
#endif
	b	5f
	lbu	t0, 0(a0)

2:
#ifndef BRANCHLIKELY
	L	t0, 0x00(a0)
#endif
	L	t1, 0x00(a1)
	ADDIU	a0, word_size
#ifndef BRANCHLIKELY
	beq	t0, t1, 3f
	ADDIU	a1, word_size
	ADDIU	a1, -word_size
#else
	beql	t0, t1, 3f
	ADDIU	a1, word_size
#endif
	ADDIU	a0, -word_size
	b	5f
	lbu	t0, 0(a0)
3:    	ADDIU	a2, -word_size
#ifndef BRANCHLIKELY
	bgtz	a2, 2b
	nop
#else
	bgtzl	a2, 2b
	L	t0, 0x00(a0)
#endif
4:	jr	ra
	move	v0, zero

..memcmp_byte:
#ifndef BRANCHLIKELY
	lbu	t0, 0(a0)
#endif
5:	lbu	t1, 0(a1)
    	ADDIU	a0, 1
	beq	t0, t1, 6f
    	ADDIU	a1, 1
	jr	ra
	subu	v0, t0, t1
#ifndef BRANCHLIKELY
6:	bne	a0, a3, ..memcmp_byte
	nop
#else
6:	bnel	a0, a3, 5b
	lbu	t0, 0(a0)
#endif
	jr	ra
	move	v0, zero
END(memcmp)
