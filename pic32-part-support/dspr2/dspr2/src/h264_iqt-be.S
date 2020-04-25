
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

	.global		dspr2_mips_h264_iqt
	.ent		dspr2_mips_h264_iqt

dspr2_mips_h264_iqt:

# void dspr2_mips_h264_iqt(uint8 b[4][4], int16 c[4][4], int16 iq[4][4])
# $a0 - b
# $a1 - c
# $a2 - iq


# register allocation for a 4x4 block:
#   t0  t4
#   t1  t5
#   t2  t6
#   t3  t7

# after transpose:
#   t0  t2
#   s0  s2
#   t4  t6
#   s1  s3


	addiu		$sp, $sp, -16		# reserve stack space for s0-s3

# columns 0 & 1

	lw		$t0,  0+0($a1)		# c[0][0] | c[0][1]
	lw		$v0,  0+0($a2)		# iq[0][0] | iq[0][1]
	mul_s.ph	$t0, $t0, $v0		# x0

	lw		$t2, 16+0($a1)		# c[2][0] | c[2][1]
	lw		$t8, 16+0($a2)		# iq[2][0] | iq[2][1]
	mul_s.ph	$t2, $t2, $t8		# x2

	lw		$t1,  8+0($a1)		# c[1][0] | c[1][1]
	lw		$v1,  8+0($a2)		# iq[1][0] | iq[1][1]
	mul_s.ph	$t1, $t1, $v1		# x1

	lw		$t3, 24+0($a1)		# c[3][0] | c[3][1]
	lw		$t9, 24+0($a2)		# iq[3][0] | iq[3][1]
	mul_s.ph	$t3, $t3, $t9		# x3


	lw		$t4,  0+4($a1)		# c[0][2] | c[0][3]
	lw		$v0,  0+4($a2)		# iq[0][2] | iq[0][3]
	mul_s.ph	$t4, $t4, $v0		# x0

	lw		$t6, 16+4($a1)		# c[2][2] | c[2][3]
	lw		$t8, 16+4($a2)		# iq[2][2] | iq[2][3]
	mul_s.ph	$t6, $t6, $t8		# x2

	lw		$t5,  8+4($a1)		# c[1][2] | c[1][3]
	lw		$v1,  8+4($a2)		# iq[1][2] | iq[1][3]
	mul_s.ph	$t5, $t5, $v1		# x1

	lw		$t7, 24+4($a1)		# c[3][2] | c[3][3]
	lw		$t9, 24+4($a2)		# iq[3][2] | iq[3][3]
	mul_s.ph	$t7, $t7, $t9		# x3

	sw		$s0, 12($sp)
	sw		$s1,  8($sp)

	subq_s.ph	$v1, $t0, $t2		# z1 = x0 - x2
	shra.ph		$t8, $t1, 1
	shra.ph		$t9, $t3, 1
	subq_s.ph	$t8, $t8, $t3		# z2 = (x1 >> 1) - x3
	addq_s.ph	$t9, $t1, $t9		# z3 = x1 + (x3 >> 1)
	addq_s.ph	$v0, $t0, $t2		# z0 = x0 + x2

	sw		$s2,  4($sp)
	sw		$s3,  0($sp)

	addq_s.ph	$t1, $v1, $t8		# h[1][i] = z1 + z2
	subq_s.ph	$t2, $v1, $t8		# h[2][i] = z1 - z2
	addq_s.ph	$t0, $v0, $t9		# h[0][i] = z0 + z3
	subq_s.ph	$t3, $v0, $t9		# h[3][i] = z0 - z3

# columns 2 & 3

	subq_s.ph	$v1, $t4, $t6		# z1 = x0 - x2
	shra.ph		$t8, $t5, 1
	shra.ph		$t9, $t7, 1
	subq_s.ph	$t8, $t8, $t7		# z2 = (x1 >> 1) - x3
	addq_s.ph	$t9, $t5, $t9		# z3 = x1 + (x3 >> 1)
	addq_s.ph	$v0, $t4, $t6		# z0 = x0 + x2

	addq_s.ph	$t5, $v1, $t8		# h[1][i] = z1 + z2
	subq_s.ph	$t6, $v1, $t8		# h[2][i] = z1 - z2
	addq_s.ph	$t4, $v0, $t9		# h[0][i] = z0 + z3
	subq_s.ph	$t7, $v0, $t9		# h[3][i] = z0 - z3

