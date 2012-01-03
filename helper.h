#include <ctype.h>
#include "log.h"

#ifndef __LIBORSP_HELPER_H__
#define __LIBORSP_HELPER_H__

static inline int from_xdigit(int c)
{
  c = tolower(c);
  if(c >= '0' && c <= '9') return c - '0';
  if(c >= 'a' && c <= 'f') return 10 + (c - 'a');
  if(c >= 'A' && c <= 'F') return 10 + (c - 'A');
  ERR("Bad xdigit '%c'.", c);
  return -1;
}

static inline int to_xdigit(int v)
{
  if(v >= 0  && v <= 9)  return v + '0';
  if(v >= 10 && v <= 15) return v - 10 + 'a';
  ERR("Bad xvalue '%d'.", v);
  return -1;
}

static inline int char_in_str(char c, char *s)
{
  int i;

  if(!s)
    return -1;

  for(i = 0; s[i]; i++)
    if(c == s[i])
      return i;

  return -1;
}

static inline int chars_in_buffer(char *chars, char *b, int sz)
{
  char *cc;
  int i;

  if(!chars)
    return sz;

  for(i = 0; i < sz; i++)
    for(cc = chars; *cc; cc++)
      if(*cc == b[i])
        return i;

  return -1;
}

#endif
