/****************************************************************************/
/*  Copyright 2011 Microchip Technology Inc                                 */
/*                                                                          */
/*  MPLAB XC16 and XC32 define  __IT_TRANSPORT, __IT_TRANSPORT_OPTION based */
/*  upon the -mit=  command-line option; others will have to define these   */
/*  options on the command line.  See the documentation for details.        */
/*                                                                          */
/****************************************************************************/

#ifndef __INSTRUMENTEDTRACE_H
#define __INSTRUMENTEDTRACE_H

#if defined(__PIC32MX__)
#include "xc.h"
#elif defined(__dsPIC30F__)
#include <p30fxxxx.h>
#elif defined(__dsPIC33F__)
#include <p33Fxxxx.h>
#elif defined(__PIC24F__)
#include <p24Fxxxx.h>
#elif defined(__PIC24H__)
#include <p24Hxxxx.h>
#else
#warning "Specify a target processor"
#endif

#define __JOIN2(X,Y)    X##Y
#define __JOIN(X,Y)     __JOIN2(X,Y)

#if (__C32_VERSION__ < 1000)
#define __builtin_unique_id __builtin_uniqueid
#endif

/* IT data formats */
#define __IT_FMT_MASK   0xFF00
#define __IT_FMT_DFLT   0x0100  /* current format with data type & 64 trace/128 log IDs */
#define __IT_FMT_PC     0x0200  /* obsolete program counter format */
#define __IT_FMT_RAW7   0x0300  /* future generic 7-bit data */
#define __IT_FMT_RAW8   0x0400  /* future generic 8-bit data */
#define __IT_FMT_RAW16  0x0500  /* future generic 16-bit data */
#define __IT_FMT_RAW32  0x0600  /* future generic 32-bit data */

/* IT primary transport facilities */
#define __IT_XMT_MASK       0x00FF
#define __IT_PORT_MASK      0x0010
#define __IT_SPI_MASK       0x0020
#define __IT_UART_MASK      0x0040  /* future */

#define __IT_XMT_DC             0x0001
#define __IT_XMT_PORTA          (__IT_PORT_MASK | 1)
#define __IT_XMT_PORTB          (__IT_PORT_MASK | 2)
#define __IT_XMT_PORTB_HI       (__IT_PORT_MASK | 3)
#define __IT_XMT_PORTC_SPLIT    (__IT_PORT_MASK | 4)
#define __IT_XMT_PORTD          (__IT_PORT_MASK | 5)
#define __IT_XMT_PORTD_HI       (__IT_PORT_MASK | 6)
#define __IT_XMT_PORTE          (__IT_PORT_MASK | 7)
#define __IT_XMT_PORTF          (__IT_PORT_MASK | 8)
#define __IT_XMT_PORTG_TRILO    (__IT_PORT_MASK | 9)
#define __IT_XMT_PORTG_TRIHI    (__IT_PORT_MASK | 10)
#define __IT_XMT_SPI1           (__IT_SPI_MASK | 1)
#define __IT_XMT_SPI2           (__IT_SPI_MASK | 2)
#define __IT_XMT_SPI1A          (__IT_SPI_MASK | 3)
#define __IT_XMT_SPI2A          (__IT_SPI_MASK | 4)
#define __IT_XMT_SPI3A          (__IT_SPI_MASK | 5)
#define __IT_XMT_UART1          (__IT_UART_MASK | 1)
#define __IT_XMT_UART2          (__IT_UART_MASK | 2)


