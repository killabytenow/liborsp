#include "encoding.h"
#include "helper.h"

void rsp_encode_char(BUFFER *t, char c)
{
  buffer_concatc(t, c);
}

void rsp_encode_hexdata(BUFFER *t, char *b, int sz)
{
  int i;

  for(i = 0; i < sz; i++)
  {
    buffer_concatc(t, to_xdigit((b[i]     ) & 0x0f));
    buffer_concatc(t, to_xdigit((b[i] >> 4) & 0x0f));
  }
}

void rsp_encode_data(BUFFER *t, char *b, int sz)
{
  int i;

  for(i = 0; i < sz; i++)
    buffer_concatc(t, b[i]);
}

void rsp_encode_oke(BUFFER *t, int oke)
{
  char n[255];
  if(oke)
  {
    buffer_concatc(t, 'E');
    sprintf(n, "%u", oke);
    buffer_concats(t, n);
  } else
    buffer_concats(t, "OK");
}

