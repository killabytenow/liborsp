#include "server.h"
#include "decoding.h"
#include "msgio.h"
#include "msgparse.h"

#define SETAR(x, y) if(!m->type) { m->type = RSPMSG_TYPE_##x; r->type = RSPMSG_TYPE_##y; }
int rsp_server_command_parse(RSPFD *fd, RSPMSG *m, RSPMSG *r)
{
  char *p = fd->buff.b;
  int l = fd->buff.s;
  int i, code;

  /* usually we will finish succesfully exiting of this */
  /* function parsing a command and returning an ack    */
  m->type = RSPMSG_TYPE_NONE;
  r->type = RSPMSG_TYPE_NONE;

  code = *p;
  p++; l--;

  switch(code)
  {
    /* deprecated commands - ignored */
    case 'b': /* set baud       */
    case 'B': /* set breakpoint */
    case 'd': /* toggle debug   */
      SETAR(ERR_DEPRECATED, RPL_VOID);
      break;

    /* no params */
    case '!': SETAR(CMD_EXT_MODE,      RPL_ACK);
    case '?': SETAR(CMD_LAST_SIGNAL,   RPL_ACK);
    case 'D': SETAR(CMD_DETACH,        RPL_ACK);
    case 'g': SETAR(CMD_READ_REGS,     RPL_ACK);
    case 'k': SETAR(CMD_KILL,          RPL_ACK);
      break;

    /* only 1 addr param */
    case 'c': SETAR(CMD_CONTINUE,      RPL_ACK);
    case 'G': SETAR(CMD_WRITE_REGS,    RPL_ACK);
      if((i = rsp_decode_hexnumber(&m->cmd.addr, p, l, NULL, 1)) < 0) /* copy register name */
        goto bad_syn;
      p += i; l -= i;
      break;

    /* only 1 reg param */
    case 'p': SETAR(CMD_READ_REG,      RPL_ACK);
      if((i = rsp_decode_string(&m->cmd.regname, p, l, NULL, 1)) < 0) /* copy register name */
        goto bad_syn;
      p += i; l -= i;
      break;

    /* reg+value */
    case 'P': SETAR(CMD_WRITE_REG,     RPL_ACK);
      if((i = rsp_decode_string(&m->cmd.regname, p, l, "=", 1)) < 0)
        goto bad_syn;
      p += i + 1; l -= i - 1;
      if((i = rsp_decode_hexdata(&m->cmd.value, p, l, NULL, 1)) < 0)
        goto bad_syn;
      p += i; l -= i;
      break;

    /* 1 signal + 1 addr param */
    case 'C': SETAR(CMD_CONT_SIGNAL,  RPL_ACK);
      if((i = rsp_decode_intnumber(&m->cmd.length, p, l, ";", 1)) < 0)
        goto bad_syn;
      p += i + 1; l -= i + 1;
      if(!(i = rsp_decode_hexnumber(&m->cmd.addr, p, l, NULL, 1)))
        goto bad_syn;
      p += i; l -= i;
      break;

    case 'R': SETAR(CMD_REMOTE_START, RPL_ACK);
      if(!(i = rsp_decode_hexdata(&m->cmd.data1, p, l, NULL, 1)))
        goto bad_syn;
      p += i; l -= i;
      break;

    /* thread purpose + thread id */
    case 'H': SETAR(CMD_SET_THREAD, RPL_ACK);
      m->cmd.thread_purpose = *p;
      p++; l--;
      if(m->cmd.thread_purpose != 'c' && m->cmd.thread_purpose != 'g')
      {
        ERR("Horrible C parameter '%c' in command 'H'.", m->cmd.thread_purpose);
        goto bad_syn;
      }
      if(!(i = rsp_decode_intnumber(&m->cmd.thread_id, p, l, NULL, 1)))
        goto bad_syn;
      p += i; l -= i;
      break;

    /* addr, length */
    case 'm': SETAR(CMD_READ_MEMORY, RPL_ACK);
      if(!(i = rsp_decode_hexnumber(&m->cmd.addr, p, l, ",", 1)))
        goto bad_syn;
      p += i + 1; l -= i + 1;
      if(!(i = rsp_decode_intnumber(&m->cmd.length, p, l, NULL, 1)))
        goto bad_syn;
      p += i; l -= i;
      break;

    /* addr, length:HEXDATA */
    case 'M': SETAR(CMD_WRITE_MEMORY, RPL_ACK);
      /* read addr + length */
      if(!(i = rsp_decode_hexnumber(&m->cmd.addr, p, l, ",", 1)))
        goto bad_syn;
      p += i + 1; l -= i + 1;
      if(!(i = rsp_decode_intnumber(&m->cmd.length, p, l, ":", 1)))
        goto bad_syn;
      p += i + 1; l -= i + 1;
      if(!(i = rsp_decode_hexdata(&m->cmd.data1, p, l, NULL, 1)))
        goto bad_syn;
      p += i; l -= i;
      break;

    /* reserved */
    case 'a':
    case 'A':
    case 'e':
    case 'E':
    case 'f':
    case 'F':
    case 'i':
    case 'I':
    case 'j':
    case 'J':
    case 'l':
    case 'L':
      SETAR(ERR_RESERVED, RPL_VOID);
      break;

    default:
      SETAR(ERR_UNKNOWN, RPL_VOID);
      break;
  }
  return 0;

bad_syn:
  SETAR(ERR_BADSYN, RPL_VOID);
  return -1;
}

