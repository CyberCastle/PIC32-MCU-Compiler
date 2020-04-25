/*-------------------------------------------------------------------------
 *
 * Standard Fixed-Point include file
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
 *-------------------------------------------------------------------------*/

#pragma once

#ifndef _STDFIX_H_
#define _STDFIX_H_

/* signed short _Fract */
#undef SFRACT_FBIT
#undef SFRACT_MIN
#undef SFRACT_MAX
#undef SFRACT_EPSILON
#define SFRACT_FBIT      __SFRACT_FBIT__
#define SFRACT_MIN       __SFRACT_MIN__
#define SFRACT_MAX       __SFRACT_MAX__
#define SFRACT_EPSILON   __SFRACT_EPSILON__

/* unsigned short _Fract */
#undef USFRACT_FBIT
#undef USFRACT_MIN
#undef USFRACT_MAX
#undef USFRACT_EPSILON
#define USFRACT_FBIT     __USFRACT_FBIT__
#define USFRACT_MIN      __USFRACT_MIN__         /* Extension.  */
#define USFRACT_MAX      __USFRACT_MAX__
#define USFRACT_EPSILON  __USFRACT_EPSILON__

/* signed _Fract */
#undef FRACT_FBIT
#undef FRACT_MIN
#undef FRACT_MAX
#undef FRACT_EPSILON
#define FRACT_FBIT       __FRACT_FBIT__
#define FRACT_MIN        __FRACT_MIN__
#define FRACT_MAX        __FRACT_MAX__
#define FRACT_EPSILON    __FRACT_EPSILON__

/* unsigned _Fract */
#undef UFRACT_FBIT
#undef UFRACT_MIN
#undef UFRACT_MAX
#undef UFRACT_EPSILON
#define UFRACT_FBIT      __UFRACT_FBIT__
#define UFRACT_MIN       __UFRACT_MIN__          /* Extension.  */
#define UFRACT_MAX       __UFRACT_MAX__
#define UFRACT_EPSILON   __UFRACT_EPSILON__

/* signed long _Fract */
#undef LFRACT_FBIT
#undef LFRACT_MIN
#undef LFRACT_MAX
#undef LFRACT_EPSILON
#define LFRACT_FBIT      __LFRACT_FBIT__
#define LFRACT_MIN       __LFRACT_MIN__
#define LFRACT_MAX       __LFRACT_MAX__
#define LFRACT_EPSILON   __LFRACT_EPSILON__

/* unsigned long _Fract */
#undef ULFRACT_FBIT
#undef ULFRACT_MIN
#undef ULFRACT_MAX
#undef ULFRACT_EPSILON
#define ULFRACT_FBIT     __ULFRACT_FBIT__
#define ULFRACT_MIN      __ULFRACT_MIN__         /* Extension.  */
#define ULFRACT_MAX      __ULFRACT_MAX__
#define ULFRACT_EPSILON  __ULFRACT_EPSILON__

/* signed long long _Fract */
#undef LLFRACT_FBIT
#undef LLFRACT_MIN
#undef LLFRACT_MAX
#undef LLFRACT_EPSILON
#define LLFRACT_FBIT     __LLFRACT_FBIT__        /* Extension.  */
#define LLFRACT_MIN      __LLFRACT_MIN__         /* Extension.  */
#define LLFRACT_MAX      __LLFRACT_MAX__         /* Extension.  */
#define LLFRACT_EPSILON  __LLFRACT_EPSILON__     /* Extension.  */

/* unsigned long long _Fract */
#undef ULLFRACT_FBIT
#undef ULLFRACT_MIN
#undef ULLFRACT_MAX
#undef ULLFRACT_EPSILON
#define ULLFRACT_FBIT    __ULLFRACT_FBIT__       /* Extension.  */
#define ULLFRACT_MIN     __ULLFRACT_MIN__        /* Extension.  */
#define ULLFRACT_MAX     __ULLFRACT_MAX__        /* Extension.  */
#define ULLFRACT_EPSILON __ULLFRACT_EPSILON__    /* Extension.  */

/* signed short _Accum */
#undef SACCUM_FBIT
#undef SACCUM_IBIT
#undef SACCUM_MIN
#undef SACCUM_MAX
#undef SACCUM_EPSILON
#define SACCUM_FBIT      __SACCUM_FBIT__
#define SACCUM_IBIT      __SACCUM_IBIT__
#define SACCUM_MIN       __SACCUM_MIN__
#define SACCUM_MAX       __SACCUM_MAX__
#define SACCUM_EPSILON   __SACCUM_EPSILON__

