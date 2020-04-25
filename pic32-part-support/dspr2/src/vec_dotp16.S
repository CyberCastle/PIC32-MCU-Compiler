
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

	.global	dspr2_mips_vec_dotp16
	.ent	dspr2_mips_vec_dotp16

dspr2_mips_vec_dotp16:

# int16 dspr2_mips_vec_dotp16(int16 *indata1, int16 *indata2, int N, int scale)
# $a0 - indata1
# $a1 - indata2
# $a2 - N = 8*n >= 8
# $a3 - scale

	andi		$a2, $a2, 0xFFF8	# make sure N is multiple of 8
	sll		$a2, $a2, 1		# N * sizeof(int16)
	addu		$a2, $a0, $a2		# final address in indata1

	subu		$t4, $a1, $a0		# indata2 pointer offsets for LWX
	addiu		$t5, $t4, 4
	addiu		$t6, $t4, 8
	addiu		$t7, $t4, 12

	ori		$t0, $zero, 0x8000	# accumulator rounding value
	sllv		$t0, $t0, $a3		# prescale the rounding value
	mthi		$zero, $ac0
	mtlo		$t0, $ac0

	addiu		$a3, $a3, 16		# scale += 16

	lw		$t0, 0($a0)		# load indata1[i,i+1]
	lwx		$t2, $t4($a0)		# load indata2[i,i+1]
	lw		$t1, 4($a0)		# load indata1[i+2,i+3]
	lwx		$t3, $t5($a0)		# load indata2[i+2,i+3]

	.align		4

loop:
	lw		$v0, 8($a0)		# load indata1[i+4,i+5]
	lwx		$t8, $t6($a0)		# load indata2[i+4,i+5]
	lw		$v1, 12($a0)		# load indata1[i+6,i+7]
	lwx		$t9, $t7($a0)		# load indata2[i+6,i+7]

	addiu		$a0, $a0, 16		# 8 * sizeof(int16)

	dpaq_s.w.ph	$ac0, $t0, $t2
	dpaq_s.w.ph	$ac0, $t1, $t3

	lw		$t0, 0($a0)		# load indata1[i,i+1]
	lwx		$t2, $t4($a0)		# load indata2[i,i+1]
	lw		$t1, 4($a0)		# load indata1[i+2,i+3]
	lwx		$t3, $t5($a0)		# load indata2[i+2,i+3]

	dpaq_s.w.ph	$ac0, $v0, $t8

	bne		$a0, $a2, loop
	dpaq_s.w.ph	$ac0, $v1, $t9

	# 13 stall cycles - mispredicted branch

	jr		$ra
	extrv_s.h	$v0, $ac0, $a3	# extract the result

	.end		dspr2_mips_vec_dotp16