# end of column processing

	li		$at, (32+(128<<6))<<1	# rounding + offset
	replv.ph	$at, $at		# offset constant

# transpose rows 0 & 1 into columns

	precrq.ph.w	$s0, $t0, $t1		# t0|t1 -> s0|t1
	ins		$t1, $t0, 16, 16

	precrq.ph.w	$s1, $t4, $t5		# t4|t5 -> s1|t5
	ins		$t5, $t4, 16, 16
	
# transpose rows 2 & 3 into columns

	precrq.ph.w	$s2, $t2, $t3		# t2|t3 -> s2|t3
	ins		$t3, $t2, 16, 16

	precrq.ph.w	$s3, $t6, $t7		# t6|t7 -> s3|t7
	ins		$t7, $t6, 16, 16

# transposed rows 0 & 1

	subq_s.ph	$v1, $s0, $s1		# z1 = h[i][0] - h[i][2]
	shra.ph		$t8, $t1, 1
	shra.ph		$t9, $t5, 1
	subq_s.ph	$t8, $t8, $t5		# z2 = (h[i][1] >> 1) - h[i][3]
	addq_s.ph	$t9, $t1, $t9		# z3 = h[i][1] + (h[i][3] >> 1)
	addq_s.ph	$v0, $s0, $s1		# z0 = h[i][0] + h[i][2]

	addq_s.ph	$s0, $v1, $t8		# z1 + z2
	subq_s.ph	$t4, $v1, $t8		# z1 - z2
	addq_s.ph	$t0, $v0, $t9		# z0 + z3
	subq_s.ph	$s1, $v0, $t9		# z0 - z3

	addq_s.ph	$s0, $s0, $at		# rounding + offset
	addq_s.ph	$t4, $t4, $at		# rounding + offset
	addq_s.ph	$t0, $t0, $at		# rounding + offset
	addq_s.ph	$s1, $s1, $at		# rounding + offset

# transposed rows 2 & 3

	subq_s.ph	$v1, $s2, $s3		# z1 = h[i][0] - h[i][2]
	shra.ph		$t8, $t3, 1
	shra.ph		$t9, $t7, 1
	subq_s.ph	$t8, $t8, $t7		# z2 = (h[i][1] >> 1) - h[i][3]
	addq_s.ph	$t9, $t3, $t9		# z3 = h[i][1] + (h[i][3] >> 1)
	addq_s.ph	$v0, $s2, $s3		# z0 = h[i][0] + h[i][2]

	addq_s.ph	$s2, $v1, $t8		# z1 + z2
	subq_s.ph	$t6, $v1, $t8		# z1 - z2
	addq_s.ph	$t2, $v0, $t9		# z0 + z3
	subq_s.ph	$s3, $v0, $t9		# z0 - z3

	addq_s.ph	$s2, $s2, $at		# rounding + offset
	addq_s.ph	$t6, $t6, $at		# rounding + offset
	addq_s.ph	$t2, $t2, $at		# rounding + offset
	addq_s.ph	$s3, $s3, $at		# rounding + offset

# inverse transpose rows 0 & 1, pack, and store

	precrq.ph.w	$v1, $t4, $s1
	ins		$s1, $t4, 16, 16
	precrq.ph.w	$v0, $t0, $s0
	ins		$s0, $t0, 16, 16

	precrqu_s.qb.ph	$v1, $v0, $v1
	precrqu_s.qb.ph	$t0, $s0, $s1

	lw		$s0, 12($sp)
	lw		$s1,  8($sp)

	sw		$v1, 0($a0)
	sw		$t0, 4($a0)

# inverse transpose rows 2 & 3, pack, and store

	precrq.ph.w	$v1, $t6, $s3
	ins		$s3, $t6, 16, 16
	precrq.ph.w	$v0, $t2, $s2
	ins		$s2, $t2, 16, 16

	precrqu_s.qb.ph	$v1, $v0, $v1
	precrqu_s.qb.ph	$t2, $s2, $s3

	lw		$s2,  4($sp)
	lw		$s3,  0($sp)

	sw		$v1, 8($a0)
	sw		$t2, 12($a0)

	jr		$ra
	addiu		$sp, $sp, 16


	.end		dspr2_mips_h264_iqt
