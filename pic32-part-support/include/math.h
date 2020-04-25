/*-------------------------------------------------------------------------
 * MPLAB XC32 Compiler - math.h
 *
 * Copyright (c) 2015, Microchip Technology Inc. and its subsidiaries ("Microchip")
 * All rights reserved.
 *
 * This software is developed by Microchip Technology Inc. and its
 * subsidiaries ("Microchip").
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1.      Redistributions of source code must retain the above copyright
 *         notice, this list of conditions and the following disclaimer.
 * 2.      Redistributions in binary form must reproduce the above
 *         copyright notice, this list of conditions and the following
 *         disclaimer in the documentation and/or other materials provided
 *         with the distribution.
 * 3.      Microchip's name may not be used to endorse or promote products
 *         derived from this software without specific prior written
 *         permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MICROCHIP "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL MICROCHIP BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING BUT NOT LIMITED TO
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWSOEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#pragma once

#ifndef __MATH_H
#define __MATH_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HUGE_VAL
/* Positive Infinity in a double */
#if 0
#define HUGE_VAL	(__extension__ 0x1.0p2047)
#else
#define HUGE_VAL (__DBL_MAX__ * 2.0)
#endif
#endif

#if !defined(_ANSI_SOURCE) && !defined(_POSIX_SOURCE)
#ifndef HUGE
#define	HUGE	HUGE_VAL
#endif
#endif

#define	M_E		2.7182818284590452354	/* e */
#define	M_LOG2E		1.4426950408889634074	/* log 2e */
#define	M_LOG10E	0.43429448190325182765	/* log 10e */
#define	M_LN2		0.69314718055994530942	/* log e2 */
#define	M_LN10		2.30258509299404568402	/* log e10 */
#define	M_PI		3.14159265358979323846	/* pi */
#define	M_PI_2		1.57079632679489661923	/* pi/2 */
#define	M_PI_4		0.78539816339744830962	/* pi/4 */
#define	M_1_PI		0.31830988618379067154	/* 1/pi */
#define	M_2_PI		0.63661977236758134308	/* 2/pi */
#define	M_2_SQRTPI	1.12837916709551257390	/* 2/sqrt(pi) */
#define	M_SQRT2		1.41421356237309504880	/* sqrt(2) */
#define	M_SQRT1_2	0.70710678118654752440	/* 1/sqrt(2) */

#if defined(__GNUC__)
#define __attrconst __attribute__((__const))
#else
#define __attrconst
#endif

/*
 *   prototypes for the standard C math functions.
 */

#if defined(__mips_hard_float)

#if (__DBL_MANT_DIG__ == __FLT_MANT_DIG__) && !defined(__FDLMATH_SHORT_DOUBLE)
#define __FDLMATH_SHORT_DOUBLE
#endif

#include <fdlmath.h>
double	rint (double);
float	rintf (float);
long double	rintl (long double);

/* from old math library */

#if (__DBL_MANT_DIG__ == __FLT_MANT_DIG__)
#define __MPROTO(x)   x ## f
#define __MPROTOL(x)  x ## f
#else
#define __MPROTO(x)   x ## f
#define __MPROTOL(x)  x ## l
#endif

int	finite (double) __attrconst;
int	finitef (float) __attrconst;
int	finitel (long double) __attrconst;
int	isinf (double) __attrconst;
int	isinff (float) __attrconst;
int	isinfl (long double) __attrconst;
int	isnan (double) __attrconst;
int	isnanf (float) __attrconst;
int	isnanl (long double) __attrconst;

#if !defined(_ANSI_SOURCE) && !defined(_POSIX_SOURCE)
/* "f", "l", and "" (double) math.h function routing */
#if defined(__FDLMATH_SHORT_DOUBLE) || defined(SHORT_DOUBLE)
#define __FDLMATH_XDOUBLE long double
#else
#define __FDLMATH_XDOUBLE double
#endif
#define __FDLMATH_INLINE static inline __attribute__((always_inline))

extern __FDLMATH_XDOUBLE _expm1x(__FDLMATH_XDOUBLE x);
__FDLMATH_INLINE float expm1f(float x)
{
    return (float)_expm1x((__FDLMATH_XDOUBLE)x);
}
__FDLMATH_INLINE double expm1(double x)
{
    return (double)_expm1x((__FDLMATH_XDOUBLE)x);
}
__FDLMATH_INLINE long double expm1l(long double x)
{
    return (long double)_expm1x((__FDLMATH_XDOUBLE)x);
}
#undef __FDLMATH_XDOUBLE
#undef __FDLMATH_INLINE

