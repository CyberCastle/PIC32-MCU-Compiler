
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

	.global		dspr2_mips_fft32
	.ent		dspr2_mips_fft32

dspr2_mips_fft32:

# void dspr2_mips_fft32(int32c *dout, int32c *din, int32c *twiddles, int32c *scratch, int log2N)
#
# $a0 - dout, read pointer A
# $a1 - din, write stride = (N / 2) * sizeof(int16c)
# $a2 - twiddles
# $a3 - scratch, write pointer A
# 16($sp) - log2N >= 3
#
# $v0 - log2N, final NB value
# $v1 - NB * sizeof(int32c)
#
# $at - twiddle pointer
#
# $s0 - offset used by lwx: $v1 + 4
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

	li		$v1, 4
	sll		$v1, $v1, $v0		# (NB = (1 << log2N) / 2) * sizeof(int32c)
	addiu		$s0, $v1, 4		# lwx offset
	
# stage 1

	addu		$s1, $a0, $v1		# write pointer B = write pointer A + NB
	addu		$s2, $a1, $v1		# final value of read pointer A

# Ar' = Ar + Br
# Ai' = Ai + Bi
# Br' = Ar - Br
# Bi' = Ai - Bi

	.align		4

loopS1:
	lwx		$t0, $v1($a1)		# load Br
	lwx		$t1, $s0($a1)		# load Bi
	lw		$t2, 0($a1)		# load Ar
	lw		$t3, 4($a1)		# load Ai

	addiu		$a1, $a1, 8		# read pointer A

	subqh_r.w	$t4, $t2, $t0		# Br' = Ar - Br
	subqh_r.w	$t5, $t3, $t1		# Bi' = Ai - Bi
	addqh_r.w	$t2, $t2, $t0		# Ar' = Ar + Br
	addqh_r.w	$t3, $t3, $t1		# Ai' = Ai + Bi

	sw		$t4, 0($s1)		# store Br'
	sw		$t5, 4($s1)		# store Bi'
	sw		$t2, 0($a0)		# store Ar'
	sw		$t3, 4($a0)		# store Ai'

	addiu		$a0, $a0, 8		# write pointer A

	bne		$a1, $s2, loopS1
	addiu		$s1, $s1, 8		# write pointer B

	# 4 stall cycles - mispredicted branch

	move		$a1, $v1		# write stride:  N / 2

	li		$v0, 8			# final NB value
	srl		$v1, $v1, 1		# NB = NB >> 1
	addiu		$s0, $v1, 4		# lwx offset

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

	lw		$t0, 0($at)		# load Tr
	lw		$t1, 4($at)		# load Ti

# warm-up software pipeline
	
	lwx		$t2, $v1($a0)		# load Br
	lwx		$t3, $s0($a0)		# load Bi
	lw		$t4, 0($a0)		# load Ar
	lw		$t5, 4($a0)		# load Ai

	mult		$ac0, $t2, $t0		# Br*Tr
	msub		$ac0, $t3, $t1		# Br*Tr - Bi*Ti
	extr_rs.w	$t6, $ac0, 31		# Br*Tr - Bi*Ti

	mult		$ac1, $t2, $t1		# Br*Ti
	madd		$ac1, $t3, $t0		# Br*Ti + Bi*Tr
	extr_rs.w	$t7, $ac1, 31		# Br*Tr + Bi*Ti

	addiu		$a0, $a0, 8		# read pointer A

	subqh_r.w	$s4, $t4, $t6		# Br' = Ar - (Br*Tr - Bi*Ti)
	subqh_r.w	$s5, $t5, $t7		# Bi' = Ai - (Br*Ti + Bi*Tr)
	addqh_r.w	$s6, $t4, $t6		# Br' = Ar + (Br*Tr - Bi*Ti)
	addqh_r.w	$s7, $t5, $t7		# Bi' = Ai + (Br*Ti + Bi*Tr)

	addiu		$a3, $a3, 8		# write pointer A
	addiu		$s1, $s1, 8		# write pointer B

	.align		4

