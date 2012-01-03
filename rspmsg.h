#ifndef __RSPMSG_H__
#define __RSPMSG_H__

#include "buffer.h"
#include "rspfd.h"

typedef struct _tag_RSPMSG {
  int       interrupt;
  int       command;

  /* request */
  int       q_encoding1;
  long      q_length;
  long      q_thread_id;
  char      q_thread_purpose;
  long      q_addr;
  int       q_separator2;
  int       q_encoding2;
  union {
    BUFFER  q_data1;
    BUFFER  q_regname;
    BUFFER  q_regs;
  };
  union {
    BUFFER  q_data2;
    BUFFER  q_signal;
    BUFFER  q_value;
  };

  /* response */
  int       a_oke;               /* 0 == OK, 0 != Enn */
  union {
    BUFFER  a_regs;
  };

  /* scratch buffer */
  BUFFER    raw;
} RSPMSG;

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

int orsp_read_msg(RSP_FD *fd, RSPMSG *m);

#endif
