
 # ****************************Begin Copyright 1D**********************************
 # Unpublished work (c) MIPS Technologies, Inc.  All rights reserved.
 # Unpublished rights reserved under the copyright laws of the United States
 # of America and other countries.
 # 
 # This code is confidential and proprietary to MIPS Technologies, Inc. ("MIPS
 # Technologies") and  may be disclosed only as permitted in writing by MIPS
 # Technologies or an authorized third party.  Any copying, reproducing,
 # modifying, use or disclosure of this code (in whole or in part) that is not
 # expressly permitted in writing by MIPS Technologies or an authorized third
 # party is strictly prohibited.  At a minimum, this code is protected under
 # trade secret, unfair competition, and copyright laws.  Violations thereof
 # may result in criminal penalties and fines.
 # 
 # MIPS Technologies reserves the right to change this code to improve function,
 # design or otherwise.  MIPS Technologies does not assume any liability arising
 # out of the application or use of this code, or of any error or omission in
 # such code.  Any warranties, whether express, statutory, implied or otherwise,
 # including but not limited to the implied warranties of merchantability or 
 # fitness for a particular purpose, are excluded.  Except as expressly provided
 # in any written license agreement from MIPS Technologies or an authorized third
 # party, the furnishing of this code does not give recipient any license to any
 # intellectual property rights, including any patent rights, that cover this code.
 # 
 # This code shall not be exported or transferred for the purpose of reexporting
 # in violation of any U.S. or non-U.S. regulation, treaty, Executive Order, law,
 # statute, amendment or supplement thereto.
 # 
 # This code may only be disclosed to the United States government ("Government"),
 # or to Government users, with prior written consent from MIPS Technologies or an
 # authorized third party.  This code constitutes one or more of the following:
 # commercial computer software, commercial computer software documentation or
 # other commercial items.  If the user of this code, or any related documentation
 # of any kind, including related technical data or manuals, is an agency,
 # department, or other entity of the Government, the use, duplication,
 # reproduction, release, modification, disclosure, or transfer of this code, or
 # any related documentation of any kind, is restricted in accordance with Federal
 # Acquisition Regulation 12.212 for civilian agencies and Defense Federal
 # Acquisition Regulation Supplement 227.7202 for military agencies.  The use of
 # this code by the Government is further restricted in accordance with the terms
 # of the license agreement(s) and/or applicable contract terms and conditions
 # covering this code from MIPS Technologies or an authorized third party.
 # *******************************End Copyright************************************


	.text
	.set		noreorder
	.set		nomacro
	.set		noat

	.global		dspr2_mips_fft16
	.ent		dspr2_mips_fft16

dspr2_mips_fft16:

# void dspr2_mips_fft16(int16c *dout, int16c *din, int16c *twiddles, int16c *scratch, int log2N)
#
# $a0 - dout, read pointer A
# $a1 - din, write stride = (N / 2) * sizeof(int16c)
# $a2 - twiddles
# $a3 - scratch, write pointer A
# 16($sp) - log2N >= 3
#
# $v0 - log2N, final NB value
# $v1 - NB * sizeof(int16c)
#
# $at - twiddle pointer
#
# $s0 - 
# $s1 - write pointer B
# $s2 - final read pointer A in BF loop
# $s3 - final read pointer A in GR loop
# $s4 - software pipelined result
# $s5 - software pipelined result
# $s6 - software pipelined result
# $s7 - software pipelined result

	addiu		$sp, $sp, -32		# reserve stack space for s0-s7

	sw		$s0, 28($sp)
	sw		$s1, 24($sp)
	sw		$s2, 20($sp)
	sw		$s3, 16($sp)
	sw		$s4, 12($sp)
	sw		$s5,  8($sp)
	sw		$s6,  4($sp)
	sw		$s7,  0($sp)

	lw		$v0, 32+16($sp)		# load log2N

	move		$at, $a0		# save dout
	andi		$t1, $v0, 0x01		# if log2N is even, swap(dout, scratch)
	movz		$a0, $a3, $t1
	movz		$a3, $at, $t1

	li		$v1, 2
	sll		$v1, $v1, $v0		# (NB = (1 << log2N) / 2) * sizeof(int16c)
	
