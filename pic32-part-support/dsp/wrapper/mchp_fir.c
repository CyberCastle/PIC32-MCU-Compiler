/*********************************************************************
 *
 *                  dsp lib function
 *
 *********************************************************************
 * FileName:        mchp_fir.c
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
 * Function:        short int* FIR (int numSamps,short int* dstSamps,short int* srcSamps,FIRStruct* filter)
 *
 * PreCondition:    1) The pointers dstSamps,srcSamps,filter->coeffsBase & filter->delayBase
 *                     must be aligned on a 4-byte boundary.
 *                  2) MIPS32: filter->numCoeffs must be larger than or equal to 4 and multiple of 4.
 *
 * Input:          numSamps    -    number of input samples (N)
 *                 dstSamps       -    ptr to output samples (y[n], 0 <= n < N)
 *                 srcSamps       -    ptr to input samples (x[n], 0 <= n < N)
 *                 filter       -    ptr to filter structure
 *
 * Output:          ptr to dstSamps returned
 *
 * Side Effects:    None
 *
 * Overview:        Does Finite Impulse Response of input samples
 *
 * Note:            None
 ********************************************************************/

short int* FIR (int numSamps,short int* dstSamps,short int* srcSamps,FIRStruct* filter)
{
    short int coeffs2x[2 * filter->numCoeffs];
    
    mips_fir16_setup(&coeffs2x[0], filter->coeffsBase, filter->numCoeffs);
    
    filter->coeffsBase = &coeffs2x[0];
    
    mips_fir16(dstSamps, srcSamps, filter->coeffsBase, filter->delayBase, numSamps, filter->numCoeffs, 0);

    return(dstSamps);
}

