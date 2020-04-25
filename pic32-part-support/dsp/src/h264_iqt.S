
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
	.set	noreorder
	.set	nomacro
	.set	noat

	.global	mips_h264_iqt
	.ent	mips_h264_iqt

mips_h264_iqt:

# void mips_h264_iqt(uint8 b[4][4], int16 c[4][4], int16 iq[4][4])
# $a0 - b
# $a1 - c
# $a2 - iq


# register allocation for a 4x4 block:
#   t0 t4 s0 s4
#   t1 t5 s1 s5
#   t2 t6 s2 s6
#   t3 t7 s3 s7


	addiu	$sp, $sp, -32		# reserve stack space for s0-s7

	sw	$s0, 28($sp)
	sw	$s1, 24($sp)
	sw	$s2, 20($sp)
	sw	$s3, 16($sp)
	sw	$s4, 12($sp)
	sw	$s5,  8($sp)
	sw	$s6,  4($sp)
	sw	$s7,  0($sp)

# column 0

	lh	$t0,  0+0($a1)		# c[0][i]
	lh	$t1,  8+0($a1)		# c[1][i]
	lh	$t2, 16+0($a1)		# c[2][i]
	lh	$t3, 24+0($a1)		# c[3][i]

	lh	$v0,  0+0($a2)		# iq[0][i]
	lh	$v1,  8+0($a2)		# iq[1][i]
	lh	$t8, 16+0($a2)		# iq[2][i]
	lh	$t9, 24+0($a2)		# iq[3][i]

	mul	$t0, $t0, $v0		# x0 = c[0][i] * iq[0][i]
	mul	$t1, $t1, $v1		# x1 = c[1][i] * iq[1][i]
	mul	$t2, $t2, $t8		# x2 = c[2][i] * iq[2][i]
	mul	$t3, $t3, $t9		# x3 = c[3][i] * iq[3][i]

	addu	$v0, $t0, $t2		# z0 = x0 + x2
	subu	$v1, $t0, $t2		# z1 = x0 - x2
	sra	$t8, $t1, 1
	subu	$t8, $t8, $t3		# z2 = (x1 >> 1) - x3
	sra	$t9, $t3, 1
	addu	$t9, $t1, $t9		# z3 = x1 + (x3 >> 1)

	addu	$t0, $v0, $t9		# h[0][i] = z0 + z3
	addu	$t1, $v1, $t8		# h[1][i] = z1 + z2
	subu	$t2, $v1, $t8		# h[2][i] = z1 - z2
	subu	$t3, $v0, $t9		# h[3][i] = z0 - z3

# column 1

	lh	$t4,  0+2($a1)		# c[0][i]
	lh	$t5,  8+2($a1)		# c[1][i]
	lh	$t6, 16+2($a1)		# c[2][i]
	lh	$t7, 24+2($a1)		# c[3][i]

	lh	$v0,  0+2($a2)		# iq[0][i]
	lh	$v1,  8+2($a2)		# iq[1][i]
	lh	$t8, 16+2($a2)		# iq[2][i]
	lh	$t9, 24+2($a2)		# iq[3][i]

	mul	$t4, $t4, $v0		# x0 = c[0][i] * iq[0][i]
	mul	$t5, $t5, $v1		# x1 = c[1][i] * iq[1][i]
	mul	$t6, $t6, $t8		# x2 = c[2][i] * iq[2][i]
	mul	$t7, $t7, $t9		# x3 = c[3][i] * iq[3][i]

	addu	$v0, $t4, $t6		# z0 = x0 + x2
	subu	$v1, $t4, $t6		# z1 = x0 - x2
	sra	$t8, $t5, 1
	subu	$t8, $t8, $t7		# z2 = (x1 >> 1) - x3
	sra	$t9, $t7, 1
	addu	$t9, $t5, $t9		# z3 = x1 + (x3 >> 1)

	addu	$t4, $v0, $t9		# h[0][i] = z0 + z3
	addu	$t5, $v1, $t8		# h[1][i] = z1 + z2
	subu	$t6, $v1, $t8		# h[2][i] = z1 - z2
	subu	$t7, $v0, $t9		# h[3][i] = z0 - z3

