/*********************************************************************
*                                                                    *
*                       Software License Agreement                   *
*                                                                    *
*   The software supplied herewith by Microchip Technology           *
*   Incorporated (the "Company") for its PIC32 controller            *
*   is intended and supplied to you, the Company's customer,         *
*   for use solely and exclusively on Microchip dsPIC                *
*   products. The software is owned by the Company and/or its        *
*   supplier, and is protected under applicable copyright laws. All  *
*   rights are reserved. Any use in violation of the foregoing       *
*   restrictions may subject the user to criminal sanctions under    *
*   applicable laws, as well as to civil liability for the breach of *
*   the terms and conditions of this license.                        *
*                                                                    *
*   THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION.  NO           *
*   WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING,    *
*   BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND    *
*   FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE     *
*   COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL,  *
*   INCIDENTAL OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.  *
*                                                                    *
*   (c) Copyright 2008 Microchip Technology, All rights reserved.    *
*********************************************************************/

/****************************************************************************
*
* MCHP_DSP_WRAPPER.H
* Interface to the DSP Library for the PIC32MX.
*
****************************************************************************/

#ifndef __MCHP_DSP_WRAPPER_LIB__     /* [ */
#define __MCHP_DSP_WRAPPER_LIB__

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************/

/* External headers. */

#include        <math.h>                /* fabs, sin, cos, atan, sqrt */

/*...........................................................................*/

/* Local defines. */

/* Some constants. */
#ifndef PI                              /* [ */
#define PI 3.1415926535897931159979634685441851615905761718750 /* double */
#endif  /* ] */
#ifndef SIN_PI_Q                                /* [ */
#define SIN_PI_Q 0.7071067811865474617150084668537601828575134277343750
                                                /* sin(PI/4), (double) */
#endif  /* ] */
#ifndef INV_SQRT2                               /* [ */
#define INV_SQRT2 SIN_PI_Q                      /* 1/sqrt(2), (double) */
                                                /* 1/sqrt(2) = sin(PI/4) */
#endif  /* ] */

#define COEFFS_IN_DATA  0xFF00                  /* page number used for */
                                                /* filter coefficients */
                                                /* when allocated in X */
                                                /* data memory */
/*...........................................................................*/

/* Local types. */

/* Type definitions. */

typedef struct
{
    short int real;
    short int imag;
} fractcomplex16;/*...........................................................................*/

typedef struct
{
    int real;
    int imag;
} fractcomplex32;/*...........................................................................*/



/****************************************************************************
*
* Interface to generic function prototypes.
*
****************************************************************************/

/* Generic function prototypes. */

/*...........................................................................*/

/****************************************************************************
*
* Interface to vector operations.
*
* A vector is a collection of numerical values, the vector elements,
* allocated contiguosly in memory, with the first element at the
* lowest memory address. One word of memory (two bytes) is used to
* store the value of each element, and this quantity must be interpreted
* as a fractional value in Q.15 format.
*
* A pointer addressing the first element of the vector is used as
* a handle which provides access to each of the vector values. The
* one dimensional arrangement of a vector fits with the memory
* storage model, so that the n-th element of an N-element vector
* can be accessed from the vector's base address BA as:
*
*       BA +  (n-1)*2,
*
* Note that because of the byte addressing capabilities of the dsPIC30F,
* the addressing of vector elements uses an increment (or decrement) size
* of 2: INC2 (or DEC2) instruction.
*
* Unary and binary operations are prototyped in this interface. The
* operand vector in a unary operation is called the source vector.
* In a binary operation the first operand is referred to as source
* one vector, and the second as source two vector. Each operation
* applies some computation to one or several elements of the source
* vector(s). Some operations result in a scalar value (also to be
* interpreted as a Q.15 fractional number), others in a vector. When
* the result is also a vector this is referred to as the destination
* vector.
*
* Some operations resulting in a vector allow computation in place;
* i.e., the results of the operations are placed back on the source
* (or source one, if binary) vector. In this case, the destination
* vector is said to (physically) replace the source (one) vector.
* When an operation can be computed in place it is indicated as such
* in the comments provided with its prototype.
*
* For some binary operations, the two operands can be the same (physical)
* source vector: the operation is applied between the source vector
* and itself. If this type of computation is possible for a given
* operation, it is indicated as such in the comments provided with
* its prototype.
*
* Some operations can be self applicable and computed in place.
*
* The operations prototyped in this interface take as an argument the
* cardinality (number of elements) of the operand vector(s). It is
* assumed that this number is in the range {1, 2, ..., (2^14)-1}, and
* that in the case of binary operations both operand vectors have the
* same cardinality. Note that no boundary checking is performed by
* the operations, and that out of range cardinalities as well as the
* use of source vectors of different sizes in binary operations may
* produce unexpected results.
*
* Additional remarks.
*
* A) Operations which return a destination vector can be nested, so that
*    for instance if:
*
*       a = Op1 (b, c), with b = Op2 (d), and c = Op3 (e, f), then
*
*       a = Op1 (Op2 (d), Op3 (e, f))
*
* B) The vector dot product and power operations could lead to saturation
*    if the sum of products is greater than 1-2^(-15) or smaller than -1.
*
* C) All the functions have been designed to operate on vectors allocated
*    in default RAM memory space (X-Data or Y-Data).
*
* D) The sum of sizes of the vector(s) involved in an operation must not
*    exceed the available memory in the target device.
*
****************************************************************************/

