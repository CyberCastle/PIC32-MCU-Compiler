/*
 * Copyright (c) 1998-2003 MIPS Technologies, Inc.
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
 * mips/types.h : basic type definitions for MIPS
 */


#ifndef __MACHINE_TYPES_H_

#define __MACHINE_TYPES_H_

#ifndef __ASSEMBLER__

#include <machine/int_types.h>

/* Backwards compatibility */
#ifndef	int8_t
typedef	__int8_t	int8_t;
#define	int8_t		__int8_t
#endif
#ifndef	uint8_t
typedef	__int8_t	uint8_t;
#define	uint8_t		__uint8_t
#endif
#ifndef	u_int8_t
typedef	__int8_t	u_int8_t;
#define	u_int8_t	__uint8_t
#endif
#ifndef	int16_t
typedef	__int16_t	int16_t;
#define	int16_t		__int16_t
#endif
#ifndef	uint16_t
typedef	__int16_t	uint16_t;
#define	uint16_t	__uint16_t
#endif
#ifndef	u_int16_t
typedef	__int16_t	u_int16_t;
#define	u_int16_t	__uint16_t
#endif
#ifndef	int32_t
typedef	__int32_t	int32_t;
#define	int32_t		__int32_t
#endif
#ifndef	uint32_t
typedef	__int32_t	uint32_t;
#define	uint32_t	__uint32_t
#endif
#ifndef	u_int32_t
typedef	__int32_t	u_int32_t;
#define	u_int32_t	__uint32_t
#endif
#ifdef __GNUC__
#ifndef	int64_t
typedef	__int64_t	int64_t;
#define	int64_t		__int64_t
#endif
#ifndef	uint64_t
typedef	__int64_t	uint64_t;
#define	uint64_t	__uint64_t
#endif
#ifndef	u_int64_t
typedef	__int64_t	u_int64_t;
#define	u_int64_t	__uint64_t
#endif
#endif

/* The longest/most efficient integer register type. */
#if __mips64
typedef __int64_t 	    register_t;
#else
typedef __int32_t 	    register_t;
#endif

#endif /* ! __ASSEMBLER__ */

#endif /* __MACHINE_TYPES_H_ */
