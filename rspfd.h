#ifndef __RSPFD_H__
#define __RSPFD_H__

#include "buffer.h"

typedef struct __tag_RSPFD {
  /* read/write functions */
  int (*getc)(struct __tag_RSPFD *fd, char *c);
  int (*putc)(struct __tag_RSPFD *fd, char c);
  int (*puts)(struct __tag_RSPFD *fd, char *str);
  int (*putb)(struct __tag_RSPFD *fd, void *buff, int buff_size);

  /* RLE de/encoding                                         */
  /* if enabled RLE de/encoding is used on read/writes       */
  int rle_encoding;
  int rle_decoding;

  /* output/input buffer */
  BUFFER buff;
//  int    buff_state;
//#define    RSP_FD_BUFF_VOID    0
//#define    RSP_FD_BUFF_READ    0
//#define    RSP_FD_BUFF_WRITE   0
} RSPFD;

static inline void rspfd_rle_write_enable(RSPFD *fd, int enable)
{
  fd->rle_encoding = enable;
}

static inline void rspfd_rle_read_enable(RSPFD *fd, int enable)
{
  fd->rle_decoding = enable;
}

#endif
