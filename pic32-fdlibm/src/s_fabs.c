/* $Id: s_fabs.c,v 1.1 2012/02/08 21:52:37 johnsonl Exp $ */

/* @(#)s_fabs.c 1.3 95/01/18 */
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
 * fabs(x) returns the absolute value of x.
 */

#include "fdlibm.h"

#ifdef FDL_MATH
#define FABS _fabsx
#else
#define FABS fabs
#endif

#ifdef __STDC__
	double FABS(double x)
#else
	double FABS(x)
	double x;
#endif
{
	__HI(x) &= 0x7fffffff;
        return x;
}
