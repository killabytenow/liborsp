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
#define RSPMSG_TYPE_MSG_CMD_SET_BAUD  (RSPMSG_TYPE_CMD_MASK | 'b')

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

#define RSPMSG_STRUCT_BEGIN_4(m, a, b, c, d) \
  typedef struct _tag_RSPMSG_##m RSPMSG_##m; \
  struct _tag_RSPMSG_##m {                   \
    BUFFER a;                                \
    BUFFER b;                                \
    BUFFER c;                                \
    BUFFER d;
#define RSPMSG_STRUCT_BEGIN_3(m, a, b, c)   RSPMSG_STRUCT_BEGIN_4(m, a, b, c, __b_reserved_4)
#define RSPMSG_STRUCT_BEGIN_2(m, a, b)      RSPMSG_STRUCT_BEGIN_3(m, a, b, __b_reserved_3)
#define RSPMSG_STRUCT_BEGIN_1(m, a)         RSPMSG_STRUCT_BEGIN_2(m, a, __b_reserved_2)
#define RSPMSG_STRUCT_BEGIN_0(m)            RSPMSG_STRUCT_BEGIN_1(m, __b_reserved_1)
#define RSPMSG_STRUCT_END           \
  };

RSPMSG_STRUCT_BEGIN_4(BUFFERS, b1, b2, b3, b4)
  RSPMSG_STRUCT_END

RSPMSG_STRUCT_BEGIN_0(CMD_INTERRUPT)
  int    code;
  RSPMSG_STRUCT_END

RSPMSG_STRUCT_BEGIN_0(CMD_EXT_MODE)
  RSPMSG_STRUCT_END

RSPMSG_STRUCT_BEGIN_0(CMD_LAST_SIGNAL)
  RSPMSG_STRUCT_END

RSPMSG_STRUCT_BEGIN_0(CMD_DETACH)
  RSPMSG_STRUCT_END

RSPMSG_STRUCT_BEGIN_0(CMD_READ_REGS)
  RSPMSG_STRUCT_END

RSPMSG_STRUCT_BEGIN_0(CMD_KILL)
  RSPMSG_STRUCT_END

RSPMSG_STRUCT_BEGIN_0(CMD_CONTINUE)
  long long addr;
  RSPMSG_STRUCT_END

RSPMSG_STRUCT_BEGIN_0(CMD_WRITE_REGS)
  RSPMSG_STRUCT_END

RSPMSG_STRUCT_BEGIN_0(CMD_READ_REG)
  RSPMSG_STRUCT_END

RSPMSG_STRUCT_BEGIN_0(CMD_WRITE_REG)
  RSPMSG_STRUCT_END

RSPMSG_STRUCT_BEGIN_0(CMD_CONT_SIGNAL)
  RSPMSG_STRUCT_END

RSPMSG_STRUCT_BEGIN_0(CMD_REMOTE_START)
  RSPMSG_STRUCT_END

RSPMSG_STRUCT_BEGIN_0(CMD_SET_THREAD)
  RSPMSG_STRUCT_END

RSPMSG_STRUCT_BEGIN_0(CMD_READ_MEMORY)
  RSPMSG_STRUCT_END

RSPMSG_STRUCT_BEGIN_0(CMD_WRITE_MEMORY)
  RSPMSG_STRUCT_END

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
    RSPMSG_BUFFERS          buffers;      /* generic buffers accesor      */

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
    RSPMSG_CMD_WRITE_MEMORY write_memory; /* RSPMSG_TYPE_CMD_WRITE_MEMORY */
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

inline static void rsp_msg_init(RSPMSG *m)
{
  bzero(&m, sizeof(RSPMSG));
  /* init common buffers -- same as
   *   buffer_init(&m->cmd.buffers.b1);
   *   [...]
   *   buffer_init(&m->cmd.buffers.b4);
   * (IT HAS BEEN REMOVED BECAUSE IT IS REDUNDANT)
   */
  /* bzero(&m->cmd.buffers, sizeof(m->cmd.buffers)); */
}

inline static void rsp_msg_destroy(RSPMSG *m)
{
  buffer_destroy(&m->cmd.buffers.b1);
  buffer_destroy(&m->cmd.buffers.b2);
  buffer_destroy(&m->cmd.buffers.b3);
  buffer_destroy(&m->cmd.buffers.b4);
}

#endif
