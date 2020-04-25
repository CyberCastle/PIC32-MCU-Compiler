/*
 *	Low-level console I/O functions
 */

#ifndef _CONIO_H

#ifdef __cplusplus
extern "C" {
#endif

#define _CONIO_H

#ifndef	_STDDEF
#include <stddef.h>
#endif	/* _STDDEF */

#if	!_HOSTED
extern void	init_uart(void);
#endif
extern char	getch(void);
extern char	getche(void);
extern void	putch(char);
extern void	ungetch(char);
#if defined(_MPC_) && !defined(__DSPICC__)
extern bit	kbhit(void);
#else
extern int	kbhit(void);
#endif
extern char *	cgets(char *);
extern void	cputs(const char *);

#ifdef __cplusplus
}
#endif

#endif