/* Vector operation prototypes. */

/*...........................................................................*/

extern short int* VectorAdd16(          /* Vector addition */
                                        /* dstV[elem] =                 */
                                        /*    = srcV1[elem] + srcV2[elem] */
                                        /* (in place capable) */
                                        /* (with itself capable) */
   int numElems,                        /* number elements in srcV[1,2] (N) */
   short int* dstV,                    /* ptr to destination vector */
   short int* srcV1,                   /* ptr to source vector one */
   short int* srcV2                    /* ptr to source vector two */

                                        /* dstV returned */
);

extern int* VectorAdd32(                /* Vector addition */
                                        /* dstV[elem] =                 */
                                        /*    = srcV1[elem] + srcV2[elem] */
                                        /* (in place capable) */
                                        /* (with itself capable) */
   int numElems,                        /* number elements in srcV[1,2] (N) */
   int* dstV,                           /* ptr to destination vector */
   int* srcV1,                          /* ptr to source vector one */
   int* srcV2                           /* ptr to source vector two */

                                        /* dstV returned */
);

/*...........................................................................*/

extern short int VectorDotProduct16 (    /* Vector dot product */
                                         /* dotVal =                     */
                                         /*    = sum(srcV1[elem]*srcV2[elem]) */
                                         /* (with itself capable) */
   int numElems,                         /* number elements in srcV[1,2] (N) */
   short int* srcV1,                     /* ptr to source vector one */
   short int* srcV2                      /* ptr to source vector two */

                                         /* dot product value returned */
);

/*...........................................................................*/

extern int VectorDotProduct32 (         /* Vector dot product */
                                        /* dotVal =                     */
                                        /*    = sum(srcV1[elem]*srcV2[elem]) */
                                        /* (with itself capable) */
   int numElems,                        /* number elements in srcV[1,2] (N) */
   int* srcV1,                          /* ptr to source vector one */
   int* srcV2                           /* ptr to source vector two */

                                        /* dot product value returned */
);

/*...........................................................................*/

extern short int* VectorMultiply16 (    /* Vector elem-to-elem multiply */
                                        /* dstV[elem] =                 */
                                        /*    = srcV1[elem] * srcV2[elem] */
                                        /* (in place capable) */
                                        /* (with itself capable) */
   int numElems,                        /* number elements in srcV[1,2] (N) */
   short int* dstV,                     /* ptr to destination vector */
   short int* srcV1,                    /* ptr to source vector one */
   short int* srcV2                     /* ptr to source vector two */

                                        /* dstV returned */
);

extern int* VectorMultiply32 (          /* Vector elem-to-elem multiply */
                                        /* dstV[elem] =                 */
                                        /*    = srcV1[elem] * srcV2[elem] */
                                        /* (in place capable) */
                                        /* (with itself capable) */
   int numElems,                        /* number elements in srcV[1,2] (N) */
   int* dstV,                           /* ptr to destination vector */
   int* srcV1,                          /* ptr to source vector one */
   int* srcV2                           /* ptr to source vector two */

                                        /* dstV returned */
);

/*...........................................................................*/

extern short int* VectorScale16 (       /* Vector scale */
                                        /* dstV[elem] = sclVal*srcV[elem] */
                                        /* (in place capable) */
   int numElems,                        /* number elements in srcV (N) */
   short int* dstV,                     /* ptr to destination vector */
   short int* srcV,                     /* ptr to source vector */
   short int sclVal                     /* scale value (Q.15 fractional) */

                                        /* dstV returned */
);

