/*
 * ****************************Begin Copyright 1D**********************************
 * Unpublished work (c) MIPS Technologies, Inc.  All rights reserved.
 * Unpublished rights reserved under the copyright laws of the United States
 * of America and other countries.
 * 
 * This code is confidential and proprietary to MIPS Technologies, Inc. ("MIPS
 * Technologies") and  may be disclosed only as permitted in writing by MIPS
 * Technologies or an authorized third party.  Any copying, reproducing,
 * modifying, use or disclosure of this code (in whole or in part) that is not
 * expressly permitted in writing by MIPS Technologies or an authorized third
 * party is strictly prohibited.  At a minimum, this code is protected under
 * trade secret, unfair competition, and copyright laws.  Violations thereof
 * may result in criminal penalties and fines.
 * 
 * MIPS Technologies reserves the right to change this code to improve function,
 * design or otherwise.  MIPS Technologies does not assume any liability arising
 * out of the application or use of this code, or of any error or omission in
 * such code.  Any warranties, whether express, statutory, implied or otherwise,
 * including but not limited to the implied warranties of merchantability or 
 * fitness for a particular purpose, are excluded.  Except as expressly provided
 * in any written license agreement from MIPS Technologies or an authorized third
 * party, the furnishing of this code does not give recipient any license to any
 * intellectual property rights, including any patent rights, that cover this code.
 * 
 * This code shall not be exported or transferred for the purpose of reexporting
 * in violation of any U.S. or non-U.S. regulation, treaty, Executive Order, law,
 * statute, amendment or supplement thereto.
 * 
 * This code may only be disclosed to the United States government ("Government"),
 * or to Government users, with prior written consent from MIPS Technologies or an
 * authorized third party.  This code constitutes one or more of the following:
 * commercial computer software, commercial computer software documentation or
 * other commercial items.  If the user of this code, or any related documentation
 * of any kind, including related technical data or manuals, is an agency,
 * department, or other entity of the Government, the use, duplication,
 * reproduction, release, modification, disclosure, or transfer of this code, or
 * any related documentation of any kind, is restricted in accordance with Federal
 * Acquisition Regulation 12.212 for civilian agencies and Defense Federal
 * Acquisition Regulation Supplement 227.7202 for military agencies.  The use of
 * this code by the Government is further restricted in accordance with the terms
 * of the license agreement(s) and/or applicable contract terms and conditions
 * covering this code from MIPS Technologies or an authorized third party.
 * *******************************End Copyright************************************
 */

#include "dsplib_def.h" 

#include "mips_unaligned.h"


#if __GNUC__ >= 4
typedef signed char v4i8 __attribute__ ((vector_size(4)));
typedef short v2q15 __attribute__ ((vector_size(4)));
#else
typedef v4i8 __attribute__ ((mode(V4QI)));
typedef v2q15 __attribute__ ((mode(V2HI)));
#endif

typedef union
{
	int32 w;
	uint32 uw;
	int16 h[2];
	v2q15 ph;
	v4i8 qb;
} data32;


// Interpolation filter coefficients
// Even memory alignment first, then odd memory alignment

static int16 coef[2][4][8] = 
{
	{
		{ 1,  -5,  20,  20,  -5,   1,   0,   0 },		// 1/2-pixel interp
		{ 1,  -5,  52,  20,  -5,   1,   0,   0 },		// 1/4-pixel interp; avg w/left
		{ 2, -10,  40,  40, -10,   2,   0,   0 },		// 1/2-pixel interp; scaled 2x
		{ 1,  -5,  20,  52,  -5,   1,   0,   0 }		// 1/4-pixel interp; avg w/right
	},
	{
		{ 0,   1,  -5,  20,  20,  -5,   1,   0 },
		{ 0,   1,  -5,  52,  20,  -5,   1,   0 },
		{ 0,   2, -10,  40,  40, -10,   2,   0 },
		{ 0,   1,  -5,  20,  52,  -5,   1,   0 }
	}
};


#define M 256		// fractional multiplication scaling constant

