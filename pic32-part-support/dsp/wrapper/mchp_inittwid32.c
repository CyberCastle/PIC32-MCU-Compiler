/*********************************************************************
 *
 *                  dsp lib function
 *
 *********************************************************************
 * FileName:        mchp_inittwid32.c
 * Dependencies:
 *
 * Processor:       PIC32
 *
 * Compiler:        MPLAB XC32
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
 * Function:        fractcomplex32* TwidFactorInit32 (int log2N,fractcomplex32* twidFactors,int conjFlag)
 *
 * PreCondition:    This function requires floating-point support.
 *
 * Input:          log2N       - log2(N), N complex factors
 *                 twidFactors - ptr to twiddle factors
 *                 conjFlag    - MIPS32: Generates Complex conjugates by default.Hence this flag
 *                              is not used. It is defined to maintain compatibility with C30.
 *
 * Output:          twidfact returned
 *
 * Side Effects:    None
 *
 * Overview:        Initialize twiddle factors
 *
 * Note:            None
 ********************************************************************/

fractcomplex32* TwidFactorInit32 (int log2N,fractcomplex32* twidFactors,int conjFlag)
{
    mips_fft32_setup(twidFactors, log2N);
    
    return(twidFactors);
}

