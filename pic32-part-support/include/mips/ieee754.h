/*
 * Copyright (c) 1996-2003 MIPS Technologies, Inc.
 * All rights reserved.
 * 
 * Unpublished rights (if any) reserved under the copyright laws of the
 * United States of America and other countries.
 * 
 * This code is proprietary to MIPS Technologies, Inc. ("MIPS
 * Technologies").  Any copying, reproducing, modifying or use of
 * this code (in whole or in part) that is not expressly permitted
 * in writing by MIPS Technologies or an authorized third party is
 * strictly prohibited.  At a minimum, this code is protected under
 * unfair competition and copyright laws.  Violations thereof may result
 * in criminal penalties and fines.
 * 
 * MIPS Technologies reserves the right to change this code to improve
 * function, design or otherwise.  MIPS Technologies does not assume any
 * liability arising out of the application or use of this code, or of
 * any error or omission in such code.  Any warranties, whether express,
 * statutory, implied or otherwise, including but not limited to the
 * implied warranties of merchantability or fitness for a particular
 * purpose, are excluded.  Except as expressly provided in any written
 * license agreement from MIPS Technologies or an authorized third party,
 * the furnishing of this code does not give recipient any license to
 * any intellectual property rights, including any patent rights, that
 * cover this code.
 * 
 * This code shall not be exported, reexported, transferred, or released,
 * directly or indirectly, in violation of the law of any country or
 * international law, regulation, treaty, Executive Order, statute,
 * amendments or supplements thereto.  Should a conflict arise regarding
 * the export, reexport, transfer, or release of this code, the laws of
 * the United States of America shall be the governing law.
 * 
 * This code constitutes one or more of the following: commercial
 * computer software, commercial computer software documentation or
 * other commercial items.  If the user of this code, or any related
 * documentation of any kind, including related technical data or
 * manuals, is an agency, department, or other entity of the United
 * States government ("Government"), the use, duplication, reproduction,
 * release, modification, disclosure, or transfer of this code, or
 * any related documentation of any kind, is restricted in accordance
 * with Federal Acquisition Regulation 12.212 for civilian agencies
 * and Defense Federal Acquisition Regulation Supplement 227.7202 for
 * military agencies.  The use of this code by the Government is further
 * restricted in accordance with the terms of the license agreement(s)
 * and/or applicable contract terms and conditions covering this code
 * from MIPS Technologies or an authorized third party.
 * 
 * 
 */

/*
 * ieee754.h : MIPS IEEE floating point definitions
 */


#ifndef IEEE754

/*
 * Machines supporting ANSI/IEEE Std 754-1985
 * not yet complete
*/
#define IEEE754 1985


#define DBL_EMIN	(-1023)
#define DBL_EMAX	1023
#define DBL_EBIAS	1023
#define DBL_EXPLEN	11
#define DBL_FRALEN	52
#define DBL_MAXINT	??		/* max int can store */

#define FLT_EMIN	(-126)
#define FLT_EMAX	127
#define FLT_EBIAS	127
#define FLT_EXPLEN	8
#define FLT_FRALEN	23
#define FLT_MAXINT	??		/* max int can store */

#define LDBL_EMIN	DBL_EMIN
#define LDBL_EMAX	DBL_EMAX
#define LDBL_EBIAS	DBL_EBIAS
#define LDBL_EXPLEN	DBL_EXPLEN
#define LDBL_FRALEN	DBL_FRALEN
#define LDBL_MAXINT	DBL_MAXINT

#endif /* IEEE754 */


