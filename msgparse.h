#ifndef __MSGPARSE_H__
#define __MSGPARSE_H__

#include "msgio.h"

typedef struct _tag_GDBHANDLER {
  int command;
  int (*handler)(RSPMSG *m);
  struct _tag_GDBHANDLER *next;
} GDBHANDLER;
GDBHANDLER *gdbc_handlers;

void rspmsg_reset(RSPMSG *c);
int rspmsg_command_parse(RSPFD *fd, RSPMSG *c);

#endif
