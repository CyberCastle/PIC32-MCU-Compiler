/*********************************************************************
 *
 *                  APPIN/APPOUT Debug Support
 *
 *********************************************************************
 * FileName:        appio.c
 * Dependencies:    appio.h
 *                  stdio.h
 *
 * Processor:       PIC32
 *
 * Compiler:        MPLAB XC32 v1.00+
 *                  MPLAB X IDE
 * Company:         Microchip Technology Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the ?Company?) for its PIC32/PIC24F Microcontroller is intended
 * and supplied to you, the Company?s customer, for use solely and
 * exclusively on Microchip PIC32/PIC24F Microcontroller products.
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN ?AS IS? CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *
 ********************************************************************/

#include <stdio.h>
#include <limits.h>
#include <proc/p32mxgeneric.h>
#define _P32XXXX_H /* so that p32xxxx.h and p32generic.h don't get included */
#define __32MXGENERIC_H
#define _XC_H

/* define _APPIO_DEBUG before including appio.h */
#define __APPIO_DEBUG
#define __APPO
#define __APPI
#define __DEBUG

extern int __XC_UART;

typedef union
{
   unsigned char byte[4];
   unsigned int	w;
} buffer_t;

void __attribute__((section(".text.APPIO_DEBUG._appio_init"))) _appio_init(void)
{
	__XC_UART = 0;
}

#if defined(__APPO)

static 
#if defined(__PIC__)
__attribute__((section(".bss.APPIO_DEBUG")))
#else
__attribute__((section(".sbss.APPIO_DEBUG")))
#endif
buffer_t outbuf;
static 
#if defined(__PIC__)
__attribute__((section(".bss.APPIO_DEBUG")))
#else
__attribute__((section(".sbss.APPIO_DEBUG")))
#endif
unsigned int outcount;

void __attribute__((section(".text.APPIO_DEBUG"))) __attribute__((alias("_appio_putc")))
_mon_putc (char c)
{
  outbuf.byte[outcount] = c;
  outcount++;

  if ((outcount > 3) || (c==0) || (c==EOF) || (c=='\n')) {
    outcount = 0;
    while(_DDPSTATbits.APOFUL);
    _APPO = outbuf.w;
    outbuf.w = 0;
  }
}

void __attribute__((section(".text.APPIO_DEBUG")))
_appio_putword (unsigned int w)
{
  while(_DDPSTATbits.APOFUL);
  _APPO = w;
}

void __attribute__((section(".text.APPIO_DEBUG"))) __attribute__((alias("_appio_write")))
//_appio_write (const char * s, unsigned int count)
_mon_write (const char * s, unsigned int count)
{
  /* Do not try to output an empty string */
  if (!s)
    return;

  /* Output the string character-by-character to the UART */
  while (count)
    {
      _mon_putc (*s);
      s++;
      count--;
    }
}

#endif /* __APPO */

#if defined(__APPI)

static 
#if defined(__PIC__)
__attribute__((section(".bss.APPIO_DEBUG")))
#else
__attribute__((section(".sbss.APPIO_DEBUG")))
#endif
buffer_t inbuf;
static unsigned int incount = 0xFFFFFFFFu;
int __attribute__((section(".text.APPIO_DEBUG"))) __attribute__((alias("_appio_getc")))
//_appio_getc (int canblock)
_mon_getc (int canblock)
{
  int retval;

  if (incount > 3) {
    /* Get a new word */
    incount = 0;
    while(!_DDPSTATbits.APIFUL);
    inbuf.w = _APPI;
  }

  retval = inbuf.byte[incount];
  incount++;

  if (retval=='\n') {
  /* End of the string */
    /* Is it the last character in the word? */
    if (incount == 4) {
      /* consume the next word */
      while(!_DDPSTATbits.APIFUL);
      inbuf.w = _APPI;
      /* This word should be 0x00000000u */
      if (inbuf.w != 0) {
        /* Error - Consume extraneous words */
        while(_DDPSTATbits.APIFUL) {
          inbuf.w = _APPI;
        }
      }
    }
    incount = 0xFFFFFFFFu;
  }

  if (retval == 0) {
      incount = 0xFFFFFFFFul;
      retval = '\n';
  }

  return retval;
}

void  __attribute__ (( section(".text.APPIO_DEBUG") ))
_appio_gets(unsigned char *s, int len)
{
    int c;
    unsigned char *p = s;

    if (len < 2)
      return;
    while (--len > 0 && (c = _mon_getc(0)) != 0) {
      *p++ = c;
      if (c == '\n')
        break;
    }

    if (p != s)
      *p = '\0';

    return;
}

unsigned int __attribute__((section(".text.APPIO_DEBUG")))
_appio_getword (int canblock)
{
    while(!_DDPSTATbits.APIFUL);
    return _APPI;
}

#endif /* __APPI */