# column 2

	lh	$s0,  0+4($a1)		# c[0][i]
	lh	$s1,  8+4($a1)		# c[1][i]
	lh	$s2, 16+4($a1)		# c[2][i]
	lh	$s3, 24+4($a1)		# c[3][i]

	lh	$v0,  0+4($a2)		# iq[0][i]
	lh	$v1,  8+4($a2)		# iq[1][i]
	lh	$t8, 16+4($a2)		# iq[2][i]
	lh	$t9, 24+4($a2)		# iq[3][i]

	mul	$s0, $s0, $v0		# x0 = c[0][i] * iq[0][i]
	mul	$s1, $s1, $v1		# x1 = c[1][i] * iq[1][i]
	mul	$s2, $s2, $t8		# x2 = c[2][i] * iq[2][i]
	mul	$s3, $s3, $t9		# x3 = c[3][i] * iq[3][i]

	addu	$v0, $s0, $s2		# z0 = x0 + x2
	subu	$v1, $s0, $s2		# z1 = x0 - x2
	sra	$t8, $s1, 1
	subu	$t8, $t8, $s3		# z2 = (x1 >> 1) - x3
	sra	$t9, $s3, 1
	addu	$t9, $s1, $t9		# z3 = x1 + (x3 >> 1)

	addu	$s0, $v0, $t9		# h[0][i] = z0 + z3
	addu	$s1, $v1, $t8		# h[1][i] = z1 + z2
	subu	$s2, $v1, $t8		# h[2][i] = z1 - z2
	subu	$s3, $v0, $t9		# h[3][i] = z0 - z3

# column 3

	lh	$s4,  0+6($a1)		# c[0][i]
	lh	$s5,  8+6($a1)		# c[1][i]
	lh	$s6, 16+6($a1)		# c[2][i]
	lh	$s7, 24+6($a1)		# c[3][i]

	lh	$v0,  0+6($a2)		# iq[0][i]
	lh	$v1,  8+6($a2)		# iq[1][i]
	lh	$t8, 16+6($a2)		# iq[2][i]
	lh	$t9, 24+6($a2)		# iq[3][i]

	mul	$s4, $s4, $v0		# x0 = c[0][i] * iq[0][i]
	mul	$s5, $s5, $v1		# x1 = c[1][i] * iq[1][i]
	mul	$s6, $s6, $t8		# x2 = c[2][i] * iq[2][i]
	mul	$s7, $s7, $t9		# x3 = c[3][i] * iq[3][i]

	addu	$v0, $s4, $s6		# z0 = x0 + x2
	subu	$v1, $s4, $s6		# z1 = x0 - x2
	sra	$t8, $s5, 1
	subu	$t8, $t8, $s7		# z2 = (x1 >> 1) - x3
	sra	$t9, $s7, 1
	addu	$t9, $s5, $t9		# z3 = x1 + (x3 >> 1)

	addu	$s4, $v0, $t9		# h[0][i] = z0 + z3
	addu	$s5, $v1, $t8		# h[1][i] = z1 + z2
	subu	$s6, $v1, $t8		# h[2][i] = z1 - z2
	subu	$s7, $v0, $t9		# h[3][i] = z0 - z3

# end of column processing

	li	$at, 255		# clipping constant

# row 0

	addu	$v0, $t0, $s0		# z0 = h[i][0] + h[i][2]
	subu	$v1, $t0, $s0		# z1 = h[i][0] - h[i][2]
	sra	$t8, $t4, 1
	subu	$t8, $t8, $s4		# z2 = (h[i][1] >> 1) - h[i][3]
	sra	$t9, $s4, 1
	addu	$t9, $t4, $t9		# z3 = h[i][1] + (h[i][3] >> 1)