# stage 1
# with pre-scaling of input data

	addu		$s1, $a0, $v1		# write pointer B = write pointer A + NB
	addu		$s2, $a1, $v1		# final value of read pointer A

# Ar' = Ar + Br
# Ai' = Ai + Bi
# Br' = Ar - Br
# Bi' = Ai - Bi

	lwx		$t2, $v1($a1)		# load BiBr
	lw		$t3, 0($a1)		# load AiAr

	.align		4

loopS1:
	shra.ph		$t2, $t2, 1		# pre-scale input data
	shra.ph		$t3, $t3, 1		# pre-scale input data

	addiu		$a1, $a1, 4		# read pointer A

	subq_s.ph	$t4, $t3, $t2		# B' = A - B
	addq_s.ph	$t8, $t3, $t2		# A' = A + B

	lwx		$t2, $v1($a1)		# load BiBr
	lw		$t3, 0($a1)		# load AiAr

	shra.ph		$t4, $t4, 1		# scale B'
	shra.ph		$t8, $t8, 1		# scale A'

	sw		$t4, 0($s1)		# store B'
	sw		$t8, 0($a0)		# store A'

	addiu		$a0, $a0, 4		# write pointer A

	bne		$a1, $s2, loopS1
	addiu		$s1, $s1, 4		# write pointer B

	# 4 stall cycles - mispredicted branch

	move		$a1, $v1		# write stride:  N / 2

	li		$v0, 4			# final NB value
	srl		$v1, $v1, 1		# NB = NB >> 1

	subu		$a0, $a0, $a1		# read pointer A

# stages 2 - (log2N-1)

loopS:
	move		$at, $a2		# twiddle pointer = twiddles

	sll		$s3, $a1, 1		# N
	addu		$s3, $a0, $s3		# final read pointer A in GR loop

	.align		4

loopGR:
	addu		$s1, $a3, $a1		# write pointer B = write pointer A + write stride
	addu		$s2, $a0, $v1		# final value of read pointer A

# Ar' = Ar + (BrTr - BiTi)
# Ai' = Ai + (BrTi + BiTr)
# Br' = Ar - (BrTr - BiTi)
# Bi' = Ai - (BrTi + BiTr)

	lw		$t0, 0($at)		# load TrTr
	lwx		$t1, $a1($at)		# load Ti(-Ti)

# warm-up software pipeline
	
	lwx		$t2, $v1($a0)		# load BiBr
	lw		$t8, 0($a0)		# load AiAr

	rotr		$t3, $t2, 16		# BrBi

	addiu		$a0, $a0, 4		# read pointer A

	mulq_rs.ph	$t6, $t2, $t0		# Bi*Tr |  Br*Tr
	mulq_rs.ph	$t7, $t3, $t1		# Br*Ti | -Bi*Ti

	# 4 stall cycles - multiplier results
	
	addq_s.ph	$t6, $t6, $t7		# B*T = (BrTi + BiTr) | (BrTr - BiTi)

	lwx		$t2, $v1($a0)		# load BiBr

	subq_s.ph	$s4, $t8, $t6		# B' = A - B*T
	addq_s.ph	$s5, $t8, $t6		# A' = A + B*T

	rotr		$t3, $t2, 16		# BrBi

	addiu		$a3, $a3, 4		# write pointer A
	addiu		$s1, $s1, 4		# write pointer B

	.align		4