extern double	acosh (double);
extern float	acoshf (float);
extern long double	acoshl (long double);

extern double	asinh (double);
extern float	asinhf (float);
extern long double	asinhl (long double);

extern double	atanh (double);
extern float	atanhf (float);
extern long double	atanhl (long double);

extern double	cbrt (double);
extern float	cbrtf (float);
extern long double	cbrtl (long double);

extern double	copysign (double, double) __attrconst;
extern float	copysignf (float, float) __attrconst;
extern long double	copysignl (long double, long double) __attrconst;

extern double	drem (double, double);
extern float	dremf (float, float);
extern long double	dreml (long double, long double);

extern double	hypot (double, double);
extern float	hypotf (float, float);
extern long double	hypotl (long double, long double);

extern double	log1p (double);
extern float	log1pf (float);
extern long double	log1pl (long double);

extern double	logb (double);
extern float	logbf (float);
extern long double	logbl (long double);
#endif

#define	acosh(p) __MPROTOL(acosh)(p)
#define	asinh(p) __MPROTOL(asinh)(p)
#define	atanh(p) __MPROTOL(atanh)(p)
#define	cbrt(p) __MPROTOL(cbrt)(p)
#define	copysign(p1,p2) __MPROTOL(copysign)(p1,p2)
#define	drem(p1,p2) __MPROTOL(drem)(p1,p2)
#define	finite(p) __MPROTOL(finite)(p)
#define	hypot(p1,p2) __MPROTOL(hypot)(p1,p2)
#define	isinf(p) __MPROTOL(isinf)(p)
#define	log1p(p) __MPROTOL(log1p)(p)
#define	logb(p) __MPROTOL(logb)(p)

#else /* !defined(__mips_hard_float) */

#if (__DBL_MANT_DIG__ != __FLT_MANT_DIG__)
double	acos (double);
double	asin (double);
double	atan (double);
double	atan2 (double, double);
double	ceil (double) __attrconst;
double	cos (double);
double	cosh (double);
double	exp (double);
double	fabs (double) __attrconst;
double	floor (double) __attrconst;
double	fmod (double, double) __attrconst;
double	frexp (double, int *);
double	ldexp (double, int);
double	log (double);
double	log10 (double);
double	modf (double, double *);
double	pow (double, double);
double	sin (double);
double	sinh (double);
double	tan (double);
double	tanh (double);
#else

#endif /* (__DBL_MANT_DIG__ != __FLT_MANT_DIG__) */

long double sqrtl (long double x);
long double atanl (long double x);
long double atan2l(long double y, long double x);
long double asinl(long double x);
long double acosl(long double x);
long double ceill (long double x) __attrconst;
long double cosl (long double x);
long double coshl (long double x);
long double expl (long double x);
long double fabsl (long double x) __attrconst;
long double floorl (long double x) __attrconst;
long double fmodl (long double, long double) __attrconst;
long double frexpl (long double x, int *);
long double ldexpl (long double x, int);
long double logl (long double x);
long double log10l (long double x);
#if (__DBL_MANT_DIG__ == __FLT_MANT_DIG__)
long double modfl(long double x, long double *);
#else
long double modfl(double, void *);
#endif
long double powl (long double base, long double exponent);
long double sinl (long double x);
long double sinhl(long double x);
long double tanl (long double x);
long double tanhl (long double x);

/*
 *  For PIC32, a long double is always 64 bits regardless of the setting of the
 *  -fno-short-double and -fshort-double options.
 */
#if !defined(_ANSI_SOURCE) && !defined(_POSIX_SOURCE)
double	acosh (double);
float	acoshf (float);
long double	acoshl (long double);

double	asinh (double);
float	asinhf (float);
long double	asinhl (long double);

double	atanh (double);
float	atanhf (float);
long double	atanhl (long double);

double	cbrt (double);
float	cbrtf (float);
long double	cbrtl (long double);

double	copysign (double, double) __attrconst;
float	copysignf (float, float) __attrconst;
long double	copysignl (long double, long double) __attrconst;

double	drem (double, double);
float	dremf (float, float);
long double	dreml (long double, long double);

double	expm1 (double);
float	expm1f (float);
long double	expm1l (long double);

int	finite (double) __attrconst;
int	finitef (float) __attrconst;
int	finitel (long double) __attrconst;

double	hypot (double, double);
float	hypotf (float, float);
long double	hypotl (long double, long double);

