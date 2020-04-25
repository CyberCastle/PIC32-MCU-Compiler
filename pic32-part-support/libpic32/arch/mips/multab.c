#ident "lib/c/arch/mips/multab.c: Copyright (c) MIPS Technologies, Inc. All rights reserved."

/*
 * Unpublished work (c) MIPS Technologies, Inc.  All rights reserved.
 * Unpublished rights reserved under the copyright laws of the United
 * States of America and other countries.
 * 
 * This code is confidential and proprietary to MIPS Technologies,
 * Inc. ("MIPS Technologies") and may be disclosed only as permitted in
 * writing by MIPS Technologies. Any copying, reproducing, modifying,
 * use or disclosure of this code (in whole or in part) that is not
 * expressly permitted in writing by MIPS Technologies is strictly
 * prohibited. At a minimum, this code is protected under trade secret,
 * unfair competition, and copyright laws. Violations thereof may result
 * in criminal penalties and fines.
 * 
 * MIPS Technologies reserves the right to change this code to improve
 * function, design or otherwise. MIPS Technologies does not assume any
 * liability arising out of the application or use of this code, or of
 * any error or omission in such code.  Any warranties, whether express,
 * statutory, implied or otherwise, including but not limited to the
 * implied warranties of merchantability or fitness for a particular
 * purpose, are excluded.  Except as expressly provided in any written
 * license agreement from MIPS Technologies, the furnishing of this
 * code does not give recipient any license to any intellectual property
 * rights, including any patent rights, that cover this code.
 * 
 * This code shall not be exported, reexported, transferred, or released,
 * directly or indirectly, in violation of the law of any country or
 * international law, regulation, treaty, Executive Order, statute,
 * amendments or supplements thereto.  Should a conflict arise regarding
 * the export, reexport, transfer, or release of this code, the laws of
 * the United States of America shall be the governing law.
 * 
 * This code may only be disclosed to the United States government
 * ("Government"), or to Government users, with prior written consent
 * from MIPS Technologies.  This code constitutes one or more of the
 * following: commercial computer software, commercial computer software
 * documentation or other commercial items.  If the user of this code,
 * or any related documentation of any kind, including related technical
 * data or manuals, is an agency, department, or other entity of the
 * Government, the use, duplication, reproduction, release, modification,
 * disclosure, or transfer of this code, or any related documentation
 * of any kind, is restricted in accordance with Federal Acquisition
 * Regulation 12.212 for civilian agencies and Defense Federal Acquisition
 * Regulation Supplement 227.7202 for military agencies.  The use of this
 * code by the Government is further restricted in accordance with the
 * terms of the license agreement(s) and/or applicable contract terms
 * and conditions covering this code from MIPS Technologies.
 * 
 * 
 */

/* 
 * multab.c: 4bit x 4bit -> 8bit multiplication table 
 */


const unsigned char __mul4x4tab[256] = 
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
    0,  2,  4,  6,  8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30,
    0,  3,  6,  9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45,
    0,  4,  8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60,
    0,  5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75,
    0,  6, 12, 18, 24, 30, 36, 42, 48, 54, 60, 66, 72, 78, 84, 90,
    0,  7, 14, 21, 28, 35, 42, 49, 56, 63, 70, 77, 84, 91, 98,105,
    0,  8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96,104,112,120,
    0,  9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99,108,117,126,135,
    0, 10, 20, 30, 40, 50, 60, 70, 80, 90,100,110,120,130,140,150,
    0, 11, 22, 33, 44, 55, 66, 77, 88, 99,110,121,132,143,154,165,
    0, 12, 24, 36, 48, 60, 72, 84, 96,108,120,132,144,156,168,180,
    0, 13, 26, 39, 52, 65, 78, 91,104,117,130,143,156,169,182,195,
    0, 14, 28, 42, 56, 70, 84, 98,112,126,140,154,168,182,196,210,
    0, 15, 30, 45, 60, 75, 90,105,120,135,150,165,180,195,210,225
};
