/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)gmon.h	5.3 (Berkeley) 5/6/91
 */

#define	GMON_MAGIC	"gmon"	/* cookie */
#define GMON_VERSION	1

/* record type */
#define GMON_TAG_TIME_HIST	0
#define GMON_TAG_CG_ARC		1
#define GMON_TAG_BB_COUNT	2
#define GMON_TAG_TIME_HIST32	3

struct gmon_hdr {
    char 		cookie[4];
    int			version;
    int			spare[3];
};


struct gmon_hist_hdr {
    unsigned long	low_pc;		/* base pc address of sample buffer */
    unsigned long 	high_pc;	/* max pc address of sampled buffer */
    int			hist_size;	/* size of sample buffer */
    int			prof_rate;	/* profiling clock rate */
    char 		dimen[15];	/* phys. dim., usually "seconds" */
    char		dimen_abbrev;	/* usually 's' for "seconds" */
};


struct new_bsd_phdr {
    unsigned long lpc;		/* base pc address of sample buffer */
    unsigned long hpc;		/* max pc address of sampled buffer */
    int ncnt;			/* size of sample buffer (plus this header) */
    int version;		/* version number */
    int profrate;		/* profiling clock rate */
    int spare[3];		/* reserved */
};

#define GMONVERSION     0x00051879

struct old_bsd_phdr {
    unsigned long	lpc;
    unsigned long	hpc;
    int			ncnt;
};

    /*
     *	histogram counters are unsigned shorts (according to the kernel).
     */
#define	HISTCOUNTER	unsigned short

    /*
     *	fraction of text space to allocate for histogram counters
     *	here, 1/2
     */
#define	HISTFRACTION	2

    /*
     *	Fraction of text space to allocate for from hash buckets.
     *	The value of HASHFRACTION is based on the minimum number of bytes
     *	of separation between two subroutine call points in the object code.
     *	Given MIN_SUBR_SEPARATION bytes of separation the value of
     *	HASHFRACTION is calculated as:
     *
     *		HASHFRACTION = MIN_SUBR_SEPARATION / (2 * sizeof(short) - 1);
     *
     *	For the MIPS, the shortest two call sequence is:
     *
     *		0: jal	x
     *		4: nop
     *		8: jal	y
     *
     *	For the MIPS16, the shortest two call sequence is:
     *
     *		0: jal	x
     *		4: xxx
     *		6: jal	y
     *
     *	so the minimum separation is 6 bytes, thus HASHFRACTION is 
     *	calculated as:
     *
     *		HASHFRACTION = 6 / (2 * 2 - 1) = 2
     *
     *	Note that the division above rounds down, thus if MIN_SUBR_FRACTION
     *	is less than three, this algorithm will not work!
     */
#define	HASHFRACTION	2

    /*
     *	Percent of text space to allocate for tostructs
     *	with a minimum.
     *  Now increased from 2% to 3% to better cope with 
     *  programs with lots of small functions.
     */
#define ARCDENSITY	3
#define MINARCS		50
#if 1
/* larger arc index type, to handle > 64K functions */
#define ARCINDEX	unsigned long
#define MAXARCS		(1<<20)
#else
#define ARCINDEX	unsigned short
#define MAXARCS		((1<< (8 * sizeof (ARCINDEX))) - 2)
#endif

struct tostruct {
    unsigned long	selfpc;
    unsigned long	count;
    ARCINDEX		link;
};

    /*
     *	a raw arc,
     *	    with pointers to the calling site and the called site
     *	    and a count.
     */
struct rawarc {
    unsigned long	raw_frompc;
    unsigned long	raw_selfpc;
    long		raw_count;
};

    /*
     *	general rounding functions.
     */
#define ROUNDDOWN(x,y)	(((x)/(y))*(y))
#define ROUNDUP(x,y)	((((x)+(y)-1)/(y))*(y))


/* structure emitted by "gcc -a" or "gcc -fprofile-arcs".  This must
   match struct bb in gcc/libgcc2.c.  It is OK for gcc to declare a
   longer structure as long as the members below are present.  */
struct __bb
{
  long                  zero_word;
  const char            *filename;
  long                  *counts;
  long                  ncounts;
  struct __bb           *next;
  const unsigned long   *addresses;
};