loopBF:
	lwx		$t2, $v1($a0)		# load Br
	lwx		$t3, $s0($a0)		# load Bi
	lw		$t4, 0($a0)		# load Ar
	lw		$t5, 4($a0)		# load Ai

	mult		$ac0, $t2, $t0		# Br*Tr
	msub		$ac0, $t3, $t1		# Br*Tr - Bi*Ti
	extr_rs.w	$t6, $ac0, 31		# Br*Tr - Bi*Ti

	addiu		$a0, $a0, 8		# read pointer A
	addiu		$a3, $a3, 8		# write pointer A

	mult		$ac1, $t2, $t1		# Br*Ti
	madd		$ac1, $t3, $t0		# Br*Ti + Bi*Tr
	extr_rs.w	$t7, $ac1, 31		# Br*Tr + Bi*Ti

	sw		$s4, -8($s1)		# store Br'
	sw		$s5, -4($s1)		# store Bi'
	sw		$s6, -16($a3)		# store Ar'
	sw		$s7, -12($a3)		# store Ai'

	subqh_r.w	$s4, $t4, $t6		# Br' = Ar - (Br*Tr - Bi*Ti)
	subqh_r.w	$s5, $t5, $t7		# Bi' = Ai - (Br*Ti + Bi*Tr)
	addqh_r.w	$s6, $t4, $t6		# Br' = Ar + (Br*Tr - Bi*Ti)
	addqh_r.w	$s7, $t5, $t7		# Bi' = Ai + (Br*Ti + Bi*Tr)

	bne		$a0, $s2, loopBF
	addiu		$s1, $s1, 8		# write pointer B

	# 4 stall cycles - mispredicted branch

	addu		$a0, $a0, $v1		# read pointer A += NB

	sw		$s4, -8($s1)		# store Br'
	sw		$s5, -4($s1)		# store Bi'
	sw		$s6, -8($a3)		# store Ar'
	sw		$s7, -4($a3)		# store Ai'

	bne		$a0, $s3, loopGR
	addu		$at, $at, $v1		# twiddle pointer += NB

	# 4 stall cycles - mispredicted branch

	srl		$v1, $v1, 1		# NB = NB >> 1
	addiu		$s0, $v1, 4		# lwx offset

	subu		$t0, $a0, $a1
	subu		$a0, $a3, $a1		# read pointer A = write pointer A - N/2

	bne		$v1, $v0, loopS
	subu		$a3, $t0, $a1		# write pointer A -= N


# stage log2N

	addu		$at, $a2, 8		# twiddle pointer = &twiddles[1]

	sll		$s3, $a1, 1		# N
	addu		$s3, $a0, $s3		# final read pointer A
	addu		$s1, $a3, $a1		# write pointer B = write pointer A + write stride

# first butterfly - simplified for T = (1, 0)
	
# Ar' = Ar + Br
# Ai' = Ai + Bi
# Br' = Ar - Br
# Bi' = Ai - Bi

	lw		$t4, 0($a0)		# load Ar
	lw		$t5, 4($a0)		# load Ai
	lw		$t2, 8($a0)		# load Br
	lw		$t3,12($a0)		# load Bi

	addiu		$a0, $a0, 16		# read pointer A

	addiu		$a3, $a3, 8		# write pointer A
	addiu		$s1, $s1, 8		# write pointer B

	subu		$v1, $at, $a3		# calculate lwx offset
	addiu		$s0, $v1, 4		# another lwx offset

	subqh_r.w	$s4, $t4, $t2		# Br' = Ar - Br
	subqh_r.w	$s5, $t5, $t3		# Bi' = Ai - Bi
	addqh_r.w	$s6, $t4, $t2		# Br' = Ar + Br
	addqh_r.w	$s7, $t5, $t3		# Bi' = Ai + Bi

	.align		4

loopSL2N:

# Ar' = Ar + (BrTr - BiTi)
# Ai' = Ai + (BrTi + BiTr)
# Br' = Ar - (BrTr - BiTi)
# Bi' = Ai - (BrTi + BiTr)

	lwx		$t0, $v1($a3)		# load Tr
	lwx		$t1, $s0($a3)		# load Ti

	lw		$t4, 0($a0)		# load Ar
	lw		$t5, 4($a0)		# load Ai
	lw		$t2, 8($a0)		# load Br
	lw		$t3,12($a0)		# load Bi

	mult		$ac0, $t2, $t0		# Br*Tr
	msub		$ac0, $t3, $t1		# Br*Tr - Bi*Ti
	extr_rs.w	$t6, $ac0, 31		# Br*Tr - Bi*Ti

	addiu		$a0, $a0, 16		# read pointer A

	mult		$ac1, $t2, $t1		# Br*Ti
	madd		$ac1, $t3, $t0		# Br*Ti + Bi*Tr
	extr_rs.w	$t7, $ac1, 31		# Br*Tr + Bi*Ti

	sw		$s4, -8($s1)		# store Br'
	sw		$s5, -4($s1)		# store Bi'
	sw		$s6, -8($a3)		# store Ar'
	sw		$s7, -4($a3)		# store Ai'

	addiu		$a3, $a3, 8		# write pointer A

	subqh_r.w	$s4, $t4, $t6		# Br' = Ar - (Br*Tr - Bi*Ti)
	subqh_r.w	$s5, $t5, $t7		# Bi' = Ai - (Br*Ti + Bi*Tr)
	addqh_r.w	$s6, $t4, $t6		# Br' = Ar + (Br*Tr - Bi*Ti)
	addqh_r.w	$s7, $t5, $t7		# Bi' = Ai + (Br*Ti + Bi*Tr)

	bne		$a0, $s3, loopSL2N
	addiu		$s1, $s1, 8		# write pointer B

	# 4 stall cycles - mispredicted branch

	sw		$s4, -8($s1)		# store Br'
	sw		$s5, -4($s1)		# store Bi'
	sw		$s6, -8($a3)		# store Ar'
	sw		$s7, -4($a3)		# store Ai'

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

	.end		dspr2_mips_fft32



