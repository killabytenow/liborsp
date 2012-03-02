#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define __STACKTRACE_MSG_MACROS__
#include "decoding.h"
#include "encoding.h"
#include "rspfd.h"
#include "msgio.h"
#include "msgparse.h"

/*****************************************************************************
 * Message parser and writer
 *****************************************************************************/

void rspmsg_reset(RSPMSG *c)
{
  /* TODO XXX TODO */
}

/*****************************************************************************
 * (SERVER) Command parser
 *****************************************************************************/