int rsp_server_command_receive(RSPFD *fd, RSPMSG *msg)
{
  int r;

  if(rsp_io_msg_read(fd, msg) < 0)
  {
    ERR("Error happened in 'rsp_io_msg_read'.");
    return -1;
  }

  if(RSPMSG_TYPE_NOT_PARSED_YET)
    return rsp_server_command_parse(fd, msg, rpl);

  return 0;
}

int rsp_server_command_receive_and_ack(RSPFD *fd, RSPMSG *msg)
{
  int r;
  RSPMSG rpl;

  /* read message. If bad checksum repeat. */
again:
  if(rsp_server_command_receive(fd, msg) < 0)
  {
    if(msg->type == RSPMSG_TYPE_ERR_BADCHK)
    {
      rpl->type = RSPMSG_TYPE_RPL_RET;
      if(rsp_io_msg_write(fd, rpl) < 0)
        return rpl->type;
      goto again;
    } else
      return -1;
  }

  /* check it is a CMD, elsewhere error */
  if(RSPMSG_IS_CMD(msg->type))
  {
    if(msg->type == RSPMSG_TYPE_CMD_DETACH
    || msg->type == RSPMSG_TYPE_CMD_INTERRUPT)
      /* these messages should not be acked */
      return 0;

    rpl->type = RSPMSG_TYPE_RPL_ACK;
    return rsp_io_msg_write(fd, rpl) < 0
             ? rpl->type
             : 0;
  }

  ERR("Unexpected message from client (message %d).", msg->type);
  return -1;
}

  switch(msg->type)
  {
    /* ERRORS */
      MSG("Received interruption %c from client.", msg->interrupt);

    case RSPMSG_TYPE_EOF:
    case RSPMSG_TYPE_RPL_ACK:
    case RSPMSG_TYPE_RPL_RET:
    case RSPMSG_TYPE_VOID:
    case RSPMSG_TYPE_CMD_INTERRUPT:
      rpl->type = RSPMSG_TYPE_RPL_NONE;
      return 0;
    m->type = ;

    case RSPMSG_TYPE_CMD:
      /* fall into command parser */
      break;

    case RSPMSG_TYPE_CMD_EXT_MODE:
      rpl->type = RSPMSG_TYPE_RPL_ACK;
      break;
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
    default:
      FAT("Unknown msg type %d.", msg->type);
  }
      default:
  if(RSPMSG_IS_ERR(msg->type))
  {
        ERR("Unknown return code %d from 'rsp_io_msg_read()'.", r);
        exit(1);
    }
    return -1;
  }

  switch(msg->type)
  {

int rsp_client_command_process(RSPFD *fd, RSPMSG *m)
{
  RSPSERVER_CMD_HANDLER *h;
  int r;

  /* search for most suitable command handler */
  switch(m->type)
  {
    case RSPMSG_TYPE_RPL_RET:
      rt = RSPMSG_TYPE_RPL_NONE;
      h = NULL;
      break;

    case RSPMSG_TYPE_INTERRUPT:
      rt = RSPMSG_TYPE_RPL_NONE;
      h = s->s_interrupt;
      break;

    case RSPMSG_TYPE_CMD:
      switch(m->cmd.code)
      {
        /* without reply */
        case 'D': rt = RSPMSG_TYPE_RPL_NONE; h = s->s_detach;          break;
        case 'k': rt = RSPMSG_TYPE_RPL_NONE; h = s->s_kill;            break;
        case 'R': rt = RSPMSG_TYPE_RPL_NONE; h = s->s_remote_restart;  break;
        /* OK/Enn replies */
        case 'G': rt = RSPMSG_TYPE_RPL_OKE;  h = s->s_write_registers; break;
        case 'H': rt = RSPMSG_TYPE_RPL_OKE;  h = s->s_set_thread;      break;

        case '?': rt = RSPMSG_TYPE_RPL_s/* last signal */
        case 'c': /* continue */
        case 'C': /* continue with signal */
      /* TODO; same as step and continue */
      break;

    case 'g': /* read registers */
      if(m->a_oke != 0)
        rsp_encode_oke(&fd->buff, m->a_oke);
      else
        rsp_encode_hexdata(&fd->buff, m->a_regs.b, m->a_regs.s);
      break;

    /* not implemented / reserved / deprecated / draft / unknown */
    case 'a':
    case 'A':
    case 'b':
    case 'B':
    case 'd':
    case 'e':
    case 'E':
    case 'f':
    case 'F':
    case 'h':
    case 'i':
    case 'I':
    case 'j':
    case 'J':
    case 'l':
    case 'L':
    default:
      ERR("Command '%c' not implemented.", m->command);
  for(h = gdbc_handlers; h && h->command != m->command; h = h->next)
    ;
  if(!h)
  {
    ERR("Command handler for '%c' not implemented.", m->command);
    fd->puts(fd, "$#00");
    return -1;
  }

  /* execute handler */
  if(!(r = h->handler(m)))
  {
    ERR("Error happened when handling command '%c'.", m->command);
    fd->puts(fd, "$#00");
    return -1;
  }

  /* fetch msg data, compound a msg, and write it to the client */
  buffer_reset(&fd->buff);
  buffer_concatc(&fd->buff, '$');
  switch(m->command)
  {
  }

  /* execute it! */
}

/*
int read_commands(int fd)
{
  int end;
  int r, c;
  BUFFER msg;

  while(1)
  {
  }

terminated:
  MSG("GDB session terminated.");
}
*/
