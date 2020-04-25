#include <errno.h>

char __attribute__((section(".gnu.warning.open")))
__warning_open[] = "Linking stub open(). Hint: Provide an app-specific implementation of int open (const char *buf, int flags, int mode)";

/*
 * open -- open a file descriptor. We don't have a filesystem, so
 *         we return an error.
 */
int
open (const char *buf,
       int flags,
       int mode)
{
  errno = EIO;
  return (-1);
}