/* Command line option -mit:
*  -mit=string   where "string" may be "off", "dc", "spi1", etc.
*  The compiler generates the equate __IT_TRANSPORT equal to this string.
*  The defines here in effect change __IT_TRANSPORT to a hex mask.
*/
#define off         0
#define dc_pc       (__IT_XMT_DC | __IT_FMT_PC)		// obsolete
#define dc          (__IT_XMT_DC | __IT_FMT_DFLT)
#define porta       (__IT_XMT_PORTA | __IT_FMT_DFLT)
/* No portb. Lines used for PGC and PGD. */
#define portbhi     (__IT_XMT_PORTB_HI | __IT_FMT_DFLT)
#define portc       (__IT_XMT_PORTC_SPLIT | __IT_FMT_DFLT)
#define portd       (__IT_XMT_PORTD | __IT_FMT_DFLT)
#define portdhi     (__IT_XMT_PORTD_HI | __IT_FMT_DFLT)
#define porte       (__IT_XMT_PORTE | __IT_FMT_DFLT)
#define portf       (__IT_XMT_PORTF | __IT_FMT_DFLT)
#define portg       (__IT_XMT_PORTG_TRILO | __IT_FMT_DFLT)
#define portghi     (__IT_XMT_PORTG_TRIHI | __IT_FMT_DFLT)
#define spi1        (__IT_XMT_SPI1 | __IT_FMT_DFLT)
#define spi2        (__IT_XMT_SPI2 | __IT_FMT_DFLT)
#define spi1a       (__IT_XMT_SPI1A | __IT_FMT_DFLT)
#define spi2a       (__IT_XMT_SPI2A | __IT_FMT_DFLT)
#define spi3a       (__IT_XMT_SPI3A | __IT_FMT_DFLT)
#define uart1       (__IT_XMT_UART1 | __IT_FMT_DFLT)
#define uart2       (__IT_XMT_UART2 | __IT_FMT_DFLT)

#define porta_7     (__IT_XMT_PORTA | __IT_FMT_RAW7)
#define portb_7     (__IT_XMT_PORTB | __IT_FMT_RAW7)
#define portc_7     (__IT_XMT_PORTC_SPLIT | __IT_FMT_RAW7)
#define portd_7     (__IT_XMT_PORTD | __IT_FMT_RAW7)
#define porte_7     (__IT_XMT_PORTE | __IT_FMT_RAW7)
#define portf_7     (__IT_XMT_PORTF | __IT_FMT_RAW7)

#define porta_8     (__IT_XMT_PORTA | __IT_FMT_RAW8)
#define portb_8     (__IT_XMT_PORTB | __IT_FMT_RAW8)
#define portc_8     (__IT_XMT_PORTC_SPLIT | __IT_FMT_RAW8)
#define portd_8     (__IT_XMT_PORTD | __IT_FMT_RAW8)
#define porte_8     (__IT_XMT_PORTE | __IT_FMT_RAW8)
#define portf_8     (__IT_XMT_PORTF | __IT_FMT_RAW8)
#define spi1_8      (__IT_XMT_SPI1 | __IT_FMT_RAW8)
#define spi2_8      (__IT_XMT_SPI2 | __IT_FMT_RAW8)
#define uart1_8     (__IT_XMT_UART1 | __IT_FMT_RAW8)
#define uart2_8     (__IT_XMT_UART2 | __IT_FMT_RAW8)

#define dc_16       (__IT_XMT_DC | __IT_FMT_RAW16)
#define porta_16    (__IT_XMT_PORTA | __IT_FMT_RAW16)
#define portb_16    (__IT_XMT_PORTB | __IT_FMT_RAW16)
#define portc_16    (__IT_XMT_PORTC_SPLIT | __IT_FMT_RAW16)
#define portd_16    (__IT_XMT_PORTD | __IT_FMT_RAW16)
#define porte_16    (__IT_XMT_PORTE | __IT_FMT_RAW16)
#define portf_16    (__IT_XMT_PORTF | __IT_FMT_RAW16)
#define spi1_16     (__IT_XMT_SPI1 | __IT_FMT_RAW16)
#define spi2_16     (__IT_XMT_SPI2 | __IT_FMT_RAW16)
#define uart1_16    (__IT_XMT_UART1 | __IT_FMT_RAW16)
#define uart2_16    (__IT_XMT_UART2 | __IT_FMT_RAW16)

#define dc_32       (__IT_XMT_DC | __IT_FMT_RAW32)
#define porta_32    (__IT_XMT_PORTA | __IT_FMT_RAW32)
#define portb_32    (__IT_XMT_PORTB | __IT_FMT_RAW32)
#define portc_32    (__IT_XMT_PORTC_SPLIT | __IT_FMT_RAW32)
#define portd_32    (__IT_XMT_PORTD | __IT_FMT_RAW32)
#define porte_32    (__IT_XMT_PORTE | __IT_FMT_RAW32)
#define portf_32    (__IT_XMT_PORTF | __IT_FMT_RAW32)
#define spi1_32     (__IT_XMT_SPI1 | __IT_FMT_RAW32)
#define spi2_32     (__IT_XMT_SPI2 | __IT_FMT_RAW32)
#define uart1_32    (__IT_XMT_UART1 | __IT_FMT_RAW32)
#define uart2_32    (__IT_XMT_UART2 | __IT_FMT_RAW32)