loopBF:
	mulq_rs.ph	$t6, $t2, $t0		# Bi*Tr |  Br*Tr
	mulq_rs.ph	$t7, $t3, $t1		# Br*Ti | -Bi*Ti

	lw		$t8, 0($a0)		# load AiAr

	addiu		$a0, $a0, 4		# read pointer A
	addiu		$a3, $a3, 4		# write pointer A

	lwx		$t2, $v1($a0)		# load BiBr

	shra.ph		$s4, $s4, 1		# scale B'
	shra.ph		$s5, $s5, 1		# scale A'

	rotr		$t3, $t2, 16		# BrBi

	sw		$s4, -4($s1)		# store B'
	sw		$s5, -8($a3)		# store A'

	addq_s.ph	$t6, $t6, $t7		# B*T = (BrTi + BiTr) | (BrTr - BiTi)

	addiu		$s1, $s1, 4		# write pointer B

	subq_s.ph	$s4, $t8, $t6		# B' = A - B*T

	bne		$a0, $s2, loopBF
	addq_s.ph	$s5, $t8, $t6		# A' = A + B*T

	# 4 stall cycles - mispredicted branch

	shra.ph		$s4, $s4, 1		# scale B'
	shra.ph		$s5, $s5, 1		# scale A'

	sw		$s4, -4($s1)		# store B'
	sw		$s5, -4($a3)		# store A'

	addu		$a0, $a0, $v1		# read pointer A += NB

	bne		$a0, $s3, loopGR
	addu		$at, $at, $v1		# twiddle pointer += NB

	# 4 stall cycles - mispredicted branch

	srl		$v1, $v1, 1		# NB = NB >> 1

	subu		$t0, $a0, $a1
	subu		$a0, $a3, $a1		# read pointer A = write pointer A - N/2

	bne		$v1, $v0, loopS
	subu		$a3, $t0, $a1		# write pointer A -= N


# stage log2N
# no post-scaling of results

	addu		$at, $a2, 4		# twiddle pointer = &twiddles[1]

	sll		$s3, $a1, 1		# N
	addu		$s3, $a0, $s3		# final read pointer A
	addu		$s1, $a3, $a1		# write pointer B = write pointer A + write stride

# first butterfly - simplified for T = (1, 0)
	
# Ar' = Ar + Br
# Ai' = Ai + Bi
# Br' = Ar - Br
# Bi' = Ai - Bi

	lw		$t2, 4($a0)		# load BiBr
	lw		$t8, 0($a0)		# load AiAr
	
	addiu		$a0, $a0, 8		# read pointer A

	lw		$t0, 0($at)		# load TrTr
	lwx		$t1, $a1($at)		# load Ti(-Ti)

	subq_s.ph	$s4, $t8, $t2		# B' = A - B
	addq_s.ph	$s5, $t8, $t2		# A' = A + B

	addiu		$a3, $a3, 4		# write pointer A
	addiu		$s1, $s1, 4		# write pointer B

	lw		$t2, 4($a0)		# load BiBr

	rotr		$t3, $t2, 16		# BrBi

	.align		4

loopSL2N:

# Ar' = Ar + (BrTr - BiTi)
# Ai' = Ai + (BrTi + BiTr)
# Br' = Ar - (BrTr - BiTi)
# Bi' = Ai - (BrTi + BiTr)

	mulq_rs.ph	$t6, $t2, $t0		# Bi*Tr |  Br*Tr
	mulq_rs.ph	$t7, $t3, $t1		# Br*Ti | -Bi*Ti

	lw		$t8, 0($a0)		# load AiAr
	lw		$t2, 12($a0)		# load BiBr

	addiu		$a0, $a0, 8		# read pointer A
	addiu		$at, $at, 4		# twiddle pointer

	sw		$s4, -4($s1)		# store B'
	sw		$s5, -4($a3)		# store A'

	addiu		$s1, $s1, 4		# write pointer B
	addiu		$a3, $a3, 4		# write pointer A

	lw		$t0, 0($at)		# load TrTr
	lwx		$t1, $a1($at)		# load Ti(-Ti)

	addq_s.ph	$t6, $t6, $t7		# B*T = (BrTi + BiTr) | (BrTr - BiTi)
	
	rotr		$t3, $t2, 16		# BrBi

	subq_s.ph	$s4, $t8, $t6		# B' = A - B*T

	bne		$a0, $s3, loopSL2N
	addq_s.ph	$s5, $t8, $t6		# A' = A + B*T


	# 4 stall cycles - mispredicted branch

	sw		$s4, -4($s1)		# store B'
	sw		$s5, -4($a3)		# store A'

	lw		$s0, 28($sp)
	lw		$s1, 24($sp)
	lw		$s2, 20($sp)
	lw		$s3, 16($sp)
	lw		$s4, 12($sp)
	lw		$s5,  8($sp)
	lw		$s6,  4($sp)
	lw		$s7,  0($sp)

	jr		$ra
	addiu		$sp, $sp, 32

	.end		dspr2_mips_fft16



