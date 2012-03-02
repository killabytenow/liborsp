#ifndef __RSPMSG_H__
#define __RSPMSG_H__

#include "buffer.h"

#define RSPMSG_TYPE_NONE             ( 0x000 )
#define RSPMSG_TYPE_VOID             ( 0x100 )
#define RSPMSG_TYPE_NOT_PARSED_YET   ( 0x200 )
#define RSPMSG_TYPE_EOF              ( 0xf00 )

#define RSPMSG_TYPE_CMD_MASK          0x1000
#define RSPMSG_TYPE_CMD_INTERRUPT     (RSPMSG_TYPE_CMD_MASK | 0)
#define RSPMSG_TYPE_CMD_EXT_MODE      (RSPMSG_TYPE_CMD_MASK | '!')
#define RSPMSG_TYPE_CMD_LAST_SIGNAL   (RSPMSG_TYPE_CMD_MASK | '?')
#define RSPMSG_TYPE_CMD_DETACH        (RSPMSG_TYPE_CMD_MASK | 'D')
#define RSPMSG_TYPE_CMD_READ_REGS     (RSPMSG_TYPE_CMD_MASK | 'g')
#define RSPMSG_TYPE_CMD_KILL          (RSPMSG_TYPE_CMD_MASK | 'k')
#define RSPMSG_TYPE_CMD_CONTINUE      (RSPMSG_TYPE_CMD_MASK | 'c')
#define RSPMSG_TYPE_CMD_WRITE_REGS    (RSPMSG_TYPE_CMD_MASK | 'G')
#define RSPMSG_TYPE_CMD_READ_REG      (RSPMSG_TYPE_CMD_MASK | 'p')
#define RSPMSG_TYPE_CMD_WRITE_REG     (RSPMSG_TYPE_CMD_MASK | 'P')
#define RSPMSG_TYPE_CMD_CONT_SIGNAL   (RSPMSG_TYPE_CMD_MASK | 'C')
#define RSPMSG_TYPE_CMD_REMOTE_START  (RSPMSG_TYPE_CMD_MASK | 'R')
#define RSPMSG_TYPE_CMD_SET_THREAD    (RSPMSG_TYPE_CMD_MASK | 'H')
#define RSPMSG_TYPE_CMD_READ_MEMORY   (RSPMSG_TYPE_CMD_MASK | 'm')
#define RSPMSG_TYPE_CMD_WRITE_MEMORY  (RSPMSG_TYPE_CMD_MASK | 'M')

#define RSPMSG_TYPE_RPL_MASK       0x2000
#define RSPMSG_TYPE_RPL_VOID       (RSPMSG_TYPE_RPL_MASK | RSPMSG_TYPE_VOID)
#define RSPMSG_TYPE_RPL_NONE       (RSPMSG_TYPE_RPL_MASK | RSPMSG_TYPE_NONE)
#define RSPMSG_TYPE_RPL_RET        (RSPMSG_TYPE_RPL_MASK | 0x10)
#define RSPMSG_TYPE_RPL_ACK        (RSPMSG_TYPE_RPL_MASK | 0x11)
#define RSPMSG_TYPE_RPL_CMD        (RSPMSG_TYPE_RPL_MASK | 0x12)
#define RSPMSG_TYPE_RPL_OKE        (RSPMSG_TYPE_RPL_MASK | 0x13)
#define RSPMSG_TYPE_RPL_RAW        (RSPMSG_TYPE_RPL_MASK | 0x14)

