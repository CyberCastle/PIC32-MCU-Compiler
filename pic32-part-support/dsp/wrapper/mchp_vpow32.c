/*********************************************************************
 *
 *                  dsp lib function
 *
 *********************************************************************
 * FileName:        mchp_vpow32.c
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

/*********************************************************************
 * Function:        int VectorPower32 (int numElems,int* srcV)
 *
 * PreCondition:    None
 *
 * Input:          numElems    -    number elements in srcV[1,2] (N)
 *                 srcV       -    ptr to source vector
 *
 * Output:          power value returned

 * Side Effects:    None
 *
 * Overview:        powVal = sum(srcV[elem]^2)
 *
 * Note:            None
 ********************************************************************/

int VectorPower32 (int numElems,int* srcV)
{
    return(mips_vec_sum_squares32(srcV, numElems, 0));
}

