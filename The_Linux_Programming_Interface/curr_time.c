/* curr_time.c

   Implement our currTime() function.
*/
#include <time.h>
#include <sys/time.h>
#include "curr_time.h"          /* Declares function defined here */

#define BUF_SIZE 1000

/* Return a string containing the current time formatted according to
   'fmt' (a strftime() format specification). Return NULL on error.
   The returned string is statically allocated, and hence will be overwritten
   by each call. */

char *
currTime(const char *fmt)
{
    static char buf[BUF_SIZE];  /* Nonreentrant */
    time_t t;
    size_t s;
    struct tm *tm;

    t = time(NULL);
    tm = localtime(&t);
    if (tm == NULL)
        return NULL;

    s = strftime(buf, BUF_SIZE, (fmt != NULL) ? fmt : "%c", tm);

    return (s == 0) ? NULL : buf;
}