/*...........................................................................*/

extern int* VectorScale32 (             /* Vector scale */
                                        /* dstV[elem] = sclVal*srcV[elem] */
                                        /* (in place capable) */
   int numElems,                        /* number elements in srcV (N) */
   int* dstV,                           /* ptr to destination vector */
   int* srcV,                           /* ptr to source vector */
   int sclVal                           /* scale value (Q.15 fractional) */

                                        /* dstV returned */
);

/*...........................................................................*/

extern short int* VectorSubtract16 (   /* Vector subtraction */
                                        /* dstV[elem] =                 */
                                        /*    = srcV1[elem] - srcV2[elem] */
                                        /* (in place capable) */
                                        /* (with itself capable) */
   int numElems,                        /* number elements in srcV[1,2] (N) */
   short int* dstV,                    /* ptr to destination vector */
   short int* srcV1,                   /* ptr to source vector one */
   short int* srcV2                    /* ptr to source vector two */

                                        /* dstV returned */
);

/*...........................................................................*/

extern int* VectorSubtract32 (         /* Vector subtraction */
                                        /* dstV[elem] =                 */
                                        /*    = srcV1[elem] - srcV2[elem] */
                                        /* (in place capable) */
                                        /* (with itself capable) */
   int numElems,                        /* number elements in srcV[1,2] (N) */
   int* dstV,                          /* ptr to destination vector */
   int* srcV1,                         /* ptr to source vector one */
   int* srcV2                          /* ptr to source vector two */

                                        /* dstV returned */
);


/*...........................................................................*/

extern short int VectorPower16 (         /* Vector power */
                                        /* powVal =                     */
                                        /*    = sum(srcV[elem]^2)       */
   int numElems,                        /* number elements in srcV (N) */
   short int* srcV                     /* ptr to source vector one */

                                        /* power value returned */
);


/*...........................................................................*/

extern int VectorPower32 (               /* Vector power */
                                        /* powVal =                     */
                                        /*    = sum(srcV[elem]^2)       */
   int numElems,                        /* number elements in srcV (N) */
   int* srcV                            /* ptr to source vector one */

                                        /* power value returned */
);


/****************************************************************************
*
* Interface to FIR filter operations.
*
* Filtering a data sequence x[n] with an FIR filter of impulse response
* b[m] (0<= m < M) is equivalent to solving the difference equation:
*
*       y[n] = sum_{m = 0:M-1}(b[m]*x[n-m])
*
* In this operation it is important to know and manage the past history
* of the data sequence (x[m], -M+1 <= m < 0) which represent the initial
* condition of the filtering operation. Also, when applying an FIR filter
* to contiguous sections of a data sequence it is necessary to remember
* the final state of the previous filtering operation (x[m], N-M+1 <= m < N-1),
* and take the state into consideration for the calculations of the next
* filtering stage. Accounting for the past history and current state is
* required to perform a correct (glitch-free) filtering operation.
*
* The management of the past history and current state of the filtering
* operation is commonly implemented via an additional sequence, referred
* to as the delay. Prior to a filtering operation the delay describes the
* past history of the data sequence. After performing the FIR filtering
* the delay contains a set of the most recently filtered data samples.
* (For correct operation, it is advisable to initialize the delay values
* to zero by calling the corresponding init function.)
*
* Even though FIR filtering is a difference equation, several properties
* of FIR filters allow for computation of the operation in more effective
* ways than that of a straight difference equation. Consequently, a set
* of such implementations are hereby provided.
*
* Note that of the four main sequences involved in FIR filtering, input
* data, output data, filter coefficients and delay, the last two are
* usually thought of as making up the filter structure. All the functions
* that follow use the same FIR filter structure to manage the filtering
* operation.
*
* In the current design, the input data sequence is referred to as the
* sequence of source samples, while the resulting filtered sequence
* contains the destination samples. The filters are characterized by
* the number of coefficients or taps, and the delay properties. All of
* these data sets are stored in memory as vectors with their elements
* representing Q.15 fractional quantities. Also, the input and output
* sequences to the filtering operation ought to be allocated in default
* RAM memory (X-Data or Y-Data). The coefficients may be allocated either
* in X-Data or program memory, while the delays must be allocated solely
* in Y-Data memory.
*
****************************************************************************/

