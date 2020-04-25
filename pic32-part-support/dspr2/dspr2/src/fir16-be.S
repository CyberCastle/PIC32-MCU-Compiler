
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

	.global		dspr2_mips_fir16
	.ent		dspr2_mips_fir16

dspr2_mips_fir16:

# void dspr2_mips_fir16(int16 *outdata, int16 *indata, int16 *coeffs2x, int16 *delayline,
#				int N, int K, int scale)
# $a0 - outdata
# $a1 - indata, indata-outdata
# $a2 - coeffs2x, &coeffs2x[K-dlp]
# $a3 - delayline
# 16($sp) - N = 4*n >= 4
# 20($sp) - K = 4*k >= 4
# 24($sp) - scale
#
# $t4 - data load index
# $t5 - K * sizeof(int16), indata-outdata+2
# $t6 - &delayline[2]
# $t7 - &coeffs2x[K-dlp+2]
# $t8 - temp
# $t9 - &delayline[dlp]
#
# $s0 - 0x4000
# $s1 - &outdata[N]
# $s2 - 16 + scale
# $s3 - &coeffs2x[K]
#
# $v0 - dlp index (dlp * sizeof(int16))
# $v1 - modsub value for dlp


	addiu		$sp, $sp, -16		# reserve stack space for s0-s3

	sw		$s0, 12($sp)
	sw		$s1,  8($sp)
	sw		$s2,  4($sp)
	sw		$s3,  0($sp)

	lhu		$v0, 0($a3)		# dlp = delayline[0]

	sll		$v0, $v0, 1		# dlp * sizeof(int16)
	addu		$t9, $a3, $v0		# &delayline[dlp]

	lw		$s1, 16+16($sp)		# load N
	lw		$t5, 16+20($sp)		# load K
	lw		$s2, 16+24($sp)		# load scale
	
	li		$s0, 0x8000		# accumulator rounding value
	sllv		$s0, $s0, $s2		# rounding value << scale

	lh		$t0, 4($t9)		# delayline[dlp]

	sll		$s1, $s1, 1		# N * sizeof(int16)
	addu		$s1, $a0, $s1		# &outdata[N]

	addiu		$t5, $t5, -4		# K-4
	sll		$t5, $t5, 1		# (K-4) * sizeof(int16)
	addu		$s3, $a2, $t5		# &coeffs2x[K-4]

	sh		$t0, 0($a3)		# delayline[0] = delayline[dlp]

	sll		$v1, $t5, 8		# dlp modsub rollback: (K-4) * sizeof(int16)
	ori		$v1, $v1, 8		# dlp modsub decrement: 8

	addiu		$s2, $s2, 16		# 16 + scale
	addiu		$t6, $a3, 4		# offset pointer for LWX

	subu		$a1, $a1, $a0		# indata offset pointer for LWX
	addiu		$t5, $a1, 4		# indata offset pointer for LWX

	mthi		$zero, $ac0		# zero high accumulator bits
	mthi		$zero, $ac1	
	mthi		$zero, $ac2
	mthi		$zero, $ac3

	.align		4

loopN:
	lwx		$t2, $a1($a0)		# load indata[i,i+1]
	mtlo		$s0, $ac0		# accumulator rounding value

	lwx		$t3, $t5($a0)		# load indata[i+2,i+3]
	mtlo		$s0, $ac1

	lwx		$t0, $t6($v0)		# d[1]d[0]
	mtlo		$s0, $ac2

	lwx		$t1, $a3($v0)		# d[3]d[2]
	move		$t4, $v0		# save dlp

	mtlo		$s0, $ac3

	sw		$t2, 4($t9)		# delayline[dlp+2,dlp+3] = indata[i,i+1]
	subu		$a2, $s3, $v0		# &coeffs2x[K-dlp-4]

	sw		$t3, 0($t9)		# delayline[dlp,dlp+1] = indata[i+2,i+3]
	addiu		$t7, $a2, 4		# offset pointer for LWX
	
	li		$t3, 0			# c[8]c[9] = 0

	.align		4

loopK:	
	lwx		$t2, $t7($v0)		# c[6]c[7]

	dpaq_s.w.ph	$ac3, $t1, $t3		# y[3] += d[7]c[4] + d[6]c[5] (next iteration)

	packrl.ph	$t8, $t3, $t2		# c[7]c[0]
	
	lwx		$t3, $a2($v0)		# c[4]c[5]
	modsub		$v0, $v0, $v1		# if (dlp == 0) dlp = K else dlp -= 4
	
	dpaq_s.w.ph	$ac0, $t0, $t8		# y[0] += d[1]c[7]
	dpaq_s.w.ph	$ac2, $t1, $t8		# y[2] += d[3]c[7]

	lwx		$t0, $t6($v0)		# d[5]d[4]
	packrl.ph	$t8, $t2, $t3		# c[5]c[6]
	
	dpaq_s.w.ph	$ac1, $t1, $t2		# y[1] += d[3]c[6] + d[2]c[7]

	dpaq_s.w.ph	$ac0, $t1, $t8		# y[0] += d[3]c[5] + d[2]c[6]
	lwx		$t1, $a3($v0)		# d[7]d[6]

	dpaq_s.w.ph	$ac3, $t0, $t2		# y[3] += d[5]c[6] + d[4]c[7]
	dpaq_s.w.ph	$ac1, $t0, $t3		# y[1] += d[5]c[4] + d[4]c[5]
	
	bne		$v0, $t4, loopK
	dpaq_s.w.ph	$ac2, $t0, $t8		# y[2] += d[5]c[5] + d[4]c[6]

	# 13 stall cycles - mispredicted branch

	packrl.ph	$t8, $t3, $zero		# 0c[0]

	dpaq_s.w.ph	$ac3, $t1, $t3		# y[3] += d[11]c[0] + d[10]c[1]
	dpaq_s.w.ph	$ac0, $t0, $t8		# y[0] += d[8]c[0]
	dpaq_s.w.ph	$ac2, $t1, $t8		# y[2] += d[10]c[0]

	modsub		$v0, $v0, $v1		# if (dlp == 0) dlp = K else dlp -= 4

	addiu		$a0, $a0, 8		# 4 * sizeof(int16)

	extrv_s.h	$t1, $ac1, $s2
	extrv_s.h	$t3, $ac3, $s2
	extrv_s.h	$t0, $ac0, $s2
	extrv_s.h	$t2, $ac2, $s2

	sh		$t1, -6($a0)
	mthi		$zero, $ac0		# zero high accumulator bits
	sh		$t3, -2($a0)
	mthi		$zero, $ac1	
	sh		$t0, -8($a0)
	mthi		$zero, $ac2
	sh		$t2, -4($a0)
	mthi		$zero, $ac3

	bne		$a0, $s1, loopN
	addu		$t9, $a3, $v0		# address of delayline[dlp]

	# 13 stall cycles - mispredicted branch

	lh		$t0, 0($a3)		# delayline[0]
	srl		$v0, $v0, 1		# dlp / sizeof(int16)

	lw		$s0, 12($sp)
	lw		$s1,  8($sp)
	lw		$s2,  4($sp)
	lw		$s3,  0($sp)

	sh		$t0, 4($t9)		# delayline[dlp] = delayline[0]
	sh		$v0, 0($a3)		# delayline[0] = dlp

	jr		$ra
	addiu		$sp, $sp, 16

	.end		dspr2_mips_fir16