/**********************************************************************
*  FUTURE:  Each __IT_TRANSPORT value can have up to six 
*  options:  __IT_TRANSPORT_OPTION1, __IT_TRANSPORT_OPTION2, ...,
*  __IT_TRANSPORT_OPTION6. Options are assigned left to right.
*
*  Some Potentials:
*  -mit=dc (TmrNum, BufSize)
*  -mit=uart1 (Baud, BufSize, DmaCh)
*
***********************************************************************/

#if (__IT_TRANSPORT & __IT_UART_MASK)
#ifndef __IT_TRANSPORT_OPTION1
#warning Please specify a transmission BAUD RATE, using 57600 as default
#define __IT_TRANSPORT_OPTION1 57600
#endif
#endif


/* INSTRUMENTED TRACE TYPE BIT MASK EQUATES                                */
/*                                                                         */
/*       +-----------------------------------------------+                 */
/*  bit  |  7  |  6  |  5  |  4  |  3  |  2     1     0  |                 */
/*       +-----------------------------------------------+                 */
/*  use  | reserved  | ptr | sgn | flt |  type size - 1  |                 */
/*       +-----------------------------------------------+                 */
/*                                                                         */
/*  bit 6: 0= byte contains data type value; 1= byte contains trace id     */
/*  bit 7: reserved for CLK line in I/O port trace                         */

#define __IT_PC          0xFF   /* for obsolete dc_pc protocol only */
#define __IT_SIZE_MASK   0x07
#define __IT_IS_FLOAT    0x08
#define __IT_IS_UNSIGNED 0x10
#define __IT_IS_PTR      0x20

#define __IT_CHAR     (sizeof(char)-1)                    // 1
#define __IT_UCHAR    ((__IT_CHAR) | __IT_IS_UNSIGNED)
#define __IT_SHORT    (sizeof(short)-1)                   // 2
#define __IT_USHORT   ((__IT_SHORT) | __IT_IS_UNSIGNED)
#define __IT_INT      (sizeof(int)-1)                     // PIC32=4; PIC24=2
#define __IT_UINT     ((__IT_INT) | __IT_IS_UNSIGNED)     
#define __IT_LONG     (sizeof(long)-1)                    // 4
#define __IT_ULONG    ((__IT_LONG) | __IT_IS_UNSIGNED)
#define __IT_LLONG    (sizeof(long long)-1)               // 8
#define __IT_ULLONG   ((__IT_LLONG) | __IT_IS_UNSIGNED)
#define __IT_FLOAT    ((sizeof(float)-1) | __IT_IS_FLOAT) // 4, but in double (8) fmt on PIC32 stack
#define __IT_LDOUBLE  ((sizeof(double)-1) | __IT_IS_FLOAT) // 8
#define __IT_PTR      ((sizeof(char *)-1) | __IT_IS_PTR)  // PIC32=4; PIC24=2


/* PORT EQUATES */

#if ((__IT_TRANSPORT & __IT_XMT_MASK) ==__IT_XMT_PORTA)
#define __IT_INIT_TRIS      TRISA &= 0xFF00
#define __IT_USE_DISI       1
#define __IT_PORT_LATCH     LATA
#define __IT_STROBE_PORT    _LATA7 = 1
#define __IT_LOAD_LATCH(x)  *((volatile unsigned char *)&LATA) = (x)
#endif

#if ((__IT_TRANSPORT & __IT_XMT_MASK) ==__IT_XMT_PORTB_HI)
#define __IT_INIT_TRIS      TRISB &= 0x00FF
#define __IT_USE_DISI       1
#define __IT_PORT_LATCH     LATB
#define __IT_STROBE_PORT    _LATB15 = 1
#define __IT_LOAD_LATCH(x)  *(((volatile unsigned char *)&LATB)+1) = (x)
#endif

/*****************************************************
* NOTE: Ports C and D will not work with A1 silicon. *
******************************************************/

#if ((__IT_TRANSPORT & __IT_XMT_MASK) ==__IT_XMT_PORTC_SPLIT)
#define __IT_INIT_TRIS      TRISC &= 0x0FE1
#define __IT_USE_DISI       1   /* non-contiguous bits; longer processing */
#define __IT_PORT_LATCH     LATC
#define __IT_STROBE_PORT    _LATC15 = 1
/* Load "x" into variable in case "x" is a volatile sfr */
#define __IT_LOAD_LATCH(x){ unsigned char __it_tmpchar = (unsigned char)(x); \
                            *(((volatile unsigned char *)&LATC)+1) = __it_tmpchar; \
                            *((volatile unsigned char *)&LATC) = __it_tmpchar<<1; }