#if defined(vax) || defined(tahoe)
long double	infnan (int);
#endif

int	isinf (double) __attrconst;
int	isinff (float) __attrconst;
int	isinfl (long double) __attrconst;

int	isnan (double) __attrconst;
int	isnanf (float) __attrconst;
int	isnanl (long double) __attrconst;

double	log1p (double);
float	log1pf (float);
long double	log1pl (long double);

double	logb (double);
float	logbf (float);
long double	logbl (long double);

double	rint (double);
float	rintf (float);
long double	rintl (long double);

#endif  /* !defined(_ANSI_SOURCE) && !defined(_POSIX_SOURCE) */

/*
 *   prototypes for the float math functions.
 */
float acosf(float);
float asinf(float);
float atanf(float);
float atan2f(float, float);
float sqrtf (float);
float ceilf(float) __attrconst;
float cosf(float);
float coshf(float);
float expf(float);
float fabsf(float) __attrconst;
float floorf(float) __attrconst;
float fmodf(float, float) __attrconst;
float frexpf(float, int *);
float ldexpf(float, int);
float logf(float);
float log10f(float);

/*
 *   Without -fno-short-double, float * and double * are compatible, but a
 *   warning can still be produced.
 */
#if (__DBL_MANT_DIG__ == __FLT_MANT_DIG__)
float modff(float, void *);
#else
float modff(float, float *);
#endif

float powf(float, float);
float sinf(float);
float sinhf(float);
float sqrtf(float);
float tanf(float);
float tanhf(float);

#if (__DBL_MANT_DIG__ == __FLT_MANT_DIG__)
#define __MPROTO(x)   x ## f
#define __MPROTOL(x)  x ## f
#else
#define __MPROTO(x)   x ## f
#define __MPROTOL(x)  x ## l
#endif

#define	acos(p) __MPROTOL(acos)(p)
#define	asin(p) __MPROTOL(asin)(p)
#define	atan(p) __MPROTOL(atan)(p)
#define	atan2(p1,p2) __MPROTOL(atan2)(p1,p2)
#define	sqrt(p) __MPROTOL(sqrt)(p)
#define	ceil(p) __MPROTOL(ceil)(p)
#define	cos(p) __MPROTOL(cos)(p)
#define	cosh(p) __MPROTOL(cosh)(p)
#define	exp(p) __MPROTOL(exp)(p)
#define	fabs(p) __MPROTOL(fabs)(p)
#define	floor(p) __MPROTOL(floor)(p)
#define	fmod(p1,p2) __MPROTOL(fmod)(p1,p2)
#define	frexp(p1,p2) __MPROTOL(frexp)(p1,p2)
#define	ldexp(p1,p2) __MPROTOL(ldexp)(p1,p2)
#define	log(p) __MPROTOL(log)(p)
#define	log10l(p) __MPROTOL(log10)(p)
#define	modf(p1,p2) __MPROTOL(modf)(p1,p2)
#define	pow(p1,p2) __MPROTOL(pow)(p1,p2)
#define	sin(p) __MPROTOL(sin)(p)
#define	sinh(p) __MPROTOL(sinh)(p)
#define	tan(p) __MPROTOL(tan)(p)
#define	tanh(p) __MPROTOL(tanh)(p)

#define	acosh(p) __MPROTOL(acosh)(p)
#define	asinh(p) __MPROTOL(asinh)(p)
#define	atanh(p) __MPROTOL(atanh)(p)
#define	cbrt(p) __MPROTOL(cbrt)(p)
#define	copysign(p1,p2) __MPROTOL(copysign)(p1,p2)
#define	drem(p1,p2) __MPROTOL(drem)(p1,p2)
#define	expm1(p) __MPROTOL(expm1)(p)
#define	finite(p) __MPROTOL(finite)(p)
#define	hypot(p1,p2) __MPROTOL(hypot)(p1,p2)
#define	isinf(p) __MPROTOL(isinf)(p)
#define	log1p(p) __MPROTOL(log1p)(p)
#define	logb(p) __MPROTOL(logb)(p)
#define	rint(p) __MPROTOL(rint)(p)

#endif /*__mips_hard_float*/

#undef __attrconst

/* Global control over mchplibm error handling.  */

#define MATH_ERRNO     (1 << 0)
#define MATH_ERREXCEPT (1 << 1)

extern unsigned int __attribute__((section(".data.math_errhandling_flag"))) math_errhandling;

#ifdef __cplusplus
}
#endif


#endif /* __MATH_H */
