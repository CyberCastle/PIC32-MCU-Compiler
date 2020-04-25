/*********************************************************************
 *
 *              Macros for L1 Cache management
 *
 *********************************************************************
 * Filename:        sys/l1cache.h
 *
 * Processor:       PIC32
 *
 * Compiler:        MPLAB XC32
 *
 * Company:         Microchip Technology Inc.
 *
 * Software License Agreement
 *
 * Copyright (c) 2014, Microchip Technology Inc. and its subsidiaries ("Microchip")
 * All rights reserved. 
 *
 * This software is developed by Microchip Technology Inc. and its
 * subsidiaries ("Microchip").
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1.      Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2.      Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3.      Microchip's name may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MICROCHIP "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING BUT NOT LIMITED TO
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWSOEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *-------------------------------------------------------------------------*/

#pragma once
#ifndef __SYS_L1CACHE_H
#define __SYS_L1CACHE_H

/* See also sys/kmem.h for other kseg translation macros */
#ifndef _XC_H
#include <xc.h>
#endif

#ifndef _STDLIB_H_
#include <stdlib.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__PIC32_HAS_L1CACHE)
/*  Access a KSEG0 Virtual Address variable as uncached (KSEG1) */
#  define __PIC32_UNCACHED_VAR(v) __PIC32_KVA0_TO_KVA1_VAR(v)
/*  Access a KSEG0 Virtual Address pointer as uncached (KSEG1) */
#  define __PIC32_UNCACHED_PTR(v) __PIC32_KVA0_TO_KVA1_PTR(v)
/*  Access a KSEG1 Virtual Address variable as cached (KSEG0) */
#  define __PIC32_CACHED_VAR(v) __PIC32_KVA1_TO_KVA0_VAR(v)
/*  Access a KSEG1 Virtual Address pointer as cached (KSEG0) */
#  define __PIC32_CACHED_PTR(v) __PIC32_KVA1_TO_KVA0_PTR(v)
#else
#  define __PIC32_UNCACHED_VAR(v) (v)
#  define __PIC32_UNCACHED_PTR(v) (v)
#  define __PIC32_CACHED_VAR(v) (v)
#  define __PIC32_CACHED_PTR(v) (v)
#endif

/* Helper macros used by those above. */

/*  Convert a KSEG0 Virtual Address variable or pointer to a KSEG1 virtual 
 *  address access.
 */
#  define __PIC32_KVA0_TO_KVA1_VAR(v) (*(__typeof__(v)*)((unsigned long)&(v) | 0x20000000u))
#  define __PIC32_KVA0_TO_KVA1_PTR(v) ((__typeof__(v)*)((unsigned long)(v) | 0x20000000u))
#  define __PIC32_KVA1_TO_KVA0_VAR(v) (*(__typeof__(v)*)((unsigned long)&(v) & ~0x20000000u))
#  define __PIC32_KVA1_TO_KVA0_PTR(v) ((__typeof__(v)*)((unsigned long)(v) & ~0x20000000u))
    
static __inline__ void * __pic32_alloc_coherent (size_t size)
{
  void *retptr;
  retptr = malloc (size);
  if (retptr == NULL) {
    return NULL;
  }
  /* malloc returns a cached pointer, but convert it to an uncached pointer */
  return __PIC32_UNCACHED_PTR (retptr);
}

static __inline__ void * __pic32_realloc_coherent (void *p, size_t size)
{
  void *retptr;
  retptr = realloc(p, size);
  if (retptr == NULL) {
    return NULL;
  }
  /* realloc returns a cached pointer, but convert it to an uncached pointer */
  return __PIC32_UNCACHED_PTR (retptr);  
}

static __inline__ void * __pic32_calloc_coherent  (size_t num, size_t size)
{
  void *retptr;
  retptr = calloc(num,size);
  if (retptr == NULL) {
    return NULL;
  }
  /* calloc returns a cached pointer, but convert it to an uncached pointer */
  return __PIC32_UNCACHED_PTR (retptr);  
}

static __inline__ void __pic32_free_coherent (void* ptr)
{
  /* Convert back to a cached pointer before calling free. */
  free (__PIC32_CACHED_PTR (ptr));
}

#ifdef __cplusplus
}
#endif

#endif /* __SYS_L1CACHE_H */
