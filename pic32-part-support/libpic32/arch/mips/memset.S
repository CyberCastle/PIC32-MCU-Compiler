.ident "libc/arch/mips/memset.S"
.file 1 "Microchip PIC32 Library"
.loc 1 0

/*
 * memset.S: fast memset for MIPS
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

#if __mips >= 3 && __mips != 32
/* we're a leaf function, so its safe to use 64-bit ops, as
   an optimisation even if -mgp32 was used. */
	.set gp64
# undef __mips64
# define __mips64 1
#endif

#if __mips64
#define S	sd
#define SL	sdl
#define SR	sdr
#define RS	8
#else
#define S	sw
#define SL	swl
#define SR	swr
#define RS	4
#endif

#ifdef MIPSEL
#	define	SHI	SR
#	define	SLO	SL
#endif
#ifdef MIPSEB
#	define	SHI	SL
#	define	SLO	SR
#endif

/* memset(to, val, n) */
LEAF(memset)
	.set	noreorder
	move	v0, a0			# save to for return
	beq	a2, zero, ..ret
	sltu	t2, a2, 16
	bne	t2, zero, ..small	# do small blocks byte at a time

	/* replicate fill byte into register */
	and	a1, 0xff
#if __mips64
	dsll	t2, a1, 8
	or	a1, t2
	dsll	t2, a1, 16
	or	a1, t2
	dsll	t2, a1, 32
	or	a1, t2
#else
	sll	t2, a1, 8
	or	a1, t2
	sll	t2, a1, 16
	or	a1, t2
#endif

	and	v1, a0, RS-1		# get unaligned address
	beq	v1, zero, 1f		# skip if already aligned
	li	a3, RS			# calculate...
	subu	a3, v1			#  number of bytes to align

	subu	a2, a3			# subtract from remaining count
	SHI	a1, 0(a0)		# store 1..RS bytes to align a0
	addu	a0, a3			# bump pointer

	/* Try a 4X unrolled block fill */
1:	and	v1, a2, (RS*4)-1	# remaining size % blocksize
	subu	a3, a2, v1		# size of remaining blocks
	beq	a3, zero, 1f		# none?
	move	a2, v1			# bytes remaining after block copy
	addu	a3, a0			# compute ending address

2:	S	a1, RS*0(a0)
	S	a1, RS*1(a0)
	S	a1, RS*2(a0)
	addu	a0, RS*4
	bne	a0, a3, 2b
	S	a1, -RS(a0)

	/* Try a word at a time */
1:	and	v1, a2, RS-1		# remaining size % word size
	subu	a3, a2, v1		# size of remaining words
	beq	a3, zero, ..small	# none?
	move	a2, v1			# bytes remaining after word copy
	addu	a3, a0			# compute ending address

2:	addu	a0, RS
	bne	a0, a3, 2b
	S	a1, -RS(a0)

..small:
	/* Last resort: byte at a time */
	beq	a2, zero, ..ret
	addu	a3, a2, a0		# compute ending address

1:	addu	a0, 1
	bne	a0, a3, 1b
	sb	a1, -1(a0)

..ret:	j	ra
	nop
	.set	reorder
END(memset)
