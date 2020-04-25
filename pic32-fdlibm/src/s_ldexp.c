/* $Id: s_ldexp.c,v 1.1 2012/02/08 21:52:37 johnsonl Exp $ */

/* @(#)s_ldexp.c 1.3 95/01/18 */
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

#include "fdlibm.h"
#include <errno.h>

#ifdef FDL_MATH
#define LDEXP _ldexpx
#else
#define LDEXP ldexp
#endif

#ifdef __STDC__
	double LDEXP(double value, int exp)
#else
	double LDEXP(value, exp)
	double value; int exp;
#endif
{
	if(!finite(value)||value==0.0) return value;
	value = scalbn(value,exp);
	if(!finite(value)||value==0.0) errno = ERANGE;
	return value;
}
