/*********************************************************************
 *
 *                  dsp lib function
 *
 *********************************************************************
 * FileName:        mchp_fft32.c
 * Dependencies:
 *
 * Processor:       PIC32
 *
 * Compiler:        MPLAB C Compiler for PIC32 MCUs
 *                  MPLAB IDE
 * Company:         Microchip Technology Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PIC32MX Microcontroller is intended
 * and supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip Microcontroller products.
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *
 * $Id$
 *
 ********************************************************************/
#include <mchp_dsp_wrapper.h>

/*********************************************************************
 * Function:        fractcomplex32* FFTComplex32 (int log2N,fractcomplex32* dstCV,fractcomplex32* srcCV,fractcomplex32* twidFactors,int factPage)
 *
 * PreCondition:    1) The pointers dstCV,srcCV & twidFactors must be aligned on a 4-byte boundary.
 *                  2) MIPS32: log2N must be larger than or equal to 3.
 *
 * Input:          log2N       - log2(N), N complex factors
 *                 dstCV       - ptr to destination complex vector
 *                 srcCV       - ptr to source complex vector
 *                 twidFactors - base address of twiddle factors
 *                 factPage    - Not Applicable for PIC32. Defined to maintain
 *                               compatibility with C30
 *
 * Output:          ptr to dstCV returned
 *
 * Side Effects:    None
 *
 * Overview:        Does Fast Fourier Transform
 *
 * Note:            None
 ********************************************************************/

fractcomplex32* FFTComplex32 (int log2N,fractcomplex32* dstCV,fractcomplex32* srcCV,fractcomplex32* twidFactors,int factPage)
{
    int N = 1 << log2N;  /* N = 2^6 = 64 */

    fractcomplex32 scratch[N];

    mips_fft32(dstCV, srcCV, twidFactors, scratch, log2N);
    
    return(dstCV);
}
