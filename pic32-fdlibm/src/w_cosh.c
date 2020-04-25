/* $Id: w_cosh.c,v 1.1 2012/02/08 21:52:37 johnsonl Exp $ */

/* @(#)w_cosh.c 1.3 95/01/18 */
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
 * wrapper cosh(x)
 */

#include "fdlibm.h"

#ifdef FDL_MATH
#define COSH _coshx
#else
#define COSH cosh
#endif

#ifdef __STDC__
	double COSH(double x)		/* wrapper cosh */
#else
	double COSH(x)			/* wrapper cosh */
	double x;
#endif
{
#ifdef _IEEE_LIBM
	return __ieee754_cosh(x);
#else
	double z;
	z = __ieee754_cosh(x);
	if(_LIB_VERSION == _IEEE_ || isnan(x)) return z;
	if(fabs(x)>7.10475860073943863426e+02) {	
	        return __kernel_standard(x,x,5); /* cosh overflow */
	} else
	    return z;
#endif
}
