#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define __STACKTRACE_MSG_MACROS__
#include "decoding.h"
#include "encoding.h"
#include "rspfd.h"
#include "rspmsg.h"
#include "client.h"

/*****************************************************************************
 * Message parser and writer
 *****************************************************************************/

/*****************************************************************************
 * Command parser
 *****************************************************************************/

void gdbc_command_reset(RSPMSG *c)
{
  /* TODO XXX TODO */
}

int gdbc_parse_command(RSPMSG *c)
{
  char *p = c->raw.b;
  int l = c->raw.s;
  int i;

  gdbc_command_reset(c);

  c->command = *p;
  p++; l--;

  switch(c->command)
  {
    /* deprecated commands - ignored */
    case 'b': /* set baud       */
    case 'B': /* set breakpoint */
    case 'd': /* toggle debug   */
      return GDBC_CMD_DEPRECATED;

    /* no params */
    case '!': /* last signal    */
    case '?': /* last signal    */
    case 'D': /* detach         */
    case 'g': /* read registers */
    case 'k': /* kill */
      break;

    /* only 1 addr param */
    case 'c': /* continue */
    case 'G': /* write registers */
      if((i = orsp_decode_hexnumber(&c->q_addr, p, l, NULL, 1)) < 0) /* copy register name */
        return GDBC_CMD_BADPARAMS;
      p += i; l -= i;
      break;

    /* only 1 reg param */
    case 'p': /* read reg */
      if((i = orsp_decode_string(&c->q_regname, p, l, NULL, 1)) < 0) /* copy register name */
        return GDBC_CMD_BADPARAMS;
      p += i; l -= i;
      break;

    /* reg+value */
    case 'P': /* write reg */
      if((i = orsp_decode_string(&c->q_regname, p, l, "=", 1)) < 0)
        return GDBC_CMD_BADPARAMS;
      p += i + 1; l -= i - 1;
      if((i = orsp_decode_hexdata(&c->q_value, p, l, NULL, 1)) < 0)
        return GDBC_CMD_BADPARAMS;
      p += i; l -= i;
      break;

    /* 1 signal + 1 addr param */
    case 'C': /* continue with signal */
      if((i = orsp_decode_intnumber(&c->q_length, p, l, ";", 1)) < 0)
        return GDBC_CMD_BADPARAMS;
      p += i + 1; l -= i + 1;
      if(!(i = orsp_decode_hexnumber(&c->q_addr, p, l, NULL, 1)))
        return GDBC_CMD_BADPARAMS;
      p += i; l -= i;
      break;

    case 'R': /* remote restart */
      if(!(i = orsp_decode_hexdata(&c->q_data1, p, l, NULL, 1)))
        return GDBC_CMD_BADPARAMS;
      p += i; l -= i;
      break;

    /* thread purpose + thread id */
    case 'H': /* set thread */
      c->q_thread_purpose = *p;
      p++; l--;
      if(c->q_thread_purpose != 'c' && c->q_thread_purpose != 'g')
      {
        ERR("Horrible C parameter '%c' in command 'H'.", c->q_thread_purpose);
        return GDBC_CMD_BADPARAMS;
      }
      if(!(i = orsp_decode_intnumber(&c->q_thread_id, p, l, NULL, 1)))
        return GDBC_CMD_BADPARAMS;
      p += i; l -= i;
      break;

    /* addr, length */
    case 'm': /* read memory */
      if(!(i = orsp_decode_hexnumber(&c->q_addr, p, l, ",", 1)))
        return GDBC_CMD_BADPARAMS;
      p += i + 1; l -= i + 1;
      if(!(i = orsp_decode_intnumber(&c->q_length, p, l, NULL, 1)))
        return GDBC_CMD_BADPARAMS;
      p += i; l -= i;
      break;

    /* addr, length:HEXDATA */
    case 'M': /* write memory */
      /* read addr + length */
      if(!(i = orsp_decode_hexnumber(&c->q_addr, p, l, ",", 1)))
        return GDBC_CMD_BADPARAMS;
      p += i + 1; l -= i + 1;
      if(!(i = orsp_decode_intnumber(&c->q_length, p, l, ":", 1)))
        return GDBC_CMD_BADPARAMS;
      p += i + 1; l -= i + 1;
      if(!(i = orsp_decode_hexdata(&c->q_data1, p, l, NULL, 1)))
        return GDBC_CMD_BADPARAMS;
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
      return GDBC_CMD_RESERVED;

    default:
      return GDBC_CMD_UNKNOWN;
  }

  return GDBC_CMD_OK;
}

int gdbc_read_command(RSP_FD *fd, RSPMSG *msg)
{
  int r;

  while(1)
  {
    r = orsp_read_msg(fd, msg);

    switch(r)
    {
      case GDBC_PARSER_EOF:
        return -1;

      case GDBC_PARSER_BADSYN:
        ERR("Received bad message '%s'.", (char *) msg->raw.b);
        ERR("Packet dumped:");
        /* XXX TODO XXX */
        continue;

      case GDBC_PARSER_BADCHK:
        ERR("Invalid checksum. Ask for retransmission.");
        fd->puts(fd, "-");
        continue;

      case GDBC_OK:
        /* fall into command parsers */
        break;

      default:
        ERR("Unknown return code %d from 'gdbc_parse_read_msg()'.", r);
        exit(1);
    }

    switch(r = gdbc_parse_command(msg))
    {
      case GDBC_CMD_OK:
        fd->puts(fd, "+");
        return 0;
      case GDBC_CMD_BADPARAMS:
        ERR("Error processing message '%c'.", msg->command);
        ERR("Packet dumped:");
        fd->puts(fd, "$#00");
        break;
      case GDBC_CMD_DEPRECATED:
        ERR("Command '%c' is deprecated.", msg->command);
        fd->puts(fd, "$#00");
        break;
      case GDBC_CMD_RESERVED:
        ERR("Received reserved GDB command '%c'.", msg->command);
        fd->puts(fd, "$#00");
        break;
      case GDBC_CMD_UNKNOWN:
        ERR("Received GDB unknown command '%c'.", msg->command);
        fd->puts(fd, "$#00");
        break;
      default:
        FAT("Unknown 'gdbc_parse_command()' return code '%d'.", r);
    }
  }

  return gdbc_parse_command(msg);
}

int gdbc_process_command(RSP_FD *fd, RSPMSG *m)
{
  GDBHANDLER *h;
  int r;

  /* search for most suitable command handler */
  for(h = gdbc_handlers; h && h->command != m->command; h = h->next)
    ;
  if(!h)
  {
    ERR("Command handler for '%c' not found.", m->command);
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
  buffer_reset(&m->raw);
  buffer_concatc(&m->raw, '$');
  switch(m->command)
  {
    /* without acknowledgement */
    case 'D': /* detach */
    case 'k': /* kill */
    case 'R': /* remote restart */
      /* finish function because nothing is sent to client */
      return 0;

    /* OK/Enn replies */
    case 'G': /* write registers */
    case 'H': /* set thread */
      orsp_encode_oke(&m->raw, m->a_oke);
      break;

    case '?': /* last signal */
    case 'c': /* continue */
    case 'C': /* continue with signal */
      /* TODO; same as step and continue */
      break;

    case 'g': /* read registers */
      if(m->a_oke != 0)
        orsp_encode_oke(&m->raw, m->a_oke);
      else
        orsp_encode_hexdata(&m->raw, m->a_regs.b, m->a_regs.s);
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
