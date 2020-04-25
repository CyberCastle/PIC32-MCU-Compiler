#ifndef _UNIXIO_H_
#define _UNIXIO_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *	Declarations for Unix style low-level I/O functions.
 */

#ifndef _STDDEF_H_
#include <stddef.h>
#endif /* _STDDEF_H_ */

extern int	errno;			/* system error number */

#if 0
extern int	open(const char *, int, ...);
#endif
extern int open(const char *, unsigned int, unsigned int);
extern int	close(int);
extern long	lseek(int, long, int);
extern size_t	read(int, void *, size_t);
extern size_t	write(int, const void *, size_t);
extern int creat(const char *, int);

#ifdef __cplusplus
}
#endif


#endif /* _UNIXIO_H_ */

