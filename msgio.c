#include "helper.h"
#include "rspmsg.h"
#include "rspfd.h"

#undef getc
#undef putc

int rsp_io_msg_read(RSPFD *fd, RSPMSG *m)
{
  int i, lc, r, seqct, escape_mask, rle_expansion, pos;
  char c;
  unsigned ccs, ecs;

  /* clean reception buffer */
  buffer_reset(&fd->buff);
  m->type           = RSPMSG_TYPE_ERR_INVALID;
  m->seq_id         = 0;
  m->seq_id_enabled = 0;
  m->checksum       = 0;

  /* read first char */
  r = fd->getc(fd, &c);
  if(r < 0)
  {
    m->type = RSPMSG_TYPE_ERR_IO;
    return -1;
  }
  if(!r)
  {
    m->type = RSPMSG_TYPE_EOF;
    return 0;
  }

  /* if message does not start with $ then it is an special message */
  if(c != '$')
  {
    buffer_concatc(&fd->buff, c);
    switch(c)
    {
      case '+': m->type = RSPMSG_TYPE_RPL_ACK;       break;
      case '-': m->type = RSPMSG_TYPE_RPL_RET;       break;
      case 'c': m->type = RSPMSG_TYPE_CMD_INTERRUPT; break;
      default:
        MSG("Received invalid message '%c'.", c);
        m->type = RSPMSG_TYPE_ERR_INVALID;
        return -1;
    }
    return 0;
  } else {
    /* we cannot determine it yet */
    m->type = RSPMSG_TYPE_NOT_PARSED_YET;
  }

  /* read the message until '#' */
  seqct = 3;
  escape_mask = 0;
  rle_expansion = 0;
  ccs = 0;
  lc = -1;
  pos = 0;
  while(1)
  {
    /* read char */
    r = fd->getc(fd, &c);
    if(r <= 0)
    {
      if(!r)
        WRN("message truncated -- EOF");
      else
        ERR_ERRNO("Read error");
      m->type = RSPMSG_TYPE_ERR_IO;
      return -1;
    }

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
        pos++;
        continue;
      } else
      if(seqct == 1 && c == ':')
      {
        m->seq_id = ((((char *) fd->buff.b)[0] - '0') * 10) + (((char *) fd->buff.b)[1] - '0');
        m->seq_id_enabled = 1;
        pos = 0;
        buffer_reset(&fd->buff);
        seqct--;
        continue;
      } else
        seqct = 0;
    }

    /* following chars are forbidden */
    if(c == '#' || c == '$')
    {
      WRN("unexpected char '%c' inside of message.", c);
      WRN("(escape_mask=%d,  rle_expansion=%d).", escape_mask, rle_expansion);
      m->type = RSPMSG_TYPE_ERR_INVALID;
      return -1;
    }

    /* parse message */
    if(rle_expansion)
    {
      rle_expansion = c - 29;
      if(rle_expansion < 0 || rle_expansion > 126)
      {
        ERR("RLE factor out of bounds (%02x*%d).", lc, rle_expansion);
        m->type = RSPMSG_TYPE_ERR_INVALID;
        return -1;
      }
      for(; rle_expansion > 1; rle_expansion--)
        buffer_concatc(&fd->buff, lc);
      rle_expansion = 0;
    } else {
      switch(c)
      {
        case '{':
          if(escape_mask != 0)
          {
            ERR("Escape char { cannot appear after a escape char {.");
            m->type = RSPMSG_TYPE_ERR_INVALID;
            return -1;
          }
          escape_mask = 0x20;
          break;

        case '*':
          if(fd->rle_decoding)
          {
            if(!pos || escape_mask)
            {
              ERR("Unexpected RLE operator * at beginning of the message.");
              m->type = RSPMSG_TYPE_ERR_INVALID;
              return -1;
            }
            rle_expansion = -1;
            break;
          } /* else fallback to standard char processing */

        default:
          lc = c ^ escape_mask;
          buffer_concatc(&fd->buff, lc);
          escape_mask = 0;
      }
    }
    pos++;
  }

  /* we have found the hash - get and validate the cheksum */
  ecs = 0;
  for(i = 0; i < 2; i++)
  {
    r = fd->getc(fd, &c);
    if(r <= 0)
    {
      m->type = RSPMSG_TYPE_ERR_IO;
      if(!r)
        WRN("CRC message truncated -- EOF");
      else
        ERR_ERRNO("Read error");
      return -1;
    }
    if((c = from_xdigit(c)) < 0)
    {
      ERR("Unexpected character %02x in CRC.", c);
      m->type = RSPMSG_TYPE_ERR_INVALID;
      return -1;
    }
    ecs += (c << (4 * i));
  }
  if(ccs != ecs)
  {
    m->type = RSPMSG_TYPE_ERR_BADCHK;
    return -1;
  }
  if(fd->buff.s == 0)
    m->type = RSPMSG_TYPE_VOID;
  m->checksum = ecs;

  return 0;
}

