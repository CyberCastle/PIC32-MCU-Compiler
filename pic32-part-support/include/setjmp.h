/*
 * Copyright (c) 1996-2006 MIPS Technologies, Inc.
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
 * setjmp.h : setjmp/lonjmp definitions
 */


#ifndef __SETJMP_H
#ifdef __cplusplus
extern "C" {
#endif
#define __SETJMP_H


/* --- Constants --- */
#ifndef _JB_LEN

#if defined(i8088)
#define _JB_LEN	3
#elif defined(ATARI_ST)
#define _JB_LEN	13
#elif defined(__MIPSEB__) || defined(__MIPSEL__)
#define _JB_LEN 24		/* lots of registers */
#define _JB_TYPE long long	/* always use max reg size */
#elif defined(__sparc__)
#define _JB_LEN 9		/* tiny */
#elif defined(__mc68000__) || defined(__mc68020__) || defined (__mc68030__) 
#define _JB_LEN 14		/* enough */
#elif defined(__ns32000__)
#define _JB_LEN 14
#elif defined(__i386__)
#define _JB_LEN 6
#else
#error _JB_LEN not defined
#endif

#endif

#ifndef _JB_TYPE
#define _JB_TYPE int
#endif

/* --- Types --- */
typedef _JB_TYPE	jmp_buf[_JB_LEN];
typedef _JB_TYPE	sigjmp_buf[_JB_LEN + 1];

/* --- Prototypes --- */
int	setjmp (jmp_buf);
void	longjmp (jmp_buf, int);
int	sigsetjmp (sigjmp_buf, int);
void	siglongjmp (sigjmp_buf, int);
#ifndef _POSIX_SOURCE
int	_setjmp (jmp_buf);
void	_longjmp (jmp_buf, int);
#endif

#ifdef __cplusplus
}
#endif
#endif /* !defined __SETJMP_H */

