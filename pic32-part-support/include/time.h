#ifndef	_TIME_H_
#define _TIME_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _STDDEF_H_
#include <stddef.h>
#endif /* _STDDEF_H_ */

typedef	unsigned long	time_t;		/* for representing times in seconds */
typedef	long	clock_t;
struct tm {
	int	tm_sec;
	int	tm_min;
	int	tm_hour;
	int	tm_mday;
	int	tm_mon;
	int	tm_year;
	int	tm_wday;
	int	tm_yday;
	int	tm_isdst;
};

#define	CLOCKS_PER_SEC	1
#define	difftime(t1, t0)	((long double)((time_t)(t1)-(time_t)(t0)))

extern int	time_zone;	/* minutes WESTWARD of Greenwich */
				/* this value defaults to 0 since with
				   operating systems like MS-DOS there is
				   no time zone information available */

extern clock_t clock(void);
extern time_t	time(time_t *);	/* seconds since 00:00:00 Jan 1 1970 */
extern int	stime(time_t *);	/* set time */
extern char *	asctime(const struct tm *);	/* converts struct tm to ascii time */
extern char *	ctime(const time_t *);	/* current local time in ascii form */
extern struct tm *	gmtime(const time_t *);	/* Universal time */
extern struct tm *	localtime(const time_t *);	/* local time */
extern time_t		mktime(struct tm *);
extern size_t		strftime(char *, size_t, const char *, const struct tm *);

typedef struct __tzrule_struct
{
  char ch;
  int m;
  int n;
  int d;
  int s;
  time_t change;
  long offset; /* Match type of _timezone. */
} __tzrule_type;

typedef struct __tzinfo_struct
{
  int __tznorth;
  int __tzyear;
  __tzrule_type __tzrule[2];
} __tzinfo_type;

__tzinfo_type *__gettzinfo (void);

/* defines for the opengroup specifications Derived from Issue 1 of the SVID.  */
extern long _timezone;
extern int _daylight;
extern char *_tzname[2];

#ifdef __cplusplus
}
#endif


#endif /* _TIME_H_ */