static int16 coef2x[4][12] = 
{
	{ 1*M,   1*M,  -5*M,  -5*M,  20*M,  20*M,  20*M,  20*M,  -5*M,  -5*M,   1*M,   1*M },
	{ 1*M,   1*M,  -5*M,  -5*M,  52*M,  52*M,  20*M,  20*M,  -5*M,  -5*M,   1*M,   1*M },
	{ 2*M,   2*M, -10*M, -10*M,  40*M,  40*M,  40*M,  40*M, -10*M, -10*M,   2*M,   2*M },
	{ 1*M,   1*M,  -5*M,  -5*M,  20*M,  20*M,  52*M,  52*M,  -5*M,  -5*M,   1*M,   1*M }
};


//////////////////////////////////////////////////////////////////////


static inline
v2q15 preceu_low_addr(v4i8 a)
{
#ifdef MIPSEB
	return __builtin_mips_preceu_ph_qbl(a);
#else
	return __builtin_mips_preceu_ph_qbr(a);
#endif
}


static inline
v2q15 preceu_high_addr(v4i8 a)
{
#ifdef MIPSEB
	return __builtin_mips_preceu_ph_qbr(a);
#else
	return __builtin_mips_preceu_ph_qbl(a);
#endif
}


static inline
v2q15 precequ_low_addr(v4i8 a)
{
#ifdef MIPSEB
	return __builtin_mips_precequ_ph_qbl(a);
#else
	return __builtin_mips_precequ_ph_qbr(a);
#endif
}


static inline
v2q15 precequ_high_addr(v4i8 a)
{
#ifdef MIPSEB
	return __builtin_mips_precequ_ph_qbr(a);
#else
	return __builtin_mips_precequ_ph_qbl(a);
#endif
}


//////////////////////////////////////////////////////////////////////


static inline 
v4i8 clip16x4(data32 a[2])
{
	v2q15 v0 = a[0].ph;
	v2q15 v1 = a[1].ph;

	v0 = __builtin_mips_shll_s_ph (v0, 7);
	v1 = __builtin_mips_shll_s_ph (v1, 7);

#ifdef MIPSEB
	return __builtin_mips_precrqu_s_qb_ph(v0, v1);
#else
	return __builtin_mips_precrqu_s_qb_ph(v1, v0);
#endif
}


static inline
v4i8 round_clip16x4(data32 a[2], int k)
{
	v2q15 v0 = a[0].ph;
	v2q15 v1 = a[1].ph;

	v0 = __builtin_mips_shra_r_ph(v0, k);
	v1 = __builtin_mips_shra_r_ph(v1, k);
	
	v0 = __builtin_mips_shll_s_ph(v0, 7);
	v1 = __builtin_mips_shll_s_ph(v1, 7);

#ifdef MIPSEB
	return __builtin_mips_precrqu_s_qb_ph(v0, v1);
#else
	return __builtin_mips_precrqu_s_qb_ph(v1, v0);
#endif
}


static inline
v4i8 round_clip16x4UA(data32 a[2], int k)
{
	v2q15 v0 = (v2q15) mips_ulw(&a[0]);
	v2q15 v1 = (v2q15) mips_ulw(&a[1]);

	v0 = __builtin_mips_shra_r_ph(v0, k);
	v1 = __builtin_mips_shra_r_ph(v1, k);
	
	v0 = __builtin_mips_shll_s_ph(v0, 7);
	v1 = __builtin_mips_shll_s_ph(v1, 7);

#ifdef MIPSEB
	return __builtin_mips_precrqu_s_qb_ph(v0, v1);
#else
	return __builtin_mips_precrqu_s_qb_ph(v1, v0);
#endif
}


static inline
v4i8 add_round_clip16x4(data32 a[2], int b, int k)
{
	v2q15 v0 = a[0].ph;
	v2q15 v1 = a[1].ph;

	v2q15 bb = __builtin_mips_repl_ph(b);

	v0 = __builtin_mips_addq_s_ph(v0, bb);
	v1 = __builtin_mips_addq_s_ph(v1, bb);

	v0 = __builtin_mips_shra_r_ph(v0, k);
	v1 = __builtin_mips_shra_r_ph(v1, k);
	
	v0 = __builtin_mips_shll_s_ph(v0, 7);
	v1 = __builtin_mips_shll_s_ph(v1, 7);

#ifdef MIPSEB
	return __builtin_mips_precrqu_s_qb_ph(v0, v1);
#else
	return __builtin_mips_precrqu_s_qb_ph(v1, v0);
#endif
}


//////////////////////////////////////////////////////////////////////


