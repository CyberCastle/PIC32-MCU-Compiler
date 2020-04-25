/* $Id: w_sinh.c,v 1.1 2012/02/08 21:52:37 johnsonl Exp $ */

/* @(#)w_sinh.c 1.3 95/01/18 */
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
 * wrapper sinh(x)
 */

#include "fdlibm.h"

#ifdef FDL_MATH
#define SINH _sinhx
#else
#define SINH sinh
#endif

#ifdef __STDC__
	double SINH(double x)		/* wrapper sinh */
#else
	double SINH(x)			/* wrapper sinh */
	double x;
#endif
{
#ifdef _IEEE_LIBM
	return __ieee754_sinh(x);
#else
	double z; 
	z = __ieee754_sinh(x);
	if(_LIB_VERSION == _IEEE_) return z;
	if(!finite(z)&&finite(x)) {
	    return __kernel_standard(x,x,25); /* sinh overflow */
	} else
	    return z;
#endif
}
