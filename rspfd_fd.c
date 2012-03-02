#include <unistd.h>
#include <errno.h>

#include "rspfd_fd.h"
#include "log.h"

/* following line avoids undesired interactions with putc(3) macros. */
#undef putc

//static
int __fd_getc(RSPFD_FD *fd, char *c)
{
  int r;

  while((r = read(fd->fd, c, sizeof(char))) < 0 && errno == EINTR)
    ;
  if(r < 0)
    FAT_ERRNO("cannot read input");

  return r;
}

//static
inline int __fd_putb(RSPFD_FD *fd, void *buf, int count)
{
  int r;

  while(count > 0)
  {
    while((r = write(fd->fd, buf, count)) == EINTR)
      ;
    if(r < 0)
      FAT_ERRNO("cannot write to fd %d", fd->fd);
    if(r != count)
      FAT("cannot write %d chars, only %d written.", count, r);
    buf += r;
    count -= r;
  }

  return r;
}

//static
int __fd_putc(RSPFD_FD *fd, char c)
{
  return __fd_putb(fd, &c, 1);
}

//static
int __fd_puts(RSPFD_FD *fd, char *s)
{
  return __fd_putb(fd, s, strlen(s));
}

int rspfd_fd_init(RSPFD_FD *fd, int f)
{
  if((fd->fd = dup(f)) < 0)
    FAT_ERRNO("cannot dup fd %d", f);
  fd->getc = __fd_getc;
  fd->putc = __fd_putc;
  fd->puts = __fd_puts;
  fd->putb = __fd_putb;
  return 0;
}

int rspfd_fd_fini(RSPFD_FD *fd)
{
  close(fd->fd);
  return 0;
}