static inline
v4i8 average8x4(v4i8 a, v4i8 b)
{
   	return __builtin_mips_adduh_r_qb(a, b);
}


//////////////////////////////////////////////////////////////////////


static inline
int32 fir8(uint8 *src, int stride, int c)
{
	int k;
	int64 r;

	asm("mult %1, %2" : "=x" (r) : "r" ((int)*src), "r" ((int)coef[0][c][0]));
	
	for (k = 1; k < 6; k++)
	{
		src += stride;
		asm("madd %1, %2" : "+x" (r) : "r" ((int)*src), "r" ((int)coef[0][c][k]));
	}

	return r;
}


static inline
void fir8x4h(data32 r[2], uint8 *src, int c)
{
	int64 ac0 = 0, ac1 = 0, ac2 = 0, ac3 = 0;

	v2q15 *ce32 = (v2q15 *)coef[0][c];
	v2q15 *co32 = (v2q15 *)coef[1][c];

	v4i8 s0 = (v4i8) mips_ulw(&src[0]);
	v4i8 s1 = (v4i8) mips_ulw(&src[4]);
	v4i8 s2 = (v4i8) mips_ulw(&src[8]);

	v2q15 d0 = preceu_low_addr(s0);
	v2q15 d1 = preceu_high_addr(s0);
	v2q15 d2 = preceu_low_addr(s1);
	v2q15 d3 = preceu_high_addr(s1);
	v2q15 d4 = preceu_low_addr(s2);

	ac0 = __builtin_mips_dpaq_s_w_ph(ac0, d0, ce32[0]);
	ac0 = __builtin_mips_dpaq_s_w_ph(ac0, d1, ce32[1]);
	ac0 = __builtin_mips_dpaq_s_w_ph(ac0, d2, ce32[2]);

	ac1 = __builtin_mips_dpaq_s_w_ph(ac1, d0, co32[0]);
	ac1 = __builtin_mips_dpaq_s_w_ph(ac1, d1, co32[1]);
	ac1 = __builtin_mips_dpaq_s_w_ph(ac1, d2, co32[2]);
	ac1 = __builtin_mips_dpaq_s_w_ph(ac1, d3, co32[3]);

	ac2 = __builtin_mips_dpaq_s_w_ph(ac2, d1, ce32[0]);
	ac2 = __builtin_mips_dpaq_s_w_ph(ac2, d2, ce32[1]);
	ac2 = __builtin_mips_dpaq_s_w_ph(ac2, d3, ce32[2]);

	ac3 = __builtin_mips_dpaq_s_w_ph(ac3, d1, co32[0]);
	ac3 = __builtin_mips_dpaq_s_w_ph(ac3, d2, co32[1]);
	ac3 = __builtin_mips_dpaq_s_w_ph(ac3, d3, co32[2]);
	ac3 = __builtin_mips_dpaq_s_w_ph(ac3, d4, co32[3]);

	r[0].h[0] = __builtin_mips_extr_s_h(ac0, 1);
	r[0].h[1] = __builtin_mips_extr_s_h(ac1, 1);
	r[1].h[0] = __builtin_mips_extr_s_h(ac2, 1);
	r[1].h[1] = __builtin_mips_extr_s_h(ac3, 1);
}


static inline
void fir8x4v(data32 r[2], uint8 *src, int stride, int c)
{
	int i;

	v2q15 r0 = { 0, 0 };
	v2q15 r1 = { 0, 0 };

	v2q15 *c32 = (v2q15 *)coef2x[c];

	for (i = 0; i < 6; i++)
	{
		v4i8 s = (v4i8) mips_ulw(src);
		v2q15 d0 = precequ_low_addr(s);
		v2q15 d1 = precequ_high_addr(s);

		v2q15 p0 = __builtin_mips_mulq_rs_ph(d0, c32[i]);
		v2q15 p1 = __builtin_mips_mulq_rs_ph(d1, c32[i]);

		r0 = __builtin_mips_addq_s_ph(r0, p0);
		r1 = __builtin_mips_addq_s_ph(r1, p1);

		src += stride;
	}

	r[0].ph = r0;
	r[1].ph = r1;
}


//////////////////////////////////////////////////////////////////////