#define RSPMSG_TYPE_ERR_MASK       0x8000
#define RSPMSG_TYPE_ERR_INVALID    (RSPMSG_TYPE_ERR_MASK | 0x10)
#define RSPMSG_TYPE_ERR_IO         (RSPMSG_TYPE_ERR_MASK | 0x11)
#define RSPMSG_TYPE_ERR_BADCHK     (RSPMSG_TYPE_ERR_MASK | 0x12)
#define RSPMSG_TYPE_ERR_BADSYN     (RSPMSG_TYPE_ERR_MASK | 0x13)
#define RSPMSG_TYPE_ERR_DEPRECATED (RSPMSG_TYPE_ERR_MASK | 0x14)
#define RSPMSG_TYPE_ERR_RESERVED   (RSPMSG_TYPE_ERR_MASK | 0x15)
#define RSPMSG_TYPE_ERR_UNKNOWN    (RSPMSG_TYPE_ERR_MASK | 0x16)

#define RSPMSG_IS_CMD(x)           (RSPMSG_TYPE_CMD_MASK & (x))
#define RSPMSG_IS_RPL(x)           (RSPMSG_TYPE_RPL_MASK & (x))
#define RSPMSG_IS_ERR(x)           (RSPMSG_TYPE_ERR_MASK & (x))

typedef struct _tag_RSPMSG_RESERVED {
  BUFFER    __b_reserved_1;
  BUFFER    __b_reserved_2;
  BUFFER    __b_reserved_3;
  BUFFER    __b_reserved_4;
} RSPMSG_RESERVED;
#define RSPMSG_STRUCT_BEGIN(m)      \
  typedef struct _tag_RSPMSG_##m m; \
  struct _tag_RSPMSG_##m {
#define RSPMSG_STRUCT_BUFFER_0      \
    union {                         \
      RSPMSG_RESERVED;              \
    };
#define RSPMSG_STRUCT_BUFFER_0      \
    union {                         \
      RSPMSG_RESERVED;              \
    };
#define RSPMSG_STRUCT_BEGIN(m)      \
#define RSPMSG_STRUCT_END           \
  };

typedef struct _tag_RSPMSG_CMD_INTERRUPT RSPMSG_CMD_INTERRUPT;
struct _tag_RSPMSG_CMD_INTERRUPT {
  RSPMSG_RESERVED;
  int    code;
};
typedef struct _tag_RSPMSG_CMD_EXT_MODE {
  RSPMSG_RESERVED;
} RSPMSG_CMD_EXT_MODE;
typedef struct _tag_RSPMSG_CMD_LAST_SIGNAL {
  RSPMSG_RESERVED;
} RSPMSG_CMD_LAST_SIGNAL;
typedef struct _tag_RSPMSG_CMD_DETACH {
  RSPMSG_RESERVED;
} RSPMSG_CMD_DETACH;
typedef struct _tag_RSPMSG_CMD_READ_REGS {
  RSPMSG_RESERVED;
} RSPMSG_CMD_READ_REGS;
typedef struct _tag_RSPMSG_CMD_KILL {
  RSPMSG_RESERVED;
} RSPMSG_CMD_KILL;
typedef struct _tag_RSPMSG_CMD_CONTINUE {
  RSPMSG_RESERVED;
} RSPMSG_CMD_CONTINUE;
typedef struct _tag_RSPMSG_CMD_WRITE_REGS {
  RSPMSG_RESERVED;
} RSPMSG_CMD_WRITE_REGS;
typedef struct _tag_RSPMSG_CMD_READ_REG {
  RSPMSG_RESERVED;
} RSPMSG_CMD_READ_REG;
typedef struct _tag_RSPMSG_CMD_WRITE_REG {
  RSPMSG_RESERVED;
} RSPMSG_CMD_WRITE_REG;
typedef struct _tag_RSPMSG_TYPE_CMD_CONT_SIGNAL {
  RSPMSG_RESERVED;
} RSPMSG_CMD_CONT_SIGNAL;
typedef struct _tag_RSPMSG_CMD_REMOTE_START {
  RSPMSG_RESERVED;
} RSPMSG_CMD_REMOTE_START;
typedef struct _tag_RSPMSG_CMD_SET_THREAD {
  RSPMSG_RESERVED;
} RSPMSG_CMD_SET_THREAD;
typedef struct _tag_RSPMSG_CMD_READ_MEMORY {
  RSPMSG_RESERVED;
/* todo */
} RSPMSG_CMD_READ_MEMORY;
typedef struct _tag_RSPMSG_CMD_WRITE_MEMORY {
  RSPMSG_RESERVED;
/* todo */
} RSPMSG_CMD_WRITE_MEMORY;

