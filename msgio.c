#include "rspmsg.h"

int rsp_msg_io_read(RSP_FD *fd, RSP_MSG *m)
{
  int s, i, c, lc, r;
  unsigned ccs, ecs;

  /* clean reception buffer */
  buffer_reset(&fd->buff);
  m->seqid = 0;

  /* read first char */
  r = fd->getc(fd, &c);
  if(r < 0) return GDBC_PARSER_READ_ERROR;
  if(!r)    return GDBC_PARSER_EOF;

  /* if message does not start with $ then it is an special message */
  if(c != '$')
  {
    buffer_concatc(&fd->buff, c);
    switch(c)
    {
      case '+':
        m->type = RSP_MSG_OK_REPLY;
        break;
      case '-':
        m->type = RSP_MSG_AGAIN_REQUEST;
        break;
      default:
        MSG("Received interrupt %d.", c);
        m->type = RSP_MSG_INTERRUPT;
    }
    return GDBC_OK;
  } else
    m->type = RSP_MSG;

  /* read the message until '#' */
  seqct = 3;
  escape_mask = 0;
  rle_expansion = 0;
  ccs = 0;
  lc = -1;
  while(1)
  {
    /* read char */
    r = fd->getc(fd, &c);
    if(r < 0) return GDBC_PARSER_READ_ERROR;
    if(!r)    return GDBC_PARSER_EOF;

    /* is this the end of message? */
    if(!escape_mask && !rle_expansion && c == '#')
      break;

    /* add this char to the checksum accumulator */
    ccs = ((unsigned) (ccs + c)) & ((unsigned) 0xff);

    /* fetch sequence id */
    if(seqct > 0)
    {
      if(((seqct == 2 || seqct == 3) && (c >= 0 && c <= 9)))
      {
        lc = c;
        buffer_concatc(&fd->buff, c);
        seqct--;
        continue;
      } else
      if(seqct == 1 && c == ':')
      {
        m->seqid = ((fd->buff->b[0] - '0') * 10) + (fd->buff->b[1] - '0');
        buffer_reset(&fd->buff);
        seqct--;
        continue;
      } else
        seqct = 0;
    }

    /* following chars are forbidden */
    if(c == '#' || c == '$')
      return GDBC_PARSER_BADSYN;

    /* parse message */
    if(rle_expansion)
    {
      rle_expansion = c - 29;
      if(rle_expansion < 0 || rle_expansion > 126)
      {
        ERR("RLE factor out of bounds (%02x*%02x).", lc, *rle_expansion+29);
        return GDBC_PARSER_BADSYN;
      }
      for(; rle_expansion > 1; rle_expansion--)
        buffer_concatc(&fd->buff, lc);
      rle_expansion = 0;
    } else {
      switch(c)
      {
        case '{':
          if(escape_mask != 0)
            return GDBC_PARSER_BADSYN;
          escape_mask = 0x20;
          break;

        case '*':
          if(fd->rle_decoding)
          {
            if(lc < 0 || escape_mask)
              return GDBC_PARSER_BADSYN;
            rle_expansion = -1;
            break;
          } /* else fallback to standard char processing */

        default:
          lc = c ^ escape_mask;
          buffer_concatc(&fd->buff, lc);
          escape_mask = 0;
      }
    }
  }

  /* we have found the hash - get and validate the cheksum */
  ccs = 0;
  for(i = 0; i < 2; i++)
  {
    r = fd->getc(fd, &c);
    if(r < 0) return GDBC_PARSER_READ_ERROR;
    if(!r)    return GDBC_PARSER_EOF;
    if((c = from_xdigit(c)) < 0)
      return GDBC_PARSER_BADSYN;
    ccs -= (c << (4 * i));
  }
  if(ccs)
    return GDBC_PARSER_BADCHK;

  return GDBC_OK;
}

static int rsp_msg_io_write_char(RSP_FD *fd, char c)
{
  int s = 1;
  if(c == '$' || c == '#' c == 0x7d || c & 0x80)
  {
    if(fd) fd->putc(fd, 0x7d);
    c = c ^ 0x20;
    s++;
  }
  if(fd) fd->putc(fd, c);
  return s;
}

int rsp_msg_io_write(RSP_FD *fd, RSP_MSG *m)
{
  char *p;
  int lc, rep, i, prep, cs;

  if(fd->buff.s <= 0)
  {
    fd->puts(fd, "$#00");
    return 0;
  }

  lc = p[0];
  rep = 1;
  fd->putc(fd, '$');
  cs = 0;
  for(i = 1; i < fd->buff.s; i++)
  {
    if(lc == p[i])
    {
      rep++;
    } else {
      switch(rep)
      {
        case 2:
          orsp_msg_io_write_char(fd, lc);
          rep--;
        case 1:
          orsp_msg_io_write_char(fd, lc);
          rep--;
          break;

        case 3:
          if(orsp_msg_io_write_char(fd, lc) == 1)
          {
            orsp_msg_io_write_char(fd, lc);
            orsp_msg_io_write_char(fd, lc);
          } else {
            orsp_msg_io_write_char(fd, '*');
            orsp_msg_io_write_char(fd, 3 + 29);
          }
          rep -= 3;
          break;

        case (36 - 29): /* '$' */
          orsp_msg_io_write_char(fd, lc);
          rep--;
        case (35 - 29): /* '#' */
        case (43 - 29): /* '+' */
        case (45 - 29): /* '-' */
          orsp_msg_io_write_char(fd, lc);
          rep--;
        default:
          while(rep > 0)
          {
            prep = rep > 126 ? 126 : rep;
            orsp_msg_io_write_char(fd, lc);
            orsp_msg_io_write_char(fd, '*');
            orsp_msg_io_write_char(fd, prep + 29);
            rep -= prep;
          }
      } /* end-of-switch-and-your-puta-madre */
    } /* end-of-if-af-uf */
  } /* end-of-for-y-tu-vieja-la-calva */
            
        The printable characters `$', `#', `+' and `-'
        or with a numeric value greater than 126 should
        not be used. 
      }
    }
  }
}

