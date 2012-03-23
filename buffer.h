#include <string.h>

#include "log.h"

#ifndef __BUFFER_H__
#define __BUFFER_H__

typedef struct _tag_BUFFER {
  void *b;    /* pointer to your data             */
  int   s;    /* size of your data                */
  int   __bs; /* Buffer size. Private. Dont touch */
} BUFFER;
#define BUFFER_INIT { NULL, 0, 0 }
#define BUFFER_INC  1024

static inline void buffer_grow(BUFFER *b, size_t count)
{
  if((b->s + count) < b->__bs)
    return;
  b->__bs += (1 + count/BUFFER_INC) * BUFFER_INC;
  if(!(b->b = realloc(b->b, b->__bs)))
    FAT("no mem for buffer_grow (to %d bytes, with increment of %ld).", b->__bs, count);
}

static inline void buffer_reset(BUFFER *b)
{
  buffer_grow(b, 1);
  b->s = 0;
  *((char *) b->b) = '\0';
}

static inline void *buffer_concat(BUFFER *b, void *data, size_t count)
{
  buffer_grow(b, count + 1);
  memcpy(b->b + b->s, data, count);
  b->s += count;
  *((char *) (b->b + b->s)) = '\0';
  return data;
}

static inline void *buffer_copy(BUFFER *b, void *data, size_t count)
{
  buffer_reset(b);
  return buffer_concat(b, data, count);
}

static inline int buffer_concatc(BUFFER *b, int c)
{
  buffer_grow(b, 2);
  *((char *) (b->b + b->s++)) = c;
  *((char *) (b->b + b->s)) = '\0';
  return c;
}

static inline void *buffer_concats(BUFFER *b, char *s)
{
  return buffer_concat(b, s, strlen(s));
}

#endif /* __BUFFER_H__ */