typedef struct _tag_RSPMSG {
  /* envelope */
  /*   following data comes with the message, but outside of the message */
  /*   payload -- if somebody pointed to my head with a gun asking me to */
  /*   name them, I would call them "headers", but inside me I'd know it */
  /*   would be a lie.                                                   */
  int           type;
  int           seq_id;
  int           seq_id_enabled;
  int           checksum;

  /* ------------------------- */
  union {
    RSPMSG_CMD_INTERRUPT    interrupt;    /* RSPMSG_TYPE_CMD_INTERRUPT    */
    RSPMSG_CMD_EXT_MODE     ext_mode;     /* RSPMSG_TYPE_CMD_EXT_MODE     */
    RSPMSG_CMD_LAST_SIGNAL  last_signal;  /* RSPMSG_TYPE_CMD_LAST_SIGNAL  */
    RSPMSG_CMD_DETACH       detach;       /* RSPMSG_TYPE_CMD_DETACH       */
    RSPMSG_CMD_READ_REGS    read_regs;    /* RSPMSG_TYPE_CMD_READ_REGS    */
    RSPMSG_CMD_KILL         kill;         /* RSPMSG_TYPE_CMD_KILL         */
    RSPMSG_CMD_CONTINUE     cont;         /* RSPMSG_TYPE_CMD_CONTINUE     */
    RSPMSG_CMD_WRITE_REGS   write_regs;   /* RSPMSG_TYPE_CMD_WRITE_REGS   */
    RSPMSG_CMD_READ_REG     read_reg;     /* RSPMSG_TYPE_CMD_READ_REG     */
    RSPMSG_CMD_WRITE_REG    write_reg;    /* RSPMSG_TYPE_CMD_WRITE_REG    */
    RSPMSG_CMD_CONT_SIGNAL  cont_signal;  /* RSPMSG_TYPE_CMD_CONT_SIGNAL  */
    RSPMSG_CMD_REMOTE_START remote_start; /* RSPMSG_TYPE_CMD_REMOTE_START */
    RSPMSG_CMD_SET_THREAD   set_thread;   /* RSPMSG_TYPE_CMD_SET_THREAD   */
    RSPMSG_CMD_READ_MEMORY  read_memory;  /* RSPMSG_TYPE_CMD_READ_MEMORY  */
    RSPMSG_CMD_WRITE_MEMORY write_memory; /* RSPMSG_TYPE_CMD_WRITE_MEMORY  */
  } cmd;

  union {
    /* RSPMSG_TYPE_CMD */
    /*
    struct {
      union {
        BUFFER  data1;
        BUFFER  regname;
        BUFFER  regs;
      };
      union {
        BUFFER  data2;
        BUFFER  signal;
        BUFFER  value;
      };
      int       encoding1;
      long      length;
      long      thread_id;
      char      thread_purpose;
      long      addr;
      int       separator2;
      int       encoding2;
    } cmd;
    */
    /* RSPMSG_TYPE_RPL_RAW */
    /* RSPMSG_TYPE_RPL     */
    /* RSPMSG_TYPE_RPL_CMD */
    struct {
      union {
        BUFFER      a_raw;
        BUFFER      a_regs;
      } cmd;
      union {
        BUFFER      buffer2;
      } rpl;
      BUFFER    buffers[2];
      int       code;
    } rpl;
    /* OTHER SHORT MESSAGES */
    struct {
      BUFFER    buffers[2];
      int       oke;        /* RSPMSG_TYPE_RPL_NOK       */
    };
  };
} RSPMSG;

#endif
