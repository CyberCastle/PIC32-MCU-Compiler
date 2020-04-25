#ident "lib/c/arch/mips/divsi.c: Copyright (c) MIPS Technologies, Inc. All rights reserved."

/*
 * Unpublished work (c) MIPS Technologies, Inc.  All rights reserved.
 * Unpublished rights reserved under the copyright laws of the United
 * States of America and other countries.
 * 
 * This code is confidential and proprietary to MIPS Technologies,
 * Inc. ("MIPS Technologies") and may be disclosed only as permitted in
 * writing by MIPS Technologies. Any copying, reproducing, modifying,
 * use or disclosure of this code (in whole or in part) that is not
 * expressly permitted in writing by MIPS Technologies is strictly
 * prohibited. At a minimum, this code is protected under trade secret,
 * unfair competition, and copyright laws. Violations thereof may result
 * in criminal penalties and fines.
 * 
 * MIPS Technologies reserves the right to change this code to improve
 * function, design or otherwise. MIPS Technologies does not assume any
 * liability arising out of the application or use of this code, or of
 * any error or omission in such code.  Any warranties, whether express,
 * statutory, implied or otherwise, including but not limited to the
 * implied warranties of merchantability or fitness for a particular
 * purpose, are excluded.  Except as expressly provided in any written
 * license agreement from MIPS Technologies, the furnishing of this
 * code does not give recipient any license to any intellectual property
 * rights, including any patent rights, that cover this code.
 * 
 * This code shall not be exported, reexported, transferred, or released,
 * directly or indirectly, in violation of the law of any country or
 * international law, regulation, treaty, Executive Order, statute,
 * amendments or supplements thereto.  Should a conflict arise regarding
 * the export, reexport, transfer, or release of this code, the laws of
 * the United States of America shall be the governing law.
 * 
 * This code may only be disclosed to the United States government
 * ("Government"), or to Government users, with prior written consent
 * from MIPS Technologies.  This code constitutes one or more of the
 * following: commercial computer software, commercial computer software
 * documentation or other commercial items.  If the user of this code,
 * or any related documentation of any kind, including related technical
 * data or manuals, is an agency, department, or other entity of the
 * Government, the use, duplication, reproduction, release, modification,
 * disclosure, or transfer of this code, or any related documentation
 * of any kind, is restricted in accordance with Federal Acquisition
 * Regulation 12.212 for civilian agencies and Defense Federal Acquisition
 * Regulation Supplement 227.7202 for military agencies.  The use of this
 * code by the Government is further restricted in accordance with the
 * terms of the license agreement(s) and/or applicable contract terms
 * and conditions covering this code from MIPS Technologies.
 * 
 * 
 */

/* 
 * divsi.c: 32bit / 32bit -> 32quo, 32mod division
 */



#include <machine/types.h>
#include <limits.h>

#ifdef TEST
#define fn(x)	test ## x
#else
#define fn(x)	x
#endif

unsigned int
fn(__udivmodsi4) (unsigned int num, unsigned int den, unsigned int *modp)
{
  unsigned int bit = 1;
  unsigned int res = 0;

  if (den == 0) {
#if __mips
      __asm__ __volatile__ ("break 7");
#else
      abort();
#endif
  }

  /* check for obvious short cuts */
  if (num == den) {
      if (modp) *modp = 0;
      return 1;
  }

  if (num < den) {
      if (modp) *modp = num;
      return 0;
  }

  if ((den & (den - 1)) == 0)
    {
      /* quick power of 2 */
      if (modp) *modp = num & (den - 1);
      while (den >= 0x10)
        {
	  den >>=4;
	  num >>=4;
	}
      while (den != 1)
        {
	  den >>=1;
	  num >>=1;
        }
      return num;
    }

  while (den < num && den < 0x10000000)
    {
      den <<=4;
      bit <<=4;
    }
  while (den < num && !(den & (1L<<31)))
    {
      den <<=1;
      bit <<=1;
    }

  while (bit)
    {
      if (num >= den)
	{
	  num -= den;
	  res |= bit;
	}
      bit >>=1;
      den >>=1;
    }

  if (modp) *modp = num;
  return res;
}

unsigned int
fn(__udivsi3) (unsigned int a, unsigned int b)
{
    return fn(__udivmodsi4) (a, b, 0);
}

unsigned int
fn(__umodsi3) (unsigned int a, unsigned int b)
{
    unsigned int mod;
    (void) fn(__udivmodsi4) (a, b, &mod);
    return mod;
}


unsigned int
fn(__divsi3) (int a, int b)
{
    unsigned int quo, sign;

    if (b == -1 && a == INT_MIN) {
#if __mips
	__asm__ __volatile__ ("break 6");
#else
	abort ();
#endif
    }

    if ((sign = (a < 0)))
	a = -a;
    if (b < 0) {
	b = -b;
	sign ^= 1;
    }

    quo = fn(__udivmodsi4) (a, b, 0);
    return sign ? -quo : quo;
}

unsigned int
fn(__modsi3) (int a, int b)
{
    unsigned int mod, sign;

    if (b == -1 && a == INT_MIN) {
#if __mips
	__asm__ __volatile__ ("break 6");
#else
	abort ();
#endif
    }

    if ((sign = (a < 0)))
	a = -a;
    if (b < 0)
	b = -b;

    (void) fn(__udivmodsi4) (a, b, &mod);
    return sign ? -mod : mod;
}

#ifdef TEST
#include <stdlib.h>
#include <stdio.h>

main ()
{
    int i;
    srandom (time (0));
    for (i = 1; i < 1000000; i++) {
	long a = random ();
	long b = random ();
	long v, want;

	if (random () & 1)
	    a = -a;
	if (random () & 2)
	    b = -b;

	/* don't allow error conditions */
	if (b == 0)
	    b = 1;
	else if (b == -1 && a == INT_MIN)
	    b = -2;

	want = a / b;
	if ((v = test__divsi3 (a, b)) != want)
	    fprintf (stderr, "__divsi3 (%ld,%ld) = %ld, want %ld\n",
		     a, b, v, want);

	want = a % b;
	if ((v = test__modsi3 (a, b)) != want)
	    fprintf (stderr, "__modsi3 (%ld,%ld) = %ld, want %ld\n",
		     a, b, v, want);

	want = (unsigned int)a / (unsigned int)b;
	if ((v = test__udivsi3 (a, b)) != want)
	    fprintf (stderr, "__udivsi3 (%lu,%lu) = %lu, want %lu\n",
		     a, b, v, want);

	want = (unsigned int)a % (unsigned int)b;
	if ((v = test__umodsi3 (a, b)) != want)
	    fprintf (stderr, "__umodsi3 (%lu,%lu) = %lu, want %lu\n",
		     a, b, v, want);
    }
    exit (0);
}
#endif
