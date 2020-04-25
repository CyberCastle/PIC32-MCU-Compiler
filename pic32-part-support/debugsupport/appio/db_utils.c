/********************************************************************
 *
 *                  debug utility interface.
 *
 *********************************************************************
 * FileName:        db_utils.c
 * Dependencies:    None
 * Processor:       PIC32
 *
 * Compiler:        MPLAB XC32 v1.00+
 *                  MPLAB X IDE
 * Company:         Microchip Technology, Inc.
 *
 * Copyright (c) 2009 Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the "Company") for its PIC32 Microcontroller is intended
 * and supplied to you, the Company's customer, for use solely and
 * exclusively on Microchip PIC32 Microcontroller products.
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ********************************************************************/
#define __MPLAB_DEBUGGER_PIC32MXSK 1
#define PIC32_STARTER_KIT 1

//#include <sys/appio.h>
#include <stdio.h>
#include <proc/p32mxgeneric.h>
#define _P32XXXX_H /* so that p32xxxx.h and p32generic.h don't get included */
#define __32MXGENERIC_H
#define _XC_H

extern int __XC_UART;

char __pic32mxsk_stdout_buf[256];

void __attribute__((section(".text.StarterKitIO"))) _pic32mxsk_init(void)
{
        __XC_UART = 0;
}

void  __attribute__ (( section(".text.StarterKitIO"),noinline,nomips16))
db_puts(const unsigned char *s, int len)
{
	asm volatile ("li $v0, 1; sdbbp 1" ::: "v0");
}

void  __attribute__ (( section(".text.StarterKitIO"),noinline,nomips16))
db_gets(unsigned char *s, int len)
{
	asm volatile ("li $v0, 2; sdbbp 1" ::: "v0");
}

void __attribute__((section(".text.StarterKitIO")))  __attribute__((alias("_p32mxsk_write")))
_mon_write (const char * s, unsigned int count)
{
  /* Do not try to output an empty string */
  if (!s)
    return;

  db_puts(s, count);
}

void __attribute__((section(".text.StarterKitIO"))) __attribute__((alias("_p32mxsk_putc")))
_mon_putc (char c)
{
   char ary[1];
   ary[0] = c;
   db_puts(ary, 1);
}

int __attribute__((section(".text.StarterKitIO"))) __attribute__((alias("_p32mxsk_getc")))
_mon_getc (int canblock)
{
  unsigned char retval;
  db_gets(&retval, 1);
  return retval;
}
