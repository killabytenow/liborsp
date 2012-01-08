#ifndef __RSPMSG_H__
#define __RSPMSG_H__

#include "buffer.h"

typedef struct _tag_RSPMSG {
  /* envelope */
  /*   following data comes with the message, but outside of the message */
  /*   payload -- if somebody pointed to my head with a gun asking me to */
  /*   name them, I would call them "headers", but inside me I'd know it */
  /*   would be a lie.                                                   */
  int       type;
#define RSPMSG_TYPE_INVALID -1
#define RSPMSG_TYPE_EOF      0
#define RSPMSG_TYPE_RET      1
#define RSPMSG_TYPE_ACK      2
#define RSPMSG_TYPE_INT      3
#define RSPMSG_TYPE_MSG      4
  int       seq_id;
  int       seq_id_enabled;
  int       checksum;

  /* interrupt data */
  int       interrupt;

  /* command data */
  int       command;
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
} RSPMSG;

#endif
