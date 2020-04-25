
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

	.global		dspr2_mips_lms16
	.ent		dspr2_mips_lms16

dspr2_mips_lms16:

# int16 dspr2_mips_lms16(int16 in, int16 ref, int16 *coeffs, int16 *delayline,
#		   int16 *error, int16 K, int mu)
# $a0 - in
# $a1 - ref
# $a2 - coeffs
# $a3 - delayline
# 16($sp) - error from the last iteration
# 20($sp) - K = 4*k >= 8
# 24($sp) - mu (adaptation rate)


	lw		$at, 16($sp)		# error pointer
	lw		$t9, 24($sp)		# mu
	lw		$t8, 20($sp)		# K
	lh		$t0, 0($at)		# *error

	addiu		$t8, $t8, -4		# K-4

	mulq_rs.ph	$t9, $t9, $t0		# adj = mu * error

	sll		$t8, $t8, 1		# (K-4) * sizeof(int16)
	addu		$t8, $t8, $a2		# &coeffs[K-4]

	lw		$v1, 0($a3)		# delayline[0]
	li		$t0, 0x8000		# accumulator rounding value

	mtlo		$t0
	mthi		$0

	ins		$t9, $t9, 16, 16	# replicate adj

	lw		$t0, 4($a3)		# x1 = delayline[i+1]
	lw		$t4, 8($a3)		# x2 = delayline[i+2]

	lw		$t1, 0($a2)		# c1 = coeffs[i]
	lw		$t5, 4($a2)		# c2 = coeffs[i+1]

	.align		4

loop:
	mulq_rs.ph	$t2, $t0, $t9		# x1 * adj
	mulq_rs.ph	$t6, $t4, $t9		# x2 * adj

	sw		$t0, 0($a3)		# delayline[i] = x1
	sw		$t4, 4($a3)		# delayline[i+1] = x2

	addiu		$a2, $a2, 8		# 4 * sizeof(int16)
	addiu		$a3, $a3, 8		# 4 * sizeof(int16)

	addq_s.ph	$v0, $t1, $t2		# c1 += x1 * adj
	addq_s.ph	$t7, $t5, $t6		# c2 += x2 * adj

	lw		$t1, 0($a2)		# c1 = coeffs[i]
	lw		$t5, 4($a2)		# c2 = coeffs[i+1]

	dpaq_s.w.ph	$ac0, $t0, $v0		# a += x1 * c1
	dpaq_s.w.ph	$ac0, $t4, $t7		# a += x2 * c2

	lw		$t0, 4($a3)		# x1 = delayline[i+1]
	lw		$t4, 8($a3)		# x2 = delayline[i+2]

	sw		$v0, -8($a2)		# coeffs[i] = c1

	bne		$a2, $t8, loop
	sw		$t7, -4($a2)		# coeffs[i+1] = c2

	# 13 stall cycles - mispredicted branch

	ins		$a0, $v1, 16, 16	# insert "in" into delayline

	mulq_rs.ph	$t2, $t0, $t9		# x1 * adj
	mulq_rs.ph	$t6, $a0, $t9		# in * adj

	sw		$t0, 0($a3)		# delayline[K-2] = x1
	sw		$a0, 4($a3)		# delayline[K-1] = in

	addq_s.ph	$t1, $t1, $t2		# c1 += x1 * adj
	addq_s.ph	$t5, $t5, $t6		# c2 += in * adj

	dpaq_s.w.ph	$ac0, $t0, $t1		# a += x1 * c1
	dpaq_s.w.ph	$ac0, $a0, $t5		# a += in * c2

	sw		$t1, 0($a2)		# coeffs[K-2] = c1
	sw		$t5, 4($a2)		# coeffs[K-1] = c2

	extr_s.h	$v0, $ac0, 16		# result

	subu		$v1, $a1, $v0		# error = ref - result
	
	jr		$ra
	sh		$v1, 0($at)		# *error


	.end		dspr2_mips_lms16
