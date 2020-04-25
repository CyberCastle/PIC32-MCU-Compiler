/*********************************************************************
 *
 *                  dsp lib function
 *
 *********************************************************************
 * FileName:        mchp_vdot16.c
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
 * (the �Company�) for its PIC32MX Microcontroller is intended
 * and supplied to you, the Company�s customer, for use solely and
 * exclusively on Microchip Microcontroller products.
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN �AS IS� CONDITION. NO WARRANTIES,
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
 * Function:        short int VectorDotProduct16 ( int numElems,short int* srcV1,short int* srcV2)
 *
 * PreCondition:    None
 *
 * Input:          numElems    -    number elements in srcV[1,2] (N)
 *                 srcV1       -    ptr to source vector one
 *                 srcV2       -    ptr to source vector two
 *
 * Output:          dot product value returned

 * Side Effects:    None
 *
 * Overview:        dotVal = sum(srcV1[elem]*srcV2[elem])
 *
 * Note:            None
 ********************************************************************/
short int VectorDotProduct16 ( int numElems,short int* srcV1,short int* srcV2)
{
    return(mips_vec_dotp16(srcV1, srcV2, numElems, 0));
}

