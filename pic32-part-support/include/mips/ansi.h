/*
 * Copyright (c) 1996-2004 MIPS Technologies, Inc.
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
 * ansi.h : machine dependent ANSI type definitions
 */


#ifndef __MIPS_ANSI_H_
#define __MIPS_ANSI_H_
#define _ANSI_H_

/*
 * Types which are fundamental to the implementation and may appear in
 * more than one standard header are defined here.  Standard headers
 * then use:
 *	#ifdef	_SIZE_T_
 *	typedef	_SIZE_T_ size_t;
 *	#undef	_SIZE_T_
 *	#endif
 */

/* Types which may be defined by compiler. */

#ifdef __SIZE_TYPE__
#define _SIZE_T_	__SIZE_TYPE__
/* gcc >= 3.x */
#else
/* gcc < 3.x */
#define _SIZE_T_	__typeof(sizeof(int))
#endif

#ifdef __PTRDIFF_TYPE__
/* gcc >= 3.x */
#define _PTRDIFF_T_	__PTRDIFF_TYPE__
#else
/* gcc < 3.x */
#define _PTRDIFF_T_	long
#endif

#ifdef __WCHAR_TYPE__
#define _WCHAR_T_	__WCHAR_TYPE__
#else
#define _WCHAR_T_	int
#endif

#ifdef __WINT_TYPE__
#define _WINT_T_	__WINT_TYPE__
#else
#define _WINT_T_	unsigned int
#endif

/* system types - not defined by compiler */
#define _SSIZE_T_	long
#define _OFF_T_		int64_t
#define _CLOCK_T_	unsigned long
#define _TIME_T_	unsigned long
#define _VA_LIST_	void *
#define _TIMER_T_	void *
#define _CLOCKID_T_	int

#endif /* !__MIPS_ANSI_H */
