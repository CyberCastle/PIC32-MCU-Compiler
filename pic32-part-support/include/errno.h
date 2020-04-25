
/*
 * errno.h : error number definitions
 */


#ifndef __ERRNO_H
#define __ERRNO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/errno.h>

#if defined(__XC32__)
extern __attribute__((section(".bss.errno"))) int errno;
#else
extern int	errno;			/* system error number */
#endif

#ifdef __cplusplus
}
#endif

#endif /* !defined __ERRNO_H */