# row 0 / column 0

	addu	$t0, $v0, $t9
	addiu	$t0, $t0, 32 + 8192	# rounding (32) and offset (8192 = 128 << 6)
	sra	$t0, $t0, 6		# b[i][0] = ((z0 + z3 + 32) >> 6) + 128

	slt	$a3, $t0, $zero		# b[i][0] < 0 ?
	movn	$t0, $zero, $a3		# yes, clip to 0
	slt	$a3, $t0, $at		# b[i][0] < 255 ?
	movz	$t0, $at, $a3		# no, clip to 255

	sb	$t0, 0($a0)		# b[i][0]

# row 0 / column 1

	addu	$t0, $v1, $t8
	addiu	$t0, $t0, 32 + 8192	# rounding (32) and offset (8192 = 128 << 6)
	sra	$t0, $t0, 6		# b[i][1] = ((z1 + z2 + 32) >> 6) + 128

	slt	$a3, $t0, $zero		# b[i][1] < 0 ?
	movn	$t0, $zero, $a3		# yes, clip to 0
	slt	$a3, $t0, $at		# b[i][1] < 255 ?
	movz	$t0, $at, $a3		# no, clip to 255

	sb	$t0, 1($a0)		# b[i][1]

# row 0 / column 2

	subu	$t0, $v1, $t8
	addiu	$t0, $t0, 32 + 8192	# rounding (32) and offset (8192 = 128 << 6)
	sra	$t0, $t0, 6		# b[i][2] = ((z1 - z2 + 32) >> 6) + 128

	slt	$a3, $t0, $zero		# b[i][2] < 0 ?
	movn	$t0, $zero, $a3		# yes, clip to 0
	slt	$a3, $t0, $at		# b[i][2] < 255 ?
	movz	$t0, $at, $a3		# no, clip to 255

	sb	$t0, 2($a0)		# b[i][2]

# row 0 / column 3

	subu	$t0, $v0, $t9
	addiu	$t0, $t0, 32 + 8192	# rounding (32) and offset (8192 = 128 << 6)
	sra	$t0, $t0, 6		# b[i][3] = ((z0 - z3 + 32) >> 6) + 128

	slt	$a3, $t0, $zero		# b[i][3] < 0 ?
	movn	$t0, $zero, $a3		# yes, clip to 0
	slt	$a3, $t0, $at		# b[i][3] < 255 ?
	movz	$t0, $at, $a3		# no, clip to 255

	sb	$t0, 3($a0)		# b[i][3]

# row 1

	addu	$v0, $t1, $s1		# z0 = h[i][0] + h[i][2]
	subu	$v1, $t1, $s1		# z1 = h[i][0] - h[i][2]
	sra	$t8, $t5, 1
	subu	$t8, $t8, $s5		# z2 = (h[i][1] >> 1) - h[i][3]
	sra	$t9, $s5, 1
	addu	$t9, $t5, $t9		# z3 = h[i][1] + (h[i][3] >> 1)

# row 1 / column 0

	addu	$t0, $v0, $t9
	addiu	$t0, $t0, 32 + 8192	# rounding (32) and offset (8192 = 128 << 6)
	sra	$t0, $t0, 6		# b[i][0] = ((z0 + z3 + 32) >> 6) + 128

	slt	$a3, $t0, $zero		# b[i][0] < 0 ?
	movn	$t0, $zero, $a3		# yes, clip to 0
	slt	$a3, $t0, $at		# b[i][0] < 255 ?
	movz	$t0, $at, $a3		# no, clip to 255

	sb	$t0, 4($a0)		# b[i][0]

# row 1 / column 1

	addu	$t0, $v1, $t8
	addiu	$t0, $t0, 32 + 8192	# rounding (32) and offset (8192 = 128 << 6)
	sra	$t0, $t0, 6		# b[i][1] = ((z1 + z2 + 32) >> 6) + 128

	slt	$a3, $t0, $zero		# b[i][1] < 0 ?
	movn	$t0, $zero, $a3		# yes, clip to 0
	slt	$a3, $t0, $at		# b[i][1] < 255 ?
	movz	$t0, $at, $a3		# no, clip to 255

	sb	$t0, 5($a0)		# b[i][1]

