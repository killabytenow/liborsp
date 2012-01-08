#include "client.h"
#include "rspfd_fd.h"

int gdbc_read_command(RSP_FD *fd, RSPMSG *msg)
int gdbc_process_command(RSP_FD *fd, RSPMSG *m, int rle)

int main(int argc, char **argv)
{
  RSPFD_FD fd;
  RSPMSG m;
  int r;

  rspfd_fd_init(&fd, 0);
  rspfd_rle_read_enable(&fd, 0);
  rspfd_rle_write_enable(&fd, 1);

  while((r = rspmsg_command_parse(&fd, &m)) == 0)
  {
    if((r = rsp_command_process(&fd, &m)) != 0)
      FAT("Cannot process command (err=%d).", r);
  }

  rspfd_fd_fini(&fd, 0);

  return 0;
}
