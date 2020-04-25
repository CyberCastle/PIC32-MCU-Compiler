/* $Id: w_fmod.c,v 1.2 2012/05/08 20:15:13 wilkiec Exp $ */

/* @(#)w_fmod.c 1.3 95/01/18 */
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
 * wrapper fmod(x,y)
 */

#include "fdlibm.h"

#ifdef FDL_MATH
#define FMOD _fmodx
#else
#define FMOD fmod
#endif

#ifdef __STDC__
	double FMOD(double x, double y)	/* wrapper fmod */
#else
	double FMOD(x,y)		/* wrapper fmod */
	double x,y;
#endif
{
#ifdef _IEEE_LIBM
	return __ieee754_fmod(x,y);
#else
	double z;
	z = __ieee754_fmod(x,y);
	if(_LIB_VERSION == _IEEE_ ||isnan(y)||isnan(x)) return z;
	if(y==0.0) {
	        return __kernel_standard(x,y,27); /* fmod(x,0) */
	} else
	    return z;
#endif
}