#endif

#if ((__IT_TRANSPORT & __IT_XMT_MASK) ==__IT_XMT_PORTD)
#define __IT_INIT_TRIS      TRISD &= 0xFF00
#define __IT_USE_DISI       1
#define __IT_PORT_LATCH     LATD
#define __IT_STROBE_PORT    _LATD7 = 1
#define __IT_LOAD_LATCH(x)  *((volatile unsigned char *)&LATD) = (x)
#endif

#if ((__IT_TRANSPORT & __IT_XMT_MASK) ==__IT_XMT_PORTD_HI)
#define __IT_INIT_TRIS      TRISD &= 0x00FF
#define __IT_USE_DISI       1
#define __IT_PORT_LATCH     LATD
#define __IT_STROBE_PORT    _LATD15 = 1
#define __IT_LOAD_LATCH(x)  *(((volatile unsigned char *)&LATD)+1) = (x)
#endif

#if ((__IT_TRANSPORT & __IT_XMT_MASK) ==__IT_XMT_PORTE)
#define __IT_INIT_TRIS      TRISE &= 0xFF00
#define __IT_USE_DISI       1
#define __IT_PORT_LATCH     LATE
#define __IT_STROBE_PORT    _LATE7 = 1
#define __IT_LOAD_LATCH(x)  *((volatile unsigned char *)&LATE) = (x)
#endif

#if ((__IT_TRANSPORT & __IT_XMT_MASK) ==__IT_XMT_PORTF)
#define __IT_INIT_TRIS      TRISF &= 0xFF00
#define __IT_USE_DISI       1
#define __IT_PORT_LATCH     LATF
#define __IT_STROBE_PORT    _LATF7 = 1
#define __IT_LOAD_LATCH(x)  *((volatile unsigned char *)&LATF) = (x)
#endif

#if ((__IT_TRANSPORT & __IT_XMT_MASK) ==__IT_XMT_PORTG_TRILO)
#define __IT_INIT_TRIS      TRISG &= 0xFC30
#define __IT_USE_DISI       1   /* non-contiguous bits; longer processing */
#define __IT_PORT_LATCH     LATG
#define __IT_STROBE_PORT    _LATG9 = 1
/* Load "x" into variable in case "x" is a volatile sfr */
#define __IT_LOAD_LATCH(x){ unsigned int __it_tmpint = (x); \
                            LATG &= 0xFC00; \
                            LATG |= (__it_tmpint<<2) & 0x01C0; \
                            LATG |= __it_tmpint & 0x0F; }
#endif

#if ((__IT_TRANSPORT & __IT_XMT_MASK) ==__IT_XMT_PORTG_TRIHI)
#define __IT_INIT_TRIS      TRISG &= 0x0C3F
#define __IT_USE_DISI       1   /* non-contiguous bits; longer processing */
#define __IT_PORT_LATCH     LATG
#define __IT_STROBE_PORT    _LATG15 = 1
/* Load "x" into variable in case "x" is a volatile sfr */
#define __IT_LOAD_LATCH(x){ unsigned int __it_tmpint = (x); \
                            LATG &= 0x000F; \
                            LATG |= (__it_tmpint<<8) & 0x7000; \
                            LATG |= (__it_tmpint<<6) & 0x03C0; }
#endif



/* SPI EQUATES */

#if ((__IT_TRANSPORT & __IT_XMT_MASK) ==__IT_XMT_SPI1)
#define SPISTATBITS SPI1STATbits
#define SPISTAT     SPI1STAT
#define SPICON1     SPI1CON1
#define SPIBUF      SPI1BUF
#endif

#if ((__IT_TRANSPORT & __IT_XMT_MASK) ==__IT_XMT_SPI2)
#define SPISTATBITS SPI2STATbits
#define SPISTAT     SPI2STAT
#define SPICON1     SPI2CON1
#define SPIBUF      SPI2BUF
#endif

#if ((__IT_TRANSPORT & __IT_XMT_MASK) ==__IT_XMT_SPI1A)
#define SPISTATBITS SPI1ASTATbits
#define SPISTAT     SPI1ASTAT
#define SPICON1     SPI1ACON1
#define SPIBUF      SPI1ABUF
#endif

