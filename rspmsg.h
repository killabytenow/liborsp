#ifndef __RSPMSG_H__
#define __RSPMSG_H__

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
} RSPMSG;

#endif
