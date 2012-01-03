#include <unistd.h>
#include <errno.h>

#include "log.h"
#include "rspfd.h"

/* following line avoids undesired interactions with putc(3) macros. */
#undef putc

static int __fd_getc(RSP_FD *fd, char *c)
{
  int r;

  while((r = read(fd->fd, c, sizeof(char))) < 0 && errno == EINTR)
    ;
  if(r < 0)
    FAT_ERRNO("cannot read input");

  return r;
}

static inline int __fd_putb(RSP_FD *fd, void *buf, int count)
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

static int __fd_putc(RSP_FD *fd, char c)
{
  return __fd_putb(fd, &c, 1);
}

static int __fd_puts(RSP_FD *fd, char *s)
{
  return __fd_putb(fd, s, strlen(s));
}

int rspfd_init_fd(RSP_FD *fd, int f)
{
  fd->fd   = f;
  fd->getc = __fd_getc;
  fd->putc = __fd_putc;
  fd->puts = __fd_puts;
  fd->putb = __fd_putb;
  return 0;
}

void rspfd_rle_write_enable(RSP_FD *fd, int enable)
{
  fd->rle_encoding = enable;
}

void rspfd_rle_read_enable(RSP_FD *fd, int enable)
{
  fd->rle_decoding = enable;
}