static inline
void fir16x4h(data32 r[2], data32 *src, int c)
{
	int64 ac0 = 0, ac1 = 0, ac2 = 0, ac3 = 0;

	v2q15 *ce32 = (v2q15 *)coef[0][c];
	v2q15 *co32 = (v2q15 *)coef[1][c];

	v2q15 *src32 = &src->ph;

	ac0 = __builtin_mips_dpaq_s_w_ph(ac0, src32[0], ce32[0]);
	ac0 = __builtin_mips_dpaq_s_w_ph(ac0, src32[1], ce32[1]);
	ac0 = __builtin_mips_dpaq_s_w_ph(ac0, src32[2], ce32[2]);

	ac1 = __builtin_mips_dpaq_s_w_ph(ac1, src32[0], co32[0]);
	ac1 = __builtin_mips_dpaq_s_w_ph(ac1, src32[1], co32[1]);
	ac1 = __builtin_mips_dpaq_s_w_ph(ac1, src32[2], co32[2]);
	ac1 = __builtin_mips_dpaq_s_w_ph(ac1, src32[3], co32[3]);

	ac2 = __builtin_mips_dpaq_s_w_ph(ac2, src32[1], ce32[0]);
	ac2 = __builtin_mips_dpaq_s_w_ph(ac2, src32[2], ce32[1]);
	ac2 = __builtin_mips_dpaq_s_w_ph(ac2, src32[3], ce32[2]);

	ac3 = __builtin_mips_dpaq_s_w_ph(ac3, src32[1], co32[0]);
	ac3 = __builtin_mips_dpaq_s_w_ph(ac3, src32[2], co32[1]);
	ac3 = __builtin_mips_dpaq_s_w_ph(ac3, src32[3], co32[2]);
	ac3 = __builtin_mips_dpaq_s_w_ph(ac3, src32[4], co32[3]);

	r[0].h[0] = __builtin_mips_extr_rs_w(ac0, 11);
	r[0].h[1] = __builtin_mips_extr_rs_w(ac1, 11);
	r[1].h[0] = __builtin_mips_extr_rs_w(ac2, 11);
	r[1].h[1] = __builtin_mips_extr_rs_w(ac3, 11);
}


static inline
void fir16x4v(data32 r[2], data32 *src, int stride, int c)
{
	int i;
	int64 ac0 = 0, ac1 = 0, ac2 = 0, ac3 = 0;

	v2q15 r0 = { 0, 0 };
	v2q15 r1 = { 0, 0 };

	v2q15 *c32 = (v2q15 *)coef2x[c];

	for (i = 0; i < 6; i++)
	{
#ifdef MIPSEB
		ac0 = __builtin_mips_maq_s_w_phl(ac0, src[0].ph, c32[i]);
		ac1 = __builtin_mips_maq_s_w_phr(ac1, src[0].ph, c32[i]);
		ac2 = __builtin_mips_maq_s_w_phl(ac2, src[1].ph, c32[i]);
		ac3 = __builtin_mips_maq_s_w_phr(ac3, src[1].ph, c32[i]);
#else
		ac0 = __builtin_mips_maq_s_w_phr(ac0, src[0].ph, c32[i]);
		ac1 = __builtin_mips_maq_s_w_phl(ac1, src[0].ph, c32[i]);
		ac2 = __builtin_mips_maq_s_w_phr(ac2, src[1].ph, c32[i]);
		ac3 = __builtin_mips_maq_s_w_phl(ac3, src[1].ph, c32[i]);
#endif
		src += stride;
	}

	r[0].h[0] = __builtin_mips_extr_s_h(ac0, 19);
	r[0].h[1] = __builtin_mips_extr_s_h(ac1, 19);
	r[1].h[0] = __builtin_mips_extr_s_h(ac2, 19);
	r[1].h[1] = __builtin_mips_extr_s_h(ac3, 19);
}


//////////////////////////////////////////////////////////////////////


// 1/4-pixel luma motion compensation per 8.4.2.2.1 of JVT-G050 (JVT:8.4.2.2.1).
// Parameters:
//   b			- output 4x4-pixel block
//   src		- pointer to the source block inside the reference image
//   ystride	- stride used to move the src pointer to the next row
//   dx, dy		- fractional source block offset from 0/4 to 3/4

