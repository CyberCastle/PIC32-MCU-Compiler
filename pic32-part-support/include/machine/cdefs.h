/*	$OpenBSD: cdefs.h,v 1.2 1999/01/27 04:46:04 imp Exp $	*/
/*	$NetBSD: cdefs.h,v 1.3 1995/05/03 06:04:54 mellon Exp $	*/

/*
 * Copyright (c) 1995 Carnegie-Mellon University.
 * All rights reserved.
 *
 * Author: Chris G. Demetriou
 * 
 * Permission to use, copy, modify and distribute this software and
 * its documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS" 
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND 
 * FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */

#ifndef _MIPS_CDEFS_H_
#define	_MIPS_CDEFS_H_

#define	_C_LABEL(x)	_STRING(x)

/* We don't support indirect references */
/*#define	__indr_reference(sym,alias)*/

#ifdef __STDC__

#define __weak_alias(alias,sym)                         		\
    __asm__(".weakext " #alias "; " #alias "= " __STRING(sym))
#define	__warn_references(sym,msg)					\
    __asm__(".section .gnu.warning." #sym " ; .ascii \"" msg "\" ; .previous")

#else

#define __weak_alias(alias,sym)                         		\
    __asm__(".weakext alias ; alias = sym")
#define	__warn_references(sym,msg)					\
    __asm__(".section .gnu.warning.sym ; .ascii msg ; .previous")

#endif

#endif /* !_MIPS_CDEFS_H_ */
