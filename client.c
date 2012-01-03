#include "client.h"
#include "encoding.h"
#include "msgio.h"
#include "msgparse.h"

int gdbc_read_command(RSPFD *fd, RSPMSG *msg)
{
  int r;

  while(1)
  {
    r = rsp_msg_io_read(fd, msg);

    switch(r)
    {
      case GDBC_PARSER_EOF:
        return -1;

      case GDBC_PARSER_BADSYN:
        ERR("Received bad message '%s'.", (char *) fd->buff.b);
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

    switch(r = gdbc_command_parse(fd, msg))
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

  return gdbc_command_parse(fd, msg);
}

int gdbc_process_command(RSPFD *fd, RSPMSG *m)
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
  buffer_reset(&fd->buff);
  buffer_concatc(&fd->buff, '$');
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
      rsp_encode_oke(&fd->buff, m->a_oke);
      break;

    case '?': /* last signal */
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
