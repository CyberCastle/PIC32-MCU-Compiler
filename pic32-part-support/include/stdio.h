#ifndef	_STDIO_H_

#ifdef __cplusplus
extern "C"
  {
#endif

#define	_STDIO_H_

#if defined(__C30__) || defined (__C32_VERSION__)
#define _HOSTED 1
#define	BUFSIZ		32
#else
#define	BUFSIZ		512
#endif
#define	_NFILE		8

#ifndef	_STDDEF_H_
#include <stddef.h>
#endif /* _STDDEF_H_ */

#ifndef	_STDARG_H_
#include <stdarg.h>
#endif /* _STDARG_H_ */

#ifndef FILE

#if	_HOSTED
extern	struct	_iobuf {
	char *		_ptr;
	int		_cnt;
	char *		_base;
	unsigned short	_flag;
	short		_file;
	size_t		_size;
} _iob[_NFILE];

#define	FILE		struct _iobuf
    extern FILE	*_Files;

#define	L_tmpnam	81		/* max length of temporary names */
#define	_MAXTFILE	8		/* max number of temporary files */

#if	DOS
#define	FILENAME_MAX	81		/* max length of a pathname */
#define	FOPEN_MAX	5
#endif

    extern struct _tfiles
        {
          char	tname[L_tmpnam];
          FILE *	tfp;
        }	* _tfilesptr;

#define	_non_ems_sbrk(x)	sbrk(x)

#else	/* _HOSTED */

    struct __prbuf
      {
        char *		ptr;
        void (*		func)(char);
      };
#endif	/* _HOSTED */
#endif	/* FILE */

#if defined(__C30__) || defined(__C32_VERSION__)
#define _IONEVER	-1
#else
#define _IONEVER	0
#endif
#define	_IOFBF		0
#define	_IOREAD		01
#define	_IOWRT		02
#define	_IORW		03
#define	_IONBF		04
#define	_IOMYBUF	010
#define	_IOEOF		020
#define	_IOERR		040
#define	_IOSTRG		0100
#define	_IOBINARY	0200
#define	_IOLBF		0400
#define	_IODIRN		01000	/* true when file is in write mode */
#define	_IOAPPEND	02000	/* file was opened in append mode */
#define	_IOSEEKED	04000	/* a seek has occured since last write */
#define	_IOTMPFILE	010000	/* this file is a temporary */
#define _IOUPDATE       020000

#define _IO_STRALLOC 0x0200
#define _IO_TTY  0x1000
#define _IO_BINARY 0x8000

#define _io_testflag(f, flag) ((f)->_flag & (flag))
#define _io_setflag(f, flag) ((f)->_flag |= (flag))
#define _io_clearflag(f, flag) ((f)->_flag &= ~(flag))

#define	EOF		(-1)
#define	_IOSTRING	(-67)

#define	SEEK_SET	0
#define	SEEK_CUR	1
#define	SEEK_END	2
#define	TMP_MAX		255

#if	defined(_HOSTED)

#if 	defined(__C30__) || defined(__C32_VERSION__)
#define	stdin		(&_iob[0])
#define	stdout		(&_iob[1])
#define	stderr		(&_iob[2])
#ifdef	DOS
#define	stdprn		(&_iob[3])
#endif /* DOS */
#endif /* __C30__ || __C32_VERSION__ */

#define	getchar()	getc(stdin)
#define	putchar(x)	putc(x,stdout)

#else	/* _HOSTED */
#include	<conio.h>
#define	getchar()	getche()
#define	putchar(x)	putch(x)
    extern int	cprintf(char *, ...);
#if !defined(__C30__) & ~defined(__C32_VERSION__)
#pragma printf_check(cprintf)
#endif
#if	defined(_MPC_) && !defined(_PIC18) && !defined(__DSPICC__) && !defined(__PICCPRO__)
    extern void	_doprnt(char *, const register char *, ...);
#else
    extern int	_doprnt(struct __prbuf *, const register char *, register va_list);
#endif	/* _MPC_ */
#endif	/* _HOSTED */

    /* For compatibility with legacy libc */
#if defined(__C32_VERSION__)
#if !defined(PIC32_STARTER_KIT) && !defined(__APPIO_DEBUG) && !defined(_APPIO_DEBUG)
    extern int _mon_getc (int canblock);
    extern void _mon_putc (char c);
    extern void _mon_puts (const char * s);
    extern void _mon_write (const char * s, unsigned int count);
#endif
#endif

    /*	getc() and putc() must be functions for CP/M to allow the special
     *	handling of '\r', '\n' and '\032'. The same for MSDOS except that
     *	it at least knows the length of a file.
     */

#define	getc(p)		fgetc(p)
#define	putc(x,p)	fputc(x,p)

#define	feof(p)		(((p)->_flag&_IOEOF)!=0)
#define	ferror(p)	(((p)->_flag&_IOERR)!=0)
#define	fileno(p)	((unsigned short)p->_file)
#define	clrerr(p)	p->_flag &= ~_IOERR
#define	clreof(p)	p->_flag &= ~_IOEOF
#define	clearerr(p)	p->_flag &= ~(_IOERR|_IOEOF)

#if !defined(__STRICT_ANSI__) && (defined(__C30__) || defined (__C32_VERSION__))
#define define_smartio_variants(rettype,funcname,...) \
  extern rettype _ ## funcname ## _cdeEfFgGnopsuxX (__VA_ARGS__); \
  extern rettype _ ## funcname ## _cdeEfFgGnopuxX (__VA_ARGS__); \
  extern rettype _ ## funcname ## _cdeEfFnopsuxX (__VA_ARGS__); \
  extern rettype _ ## funcname ## _cdeEfFnopuxX (__VA_ARGS__); \
  extern rettype _ ## funcname ## _cdeEgGnopsuxX (__VA_ARGS__); \
  extern rettype _ ## funcname ## _cdeEgGnopuxX (__VA_ARGS__); \
  extern rettype _ ## funcname ## _cdeEnopsuxX (__VA_ARGS__); \
  extern rettype _ ## funcname ## _cdeEnopuxX (__VA_ARGS__); \
  extern rettype _ ## funcname ## _cdfFgGnopsuxX (__VA_ARGS__); \
  extern rettype _ ## funcname ## _cdfFgGnopuxX (__VA_ARGS__); \
  extern rettype _ ## funcname ## _cdfFnopsuxX (__VA_ARGS__); \
  extern rettype _ ## funcname ## _cdfFnopuxX (__VA_ARGS__); \
  extern rettype _ ## funcname ## _cdgGnopsuxX (__VA_ARGS__); \
  extern rettype _ ## funcname ## _cdgGnopuxX (__VA_ARGS__); \
  extern rettype _ ## funcname ## _cdnopsuxX (__VA_ARGS__); \
  extern rettype _ ## funcname ## _cdnopuxX (__VA_ARGS__); \
  extern rettype _ ## funcname ## _eE (__VA_ARGS__); \
  extern rettype _ ## funcname ## _eEfF (__VA_ARGS__); \
  extern rettype _ ## funcname ## _eEfFgG (__VA_ARGS__); \
  extern rettype _ ## funcname ## _eEfFgGs (__VA_ARGS__); \
  extern rettype _ ## funcname ## _eEfFs (__VA_ARGS__); \
  extern rettype _ ## funcname ## _eEgG (__VA_ARGS__); \
  extern rettype _ ## funcname ## _eEgGs (__VA_ARGS__); \
  extern rettype _ ## funcname ## _eEs (__VA_ARGS__); \
  extern rettype _ ## funcname ## _fF (__VA_ARGS__); \
  extern rettype _ ## funcname ## _fFgG (__VA_ARGS__); \
  extern rettype _ ## funcname ## _fFgGs (__VA_ARGS__); \
  extern rettype _ ## funcname ## _fFs (__VA_ARGS__); \
  extern rettype _ ## funcname ## _gG (__VA_ARGS__); \
  extern rettype _ ## funcname ## _gGs (__VA_ARGS__); \
  extern rettype _ ## funcname ## _s (__VA_ARGS__); \
  extern rettype _ ## funcname (__VA_ARGS__); \
  extern rettype _d ## funcname ## _cdeEfFgGnopsuxX (__VA_ARGS__); \
  extern rettype _d ## funcname ## _cdeEfFgGnopuxX (__VA_ARGS__); \
  extern rettype _d ## funcname ## _cdeEfFnopsuxX (__VA_ARGS__); \
  extern rettype _d ## funcname ## _cdeEfFnopuxX (__VA_ARGS__); \
  extern rettype _d ## funcname ## _cdeEgGnopsuxX (__VA_ARGS__); \
  extern rettype _d ## funcname ## _cdeEgGnopuxX (__VA_ARGS__); \
  extern rettype _d ## funcname ## _cdeEnopsuxX (__VA_ARGS__); \
  extern rettype _d ## funcname ## _cdeEnopuxX (__VA_ARGS__); \
  extern rettype _d ## funcname ## _cdfFgGnopsuxX (__VA_ARGS__); \
  extern rettype _d ## funcname ## _cdfFgGnopuxX (__VA_ARGS__); \
  extern rettype _d ## funcname ## _cdfFnopsuxX (__VA_ARGS__); \
  extern rettype _d ## funcname ## _cdfFnopuxX (__VA_ARGS__); \
  extern rettype _d ## funcname ## _cdgGnopsuxX (__VA_ARGS__); \
  extern rettype _d ## funcname ## _cdgGnopuxX (__VA_ARGS__); \
  extern rettype _d ## funcname ## _cdnopsuxX (__VA_ARGS__); \
  extern rettype _d ## funcname ## _cdnopuxX (__VA_ARGS__); \
  extern rettype _d ## funcname ## _eE (__VA_ARGS__); \
  extern rettype _d ## funcname ## _eEfF (__VA_ARGS__); \
  extern rettype _d ## funcname ## _eEfFgG (__VA_ARGS__); \
  extern rettype _d ## funcname ## _eEfFgGs (__VA_ARGS__); \
  extern rettype _d ## funcname ## _eEfFs (__VA_ARGS__); \
  extern rettype _d ## funcname ## _eEgG (__VA_ARGS__); \
  extern rettype _d ## funcname ## _eEgGs (__VA_ARGS__); \
  extern rettype _d ## funcname ## _eEs (__VA_ARGS__); \
  extern rettype _d ## funcname ## _fF (__VA_ARGS__); \
  extern rettype _d ## funcname ## _fFgG (__VA_ARGS__); \
  extern rettype _d ## funcname ## _fFgGs (__VA_ARGS__); \
  extern rettype _d ## funcname ## _fFs (__VA_ARGS__); \
  extern rettype _d ## funcname ## _gG (__VA_ARGS__); \
  extern rettype _d ## funcname ## _gGs (__VA_ARGS__); \
  extern rettype _d ## funcname ## _s (__VA_ARGS__); \
  extern rettype _d ## funcname (__VA_ARGS__)
#else
#define define_smartio_variants(rettype,funcname,...)
#endif

#if	_HOSTED
    extern int	_flsbuf(char, FILE *);
    extern int	_filbuf(FILE *);
    extern int	fclose(FILE *);
    extern int	fflush(FILE *);
    extern int	fgetc(FILE *);
    extern int	ungetc(int, FILE *);
    extern int	fputc(int, FILE *);
    extern int	getw(FILE *);
    extern int	putw(int, FILE *);
    extern int	fputs(const char *, FILE *);
    extern int	fread(void *, size_t, size_t, FILE *);
    extern size_t	fwrite(const void *, size_t, size_t, FILE *);
    extern int	fseek(FILE *, long, int);
    extern int	rewind(FILE *);
    extern void	setbuf(FILE *, char *);
    extern int	setvbuf(FILE *, char *, int, size_t);
    extern int	fprintf(FILE *, const char *, ...);
    define_smartio_variants(int,fprintf,FILE *, const char *, ...);
    extern int	fscanf(FILE *, const char *, ...);
    define_smartio_variants(int, fscanf, FILE *, const char *, ...);
    extern int	vfprintf(FILE *, const char *, va_list);
    define_smartio_variants(int,vfprintf,FILE *, const char *, va_list);
    extern int	vfscanf(FILE *, const char *, va_list);
    define_smartio_variants(int, vfscanf, FILE *, const char *, va_list);
    extern int	remove(const char *);
    extern int	rename(const char *, const char *);
    extern FILE *	fopen(const char *, const char *);
    extern FILE *	freopen(const char *, const char *, FILE *);
    extern FILE *	fdopen(int, const char *);
    extern long	ftell(FILE *);
    extern char *	fgets(char *, int, FILE *);
    extern void	perror(const char *);
    extern char*	_bufallo	(void);
    extern void	_buffree	(char *pp);
    extern char *	tmpnam(char *);
    extern FILE *	tmpfile(void);

#if	unix
    extern FILE *	popen(char *, char *);
    extern int	pclose(FILE *);
#endif
    extern void	(*_atexitptr)(void);

#endif	/* __HOSTED */

#if	defined(_MPC_) && !defined(_PIC18) && !defined(__DSPICC__) && !defined(__PICCPRO__)
#pragma	printf_check(fprintf)
#pragma	printf_check(vfprintf)

    extern int	_doscan(const char *, const char *, va_list);
#if 0
#define vprintf(s, l)		_doprnt(0, (s), (l))
#define vsprintf(b, s, l)	_doprnt((b), (s), (l))
#define vscanf(s, l)		_doscan(0, (s), (l))
#define vsscanf(b, s, l)	_doscan((b), (s), (l))
#endif
#pragma	printf_check(printf) const
#pragma	printf_check(sprintf) const
#pragma	printf_check(vsprintf) const

#if defined(_PIC16)
    extern unsigned char 	sprintf(far char *, const char *, ...);
#else	/* _PIC16 */
    extern unsigned char 	sprintf(char *, const char *, ...);
#endif
#if	defined(_PIC18)
    extern int	printf(const char *, ...);
#else
    extern unsigned char	printf(const char *, ...);
#endif
#else /* _MPC_ */

    extern char *	gets(char *);
    extern int	puts(const char *);

    extern int	scanf(const char *, ...);
    define_smartio_variants(int,scanf,const char *, ...);
    extern int	sscanf(const char *, const char *, ...);
    define_smartio_variants(int,sscanf,const char *, const char *, ...);

    extern int 	vprintf(const char *, va_list);
    define_smartio_variants(int,vprintf,const char *, va_list);
    extern int	vsprintf(char *, const char *, va_list);
    define_smartio_variants(int,vsprintf,char *, const char *, va_list);
    extern int	vscanf(const char *, va_list ap);
    define_smartio_variants(int,vscanf,const char *, va_list ap);
    extern int	vsscanf(const char *, const char *, va_list);
    define_smartio_variants(int,vsscanf,const char *, const char *, va_list);

#if !defined(_POSIX_SOURCE) && !defined(_ANSI_SOURCE)
    extern int snprintf (char *, size_t, const char *, ...);
    define_smartio_variants(int,snprintf,char *, size_t, const char *, ...);
    extern int vsnprintf (char *, size_t, const char *, __va_list);
    define_smartio_variants(int,vsnprintf,char *, size_t, const char *, __va_list);
    extern int asprintf (char **, const char *, ...);
    define_smartio_variants(int,asprintf,char **, const char *, ...);
    extern int vasprintf (char **, const char *, __va_list);
    define_smartio_variants(int,vasprintf,char **, const char *, __va_list);
    extern int __vasprintf (char **, const char *, __va_list);
    define_smartio_variants(int,__vasprintf,char **, const char *, __va_list);
#endif

#if !defined(__C30__) && !defined(__C32_VERSION__)
#pragma	printf_check(printf) const
#pragma	printf_check(sprintf) const
#endif

    extern int sprintf(char *, const char *, ...);
    define_smartio_variants(int,sprintf,char *, const char *, ...);
    extern int printf(const char *, ...);
    define_smartio_variants(int,printf,const char *, ...);

#if !defined(__LIBBUILD__) && defined(__DBL_MANT_DIG__) && defined(__FLT_MANT_DIG__)
#if (__DBL_MANT_DIG__ != __FLT_MANT_DIG__)
#if !defined(__STRICT_ANSI__)
#define fprintf(file,fmt,a...) _dfprintf_cdeEfFgGnopsuxX(file,fmt,##a)
#define sprintf(stream,fmt,a...) _dsprintf_cdeEfFgGnopsuxX(stream,fmt,##a)
#define vprintf(fmt,arglist) _dvprintf_cdeEfFgGnopsuxX(fmt,arglist)
#define vsprintf(stream,fmt,arglist) _dvsprintf_cdeEfFgGnopsuxX(stream,fmt,arglist)
#define sprintf(stream,fmt,a...) _dsprintf_cdeEfFgGnopsuxX(stream,fmt,##a)
#define printf(fmt,a...) _dprintf_cdeEfFgGnopsuxX(fmt,##a)
#if !defined(_POSIX_SOURCE) && !defined(_ANSI_SOURCE)
#define snprintf(stream,sizeval,fmt,a...) _dsnprintf_cdeEfFgGnopsuxX(stream,sizeval,fmt,##a)
#define vsnprintf(stream,sizeval,fmt,arglist) _dvsnprintf_cdeEfFgGnopsuxX(stream,sizeval,fmt,arglist)
#define vasprintf(stream,fmt,arglist) _dvasprintf_cdeEfFgGnopsuxX(stream,fmt,arglist)
#define __vasprintf(stream,fmt,arglist) _d__vasprintf_cdeEfFgGnopsuxX(stream,fmt,arglist)
#define asprintf(stream,fmt,a...) _dasprintf_cdeEfFgGnopsuxX(stream,fmt,##a)
#endif /* _POSIX_SOURCE  */
#define scanf(fmt,a...) _dscanf_cdeEfFgGnopsuxX(fmt,##a)
#define sscanf(instring,fmt,a...) _dsscanf_cdeEfFgGnopsuxX(instring,fmt,##a)
#define vsscanf(instring,fmt,arglist) _dvsscanf_cdeEfFgGnopsuxX(instring,fmt,arglist)
#define vscanf(fmt,arglist) _dvscanf_cdeEfFgGnopsuxX(fmt,arglist)

#else /* __STRICT_ANSI__ */

#define fprintf _dfprintf_cdeEfFgGnopsuxX
#define sprintf _dsprintf_cdeEfFgGnopsuxX
#define vprintf _dvprintf_cdeEfFgGnopsuxX
#define vsprintf _dvsprintf_cdeEfFgGnopsuxX
#define sprintf _dsprintf_cdeEfFgGnopsuxX
#define printf _dprintf_cdeEfFgGnopsuxX
#if !defined(_POSIX_SOURCE) && !defined(_ANSI_SOURCE)
#define snprintf _dsnprintf_cdeEfFgGnopsuxX
#define vsnprintf _dvsnprintf_cdeEfFgGnopsuxX
#define vasprintf _dvasprintf_cdeEfFgGnopsuxX
#define __vasprintf _d__vasprintf_cdeEfFgGnopsuxX
#define asprintf _dasprintf_cdeEfFgGnopsuxX
#endif /* _POSIX_SOURCE  */
#define scanf _dscanf_cdeEfFgGnopsuxX
#define sscanf _dsscanf_cdeEfFgGnopsuxX
#define vsscanf _dvsscanf_cdeEfFgGnopsuxX
#define vscanf _dvscanf_cdeEfFgGnopsuxX
#endif /* __STRICT_ANSI__ */
#endif /* (__DBL_MANT_DIG != __FLT_MANT_DIG__) */
#endif /* __LIBBUILD__ */


#endif	/* _MPC_ */
#ifdef __cplusplus
  }
#endif
#endif	/* _STDIO_H_ */

