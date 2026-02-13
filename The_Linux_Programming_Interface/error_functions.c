/* error_functions.h */
void usageErr(const char *format, ...);

/* error_functions.c */
#include <stdarg.h>
#include "error_functions.h"
#include "tlpi_hdr.h"

void
usageErr(const char *format, ...)
{
    va_list argList;
    
    fflush(stdout);           /* Flush any pending stdout */
    
    fprintf(stderr, "Usage: ");
    va_start(argList, format);
    vfprintf(stderr, format, argList);
    va_end(argList);
    
    fflush(stderr);           /* In case stderr is not line-buffered */
    exit(EXIT_FAILURE);
}