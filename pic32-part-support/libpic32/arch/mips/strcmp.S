.ident "lib/c/arch/mips/strcmp.S"
.file 1 "Microchip PIC32 Library"
.loc 1 0

/*
 * Originally written by Kjeld Borch Egevang <kjelde@mips.com>, 2002.
 */

/*
 * Copyright (c) 2014
 *      Imagination Technologies Limited.
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
 * THIS SOFTWARE IS PROVIDED BY IMAGINATION TECHNOLOGIES LIMITED ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL IMAGINATION TECHNOLOGIES LIMITED BE LIABLE
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

/* int strcmp(const char *s1, const char *s2); */

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
#define	SLL	dsll
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
#define	SLL	sll
#endif

#if __STDC__
#define CONCAT(a,b)	a##b
#else
#define CONCAT(a,b)	a/**/b
#endif

#ifndef BRANCHLIKELY

#define	STRCMP_WORD(OFS)			\
CONCAT(..strcmp_word,OFS):			;\
3:	SUBU	t0, v0, t1			;\
	beq	v0, v1, 4f			;\
	and	t0, t7				;\
	ADDIU	a0, OFS*word_size		;\
	b	..strcmp_byte			;\
	ADDIU	a1, OFS*word_size		;\
4:	beq	t0, zero, 5f			;\
	and	t4, t0, v1			;\
	beq	t4, t0, 5f			;\
	nop					;\
	jr	ra				;\
	move	v0, zero			;\
5:	L	v0, (OFS+1)*word_size(a0)	;\
	L	v1, (OFS+1)*word_size(a1)	;\

#else /* BRANCHDELAY */

#define	STRCMP_WORD(OFS)			\
CONCAT(..strcmp_word,OFS):			;\
3:	SUBU	t0, v0, t1			;\
	beq	v0, v1, 4f			;\
	and	t0, t7				;\
	ADDIU	a0, OFS*word_size		;\
	b	..strcmp_byte			;\
	ADDIU	a1, OFS*word_size		;\
4:	beql	t0, zero, 5f			;\
	L	v0, (OFS+1)*word_size(a0)	;\
	and	t4, t0, v1			;\
	beql	t4, t0, 3f			;\
	L	v1, (OFS+1)*word_size(a1)	;\
	jr	ra				;\
	move	v0, zero			;\
5:	L	v1, (OFS+1)*word_size(a1)	;\

#endif


LEAF(strcmp)
	.set	noreorder

	or	t0, a0, a1		/* Check for unaligned strings */
	andi	t0, word_mask

	bne	t0, zero, ..strcmp_byte
	lui	t1, 0x0101
	ori	t1, 0x0101
#ifdef	MIPS64
	dsll	t7, t1, 32
	or	t1, t7
#endif
	SLL	t7, t1, 7

..strcmp_loop:
	L	v0, 0x00(a0)
2:	L	v1, 0x00(a1)
	STRCMP_WORD(0)
	STRCMP_WORD(1)
	STRCMP_WORD(2)
3:	SUBU	t0, v0, t1
	beq	v0, v1, 4f
	and	t0, t7
	ADDIU	a0, 3*word_size
	b	..strcmp_byte
	ADDIU	a1, 3*word_size
4:	ADDIU	a0, 4*word_size
	beq	t0, zero, ..strcmp_loop
	ADDIU	a1, 4*word_size
	and	t4, t0, v1
	beq	t4, t0, 2b
	L	v0, 0x00(a0)
	jr	ra
	move	v0, zero

#ifdef BRANCHLIKELY
#define	STRCMP_T0T1(OFS)	\
	bnel	t0, zero, 8f	;\
	lbu	t2, OFS(a0)	;\
	jr	ra		;\
	SUBU	v0, t0, t1	;\
8:	beql	t0, t1, 8f	;\
	lbu	t3, OFS(a1)	;\
	jr	ra		;\
	SUBU	v0, t0, t1	;\
8:

#define	STRCMP_T2T3(OFS)	\
	bnel	t2, zero, 8f	;\
	lbu	t0, OFS(a0)	;\
	jr	ra		;\
	SUBU	v0, t2, t3	;\
8:	beql	t2, t3, 8f	;\
	lbu	t1, OFS(a1)	;\
	jr	ra		;\
	SUBU	v0, t2, t3	;\
8:
#endif /* BRANCHLIKELY */

..strcmp_byte:
#ifndef BRANCHLIKELY
1:	lbu	t0, 0(a0)
	lbu	t1, 0(a1)
	beq	t0, zero, 2f
    	ADDIU	a0, 1
	beq	t0, t1, 1b
    	ADDIU	a1, 1
2:	jr	ra
	SUBU	v0, t0, t1
#else /* BRANCHLIKELY */
	lbu	t0, 0(a0)
	lbu	t1, 0(a1)
1:	STRCMP_T0T1(1)
	STRCMP_T2T3(2)
	STRCMP_T0T1(3)
	STRCMP_T2T3(4)
	STRCMP_T0T1(5)
	STRCMP_T2T3(6)
	STRCMP_T0T1(7)
	bnel	t2, zero, 2f
	lbu	t0, 8(a0)
	jr	ra
	SUBU	v0, t2, t3
2:
	ADDIU	a0, 0x08
	ADDIU	a1, 0x08
	beql	t2, t3, 1b
	lbu	t1, 0(a1)
	jr	ra
	SUBU	v0, t2, t3
#endif /* BRANCHLIKELY */
END(strcmp)
