/*****************************************************************************
 * stacktrace.h
 *
 * This file is used to link a normal program with the stacktrace library.
 *
 * ---------------------------------------------------------------------------
 * stacktrace - Stacktrace printer.
 *   (C) 2008-2011 Gerardo García Peña <killabytenow@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify it
 *   under the terms of the GNU General Public License as published by the Free
 *   Software Foundation; either version 2 of the License, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful, but WITHOUT
 *   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *   more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc., 51
 *   Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/

#ifndef __LOG_H__
#define __LOG_H__

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* messages (for debugging and error purposes) */
#define LOGFUNC(F, L, P, X) \
          static inline void F(char *f, ...)                \
            __attribute__ ((__format__ (__printf__, 1, 2)));\
          static inline void F(char *f, ...)                \
          {                                                 \
            P                                               \
            va_list ap;                                     \
            va_start(ap, f);                                \
            fputs("liborsp:" L, stderr);                    \
            vfprintf(stderr, f, ap);                        \
            va_end(ap);                                     \
            X                                               \
          }
LOGFUNC(ERR, "error:",   , fputc('\n', stderr); )
LOGFUNC(WRN, "warning:", , fputc('\n', stderr); )
LOGFUNC(MSG, "",         , fputc('\n', stderr); )
LOGFUNC(FAT, "fatal: ",  , fputc('\n', stderr); exit(1);)
LOGFUNC(ERR_ERRNO, "error:",                              \
          char e[255]; strerror_r(errno,  e, sizeof(e));, \
          fputs(e, stderr); )
LOGFUNC(FAT_ERRNO, "fatal:",                              \
          char e[255]; strerror_r(errno,  e, sizeof(e));, \
          fputs(e, stderr); exit(1); )
#undef LOGFUNC

#endif
