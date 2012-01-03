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

/* messages (for debugging and error purposes) */
#define __MSG(x, ...)         {                                           \
                                fprintf(stderr, "liborsp:%s: ", (x));     \
                                fprintf(stderr, __VA_ARGS__);             \
                              }
#define ERR(...)              { __MSG("error: ",   __VA_ARGS__);          }
#define WRN(...)              { __MSG("warning: ", __VA_ARGS__);          }
#define MSG(...)              { __MSG("",          __VA_ARGS__);          }
#define FAT(...)              { __MSG("fatal: ",   __VA_ARGS__); exit(1); }
#define ERR_ERRNO(f, ...)     {                                           \
                                char __ed[255];                           \
                                strerror_r(errno,  __ed, sizeof(__ed));   \
                                ERR(f ": %s", ## __VA_ARGS__, __ed);      \
                              }
#define FAT_ERRNO(f, ...)     {                                           \
                                char __ed[255];                           \
                                strerror_r(errno,  __ed, sizeof(__ed));   \
                                FAT(f ": %s", ## __VA_ARGS__, __ed);      \
                              }

#endif
