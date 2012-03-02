#include "client.h"
#include "rspfd_fd.h"

//int gdbc_read_command(RSPFD *fd, RSPMSG *msg);
//int gdbc_process_command(RSPFD *fd, RSPMSG *m, int rle);

int main(int argc, char **argv)
{
  RSPFD_FD fd;
  RSPMSG m;
  int r;

  /* configure fd for a server through STDIN */
  rspfd_fd_init(&fd, 0);
  rspfd_rle_read_enable((RSPFD *) &fd, 0);
  rspfd_rle_write_enable((RSPFD *) &fd, 1);

  while((r = rspmsg_command_parse(&fd, &m)) == 0)
  {
    if((r = rsp_command_process(&fd, &m)) != 0)
      FAT("Cannot process command (err=%d).", r);
  }

  rspfd_fd_fini(&fd);

  return 0;
}
