#ifndef __MSGIO_H__
#define __MSGIO_H__

#include "buffer.h"
#include "rspfd.h"
#include "rspmsg.h"

#define GDBC_OK             0x0000
#define GDBC_EOF            0x8000

#define GDBC_PARSER_OK      GDBC_OK
#define GDBC_PARSER_EOF     GDBC_EOF
#define GDBC_PARSER_BADSYN  0x0002
#define GDBC_PARSER_BADCHK  0x0003

#define GDBC_CMD_OK         GDBC_OK
#define GDBC_CMD_DEPRECATED 0x0004
#define GDBC_CMD_BADPARAMS  0x0005
#define GDBC_CMD_RESERVED   0x0007
#define GDBC_CMD_UNKNOWN    0x0006
#define GDBC_CMD_NOMORE     GDBC_EOF

int orsp_msg_io_read(RSPFD *fd, RSPMSG *m);
int orsp_msg_io_write(RSPFD *fd, RSPMSG *m);

#endif