void mips_h264_mc_luma(uint8 b[4][4], uint8 *src, int ystride, int dx, int dy)
{
	int x, y, k;
	data32 tmp[4+2+3][(4+2+3+1)/2];

	// combined decision variable

	unsigned dxdy = (dx << 4) | dy;

	// branch to the appropriate case

	switch (dxdy)
	{
		case 0x00:
			
			// no interpolation; just block copy

			for (y = 0; y < 4; y++)
			{
				mips_usw(b[y], mips_ulw(src));
				src += ystride;
			}

			break;

		case 0x01:
		case 0x02:
		case 0x03:

			// vertical interpolation (dx = 0)

			src -= 2 * ystride;

			for (y = 0; y < 4; y++)
			{
				data32 r[2];
				fir8x4v(r, src, ystride, dy);
				*(v4i8 *)b[y] = add_round_clip16x4(r, (dy & 0x01) << 4, 6);

				src += ystride;
			}

			break;

		case 0x10:
		case 0x20:
		case 0x30:

			// horizontal interpolation (dy = 0)

			for (y = 0; y < 4; y++)
			{
				data32 r[2];
				fir8x4h(r, &src[-2], dx);
				*(v4i8 *)b[y] = add_round_clip16x4(r, (dx & 0x01) << 4, 6);

				src += ystride;
			}

			break;

		case 0x22:
			// middle pixel (dx = 2, dy = 2)
			// do vertical interpolation to a temporary buffer first

			src -= 2 * ystride;

			for (y = 0; y < 4; y++)
			{
				fir8x4v(&tmp[0][0], &src[0-2], ystride, 0);
				fir8x4v(&tmp[0][2], &src[4-2], ystride, 0);
				tmp[0][4].h[0] = fir8(&src[8-2], ystride, 0);

				// now do horizontal interpolation

				data32 r[2];
				fir16x4h(r, tmp[0], 0);
				*(v4i8 *)b[y] = clip16x4(r);

				src += ystride;
			}

			break;

		case 0x21:
		case 0x23:
			
			// middle column (dx = 2)
			// do horizontal interpolation to a temporary buffer first
			
			src -= 2 * ystride;

			for (y = 0; y < 4+2+3; y++)
			{
				fir8x4h(tmp[y], &src[-2], 0);
				src += ystride;
			}

			// now do vertical interpolation

			for (y = 0; y < 4; y++)
			{
				data32 r[2];
				fir16x4v(r, tmp[y], tmp[1] - tmp[0], 0);

				v4i8 r1 = clip16x4(r);
				v4i8 r2 = round_clip16x4(tmp[y + 2 + (dy >> 1)], 5);
				*(v4i8 *)b[y] = average8x4(r1, r2);
			}
			
			break;

		case 0x12:
		case 0x32:

			// middle row (dy = 2)
			// do vertical interpolation to a temporary buffer first

			src -= 2 * ystride;

			for (y = 0; y < 4; y++)
			{
				fir8x4v(&tmp[0][0], &src[0-2], ystride, 0);
				fir8x4v(&tmp[0][2], &src[4-2], ystride, 0);
				tmp[0][4].h[0] = fir8(&src[8-2], ystride, 0);

				// now do horizontal interpolation

				data32 r[2];
				fir16x4h(r, tmp[0], 0);
								
				int16 *tmp16 = &tmp[0][0].h[0];

				v4i8 r1 = clip16x4(r);
				v4i8 r2 = round_clip16x4UA((data32 *)&tmp16[2 + (dx >> 1)], 5);
				*(v4i8 *)b[y] = average8x4(r1, r2);

				src += ystride;
			}

			break;

		case 0x11:
		case 0x31:
		case 0x13:
		case 0x33:

			// diagonal 1/4-pixel interpolation
			// do vertical interpolation to a temporary buffer first

			src -= 2 * ystride;

			for (y = 0; y < 4; y++)
			{
				data32 r[2];
				fir8x4v(r, &src[dx >> 1], ystride, 0);
				tmp[y][0].qb = round_clip16x4(r, 5);

				src += ystride;
			}

			src -= 2 * ystride;

			// now do horizontal interpolation

			if (dy == 3) src += ystride;

			for (y = 0; y < 4; y++)
			{
				data32 r[2];
				fir8x4h(r, &src[-2], 0);
				v4i8 c = round_clip16x4(r, 5);
				*(v4i8 *)b[y] = average8x4(c, tmp[y][0].qb);

				src += ystride;
			}

			break;
	}
}


