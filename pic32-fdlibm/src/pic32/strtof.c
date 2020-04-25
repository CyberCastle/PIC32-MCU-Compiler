#include <stdlib.h>

float strtof (const char *string, char **endPtr)
{
  return (float) strtod (string, endPtr);
}
 