/* FIR filter operation prototypes. */


typedef struct {
   int numCoeffs;                       /* number of coeffs in filter (M) */
                                        /* (same as filter order if lattice) */
                                        /* (h[m], 0 <= m < M) */
                                        /* (if lattice, k[m], 0 <= m < M) */
   short int* coeffsBase;              /* base address of filter coeffs */
                                        /* either in X data or program memory */
                                        /* if in X data memory, it points at */
                                        /* h[0] (if lattice, k[0]) */
                                        /* if in program memory, base is the */
                                        /* offset from program page boundary */
                                        /* to address where coeffs located */
                                        /* (inline assembly psvoffset ()) */
                                        /* when indicated, it must be at a */
                                        /* 'zero' power of 2 address (since */
                                        /* in those cases it is implemented */
                                        /* as an increasing circular buffer) */
   short int* coeffsEnd;               /* end address of filter coeffs */
                                        /* must be an odd number */
                                        /* if in data memory, points at */
                                        /* last byte of coefficients buffer */
                                        /* if in program memory, end is the */
                                        /* offset from program page boundary */
                                        /* to address of last byte of coeffs */
   int coeffsPage;                      /* if in X data memory, set to */
                                        /* defined value COEFFS_IN_DATA */
                                        /* if in program memory, page number */
                                        /* where coeffs are located */
                                        /* (inline assembly psvpage ()) */
   short int* delayBase;               /* base address of delay buffer, */
                                        /* only in Y data */
                                        /* points at d[0] of d[m], 0 <= m < M */
                                        /* when indicated, it must be at a */
                                        /* 'zero' power of 2 address (since */
                                        /* in those cases it is implemented */
                                        /* as an increasing circular buffer) */
   short int* delayEnd;                /* end address of delay buffer, */
                                        /* points at last byte of buffer */
   short int* delay;                   /* current value of delay pointer */
} FIRStruct;


extern short int* FIR (                /* FIR filtering */
   int numSamps,                        /* number of input samples (N) */
   short int* dstSamps,                /* ptr to output samples */
                                        /* (y[n], 0 <= n < N) */
   short int* srcSamps,                /* ptr to input samples */
                                        /* (x[n], 0 <= n < N) */
   FIRStruct* filter                    /* filter structure: */
                                        /* number of coefficients in filter */
                                        /* same as number of delay elements */
                                        /* (h[m], 0 <= m < M, an increasing */
                                        /*  circular buffer) */
                                        /* (d[m], 0 <= m < M, an increasing */
                                        /*  circular buffer) */

                                        /* returns dstSamps */
);


/****************************************************************************
*
* Interface to IIR filter operations.
*
* Filtering a data sequence x[n] with an IIR filter of impulse response
* {b[m] (0<= m < M), a[p] (0 <= p < P)} is equivalent to solving the
* difference equation:
*
*       sum_{p = 0:P-1}(a[p]*y[n-p]) = sum_{m = 0:M-1}(b[m]*x[n-m])
*
* In this operation it is important to know and manage the past history
* of the input and output data sequences (x[m], -M+1 <= m < 0, and y[p],
* -P+1 <= p < 0) which represent the initial conditions of the filtering
* operation. Also, when applying an IIR filter to contiguous sections of
* a data sequence it is necessary to remember the final state of the
* last filtering operation (x[m], N-M+1 <= m < N-1, and y[p], N-P+1 <= p < N-1),
* and take the state into consideration for the calculations of the next
* filtering stage. Accounting for the past history and current state is
* required to perform a correct (glitch-free) filtering operation.
*
* The management of the past history and current state of the filtering
* operation is commonly implemented via additional sequences, referred
* to as the delays. Prior to a filtering operation the delays describe the
* past history of the filter. After performing the IIR filtering operation
* the delays contain a set of the most recently filtered data samples, and
* of the most recent output samples. (For correct operation, it is advisable
* to initialize the delay values to zero by calling the corresponding init
* function.)
*
* Even though IIR filtering is a difference equation, several properties
* of IIR filters allow for computation of the operation in more effective
* ways than that of a straight difference equation. Consequently, a set
* of such implementations are hereby provided.
*
* Note that of the six main sequences involved in IIR filtering, input
* data, output data, filter coefficients (a,b) and delays, the last four
* are usually thought of as making up the filter structure. However, since
* different implementations allow for particular arrangaments of the filter
* structure for efficiency gain, the structure has not been standardized,
* but rather taylored to best fit the particular implementations.
*
* In the current design, the input data sequence is referred to as the
* sequence of source samples, while the resulting filtered sequence
* contains the destination samples. The filters are characterized by
* the number of coefficients or taps in the 'a' and 'b' sets, and the
* delay properties. All of these data sets are stored in memory as
* vectors with their elements representing Q.15 fractional quantities.
* Also, except for the 'IIRLattice' and associated 'IIRLatticeStruct'
* implementations, the filters are made up of cascaded second order
* sections. In all cases, the input, output and coefficient vectors
* are allocated in default RAM memory space (X-Data and Y-Data). The
* coefficients may be allocated in either X-Data or program memory,
* while the delays ought to be in Y-Data.
*
****************************************************************************/

