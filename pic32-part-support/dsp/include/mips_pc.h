/*
 * ****************************Begin Copyright 1D**********************************
 * Unpublished work (c) MIPS Technologies, Inc.  All rights reserved.
 * Unpublished rights reserved under the copyright laws of the United States
 * of America and other countries.
 * 
 * This code is confidential and proprietary to MIPS Technologies, Inc. ("MIPS
 * Technologies") and  may be disclosed only as permitted in writing by MIPS
 * Technologies or an authorized third party.  Any copying, reproducing,
 * modifying, use or disclosure of this code (in whole or in part) that is not
 * expressly permitted in writing by MIPS Technologies or an authorized third
 * party is strictly prohibited.  At a minimum, this code is protected under
 * trade secret, unfair competition, and copyright laws.  Violations thereof
 * may result in criminal penalties and fines.
 * 
 * MIPS Technologies reserves the right to change this code to improve function,
 * design or otherwise.  MIPS Technologies does not assume any liability arising
 * out of the application or use of this code, or of any error or omission in
 * such code.  Any warranties, whether express, statutory, implied or otherwise,
 * including but not limited to the implied warranties of merchantability or 
 * fitness for a particular purpose, are excluded.  Except as expressly provided
 * in any written license agreement from MIPS Technologies or an authorized third
 * party, the furnishing of this code does not give recipient any license to any
 * intellectual property rights, including any patent rights, that cover this code.
 * 
 * This code shall not be exported or transferred for the purpose of reexporting
 * in violation of any U.S. or non-U.S. regulation, treaty, Executive Order, law,
 * statute, amendment or supplement thereto.
 * 
 * This code may only be disclosed to the United States government ("Government"),
 * or to Government users, with prior written consent from MIPS Technologies or an
 * authorized third party.  This code constitutes one or more of the following:
 * commercial computer software, commercial computer software documentation or
 * other commercial items.  If the user of this code, or any related documentation
 * of any kind, including related technical data or manuals, is an agency,
 * department, or other entity of the Government, the use, duplication,
 * reproduction, release, modification, disclosure, or transfer of this code, or
 * any related documentation of any kind, is restricted in accordance with Federal
 * Acquisition Regulation 12.212 for civilian agencies and Defense Federal
 * Acquisition Regulation Supplement 227.7202 for military agencies.  The use of
 * this code by the Government is further restricted in accordance with the terms
 * of the license agreement(s) and/or applicable contract terms and conditions
 * covering this code from MIPS Technologies or an authorized third party.
 * *******************************End Copyright************************************
 */

#ifndef _MIPS_PC_H_
#define _MIPS_PC_H_

#define MIPS_PC0_CYCLES				0
#define MIPS_PC0_INSTRUCTIONS		1
#define MIPS_PC0_STALLS				18
#define MIPS_PC0_DSP_INSTRUCTIONS	26

#define MIPS_PC1_CYCLES				0
#define MIPS_PC1_INSTRUCTIONS		1

/* ========================================================================= */

static inline void mips_pc0_select(int pc)
{
	unsigned v = (pc << 5) | 0x0F;
	asm volatile("mtc0 %0, $25, 0" :: "r" (v));
	asm volatile("ehb");
}

static inline void mips_pc0_reset()
{
	asm volatile("mtc0 $0, $25, 1");
	asm volatile("ehb");
}

static inline unsigned mips_pc0_read()
{
	unsigned v;
	asm volatile("mfc0 %0, $25, 1" : "=r" (v));
	return v;
}

/* ========================================================================= */

static inline void mips_pc1_select(int pc)
{
	unsigned v = (pc << 5) | 0x0F;
	asm volatile("mtc0 %0, $25, 2" :: "r" (v));
	asm volatile("ehb");
}

static inline void mips_pc1_reset()
{
	asm volatile("mtc0 $0, $25, 3");
	asm volatile("ehb");
}

static inline unsigned mips_pc1_read()
{
	unsigned v;
	asm volatile("mfc0 %0, $25, 3" : "=r" (v));
	return v;
}

#endif

