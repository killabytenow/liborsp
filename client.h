#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "rspmsg.h"
#include "rspfd.h"

int gdbc_read_command(RSPFD *fd, RSPMSG *msg);
int gdbc_process_command(RSPFD *fd, RSPMSG *m);

#endif
