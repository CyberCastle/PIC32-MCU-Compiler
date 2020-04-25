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
 * signal.h : signal handling definitions
 */


#ifndef __SIGNAL_H
#define __SIGNAL_H

#if 1
#ifndef _SIGNAL
#define _SIGNAL

#ifndef _FULL_SIGNAL_IMPLEMENTATION
#define SIGABRT	1
#define	SIGIOT	1
#define SIGINT	2
#define SIGILL	3
#define SIGFPE	4
#define SIGSEGV	5
#define SIGTERM	6
#define _NSIG	6
#else		//full signal implementation
#define	SIGHUP	1	/* hangup    (not used by terminal driver) */
#define	SIGINT	2	/* interrupt (^C or BREAK) */
#define	SIGQUIT	3	/* quit      (^\) */
#define	SIGILL	4	/* illegal instruction (not reset when caught) */
#define	SIGTRAP	5	/* trace trap (not reset when caught) */
#define	SIGIOT	6	/* IOT instruction */
#define SIGABRT	6	/* a synonym of SIGIOT */
#define	SIGEMT	7	/* EMT instruction */
#define	SIGMSG	7	/* message received */
#define	SIGFPE	8	/* floating point exception */
#define	SIGKILL	9	/* kill (cannot be caught or ignored) */
#define	SIGBUS	10	/* bus error */
#define	SIGSEGV	11	/* segmentation violation */
#define	SIGSYS	12	/* bad argument to system call */
#define	SIGPIPE	13	/* write on a pipe with no one to read it */
#define	SIGALRM	14	/* alarm clock */
#define	SIGTERM	15	/* software termination signal from kill */
#define _NSIG	15
#endif

#define	SIG_DFL	((void (*)(int))0)	/* default action is to exit */
#define	SIG_IGN	((void (*)(int))1)	/* ignore them */
#define SIG_ERR ((void (*)(int))-1)

#ifdef __cplusplus
extern "C" {
#endif

typedef int	sig_atomic_t;

extern void (*	signal(int, void (*)(int)))(int);
extern int raise(int);

#ifdef __cplusplus
}
#endif

#endif

#else

#include <sys/signal.h>

#if !defined(_ANSI_SOURCE) || defined(_POSIX_SOURCE)
#include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_ANSI_SOURCE) && !defined(_POSIX_SOURCE)
extern const char *const sys_signame[_NSIG];
extern const char *const sys_siglist[_NSIG];
#endif

#endif /* 1 */

#ifdef __cplusplus
}
#endif

#endif /* __SIGNAL_H */
