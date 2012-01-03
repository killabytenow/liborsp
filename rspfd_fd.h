#ifndef __RSPFD_FD_H__
#define __RSPFD_FD_H__

#include "rspfd.h"

typedef struct __tag_RSPFD_FD {
  RSPFD rspfd;
  int   fd;
} RSPFD_FD;

int rspfd_fd_init(RSPFD_FD *fd, int f);
int rspfd_fd_fini(RSPFD_FD *fd);

#endif