/* IIR filter operation prototypes. */


//typedef struct {
//  int numSectionsLess1;                 /* 1 less than number of cascaded */
//                                        /* second order sections */
//  fractional* coeffsBase;               /* ptr to filter coefficients */
//                                        /* either in X-Data or P-MEM */
//                                        /* number of coefficients is */
//                                        /* 5*number of second order sections */
//                                        /* {a2,a1,b2,b1,b0} per section */
//  int coeffsPage;                       /* page number of program memory if */
//                                        /* coefficients are in program memory */
//                                        /* COEFFS_IN_DATA if not */
//  fractional* delayBase;                /* ptr to filter delay */
//                                        /* two words for every section */
//                                        /* only in Y-Data */
//  fractional initialGain;               /* initial gain value */
//  int finalShift;                       /* output scaling (shift left) */
//                                        /* restores filter gain to 0 dB */
//                                        /* shift count may be zero, if not */
//                                        /* zero, it is the number of bits */
//                                        /* to shift output: negative means */
//                                        /* shift to the left, positive is */
//                                        /* shift right */
//} IIRCanonicStruct;     /* Direct Form II Canonic biquad filter structure */

//extern fractional* IIRCanonic (         /* Direct Form II (Canonic) */
//                                        /* biquad filtering */
//   int numSamps,                        /* number of input samples (N) */
//   fractional* dstSamps,                /* ptr to output samples */
//                                        /* (y[n], 0 <= n < N) */
//   fractional* srcSamps,                /* ptr to input samples */
//                                        /* (x[n], 0 <= n < N) */
//   IIRCanonicStruct* filter             /* filter structure */
//
//                                        /* returns dstSamps */
//);


/* ....................................................................... */

/****************************************************************************
*
* Interface to transform operations.
*
* A set of linear discrete signal transformations (and some of the inverse
* transforms) are prototyped below. The first set applies a Discrete Fourier
* transform (or its inverse) to a complex data set. The second set applies
* a Type II Discrete Cosine Transform (DCT) to a real valued sequence.
*
* A complex valued sequence is represented by a vector in which every pair
* of values corresponds with a sequence element. The first value in the pair
* is the real part of the element, and the second its imaginary part (see
* the declaration of the 'fractcomplex' structure at the beginning of this
* file for further details). Both, the real and imaginary parts, are stored
* in memory using one word (two bytes) each, and must be interpreted as Q.15
* fractionals.
*
* The following transforms have been designed to either operate out-of-place,
* or in-place. The former type populates an output sequence with the results
* of the transformation. In the latter, the input sequence is (physically)
* replaced by the transformed sequence. For out-of-place operations, the user
* must provide with enough memory to accept the results of the computation.
* The input and output sequences to the FFT family of transforms must be
* allocated in Y-Data memopry.
*
* The transforms here described make use of transform factors which must be
* supplied to the transforming function during its invokation. These factors,
* which are complex data sets, are computed in floating point arithmetic,
* and then transformed into fractionals for use by the operations. To avoid
* excessive overhead when applying a transformation, and since for a given
* transform size the values of the factors are fixed, a particular set of
* transform factors could be generated once and used many times during the
* execution of the program. Thus, it is advisable to store the factors
* returned by any of the initialization operations in a permanent (static)
* vector. The factors to a transform may be allocated either in X-Data or
* program memory.
*
* Additional remarks.
*
* A) Operations which return a destination vector can be nested, so that
*    for instance if:
*
*       a = Op1 (b, c), with b = Op2 (d), and c = Op3 (e, f), then
*
*       a = Op1 (Op2 (d), Op3 (e, f))
*
****************************************************************************/

