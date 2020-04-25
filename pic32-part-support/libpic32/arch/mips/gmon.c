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
 */

#ifndef lint
static char sccsid[] = "@(#)gmon.c	5.5 (Berkeley) 5/21/91";
#endif /* not lint */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#include "gmon.h"
#define u_char unsigned char

extern unsigned long __profil_hertz (void);

extern void _mon_puts (const char *);

extern char *_minbrk;
static void mcleanup(void);
void _gmoncontrol(int);

#define GMON_PROF_ON	0x1
#define GMON_PROF_BUSY	0x2
#define GMON_PROF_OFF	0x3
#define GMON_PROF_ERROR	0x7

    /*
     *	froms is actually a bunch of ARCINDEXes indexing tos
     */
static unsigned int	state = GMON_PROF_OFF;
static unsigned int	log_hashfraction;
static ARCINDEX		*froms;
static struct tostruct	*tos = 0;
static ARCINDEX		tolimit = 0;
static unsigned long	s_lowpc = 0;
static unsigned long	s_highpc = 0;
static unsigned long	s_textsize = 0;

static int	ssiz;
static char	*sbuf;
static int	s_scale;

    /* see profil(2) where this is describe (incorrectly) */
#define		SCALE_1_TO_1	0x10000L

#define	MSG "No space for profiling buffer(s)\r\n"

#define NO_INSTRUMENT __attribute__((no_instrument_function))

void NO_INSTRUMENT
_gmonstartup(unsigned long lowpc, unsigned long highpc)
{
    int			monsize;
    int			pad;
    int			fromsize;
    int			tossize;
    char		*buffer;
#ifdef unix
    int			o;
#endif
    unsigned long	phz;

    /*
     *	round lowpc and highpc to multiples of the density we're using
     *	so the rest of the scaling (here and in gprof) stays in ints.
     */
    s_lowpc = lowpc = ROUNDDOWN(lowpc, HISTFRACTION*sizeof(HISTCOUNTER));
    s_highpc = highpc = ROUNDUP(highpc, HISTFRACTION*sizeof(HISTCOUNTER));
    s_textsize = highpc - lowpc;

    phz = __profil_hertz ();
    if (phz > 0) {
	monsize = (s_textsize / HISTFRACTION) + sizeof(struct gmon_hist_hdr);
	pad = ROUNDUP(monsize, sizeof (double)) - monsize;
    }
    else
	monsize = pad = 0;

    fromsize = s_textsize / HASHFRACTION;
    fromsize = ROUNDUP(fromsize, sizeof (double));

    tolimit = s_textsize * ARCDENSITY / 100;
    if ( tolimit < MINARCS ) {
	tolimit = MINARCS;
    } else if ( tolimit > MAXARCS ) {
	tolimit = MAXARCS;
    }

    tossize = tolimit * sizeof (struct tostruct);
    tossize = ROUNDUP(tossize, sizeof (double));

    buffer = (char *) sbrk (tossize + fromsize + monsize + pad);
    if (buffer == (char *) -1) {
	_mon_puts (MSG);
	state = GMON_PROF_ERROR;
	return;
    }

    _minbrk = (char *) sbrk(0);		/* reset _minbrk to new base */

    tos = (struct tostruct *) buffer;
    tos[0].link = 0;
    buffer += tossize;

    froms = (ARCINDEX *)buffer;
    buffer += fromsize;

    sbuf = buffer;
    ssiz = monsize;
    s_scale = SCALE_1_TO_1;
    if (monsize > 0) {
	struct gmon_hist_hdr *hdr = (struct gmon_hist_hdr *)sbuf;

	monsize -= sizeof (struct gmon_hist_hdr);
	if (monsize <= 0)
	    return;

	hdr->low_pc = lowpc;
	hdr->high_pc = highpc;
	hdr->hist_size = monsize / sizeof (HISTCOUNTER);
	hdr->prof_rate = phz;
	strncpy (hdr->dimen, "seconds", sizeof (hdr->dimen));
	hdr->dimen_abbrev = 's';

#if 1
	s_scale = SCALE_1_TO_1 / HISTFRACTION;
#else
#ifdef unix
	o = highpc - lowpc;
	if( monsize < o ) {
	    s_scale = ( (float) monsize / o ) * SCALE_1_TO_1;
#else /* avoid floating point */
	    int quot = o / monsize;
	    
	    if (quot >= SCALE_1_TO_1)
		s_scale = 1;
	    else if (quot >= 0x100)
		s_scale = SCALE_1_TO_1 / quot;
	    else if (o >= 0x800000)
		s_scale = (SCALE_1_TO_1 << 8) / (o / (monsize >> 8));
	    else
		s_scale = (SCALE_1_TO_1 << 8) / ((o << 8) / monsize);
#endif
	}
#endif

    }

    /* The following test must be kept in sync with the corresponding
       test in mcount  */
    if ((HASHFRACTION & (HASHFRACTION - 1)) == 0) {
      /* if HASHFRACTION is a power of two, mcount can use shifting
	 instead of integer division.  Precompute shift amount. */
	unsigned int frac = HASHFRACTION * sizeof(*froms);
	unsigned int log;
	for (log = 0; ! (frac & (1 << log)); log++)
	    continue;
	log_hashfraction = log;
    }

    atexit (mcleanup);
    _gmoncontrol(1);
}