# row 1 / column 2

	subu	$t0, $v1, $t8
	addiu	$t0, $t0, 32 + 8192	# rounding (32) and offset (8192 = 128 << 6)
	sra	$t0, $t0, 6		# b[i][2] = ((z1 - z2 + 32) >> 6) + 128

	slt	$a3, $t0, $zero		# b[i][2] < 0 ?
	movn	$t0, $zero, $a3		# yes, clip to 0
	slt	$a3, $t0, $at		# b[i][2] < 255 ?
	movz	$t0, $at, $a3		# no, clip to 255

	sb	$t0, 6($a0)		# b[i][2]

# row 1 / column 3

	subu	$t0, $v0, $t9
	addiu	$t0, $t0, 32 + 8192	# rounding (32) and offset (8192 = 128 << 6)
	sra	$t0, $t0, 6		# b[i][3] = ((z0 - z3 + 32) >> 6) + 128

	slt	$a3, $t0, $zero		# b[i][3] < 0 ?
	movn	$t0, $zero, $a3		# yes, clip to 0
	slt	$a3, $t0, $at		# b[i][3] < 255 ?
	movz	$t0, $at, $a3		# no, clip to 255

	sb	$t0, 7($a0)		# b[i][3]

# row 2

	addu	$v0, $t2, $s2		# z0 = h[i][0] + h[i][2]
	subu	$v1, $t2, $s2		# z1 = h[i][0] - h[i][2]
	sra	$t8, $t6, 1
	subu	$t8, $t8, $s6		# z2 = (h[i][1] >> 1) - h[i][3]
	sra	$t9, $s6, 1
	addu	$t9, $t6, $t9		# z3 = h[i][1] + (h[i][3] >> 1)

# row 2 / column 0

	addu	$t0, $v0, $t9
	addiu	$t0, $t0, 32 + 8192	# rounding (32) and offset (8192 = 128 << 6)
	sra	$t0, $t0, 6		# b[i][0] = ((z0 + z3 + 32) >> 6) + 128

	slt	$a3, $t0, $zero		# b[i][0] < 0 ?
	movn	$t0, $zero, $a3		# yes, clip to 0
	slt	$a3, $t0, $at		# b[i][0] < 255 ?
	movz	$t0, $at, $a3		# no, clip to 255

	sb	$t0, 8($a0)		# b[i][0]

# row 2 / column 1

	addu	$t0, $v1, $t8
	addiu	$t0, $t0, 32 + 8192	# rounding (32) and offset (8192 = 128 << 6)
	sra	$t0, $t0, 6		# b[i][1] = ((z1 + z2 + 32) >> 6) + 128

	slt	$a3, $t0, $zero		# b[i][1] < 0 ?
	movn	$t0, $zero, $a3		# yes, clip to 0
	slt	$a3, $t0, $at		# b[i][1] < 255 ?
	movz	$t0, $at, $a3		# no, clip to 255

	sb	$t0, 9($a0)		# b[i][1]

# row 2 / column 2

	subu	$t0, $v1, $t8
	addiu	$t0, $t0, 32 + 8192	# rounding (32) and offset (8192 = 128 << 6)
	sra	$t0, $t0, 6		# b[i][2] = ((z1 - z2 + 32) >> 6) + 128

	slt	$a3, $t0, $zero		# b[i][2] < 0 ?
	movn	$t0, $zero, $a3		# yes, clip to 0
	slt	$a3, $t0, $at		# b[i][2] < 255 ?
	movz	$t0, $at, $a3		# no, clip to 255

	sb	$t0, 10($a0)		# b[i][2]

