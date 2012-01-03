#ifndef __RSPFD_H__
#define __RSPFD_H__

typedef struct __tag_RSP_FD {
  /* read/write functions */
  int (*getc)(struct __tag_RSP_FD *fd, char *c);
  int (*putc)(struct __tag_RSP_FD *fd, char c);
  int (*puts)(struct __tag_RSP_FD *fd, char *str);
  int (*putb)(struct __tag_RSP_FD *fd, void *buff, int buff_size);

  /* RLE de/encoding                                         */
  /* if enabled RLE de/encoding is used on read/writes       */
  int   rle_encoding;
  int   rle_decoding;

  /* following union makes possible to extend this channel for */
  /* using with "special" communications channels based on own */
  /* implementations of callback functions defined before      */
  union {
    int   fd;
    void *data;
  };
} RSP_FD;

int rspfd_init_fd(RSP_FD *fd, int f);
void rspfd_rle_read_enable(RSP_FD *fd, int enable);
void rspfd_rle_write_enable(RSP_FD *fd, int enable);

#endif