static void 
NO_INSTRUMENT
write_gmon_hdr (int fd)
{
    struct gmon_hdr hdr;
    
    memset (&hdr, 0, sizeof hdr);
    memcpy (hdr.cookie, GMON_MAGIC, sizeof hdr.cookie);
    hdr.version = GMON_VERSION;	/* endianness? */
    _gmonwrite (fd, &hdr, sizeof hdr);
}


static void
NO_INSTRUMENT
write_time_hist (int fd)
{
    if (ssiz > 0) {
	u_char tag = GMON_TAG_TIME_HIST;
	_gmonwrite (fd, &tag, sizeof tag);
	_gmonwrite (fd, sbuf, ssiz);
    }
}


static void
NO_INSTRUMENT
write_cg_arc (int fd)
{
    u_char tag = GMON_TAG_CG_ARC;
    ARCINDEX		endfrom;
    ARCINDEX		fromindex;
    ARCINDEX		toindex;
    unsigned long	frompc;
    struct rawarc	rawarc;

    endfrom = s_textsize / (HASHFRACTION * sizeof(*froms));
    for ( fromindex = 0 ; fromindex < endfrom ; fromindex++ ) {
	if ( froms[fromindex] == 0 ) {
	    continue;
	}
	frompc = s_lowpc + (fromindex * HASHFRACTION * sizeof(*froms));
	for (toindex=froms[fromindex]; toindex!=0; toindex=tos[toindex].link) {
#	    ifdef DEBUG
		fprintf( stderr ,
			"[mcleanup] frompc 0x%x selfpc 0x%x count %d\n" ,
			frompc , tos[toindex].selfpc , tos[toindex].count );
#	    endif /* DEBUG */
	    _gmonwrite (fd, &tag, sizeof tag);
	    rawarc.raw_frompc = frompc;
	    rawarc.raw_selfpc = tos[toindex].selfpc;
	    rawarc.raw_count = tos[toindex].count;
	    _gmonwrite (fd, &rawarc, sizeof rawarc);
	}
    }
}



static void
NO_INSTRUMENT
mcleanup(void)
{
    int			fd;

    _gmoncontrol(0);

    if (state == GMON_PROF_ERROR)
	return;

    fd = _gmoncreat ("gmon.out");
    if ( fd < 0 ) {
	perror( "mcleanup: gmon.out" );
	return;
    }

    write_gmon_hdr (fd);
    write_time_hist (fd);
    write_cg_arc (fd);

    _gmonclose (fd);
}


/* 
 * _mcount is called with the previous $ra in $at (or $v1 for mips16)
 */
