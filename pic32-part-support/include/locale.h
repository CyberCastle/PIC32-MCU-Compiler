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
 * locale.h: locale language support
 */


#ifndef __LOCALE_H
#ifdef __cplusplus
extern "C" {
#endif
#define __LOCALE_H

struct lconv {
    /* non monetary numeric values  */
    char	*decimal_point;	
    char	*thousands_sep;	
    char	*grouping;		/* wierd */
    
    /* for monetary values */
    char	*mon_decimal_point;
    char	*mon_thousands_sep;
    char	*mon_grouping;
    char	*int_curr_symbol;	/* e.g NOK in Norway */
    char	*currency_symbol;	/* e.g Kr in Norway */
    char	*positive_sign;		/* e.g "CR"/"+"/"" 	*/
    char	*negative_sign;		/* e.g "DB"/"-"		*/
    char	p_cs_precedes;	
    char	p_sep_by_space;
    char	p_sign_posn;
    char	n_cs_precedes;
    char	n_sep_by_space;
    char	n_sign_posn;
    char	int_frac_digits;	
    char	frac_digits;
};

struct lconv	*localeconv(void);
char *setlocale(int, const char *);

#define LC_ALL      0
#define LC_COLLATE  1
#define LC_CTYPE    2
#define LC_MONETARY 3
#define LC_NUMERIC  4
#define LC_TIME     5
#define LC_NLOCALE  6
#ifdef __cplusplus
}
#endif
#endif
