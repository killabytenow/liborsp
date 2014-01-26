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
	int r, fd;
	struct stat fdstat;
	INET_ADDR addr;
	int port;
	struct sockaddr *saddr;

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

		/* XXX: libip API cannot say type of sockaddr */
		if((saddr = ip_addr_get_bigsockaddr(&addr, port)) == NULL)
			FAT("Cannot convert address '%s' to socket: %s", argv[1], strerror(errno));

		/* XXX: libip API cannot create correct SOCK */
		if((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
			FAT("Cannot create socket: %s", strerror(errno));

		/* XXX: bad sockaddr size */
		if(connect(sock, saddr, sizeof(struct sockaddr_in)) < 0) {
			/* XXX: Se puede llegar aqui porque aún no ha conectado :) */
			TERR_ERRNO("connect() 2 failed");
			close(tt->sock);
			continue;
		}
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
