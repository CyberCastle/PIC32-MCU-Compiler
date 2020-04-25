/* $Id: s_finite.c,v 1.2 2012/05/08 20:15:13 wilkiec Exp $ */

/* @(#)s_finite.c 1.3 95/01/18 */
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
 * finite(x) returns 1 is x is finite, else 0;
 * no branching!
 */

#include "fdlibm.h"

#ifdef __STDC__
	int finite(double x)
#else
	int finite(x)
	double x;
#endif
{
	int32_t hx; 
	hx = __HI(x);
	return  (uint32_t)((hx&0x7fffffff)-0x7ff00000)>>31;
}