/* unsigned short _Accum */
#undef USACCUM_FBIT
#undef USACCUM_IBIT
#undef USACCUM_MIN
#undef USACCUM_MAX
#undef USACCUM_EPSILON
#define USACCUM_FBIT     __USACCUM_FBIT__
#define USACCUM_IBIT     __USACCUM_IBIT__
#define USACCUM_MIN      __USACCUM_MIN__         /* Extension.  */
#define USACCUM_MAX      __USACCUM_MAX__
#define USACCUM_EPSILON  __USACCUM_EPSILON__

/* signed _Accum */
#undef ACCUM_FBIT
#undef ACCUM_IBIT
#undef ACCUM_MIN
#undef ACCUM_MAX
#undef ACCUM_EPSILON
#define ACCUM_FBIT       __ACCUM_FBIT__
#define ACCUM_IBIT       __ACCUM_IBIT__
#define ACCUM_MIN        __ACCUM_MIN__
#define ACCUM_MAX        __ACCUM_MAX__
#define ACCUM_EPSILON    __ACCUM_EPSILON__

/* unsigned _Accum */
#undef UACCUM_FBIT
#undef UACCUM_IBIT
#undef UACCUM_MIN
#undef UACCUM_MAX
#undef UACCUM_EPSILON
#define UACCUM_FBIT      __UACCUM_FBIT__
#define UACCUM_IBIT      __UACCUM_IBIT__
#define UACCUM_MIN       __UACCUM_MIN__          /* Extension.  */
#define UACCUM_MAX       __UACCUM_MAX__
#define UACCUM_EPSILON   __UACCUM_EPSILON__

/* signed long _Accum */
#undef LACCUM_FBIT
#undef LACCUM_IBIT
#undef LACCUM_MIN
#undef LACCUM_MAX
#undef LACCUM_EPSILON
#define LACCUM_FBIT      __LACCUM_FBIT__
#define LACCUM_IBIT      __LACCUM_IBIT__
#define LACCUM_MIN       __LACCUM_MIN__
#define LACCUM_MAX       __LACCUM_MAX__
#define LACCUM_EPSILON   __LACCUM_EPSILON__

/* unsigned long _Accum */
#undef ULACCUM_FBIT
#undef ULACCUM_IBIT
#undef ULACCUM_MIN
#undef ULACCUM_MAX
#undef ULACCUM_EPSILON
#define ULACCUM_FBIT     __ULACCUM_FBIT__
#define ULACCUM_IBIT     __ULACCUM_IBIT__
#define ULACCUM_MIN      __ULACCUM_MIN__         /* Extension.  */
#define ULACCUM_MAX      __ULACCUM_MAX__
#define ULACCUM_EPSILON  __ULACCUM_EPSILON__

/* signed long long _Accum */
#undef LLACCUM_FBIT
#undef LLACCUM_IBIT
#undef LLACCUM_MIN
#undef LLACCUM_MAX
#undef LLACCUM_EPSILON
#define LLACCUM_FBIT     __LLACCUM_FBIT__        /* Extension.  */
#define LLACCUM_IBIT     __LLACCUM_IBIT__        /* Extension.  */
#define LLACCUM_MIN      __LLACCUM_MIN__         /* Extension.  */
#define LLACCUM_MAX      __LLACCUM_MAX__         /* Extension.  */
#define LLACCUM_EPSILON  __LLACCUM_EPSILON__     /* Extension.  */

/* unsigned long long _Accum */
#undef ULLACCUM_FBIT
#undef ULLACCUM_IBIT
#undef ULLACCUM_MIN
#undef ULLACCUM_MAX
#undef ULLACCUM_EPSILON
#define ULLACCUM_FBIT    __ULLACCUM_FBIT__       /* Extension.  */
#define ULLACCUM_IBIT    __ULLACCUM_IBIT__       /* Extension.  */
#define ULLACCUM_MIN     __ULLACCUM_MIN__        /* Extension.  */
#define ULLACCUM_MAX     __ULLACCUM_MAX__        /* Extension.  */
#define ULLACCUM_EPSILON __ULLACCUM_EPSILON__    /* Extension.  */

/* FYI - Fixed Point Constants        */
/* ---------------------------------- */
/* Suffix - Type                      */
/* hr       short _Fract              */
/* uhr      unsigned shoft _Fract     */
/* r        _Fract                    */
/* ur       unsigned _Fract           */
/* lr       long _Fract               */
/* ulr      unsigned long _Fract      */
/* hk       short _Accum              */
/* uhk      unsigned short _Accum     */
/* k        _Accum                    */
/* uk       unsigned _Accum           */

#undef fract
#undef accum
#undef sat
#define fract            _Fract
#define accum            _Accum
#define sat              _Sat

#endif
