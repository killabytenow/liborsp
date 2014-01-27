#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libip.h>
#include "rspfd_fd.h"
#include "rspmsg.h"
#include "log.h"


//int gdbc_read_command(RSPFD *fd, RSPMSG *msg);
//int gdbc_process_command(RSPFD *fd, RSPMSG *m, int rle);

int main(int argc, char **argv)
{
	RSPFD_FD rspfd;
	RSPMSG m;
	int r, fd, sock;
	struct stat fdstat;
	INET_ADDR addr;
	int port;
	BIGSOCKADDR saddr;

	if(argc > 2)
		FAT("Too much parameters. You only need to specify a serial device or an IP address.");

	if(argc < 1)
		FAT("You need to specify a serial device or IP address.");

	if(!(fd = open(argv[1], O_RDONLY))) {
		if(!fstat(fd, &fdstat)) {
			/* check file is a FIFO or char device */
			if(!S_ISCHR(fdstat.st_mode) && !S_ISFIFO(fdstat.st_mode))
				FAT("File '%s' is not a FIFO nor character device.", argv[1]);

			rspfd_fd_init(&rspfd, fd);
		} else {
			FAT("Cannot stat file '%s': %s", argv[1], strerror(errno));
		}
	} else {
		/* check if it is an IP address */
		if(ip_addr_parse(argv[1], &addr, &port))
			FAT("Invalid address '%s'.", argv[1]);

		if(addr.type != INET_FAMILY_IPV4 && addr.type != INET_FAMILY_IPV6)
			FAT("Invalid address type (%s).", argv[1]);

		if(ip_addr_to_bigsockaddr(&addr, port, &saddr))
			FAT("Cannot convert address '%s' to socket: %s", argv[1], strerror(errno));
		if((sock = ip_bigsockaddr_get_socket(&saddr, SOCK_STREAM, 0)) < 0)
			FAT("Cannot create socket: %s", strerror(errno));

		if(connect(sock, &saddr.sa, saddr.size) < 0)
			FAT_ERRNO("connect() 2 failed");
	}

	/* configure fd for a server through STDIN */
	rspfd_fd_init(&rspfd, 0);
	rspfd_rle_read_enable((RSPFD *) &rspfd, 0);
	rspfd_rle_write_enable((RSPFD *) &rspfd, 1);

	while((r = rspmsg_command_parse(&rspfd, &m)) == 0) {
		if((r = rsp_command_process(&rspfd, &m)) != 0)
			FAT("Cannot process command (err=%d).", r);
	}

	rspfd_fd_fini(&rspfd);

	return 0;
}
