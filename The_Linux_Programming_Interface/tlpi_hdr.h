/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2020.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                   *
\*************************************************************************/

/* tlpi_hdr.h

   Standard header file used by nearly all of our example programs.
*/
#ifndef TLPI_HDR_H
#define TLPI_HDR_H      /* Prevent accidental double inclusion */

#include <sys/types.h>  /* Type definitions used by many programs */
#include <stdio.h>      /* Standard I/O functions */
#include <stdlib.h>     /* Prototypes of commonly used library functions,
                           plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>     /* Prototypes for many system calls */
#include <errno.h>      /* Declares errno and defines error constants */
#include <string.h>     /* Commonly used string-handling functions */

#include "get_num.h"    /* Declares our functions for handling numeric
                           arguments (getInt(), getLong()) */

#include "error_functions.h"  /* Declares our error-handling functions */

/* Unfortunately some UNIX implementations define FALSE and TRUE -
   here we'll undefine them */

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

typedef enum { FALSE, TRUE } Boolean;

#define min(m,n) ((m) < (n) ? (m) : (n))
#define max(m,n) ((m) > (n) ? (m) : (n))

/* Some systems don't define 'socklen_t' */
#if defined(__sgi)
typedef int socklen_t;
#endif

#if defined(__sun)
#include <sys/file.h>           /* Has definition of FASYNC */
#endif

/* Following shortens the the statements that handle sa_handler
   (the same as sa_sigaction) */

#if ! defined(__sgi)    /* Not required for IRIX? */
#define SigAction(_signo, _act, _oact) \
    if (sigaction(_signo, _act, _oact) == -1) \
        errExit("sigaction")
#else
#define SigAction(_signo, _act, _oact) \
    if (sigaction(_signo, _act, _oact) == -1) \
        errExit("sigaction")
#endif

#endif
/* 
    fork():向内存申请子进程，并且拷贝父进程的数据，与父进程同步。
    vfork():占用父进程内存，有和父进程异步。
    _exit():系统调用。
    exit():库函数。
*/