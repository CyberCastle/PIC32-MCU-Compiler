/* $Id: w_sqrt.c,v 1.2 2011-12-02 16:23:47-07 luther Exp $ */

/* @(#)w_sqrt.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

/*
 * wrapper sqrt(x)
 */

#include "pic32_fdlibm.h"

#ifdef FDL_MATH
#define SQRT _sqrtx
#else
#define SQRT sqrt
#endif

#ifdef __STDC__
	double SQRT(double x)		/* wrapper sqrt */
#else
	double SQRT(x)			/* wrapper sqrt */
	double x;
#endif
{
#if defined(__mips_hard_float)
	return _sqrtfd (x);             /* use sqrt.d instruction if -mhard-float
                                           option is used with XC32 */
#else
#ifdef _IEEE_LIBM
	return __ieee754_sqrt(x);
#else
	double z;
	z = __ieee754_sqrt(x);
	if(_LIB_VERSION == _IEEE_ || isnan(x)) return z;
	if(x<0.0) {
	    return __kernel_standard(x,x,26); /* sqrt(negative) */
	} else
	    return z;
#endif
#endif
}
