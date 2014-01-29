#include "msgio.h"

int rsp_client_command_parse(RSPFD *fd, RSPMSG *m)
{
  char *p = fd->buff.b;
  int l = fd->buff.s;
  int i, code;

  /* usually we will finish succesfully exiting of this */
  /* function parsing a command and returning an ack    */
  m->type = RSPMSG_TYPE_NONE;

  code = *p;
  p++; l--;

#if 0
  switch(code)
  {
    /* deprecated commands - ignored */
    case 'b': /* set baud       */
    case 'B': /* set breakpoint */
    case 'd': /* toggle debug   */
      m->type = RSPMSG_TYPE_ERR_DEPRECATED;
      break;

    /* no params */
    case '!': m->type = RSPMSG_TYPE_CMD_EXT_MODE;    break;
    case '?': m->type = RSPMSG_TYPE_CMD_LAST_SIGNAL; break;
    case 'D': m->type = RSPMSG_TYPE_CMD_DETACH;      break;
    case 'g': m->type = RSPMSG_TYPE_CMD_READ_REGS;   break;
    case 'k': m->type = RSPMSG_TYPE_CMD_KILL;        break;

    /* only 1 addr param */
    case 'c':
      m->type = RSPMSG_TYPE_CMD_CONTINUE;
      if((i = rsp_decode_hexnumber(&(m->cmd.cont.addr), p, l, NULL, 1)) < 0) /* copy register name */
        goto bad_syn;
      p += i; l -= i;
      break;

    case 'G':
#if 0
      m->type = RSPMSG_TYPE_CMD_WRITE_REGS;
      if((i = rsp_decode_hexnumber(&m->cmd.buffers.arg1, p, l, NULL, 1)) < 0) /* copy register name */
        goto bad_syn;
      p += i; l -= i;
#endif
      break;

    /* only 1 reg param */
    case 'p':
#if 0
      m->type = RSPMSG_TYPE_CMD_READ_REG;
      if((i = rsp_decode_string(&m->cmd.regname, p, l, NULL, 1)) < 0) /* copy register name */
        goto bad_syn;
      p += i; l -= i;
#endif
      break;

    /* reg+value */
    case 'P':
#if 0
      m->type = RSPMSG_TYPE_CMD_WRITE_REG;
      if((i = rsp_decode_string(&m->cmd.regname, p, l, "=", 1)) < 0)
        goto bad_syn;
      p += i + 1; l -= i - 1;
      if((i = rsp_decode_hexdata(&m->cmd.value, p, l, NULL, 1)) < 0)
        goto bad_syn;
      p += i; l -= i;
#endif
      break;

    /* 1 signal + 1 addr param */
    case 'C':
#if 0
      m->type = RSPMSG_TYPE_CMD_CONT_SIGNAL;
      if((i = rsp_decode_intnumber(&m->cmd.length, p, l, ";", 1)) < 0)
        goto bad_syn;
      p += i + 1; l -= i + 1;
      if(!(i = rsp_decode_hexnumber(&m->cmd.addr, p, l, NULL, 1)))
        goto bad_syn;
      p += i; l -= i;
#endif
      break;

    case 'R':
#if 0
      m->type = RSPMSG_TYPE_CMD_REMOTE_START;
      if(!(i = rsp_decode_hexdata(&m->cmd.data1, p, l, NULL, 1)))
        goto bad_syn;
      p += i; l -= i;
#endif
      break;

    /* thread purpose + thread id */
    case 'H':
#if 0
      m->type = RSPMSG_TYPE_CMD_SET_THREAD;
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
#endif
      break;

    /* addr, length */
    case 'm':
#if 0
      m->type = RSPMSG_TYPE_CMD_READ_MEMORY;
      if(!(i = rsp_decode_hexnumber(&m->cmd.addr, p, l, ",", 1)))
        goto bad_syn;
      p += i + 1; l -= i + 1;
      if(!(i = rsp_decode_intnumber(&m->cmd.length, p, l, NULL, 1)))
        goto bad_syn;
      p += i; l -= i;
#endif
      break;

    /* addr, length:HEXDATA */
    case 'M':
#if 0
      m->type = RSPMSG_TYPE_CMD_WRITE_MEMORY;
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
#endif
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
      m->type = RSPMSG_TYPE_ERR_RESERVED;
      break;

    default:
      m->type = RSPMSG_TYPE_ERR_RESERVED;
      break;
  }
  return 0;

bad_syn:
  m->type = RSPMSG_TYPE_ERR_BADSYN;
  return -1;
}

