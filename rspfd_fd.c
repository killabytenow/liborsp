#include <unistd.h>
#include <errno.h>

#include "rspfd_fd.h"
#include "log.h"

/* following line avoids undesired interactions with putc(3) macros. */
#undef putc

//static
int __fd_getc(RSPFD *fd, char *c)
{
  int r;

  while((r = read(((RSPFD_FD *) fd)->fd, c, sizeof(char))) < 0 && errno == EINTR)
    ;
  if(r < 0)
    FAT_ERRNO("cannot read input");

  return r;
}

//static
inline int __fd_putb(RSPFD *fd, void *buf, int count)
{
  int r;

  while(count > 0)
  {
    while((r = write(((RSPFD_FD *) fd)->fd, buf, count)) == EINTR)
      ;
    if(r < 0)
      FAT_ERRNO("cannot write to fd %d", ((RSPFD_FD *) fd)->fd);
    if(r != count)
      FAT("cannot write %d chars, only %d written.", count, r);
    buf += r;
    count -= r;
  }

  return r;
}

//static
int __fd_putc(RSPFD *fd, char c)
{
  return __fd_putb(fd, &c, 1);
}

//static
int __fd_puts(RSPFD *fd, char *s)
{
  return __fd_putb(fd, s, strlen(s));
}

int rspfd_fd_init(RSPFD_FD *fd, int f)
{
  if((fd->fd = dup(f)) < 0)
    FAT_ERRNO("cannot dup fd %d", f);
  fd->rspdf.getc = __fd_getc;
  fd->rspdf.putc = __fd_putc;
  fd->rspdf.puts = __fd_puts;
  fd->rspdf.putb = __fd_putb;
  return 0;
}

int rspfd_fd_fini(RSPFD_FD *fd)
{
  close(fd->fd);
  return 0;
}