#if ((__IT_TRANSPORT & __IT_XMT_MASK) ==__IT_XMT_SPI2A)
#define SPISTATBITS SPI2STATbits
#define SPISTAT     SPI2ASTAT
#define SPICON1     SPI2ACON1
#define SPIBUF      SPI2ABUF
#endif

#if ((__IT_TRANSPORT & __IT_XMT_MASK) ==__IT_XMT_SPI3A)
#define SPISTATBITS SPI3ASTATbits
#define SPISTAT     SPI3ASTAT
#define SPICON1     SPI3ACON1
#define SPIBUF      SPI3ABUF
#endif



#if (__IT_TRANSPORT == off)
#define __TRACE(id)         /* TRACE disabled */
#define __LOG(id,value)     /* LOG disabled */
#endif

#if !(defined(__IT_TRANSPORT) && defined(__DEBUG))
#undef __TRACE
#define __TRACE(id)         /* TRACE disabled */
#undef __LOG
#define __LOG(id,value)     /* LOG disabled */
#else

#if ((__IT_TRANSPORT & __IT_FMT_MASK) == __IT_FMT_DFLT)
#define __IT_FMT __IT_FMT_DFLT
#elif ((__IT_TRANSPORT & __IT_FMT_MASK) == __IT_FMT_PC)
#define __IT_FMT __IT_FMT_PC
#elif ((__IT_TRANSPORT & __IT_FMT_MASK) == __IT_FMT_RAW7)
#define __IT_FMT __IT_FMT_RAW7
#elif ((__IT_TRANSPORT & __IT_FMT_MASK) == __IT_FMT_RAW8)
#define __IT_FMT __IT_FMT_RAW8
#elif ((__IT_TRANSPORT & __IT_FMT_MASK) == __IT_FMT_RAW16)
#define __IT_FMT __IT_FMT_RAW16
#else
#define __IT_FMT __IT_FMT_RAW32
#endif

#if (__IT_TRANSPORT & __IT_PORT_MASK)
#define __IT_MEDIUM  __IT_PORT_MASK
#elif (__IT_TRANSPORT & __IT_SPI_MASK)
#define __IT_MEDIUM  __IT_SPI_MASK
#elif (__IT_TRANSPORT & __IT_UART_MASK)
#define __IT_MEDIUM  __IT_UART_MASK
#else
#define __IT_MEDIUM  __IT_XMT_DC
#endif

/* In effect, set the "address" of the symbol __ITMethod to __IT_TRANSPORT. */
/* The REAL ICE plugin will get this value from the symbol table.           */
#define __STRINGIZE2(X) #X
#define __STRINGIZE(X)  __STRINGIZE2(X)
__asm__(".extern __ITMethod\n\t.equ __ITMethod," __STRINGIZE(__IT_TRANSPORT));


/* Undefine hex transport equates so that __IT_TRANSPORT    */
/* will again function as the command line string value.    */
#undef off
#undef dc_pc
#undef dc
#undef porta
#undef portb
#undef portbhi
#undef portc
#undef portd
#undef portdhi
#undef porte
#undef portf
#undef portg
#undef portghi
#undef spi1
#undef spi2
#undef spi1a
#undef spi2a
#undef spi3a

#undef uart1
#undef uart2

#undef porta_7
#undef portb_7
#undef portc_7
#undef portd_7
#undef porte_7
#undef portf_7

#undef porta_8
#undef portb_8
#undef portc_8
#undef portd_8
#undef porte_8
#undef portf_8
#undef spi1_8
#undef spi2_8
#undef uart1_8
#undef uart2_8

#undef dc_16
#undef porta_16
#undef portb_16
#undef portc_16
#undef portd_16
#undef porte_16
#undef portf_16
#undef spi1_16
#undef spi2_16
#undef uart1_16
#undef uart2_16

#undef dc_32
#undef porta_32
#undef portb_32
#undef portc_32
#undef portd_32
#undef porte_32
#undef portf_32
#undef spi1_32
#undef spi2_32
#undef uart1_32
#undef uart2_32