/* Transform operation prototypes. */

extern fractcomplex16* TwidFactorInit16 (   /* Initialize twiddle factors */
                                        /* WN(k) = exp(i*2*pi*k/N) */
                                        /* computed in floating point */
                                        /* converted to fractionals */
   int log2N,                           /* log2(N), N complex factors */
                                        /* (although only N/2 are computed */
                                        /* since only half of twiddle factors */
                                        /* are used for I/FFT computation) */
   fractcomplex16* twidFactors,           /* ptr to twiddle factors */
   int conjFlag                         /* indicates whether to generate */
                                        /* complex conjugates of twiddles */
                                        /* 0 : no conjugates (default) */
                                        /* 1 : conjugates */

                                        /* twidfact returned */
                                        /* only the first half: */
                                        /* WN(0)...WN(N/2-1) */
                                        /* (or their conjugates) */
);


/*...........................................................................*/

extern fractcomplex16* FFTComplex16 (       /* Fast Fourier Transform */
                                        /* (complex, out-of-place) */
   int log2N,                           /* log2(N), N-point transform */
   fractcomplex16* dstCV,                 /* ptr to destination complex vector */
                                        /* with time samples */
                                        /* in natural order */
                                        /* MUST be N modulo aligned */
   fractcomplex16* srcCV,                 /* ptr to source complex vector */
                                        /* with time samples */
                                        /* in natural order */
   fractcomplex16* twidFactors,           /* base address of twiddle factors */
                                        /* either in X data or program memory */
                                        /* if in X data memory, it points at */
                                        /* WN(0).real */
                                        /* if in program memory, base is the */
                                        /* offset from program page boundary */
                                        /* to address where factors located */
                                        /* (inline assembly psvoffset ()) */
   int factPage                         /* if in X data memory, set to */
                                        /* defined value COEFFS_IN_DATA */
                                        /* if in program memory, page number */
                                        /* where factors are located */
                                        /* (inline assembly psvpage ()) */

                                        /* dstCV returned */
                                        /* with frequency components */
                                        /* in natural order */
                                        /* and scaled by 1/(1<<log2N) */
);

extern fractcomplex32* TwidFactorInit32 (   /* Initialize twiddle factors */
                                        /* WN(k) = exp(i*2*pi*k/N) */
                                        /* computed in floating point */
                                        /* converted to fractionals */
   int log2N,                           /* log2(N), N complex factors */
                                        /* (although only N/2 are computed */
                                        /* since only half of twiddle factors */
                                        /* are used for I/FFT computation) */
   fractcomplex32* twidFactors,           /* ptr to twiddle factors */
   int conjFlag                         /* indicates whether to generate */
                                        /* complex conjugates of twiddles */
                                        /* 0 : no conjugates (default) */
                                        /* 1 : conjugates */

                                        /* twidfact returned */
                                        /* only the first half: */
                                        /* WN(0)...WN(N/2-1) */
                                        /* (or their conjugates) */
);


/*...........................................................................*/

extern fractcomplex32* FFTComplex32 (       /* Fast Fourier Transform */
                                        /* (complex, out-of-place) */
   int log2N,                           /* log2(N), N-point transform */
   fractcomplex32* dstCV,                 /* ptr to destination complex vector */
                                        /* with time samples */
                                        /* in natural order */
                                        /* MUST be N modulo aligned */
   fractcomplex32* srcCV,                 /* ptr to source complex vector */
                                        /* with time samples */
                                        /* in natural order */
   fractcomplex32* twidFactors,           /* base address of twiddle factors */
                                        /* either in X data or program memory */
                                        /* if in X data memory, it points at */
                                        /* WN(0).real */
                                        /* if in program memory, base is the */
                                        /* offset from program page boundary */
                                        /* to address where factors located */
                                        /* (inline assembly psvoffset ()) */
   int factPage                         /* if in X data memory, set to */
                                        /* defined value COEFFS_IN_DATA */
                                        /* if in program memory, page number */
                                        /* where factors are located */
                                        /* (inline assembly psvpage ()) */

                                        /* dstCV returned */
                                        /* with frequency components */
                                        /* in natural order */
                                        /* and scaled by 1/(1<<log2N) */
);

/*...........................................................................*/


/***************************************************************************/

#ifdef __cplusplus
}
#endif


#endif  /* ] __MCHP_DSP_WRAPPER_LIB__ */

/***************************************************************************/
/* EOF */

