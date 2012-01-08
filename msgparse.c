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
#include "msgio.h"
#include "msgparse.h"

/*****************************************************************************
 * Message parser and writer
 *****************************************************************************/

void rspmsg_reset(RSPMSG *c)
{
  /* TODO XXX TODO */
}

/*****************************************************************************
 * (SERVER) Command parser
 *****************************************************************************/

int rspmsg_command_parse(RSPFD *fd, RSPMSG *c)
{
  char *p = fd->buff.b;
  int l = fd->buff.s;
  int i;

  rspmsg_reset(c);

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

