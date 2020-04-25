/* Tiny test of all math functions */
#include <stdio.h>
#include <stdint.h>

#ifdef __XC32__
#include <xc.h>
#endif

#ifdef HEXOUT
#include <stdint.h>
#endif

/* This is for f, "" (d), and l wrappers around FDLIBM library */
#ifdef FDL_MATH
#include "fdlmath.h"
#else
#include <math.h>
#endif

int main(int argc, char *argv[])
{
#ifdef __XC32__
    __XC_UART = 1;
#endif

#ifdef __i386__
#include <fpu_control.h>
#include <stdint.h>
    uint16_t cw;

    cw = 0x23f;
    _FPU_SETCW(cw);
#endif

    int e;
    float f1, f2;
    double d1, d2;
    long double l1, l2;

#ifdef HEXOUT
    /* "f" functions */
    f1 = acosf(-1.0);
    printf("acosf(-1.0)\t\t%08lx\n", *(int32_t *)&f1);
    f1 = asinf(1.0);
    printf("asinf(1.0)\t\t%08lx\n", *(int32_t *)&f1);
    f1 = atanf(1.0);
    printf("atanf(1.0)\t\t%08lx\n", *(int32_t *)&f1);
    f1 = atan2f(1.0, 1.0);
    printf("atan2f(1.0, 1.0)\t%08lx\n", *(int32_t *)&f1);
    f1 = ceilf(1.5);
    printf("ceilf(1.5)\t\t%08lx\n", *(int32_t *)&f1);
    f1 = cosf(1.0);
    printf("cosf(1.0)\t\t%08lx\n", *(int32_t *)&f1);
    f1 = coshf(1.0);
    printf("coshf(1.0)\t\t%08lx\n", *(int32_t *)&f1);
    f1 = expf(1.0);
    printf("expf(1.0)\t\t%08lx\n", *(int32_t *)&f1);
    f1 = fabsf(-1.0);
    printf("fabsf(-1.0)\t\t%08lx\n", *(int32_t *)&f1);
    f1 = floorf(1.5);
    printf("floorf(1.5)\t\t%08lx\n", *(int32_t *)&f1);
    f1 = fmodf(3.0, 2.0);
    printf("fmodf(3.0, 2.0)\t\t%08lx\n", *(int32_t *)&f1);
    f1 = frexpf(1.0, &e);
    printf("frexpf(1.0, &e)\t\t%08lx,%d\n", *(int32_t *)&f1, e);
    f1 = ldexpf(1.0, 1);
    printf("ldexpf(1.0, 1)\t\t%08lx\n", *(int32_t *)&f1);
    f1 = logf(2.0);
    printf("logf(2.0)\t\t%08lx\n", *(int32_t *)&f1);
    f1 = log10f(2.0);
    printf("log10f(2.0)\t\t%08lx\n", *(int32_t *)&f1);
    f1 = modff(1.5, &f2);
    printf("modff(1.5, &f)\t\t%08lx,%08lx\n", *(int32_t *)&f1, *(int32_t *)&f2);
    f1 = powf(2.0, -1.0);
    printf("powf(2.0, -1.0)\t\t%08lx\n", *(int32_t *)&f1);
    f1 = sinf(1.0);
    printf("sinf(1.0)\t\t%08lx\n", *(int32_t *)&f1);
    f1 = sinhf(1.0);
    printf("sinhf(1.0)\t\t%08lx\n", *(int32_t *)&f1);
    f1 = sqrtf(2.0);
    printf("sqrtf(2.0)\t\t%08lx\n", *(int32_t *)&f1);
    f1 = tanf(1.0);
    printf("tanf(1.0)\t\t%08lx\n", *(int32_t *)&f1);
    f1 = tanhf(1.0);
    printf("tanhf(1.0)\t\t%08lx\n", *(int32_t *)&f1);

    /* "" (d) functions */
#ifdef SHORT_DOUBLE
    d1 = acos(-1.0);
    printf("acos(-1.0)\t\t%08lx\n", *(int32_t *)&d1);
    d1 = asin(1.0);
    printf("asin(1.0)\t\t%08lx\n", *(int32_t *)&d1);
    d1 = atan(1.0);
    printf("atan(1.0)\t\t%08lx\n", *(int32_t *)&d1);
    d1 = atan2(1.0, 1.0);
    printf("atan2(1.0, 1.0)\t\t%08lx\n", *(int32_t *)&d1);
    d1 = ceil(1.5);
    printf("ceil(1.5)\t\t%08lx\n", *(int32_t *)&d1);
    d1 = cos(1.0);
    printf("cos(1.0)\t\t%08lx\n", *(int32_t *)&d1);
    d1 = cosh(1.0);
    printf("cosh(1.0)\t\t%08lx\n", *(int32_t *)&d1);
    d1 = exp(1.0);
    printf("exp(1.0)\t\t%08lx\n", *(int32_t *)&d1);
    d1 = fabs(-1.0);
    printf("fabs(-1.0)\t\t%08lx\n", *(int32_t *)&d1);
    d1 = floor(1.5);
    printf("floor(1.5)\t\t%08lx\n", *(int32_t *)&d1);
    d1 = fmod(3.0, 2.0);
    printf("fmod(3.0, 2.0)\t\t%08lx\n", *(int32_t *)&d1);
    d1 = frexp(1.0, &e);
    printf("frexp(1.0, &e)\t\t%08lx,%d\n", *(int32_t *)&d1, e);
    d1 = ldexp(1.0, 1);
    printf("ldexp(1.0, 1)\t\t%08lx\n", *(int32_t *)&d1);
    d1 = log(2.0);
    printf("log(2.0)\t\t%08lx\n", *(int32_t *)&d1);
    d1 = log10(2.0);
    printf("log10(2.0)\t\t%08lx\n", *(int32_t *)&d1);
    d1 = modf(1.5, &d2);
    printf("modf(1.5, &f)\t\t%08lx,%08lx\n", *(int32_t *)&d1, *(int32_t *)&d2);
    d1 = pow(2.0, -1.0);
    printf("pow(2.0, -1.0)\t\t%08lx\n", *(int32_t *)&d1);
    d1 = sin(1.0);
    printf("sin(1.0)\t\t%08lx\n", *(int32_t *)&d1);
    d1 = sinh(1.0);
    printf("sinh(1.0)\t\t%08lx\n", *(int32_t *)&d1);
    d1 = sqrt(2.0);
    printf("sqrt(2.0)\t\t%08lx\n", *(int32_t *)&d1);
    d1 = tan(1.0);
    printf("tan(1.0)\t\t%08lx\n", *(int32_t *)&d1);
    d1 = tanh(1.0);
    printf("tanh(1.0)\t\t%08lx\n", *(int32_t *)&d1);
#else
    printf("acos(-1.0)\t\t%016llx\n", acos(-1.0));
    printf("asin(1.0)\t\t%016llx\n", asin(1.0));
    printf("atan(1.0)\t\t%016llx\n", atan(1.0));
    printf("atan2(1.0, 1.0)\t\t%016llx\n", atan2(1.0, 1.0));
    printf("ceil(1.5)\t\t%016llx\n", ceil(1.5));
    printf("cos(1.0)\t\t%016llx\n", cos(1.0));
    printf("cosh(1.0)\t\t%016llx\n", cosh(1.0));
    printf("exp(1.0)\t\t%016llx\n", exp(1.0));
    printf("fabs(-1.0)\t\t%016llx\n", fabs(-1.0));
    printf("floor(1.5)\t\t%016llx\n", floor(1.5));
    printf("fmod(3.0, 2.0)\t\t%016llx\n", fmod(3.0, 2.0));
    d1 = frexp(1.0, &e);
    printf("frexp(1.0, &e)\t\t%016llx,%d\n", d1, e);
    printf("ldexp(1.0, 1)\t\t%016llx\n", ldexp(1.0, 1));
    printf("log(2.0)\t\t%016llx\n", log(2.0));
    printf("log10(2.0)\t\t%016llx\n", log10(2.0));
    d1 = modf(1.5, &d2);
    printf("modf(1.5, &f)\t\t%016llx,%016llx\n", d1, d2);
    printf("pow(2.0, -1.0)\t\t%016llx\n", pow(2.0, -1.0));
    printf("sin(1.0)\t\t%016llx\n", sin(1.0));
    printf("sinh(1.0)\t\t%016llx\n", sinh(1.0));
    printf("sqrt(2.0)\t\t%016llx\n", sqrt(2.0));
    printf("tan(1.0)\t\t%016llx\n", tan(1.0));
    printf("tanh(1.0)\t\t%016llx\n", tanh(1.0));
#endif

    /* "l" functions */
    printf("acosl(-1.0)\t\t%016llx\n", acosl(-1.0));
    printf("asinl(1.0)\t\t%016llx\n", asinl(1.0));
    printf("atanl(1.0)\t\t%016llx\n", atanl(1.0));
    printf("atan2l(1.0, 1.0)\t%016llx\n", atan2l(1.0, 1.0));
    printf("ceill(1.5)\t\t%016llx\n", ceill(1.5));
    printf("cosl(1.0)\t\t%016llx\n", cosl(1.0));
    printf("coshl(1.0)\t\t%016llx\n", coshl(1.0));
    printf("expl(1.0)\t\t%016llx\n", expl(1.0));
    printf("fabsl(-1.0)\t\t%016llx\n", fabsl(-1.0));
    printf("floorl(1.5)\t\t%016llx\n", floorl(1.5));
    printf("fmodl(3.0, 2.0)\t\t%016llx\n", fmodl(3.0, 2.0));
    l1 = frexpl(1.0, &e);
    printf("frexpl(1.0, &e)\t\t%016llx,%d\n", l1, e);
    printf("ldexpl(1.0, 1)\t\t%016llx\n", ldexpl(1.0, 1));
    printf("logl(2.0)\t\t%016llx\n", logl(2.0));
    printf("log10l(2.0)\t\t%016llx\n", log10l(2.0));
    l1 = modfl(1.5, &l2);
    printf("modfl(1.5, &f)\t\t%016llx,%016llx\n", l1, l2);
    printf("powl(2.0, -1.0)\t\t%016llx\n", powl(2.0, -1.0));
    printf("sinl(1.0)\t\t%016llx\n", sinl(1.0));
    printf("sinhl(1.0)\t\t%016llx\n", sinhl(1.0));
    printf("sqrtl(2.0)\t\t%016llx\n", sqrtl(2.0));
    printf("tanl(1.0)\t\t%016llx\n", tanl(1.0));
    printf("tanhl(1.0)\t\t%016llx\n", tanhl(1.0));

#else

    /* "f" functions */
    printf("acosf(-1.0)\t\t%8.7f\n", acosf(-1.0));
    printf("asinf(1.0)\t\t%8.7f\n", asinf(1.0));
    printf("atanf(1.0)\t\t%8.7f\n", atanf(1.0));
    printf("atan2f(1.0, 1.0)\t%8.7f\n", atan2f(1.0, 1.0));
    printf("ceilf(1.5)\t\t%8.7f\n", ceilf(1.5));
    printf("cosf(1.0)\t\t%8.7f\n", cosf(1.0));
    printf("coshf(1.0)\t\t%8.7f\n", coshf(1.0));
    printf("expf(1.0)\t\t%8.7f\n", expf(1.0));
    printf("fabsf(-1.0)\t\t%8.7f\n", fabsf(-1.0));
    printf("floorf(1.5)\t\t%8.7f\n", floorf(1.5));
    printf("fmodf(3.0, 2.0)\t\t%8.7f\n", fmodf(3.0, 2.0));
    f1 = frexpf(1.0, &e);
    printf("frexpf(1.0, &e)\t\t%8.7f,%d\n", f1, e);
    printf("ldexpf(1.0, 1)\t\t%8.7f\n", ldexpf(1.0, 1));
    printf("logf(2.0)\t\t%8.7f\n", logf(2.0));
    printf("log10f(2.0)\t\t%8.7f\n", log10f(2.0));
    f1 = modff(1.5, &f2);
    printf("modff(1.5, &f)\t\t%8.7f,%8.7f\n", f1, f2);
    printf("powf(2.0, -1.0)\t\t%8.7f\n", powf(2.0, -1.0));
    printf("sinf(1.0)\t\t%8.7f\n", sinf(1.0));
    printf("sinhf(1.0)\t\t%8.7f\n", sinhf(1.0));
    printf("sqrtf(2.0)\t\t%8.7f\n", sqrtf(2.0));
    printf("tanf(1.0)\t\t%8.7f\n", tanf(1.0));
    printf("tanhf(1.0)\t\t%8.7f\n", tanhf(1.0));

    /* "" (d) functions */
#ifdef SHORT_DOUBLE
    printf("acos(-1.0)\t\t%8.7f\n", acos(-1.0));
    printf("asin(1.0)\t\t%8.7f\n", asin(1.0));
    printf("atan(1.0)\t\t%8.7f\n", atan(1.0));
    printf("atan2(1.0, 1.0)\t\t%8.7f\n", atan2(1.0, 1.0));
    printf("ceil(1.5)\t\t%8.7f\n", ceil(1.5));
    printf("cos(1.0)\t\t%8.7f\n", cos(1.0));
    printf("cosh(1.0)\t\t%8.7f\n", cosh(1.0));
    printf("exp(1.0)\t\t%8.7f\n", exp(1.0));
    printf("fabs(-1.0)\t\t%8.7f\n", fabs(-1.0));
    printf("floor(1.5)\t\t%8.7f\n", floor(1.5));
    printf("fmod(3.0, 2.0)\t\t%8.7f\n", fmod(3.0, 2.0));
    d1 = frexp(1.0, &e);
    printf("frexp(1.0, &e)\t\t%8.7f,%d\n", d1, e);
    printf("ldexp(1.0, 1)\t\t%8.7f\n", ldexp(1.0, 1));
    printf("log(2.0)\t\t%8.7f\n", log(2.0));
    printf("log10(2.0)\t\t%8.7f\n", log10(2.0));
    d1 = modf(1.5, &d2);
    printf("modf(1.5, &f)\t\t%8.7f,%8.7f\n", d1, d2);
    printf("pow(2.0, -1.0)\t\t%8.7f\n", pow(2.0, -1.0));
    printf("sin(1.0)\t\t%8.7f\n", sin(1.0));
    printf("sinh(1.0)\t\t%8.7f\n", sinh(1.0));
    printf("sqrt(2.0)\t\t%8.7f\n", sqrt(2.0));
    printf("tan(1.0)\t\t%8.7f\n", tan(1.0));
    printf("tanh(1.0)\t\t%8.7f\n", tanh(1.0));
#else
    printf("acos(-1.0)\t\t%16.15f\n", acos(-1.0));
    printf("asin(1.0)\t\t%16.15f\n", asin(1.0));
    printf("atan(1.0)\t\t%16.15f\n", atan(1.0));
    printf("atan2(1.0, 1.0)\t\t%16.15f\n", atan2(1.0, 1.0));
    printf("ceil(1.5)\t\t%16.15f\n", ceil(1.5));
    printf("cos(1.0)\t\t%16.15f\n", cos(1.0));
    printf("cosh(1.0)\t\t%16.15f\n", cosh(1.0));
    printf("exp(1.0)\t\t%16.15f\n", exp(1.0));
    printf("fabs(-1.0)\t\t%16.15f\n", fabs(-1.0));
    printf("floor(1.5)\t\t%16.15f\n", floor(1.5));
    printf("fmod(3.0, 2.0)\t\t%16.15f\n", fmod(3.0, 2.0));
    d1 = frexp(1.0, &e);
    printf("frexp(1.0, &e)\t\t%16.15f,%d\n", d1, e);
    printf("ldexp(1.0, 1)\t\t%16.15f\n", ldexp(1.0, 1));
    printf("log(2.0)\t\t%16.15f\n", log(2.0));
    printf("log10(2.0)\t\t%16.15f\n", log10(2.0));
    d1 = modf(1.5, &d2);
    printf("modf(1.5, &f)\t\t%16.15f,%16.15f\n", d1, d2);
    printf("pow(2.0, -1.0)\t\t%16.15f\n", pow(2.0, -1.0));
    printf("sin(1.0)\t\t%16.15f\n", sin(1.0));
    printf("sinh(1.0)\t\t%16.15f\n", sinh(1.0));
    printf("sqrt(2.0)\t\t%16.15f\n", sqrt(2.0));
    printf("tan(1.0)\t\t%16.15f\n", tan(1.0));
    printf("tanh(1.0)\t\t%16.15f\n", tanh(1.0));
#endif

    /* "l" functions */
    printf("acosl(-1.0)\t\t%16.15Lf\n", acosl(-1.0));
    printf("asinl(1.0)\t\t%16.15Lf\n", asinl(1.0));
    printf("atanl(1.0)\t\t%16.15Lf\n", atanl(1.0));
    printf("atan2l(1.0, 1.0)\t%16.15Lf\n", atan2l(1.0, 1.0));
    printf("ceill(1.5)\t\t%16.15Lf\n", ceill(1.5));
    printf("cosl(1.0)\t\t%16.15Lf\n", cosl(1.0));
    printf("coshl(1.0)\t\t%16.15Lf\n", coshl(1.0));
    printf("expl(1.0)\t\t%16.15Lf\n", expl(1.0));
    printf("fabsl(-1.0)\t\t%16.15Lf\n", fabsl(-1.0));
    printf("floorl(1.5)\t\t%16.15Lf\n", floorl(1.5));
    printf("fmodl(3.0, 2.0)\t\t%16.15Lf\n", fmodl(3.0, 2.0));
    l1 = frexpl(1.0, &e);
    printf("frexpl(1.0, &e)\t\t%16.15Lf,%d\n", l1, e);
    printf("ldexpl(1.0, 1)\t\t%16.15Lf\n", ldexpl(1.0, 1));
    printf("logl(2.0)\t\t%16.15Lf\n", logl(2.0));
    printf("log10l(2.0)\t\t%16.15Lf\n", log10l(2.0));
    l1 = modfl(1.5, &l2);
    printf("modfl(1.5, &f)\t\t%16.15Lf,%16.15Lf\n", l1, l2);
    printf("powl(2.0, -1.0)\t\t%16.15Lf\n", powl(2.0, -1.0));
    printf("sinl(1.0)\t\t%16.15Lf\n", sinl(1.0));
    printf("sinhl(1.0)\t\t%16.15Lf\n", sinhl(1.0));
    printf("sqrtl(2.0)\t\t%16.15Lf\n", sqrtl(2.0));
    printf("tanl(1.0)\t\t%16.15Lf\n", tanl(1.0));
    printf("tanhl(1.0)\t\t%16.15Lf\n", tanhl(1.0));

#endif

    return 0;
}