void
NO_INSTRUMENT
_mcount(void)
{
	register unsigned long		frompc, selfpc;
	register ARCINDEX		*frompcindex;
	register struct tostruct	*top;
	register struct tostruct	*prevtop;
	register ARCINDEX		toindex;
	int				idx;

	/* Save incoming argument registers not saved by caller, all
	   others are temps, or automatically saved in our prologue. */
#if __mips64
	auto long long			a0,a1,a2,a3;
	asm volatile ("sd $4,%0" : "=m" (a0));
	asm volatile ("sd $5,%0" : "=m" (a1));
	asm volatile ("sd $6,%0" : "=m" (a2));
	asm volatile ("sd $7,%0" : "=m" (a3));
#else
	auto int 			a0,a1,a2,a3;
	asm volatile ("sw $4,%0" : "=m" (a0));
	asm volatile ("sw $5,%0" : "=m" (a1));
	asm volatile ("sw $6,%0" : "=m" (a2));
	asm volatile ("sw $7,%0" : "=m" (a3));
#endif

	/*
	 *	find the return address for mcount,
	 *	and the return address for mcount's caller.
	 */


	/* called from mips16: frompc in $3 = $v1 */
	asm volatile ("move %0,$3" : "=d" (frompc));

	/* selfpc = pc passed by mcount call.
	   This identifies the function that was just entered.  */
	asm volatile ("move %0,$31" : "=d" (selfpc));

	/* frompc = pc saved from mcount's caller.
	   Identifies the function that called the caller of _mcount.  */
	if ((selfpc & 1) == 0)
	  {
	    /* called from non-mips16: frompc in $1 = $at */
	    asm volatile (" .set noat");
	    asm volatile ("move %0,$1" : "=d" (frompc));
	    asm volatile (" .set at");
	  }

	/*
	 *	check that we are profiling
	 *	and that we aren't recursively invoked.
	 */
	if (mips_atomic_bis (&state, GMON_PROF_BUSY) & GMON_PROF_BUSY)
	    goto out;

	/*
	 *	check that frompcindex is a reasonable pc value.
	 *	for example:	signal catchers get called from the stack,
	 *			not from text space.  too bad.
	 */
	frompc -= s_lowpc;
	if (frompc > s_textsize)
		goto done;

	if ((HASHFRACTION & (HASHFRACTION - 1)) == 0)
	    /* avoid integer divide if possible: */
	    idx = frompc >> log_hashfraction;
	else
	    idx = frompc / (HASHFRACTION * sizeof(*froms));
	frompcindex = &froms[idx];
	toindex = *frompcindex;
	if (toindex == 0) {
		/*
		 *	first time traversing this arc
		 */
		toindex = ++tos[0].link;
		if (toindex >= tolimit) {
			goto overflow;
		}
		*frompcindex = toindex;
		top = &tos[toindex];
		top->selfpc = selfpc;
		top->count = 1;
		top->link = 0;
		goto done;
	}
	top = &tos[toindex];
	if (top->selfpc == selfpc) {
		/*
		 *	arc at front of chain; usual case.
		 */
		top->count++;
		goto done;
	}
	/*
	 *	have to go looking down chain for it.
	 *	top points to what we are looking at,
	 *	prevtop points to previous top.
	 *	we know it is not at the head of the chain.
	 */
	for (; /* goto done */; ) {
		if (top->link == 0) {
			/*
			 *	top is end of the chain and none of the chain
			 *	had top->selfpc == selfpc.
			 *	so we allocate a new tostruct
			 *	and link it to the head of the chain.
			 */
			toindex = ++tos[0].link;
			if (toindex >= tolimit) {
				goto overflow;
			}
			top = &tos[toindex];
			top->selfpc = selfpc;
			top->count = 1;
			top->link = *frompcindex;
			*frompcindex = toindex;
			goto done;
		}
		/*
		 *	otherwise, check the next arc on the chain.
		 */
		prevtop = top;
		top = &tos[top->link];
		if (top->selfpc == selfpc) {
			/*
			 *	there it is.
			 *	increment its count
			 *	move it to the head of the chain.
			 */
			top->count++;
			toindex = prevtop->link;
			prevtop->link = top->link;
			top->link = *frompcindex;
			*frompcindex = toindex;
			goto done;
		}

	}
done:
	state = GMON_PROF_ON;
	/* and fall through */
out:
	/* Restore caller's incoming argument registers */
#if __mips64
	asm volatile ("ld $4,%0" : : "m" (a0));
	asm volatile ("ld $5,%0" : : "m" (a1));
	asm volatile ("ld $6,%0" : : "m" (a2));
	asm volatile ("ld $7,%0" : : "m" (a3));
#else
	asm volatile ("lw $4,%0" : : "m" (a0));
	asm volatile ("lw $5,%0" : : "m" (a1));
	asm volatile ("lw $6,%0" : : "m" (a2));
	asm volatile ("lw $7,%0" : : "m" (a3));
#endif
	return;		/* normal return restores saved registers */

overflow:
	state = GMON_PROF_ERROR; /* halt further profiling */
	_mon_puts ("mcount: tos overflow\r\n");
	goto out;
}

/*
 * Control profiling
 *	profiling is what mcount checks to see if
 *	all the data structures are ready.
 */
void
NO_INSTRUMENT
_gmoncontrol(int mode)
{
    /* Don't change the state if we ran into an error.  */
    if (state == GMON_PROF_ERROR)
	return;

    if (mode) {
	/* start */
	profil(sbuf + sizeof(struct gmon_hist_hdr), 
	       ssiz - sizeof(struct gmon_hist_hdr),
	       (long)s_lowpc, s_scale);
	state = GMON_PROF_ON;
    } else {
	/* stop */
	profil((char *)0, 0, 0, 0);
	state = GMON_PROF_OFF;
    }
}

