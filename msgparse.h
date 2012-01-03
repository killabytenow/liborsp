#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "msgio.h"

typedef struct _tag_GDBHANDLER {
  int command;
  int (*handler)(RSPMSG *m);
  struct _tag_GDBHANDLER *next;
} GDBHANDLER;
GDBHANDLER *gdbc_handlers;

int gdbc_read_command(RSPFD *fd, RSPMSG *msg);
int gdbc_process_command(RSPFD *fd, RSPMSG *m);

#endif