/**********************************************************************
* I/O PORT TRACE SUPPORT
* 
* By default, __TRACE() macros are interrupt protected. If interrupts
* are not a concern, processing speed can be improved by commenting out
* the "#define __IT_USE_DISI" equate from the corresponding port 
* definitions above. This will remove both the saving and restoring of 
* the interrupt state code as well as the function call and return 
* overhead. However, with this approach, if there are no __LOG() calls
* in the application, the user will have to initialize the TRIS bits 
* himself. This can be done by adding __IT_INIT_TRIS to the user's 
* startup code, or force the linker to link in special compiler startup 
* code. This can be done by adding the option "-u __force_load_<portoption>" 
* to the link command line. Example: -u __force_load_portbhi.
*
* I/O port trace is not supported on PIC32.
***********************************************************************/


/* I/O PORT DEFAULT FORMAT */

#if (__IT_MEDIUM == __IT_PORT_MASK)
extern void __JOIN(__it_trace_disi_,__IT_TRANSPORT)(unsigned int id);
extern void __JOIN(__it_log_,__IT_TRANSPORT)(unsigned int id, unsigned int type, ...);

#if (__IT_FMT == __IT_FMT_DFLT)

#if __IT_USE_DISI
/* interrupt protected version; longer processing */
#define __TRACE(id)     __JOIN(__it_trace_disi_,__IT_TRANSPORT)(\
                               __builtin_unique_id("__TRACE",id))
#else
/* shortest processing version */
#define __TRACE(id)   { __IT_LOAD_LATCH(__builtin_unique_id("__TRACE",id)); \
                        __IT_STROBE_PORT; }
#endif
#define __LOG(id,value) __JOIN(__it_log_,__IT_TRANSPORT)( \
                               __builtin_unique_id("__LOG",id), \
                               __builtin_ittype(value), value)
#endif


/* I/O PORT RAW FORMAT */

#if ((__IT_TRANSPORT & __IT_FMT_MASK) == __IT_FMT_RAW7)
#if __IT_USE_DISI
/* interrupt protected version; longer processing */
#define __TRACE(value)  __JOIN(__it_trace_disi_,__IT_TRANSPORT)( \
                               (unsigned char)value)
#else
/* shortest processing version */
#define __TRACE(value){ __IT_LOAD_LATCH((unsigned char)value); \
                        __IT_STROBE_PORT; }
#endif
/* (CAW) do you want to force a link error if they try to log values here? */
#define __LOG(x,y)      /* undefined */
#endif


#if (__IT_FMT == __IT_FMT_RAW8)
/* do raw8 */
#endif

#if (__IT_FMT == __IT_FMT_RAW16)
/* do raw16 */
#endif

#if (__IT_FMT == __IT_FMT_RAW32)
/* do raw32 */
#endif

#endif  /* end port trace  */



/* SPI DEFAULT FORMAT */

#if (__IT_MEDIUM == __IT_SPI_MASK)
extern void __JOIN(__it_trace_,__IT_TRANSPORT)(unsigned int id);
extern void __JOIN(__it_log_,__IT_TRANSPORT)(unsigned int id, unsigned int type, ...);


#if (__IT_FMT == __IT_FMT_DFLT)

#define __TRACE(id)     __JOIN(__it_trace_,__IT_TRANSPORT)(\
                               __builtin_unique_id("__TRACE",id))
#define __LOG(id,value) __JOIN(__it_log_,__IT_TRANSPORT)( \
                               __builtin_unique_id("__LOG",id), \
                               __builtin_ittype(value), value)
#endif
#endif  /* end SPI trace */



/* DATA CAPTURE TRACE SUPPORT                                              */

#if (__IT_MEDIUM == __IT_XMT_DC)
extern void __it_log_dc_pc(unsigned int type, ...);
extern void __it_trace_dc(unsigned int id);
extern void __it_log_dc(unsigned int id, unsigned int type, ...);


#if (__IT_FMT == __IT_FMT_DFLT)
#define __TRACE(id)     __it_trace_dc(__builtin_unique_id("__TRACE",id))
#define __LOG(id,value) __it_log_dc(__builtin_unique_id("__LOG",id), \
                            __builtin_ittype(value), (value))
#endif

#if (__IT_FMT == __IT_FMT_PC)   // obsolete format not supported on PIC32
#define __TRACE(x)      __it_log_dc_pc(__IT_PC)
#define __LOG(x,value)  __it_log_dc_pc(__builtin_ittype(value), (value))
#endif

#endif  /* end data capture trace */


#endif  /* __IT_TRANSPORT && __DEBUG defined  */

#endif
