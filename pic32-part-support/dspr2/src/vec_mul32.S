
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

	.global		dspr2_mips_vec_mul32
	.ent		dspr2_mips_vec_mul32

dspr2_mips_vec_mul32:

# void dspr2_mips_vec_mul32(int32 *outdata, int32 *indata1, int32 *indata2, int N)
# $a0 - outdata
# $a1 - indata1
# $a2 - indata2
# $a3 - N = 4*n >= 4

	andi		$a3, $a3, 0xFFFC	# make sure N is multiple of 4
	sll		$a3, $a3, 2		# N * sizeof(int32)
	addu		$a3, $a0, $a3		# final address in outdata

	subu		$a1, $a1, $a0		# indata1 pointer offsets for LWX
	addiu		$t4, $a1, 4
	addiu		$t5, $a1, 8
	addiu		$t6, $a1, 12

	subu		$a2, $a2, $a0		# indata2 pointer offsets for LWX
	addiu		$t7, $a2, 4
	addiu		$t8, $a2, 8
	addiu		$t9, $a2, 12
	
	lwx		$t0, $a1($a0)		# load indata1[i]
	lwx		$t1, $t4($a0)		# load indata1[i+1]

	.align		4
	
loop:
	lwx		$t2, $a2($a0)		# load indata2[i]
	lwx		$t3, $t7($a0)		# load indata2[i+1]

	mult		$t0, $t2		# outdata[i]
	mfhi		$v0
	mult		$t1, $t3		# outdata[i+1]
	mfhi		$v1

 	lwx		$t0, $t5($a0)		# load indata1[i+2]
	lwx		$t1, $t6($a0)		# load indata1[i+3]
	
	lwx		$t2, $t8($a0)		# load indata2[i+2]
	lwx		$t3, $t9($a0)		# load indata2[i+3]

	addiu		$a0, $a0, 16		# 4 * sizeof(int32)
	
	mult		$t0, $t2		# outdata[i+2]
	mfhi		$t2
	mult		$t1, $t3		# outdata[i+3]
	mfhi		$t3

	lwx		$t0, $a1($a0)		# load indata1[i+4]
	lwx		$t1, $t4($a0)		# load indata1[i+5]

	sll		$v0, $v0, 1
	sll		$v1, $v1, 1
	sll		$t2, $t2, 1
	sll		$t3, $t3, 1

	sw		$v0, -16($a0)		# store outdata[i]
	sw		$v1, -12($a0)		# store outdata[i+1]
	sw		$t2, -8($a0)		# store outdata[i+2]

	bne		$a0, $a3, loop
	sw		$t3, -4($a0)		# store outdata[i+3]

	# 4 stall cycles - mispredicted branch

        jr		$ra
        ssnop
	
	.end		dspr2_mips_vec_mul32