# row 2 / column 3

	subu	$t0, $v0, $t9
	addiu	$t0, $t0, 32 + 8192	# rounding (32) and offset (8192 = 128 << 6)
	sra	$t0, $t0, 6		# b[i][3] = ((z0 - z3 + 32) >> 6) + 128

	slt	$a3, $t0, $zero		# b[i][3] < 0 ?
	movn	$t0, $zero, $a3		# yes, clip to 0
	slt	$a3, $t0, $at		# b[i][3] < 255 ?
	movz	$t0, $at, $a3		# no, clip to 255

	sb	$t0, 11($a0)		# b[i][3]

# row 3

	addu	$v0, $t3, $s3		# z0 = h[i][0] + h[i][2]
	subu	$v1, $t3, $s3		# z1 = h[i][0] - h[i][2]
	sra	$t8, $t7, 1
	subu	$t8, $t8, $s7		# z2 = (h[i][1] >> 1) - h[i][3]
	sra	$t9, $s7, 1
	addu	$t9, $t7, $t9		# z3 = h[i][1] + (h[i][3] >> 1)

# row 3 / column 0

	addu	$t0, $v0, $t9
	addiu	$t0, $t0, 32 + 8192	# rounding (32) and offset (8192 = 128 << 6)
	sra	$t0, $t0, 6		# b[i][0] = ((z0 + z3 + 32) >> 6) + 128

	slt	$a3, $t0, $zero		# b[i][0] < 0 ?
	movn	$t0, $zero, $a3		# yes, clip to 0
	slt	$a3, $t0, $at		# b[i][0] < 255 ?
	movz	$t0, $at, $a3		# no, clip to 255

	sb	$t0, 12($a0)		# b[i][0]

# row 3 / column 1

	addu	$t0, $v1, $t8
	addiu	$t0, $t0, 32 + 8192	# rounding (32) and offset (8192 = 128 << 6)
	sra	$t0, $t0, 6		# b[i][1] = ((z1 + z2 + 32) >> 6) + 128

	slt	$a3, $t0, $zero		# b[i][1] < 0 ?
	movn	$t0, $zero, $a3		# yes, clip to 0
	slt	$a3, $t0, $at		# b[i][1] < 255 ?
	movz	$t0, $at, $a3		# no, clip to 255

	sb	$t0, 13($a0)		# b[i][1]

# row 3 / column 2

	subu	$t0, $v1, $t8
	addiu	$t0, $t0, 32 + 8192	# rounding (32) and offset (8192 = 128 << 6)
	sra	$t0, $t0, 6		# b[i][2] = ((z1 - z2 + 32) >> 6) + 128

	slt	$a3, $t0, $zero		# b[i][2] < 0 ?
	movn	$t0, $zero, $a3		# yes, clip to 0
	slt	$a3, $t0, $at		# b[i][2] < 255 ?
	movz	$t0, $at, $a3		# no, clip to 255

	sb	$t0, 14($a0)		# b[i][2]

# row 3 / column 3

	subu	$t0, $v0, $t9
	addiu	$t0, $t0, 32 + 8192	# rounding (32) and offset (8192 = 128 << 6)
	sra	$t0, $t0, 6		# b[i][3] = ((z0 - z3 + 32) >> 6) + 128

	slt	$a3, $t0, $zero		# b[i][3] < 0 ?
	movn	$t0, $zero, $a3		# yes, clip to 0
	slt	$a3, $t0, $at		# b[i][3] < 255 ?
	movz	$t0, $at, $a3		# no, clip to 255

	sb	$t0, 15($a0)		# b[i][3]


	lw	$s0, 28($sp)
	lw	$s1, 24($sp)
	lw	$s2, 20($sp)
	lw	$s3, 16($sp)
	lw	$s4, 12($sp)
	lw	$s5,  8($sp)
	lw	$s6,  4($sp)
	lw	$s7,  0($sp)

	jr	$ra
	addiu	$sp, $sp, 32


	.end	mips_h264_iqt