static int rsp_io_msg_write_msg_char(RSPFD *fd, char c)
{
  int s = 1;
  if(c == '$' || c == '#' || c == 0x7d || c & 0x80)
  {
    if(fd) fd->putc(fd, 0x7d);
    c = c ^ 0x20;
    s++;
  }
  if(fd) fd->putc(fd, c);
  return s;
}

int rsp_io_msg_write(RSPFD *fd)
{
  char *p = fd->buff.b;
  int lc, rep, i, prep, cs;

  if(fd->buff.s <= 0)
  {
    fd->puts(fd, "$#00");
    return 0;
  }

  cs = 0;
  if(fd->buff.s > 0)
  {
    lc = p[0];
    rep = 1;
    fd->putc(fd, '$');
    for(i = 1; i < fd->buff.s; i++)
    {
      if(lc == p[i])
      {
        rep++;
      } else {
        switch(rep)
        {
          case 2:
            rsp_io_msg_write_msg_char(fd, lc);
            rep--;
          case 1:
            rsp_io_msg_write_msg_char(fd, lc);
            rep--;
            break;

          case 3:
            if(rsp_io_msg_write_msg_char(fd, lc) == 1)
            {
              rsp_io_msg_write_msg_char(fd, lc);
              rsp_io_msg_write_msg_char(fd, lc);
            } else {
              rsp_io_msg_write_msg_char(fd, '*');
              rsp_io_msg_write_msg_char(fd, 3 + 29);
            }
            rep -= 3;
            break;

          case (36 - 29): /* '$' */
            rsp_io_msg_write_msg_char(fd, lc);
            rep--;
          case (35 - 29): /* '#' */
          case (43 - 29): /* '+' */
          case (45 - 29): /* '-' */
            rsp_io_msg_write_msg_char(fd, lc);
            rep--;
          default:
            while(rep > 0)
            {
              prep = rep > 126 ? 126 : rep;
              rsp_io_msg_write_msg_char(fd, lc);
              rsp_io_msg_write_msg_char(fd, '*');
              rsp_io_msg_write_msg_char(fd, prep + 29);
              rep -= prep;
            }
        } /* end-of-switch-and-your-puta-madre */
      } /* end-of-if-af-uf */
    } /* end-of-for-y-tu-vieja-la-calva */
  }
  rsp_io_msg_write_msg_char(fd, '#');
  rsp_io_msg_write_msg_char(fd, ((cs >> 4) & 0x0f) + '0');
  rsp_io_msg_write_msg_char(fd, ((cs >> 0) & 0x0f) + '0');

//m->checksum = cs;

  return 0;
}

/*****************************************************************************
 * int rsp_io_msg_write_msg(RSPFD *fd, RSPMSG *m)
 *
 *   Writes a RSP on 'fd'.
 *
 * Returns != 0 if everything ok.
 *****************************************************************************/

int rsp_io_msg_write_msg(RSPFD *fd, RSPMSG *m)
{
  int r = 0;

  if(RSPMSG_IS_ERR(m->type))
  {
    ERR("You tried to send a msg on error (type 0x%04x).", m->type);
    return -1;
  }

  buffer_reset(&fd->buff);
  switch(m->type)
  {
    case RSPMSG_TYPE_NONE:
    case RSPMSG_TYPE_VOID:
      /* no message sent really */
      break;

    case RSPMSG_TYPE_RPL_RET:
      return fd->putc(fd, '-') != EOF;

    case RSPMSG_TYPE_RPL_ACK:
      return fd->putc(fd, '+') != EOF;

    case RSPMSG_TYPE_CMD_INTERRUPT:
      return fd->putc(fd, m->cmd.interrupt.code) != EOF;

    default:
      FAT("Invalid msg type %d", m->type);
  }

  if(fd)
    r = rsp_io_msg_write_msg(fd, m);

  return r;
}

