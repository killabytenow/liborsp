#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "rspmsg.h"

typedef struct _tag_GDBHANDLER {
  int command;
  int (*handler)(RSPMSG *m);
  struct _tag_GDBHANDLER *next;
} GDBHANDLER;
GDBHANDLER *gdbc_handlers;

int gdbc_read_command(RSP_FD *fd, RSPMSG *msg);
int gdbc_process_command(RSP_FD *fd, RSPMSG *m);

#endif