int rsp_server_command_receive(RSPFD *fd, RSPMSG *msg)
{
  if(rsp_io_msg_read(fd, msg) < 0)
  {
    ERR("Error happened in 'rsp_io_msg_read'.");
    return -1;
  }

  if(RSPMSG_TYPE_NOT_PARSED_YET)
    return rsp_server_command_parse(fd, msg);

  return 0;
}

int rsp_server_command_receive_and_ack(RSPFD *fd, RSPMSG *msg)
{
  RSPMSG rpl;

  /* read message. If bad checksum repeat. */
  rsp_msg_init(&rpl);
again:
  /* receive message */
  if(!rsp_server_command_receive(fd, msg))
    goto error;

  /* decide wether ack/reply/ignore message */
  switch(msg->type)
  {
    case RSPMSG_TYPE_CMD_LAST_SIGNAL:
    case RSPMSG_TYPE_CMD_READ_REGS:
    case RSPMSG_TYPE_CMD_KILL:
    case RSPMSG_TYPE_CMD_EXT_MODE:
    case RSPMSG_TYPE_CMD_CONTINUE:
    case RSPMSG_TYPE_CMD_WRITE_REGS:
    case RSPMSG_TYPE_CMD_READ_REG:
    case RSPMSG_TYPE_CMD_WRITE_REG:
    case RSPMSG_TYPE_CMD_CONT_SIGNAL:
    case RSPMSG_TYPE_CMD_REMOTE_START:
    case RSPMSG_TYPE_CMD_SET_THREAD:
    case RSPMSG_TYPE_CMD_READ_MEMORY:
    case RSPMSG_TYPE_CMD_WRITE_MEMORY:
      rpl.type = RSPMSG_TYPE_RPL_ACK;
      break;
    
    case RSPMSG_TYPE_CMD_DETACH:
    case RSPMSG_TYPE_CMD_INTERRUPT:
    case RSPMSG_TYPE_EOF:
    case RSPMSG_TYPE_RPL_ACK:
    case RSPMSG_TYPE_RPL_RET:
    case RSPMSG_TYPE_VOID:
      rpl.type = RSPMSG_TYPE_RPL_NONE;
      return 0;

    case RSPMSG_TYPE_ERR_DEPRECATED:
    case RSPMSG_TYPE_ERR_RESERVED:
    case RSPMSG_TYPE_ERR_UNKNOWN:
    case RSPMSG_TYPE_ERR_BADSYN:
      rpl.type = RSPMSG_TYPE_RPL_VOID;
      break;
    
    case RSPMSG_TYPE_ERR_BADCHK:
      rpl.type = RSPMSG_TYPE_RPL_RET;
      break;

    default:
      FAT("Unknown message type '%d'.", msg->type);
  }

  /* write reply (and goto recv if 'reply' asked) */
  if(rsp_io_msg_write(fd, &rpl) != 0)
    goto error;
  if(rpl.type == RSPMSG_TYPE_RPL_RET)
    goto again;

  /* check it is a CMD, elsewhere error */
  if(!RSPMSG_IS_CMD(msg->type))
  {
    ERR("Unexpected message from client (message %d).", msg->type);
    goto error;
  }


  return 0;

error:
  rsp_msg_destroy(&rpl);
#endif
  return -1;
}
