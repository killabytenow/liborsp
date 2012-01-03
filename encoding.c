#include "encoding.h"
#include "helper.h"

void rsp_encode_char(BUFFER *t, char c)
{
  /* XXX should we escape here also the 'P' char??? */
  if(c == '#' || c == '$' || c == '}')
  {
    buffer_concatc(t, '}');
    c ^= 0x20;
  }
  buffer_concatc(t, c);
}

void rsp_encode_char_rle(BUFFER *t, char c, int n, int rle)
{
  int r;

  while(n > 0)
  {
    if(rle && n > 3)
    {
      /* handle extreme pr0n repetition */
      r = n > 126 ? 126 : n;

      /* following chars are banned in rle repetition factor */
      if(((r + 29) == '+') || ((r + 29) == '-')
      || ((r + 29) == '#') || ((r + 29) == '$'))
      {
        /* encode one char and try again */
        rsp_encode_char(t, c);
        n--;
      } else {
        /* encode and update chocho pointer */
        rsp_encode_char(t, c);
        buffer_concatc(t, '*');
        buffer_concatc(t, r + 29);
        n -= r;
      }
    } else {
      rsp_encode_char(t, c);
      n--;
    }
  }
}

void rst_encode_hexdata(BUFFER *t, char *b, int sz, int rle)
{
  int i, j, n;
  char buff[2];
  char last_char;

  last_char = '\0';
  n = 0;

  for(i = 0; i < sz; i++)
  {
    buff[0] = to_xdigit(b[i] & 0x0f);
    buff[1] = to_xdigit((b[i] >> 4) & 0x0f);
    for(j = 0; j < 2; j++)
      if(buff[j] != last_char)
      {
        rsp_encode_char_rle(t, last_char, n, rle);
        n = 0;
        last_char = buff[j];
      } else
        n++;
  }
}

void rsp_encode_data(BUFFER *t, char *b, int sz, int rle)
{
  int i, n;

  for(i = 0; i < sz; )
  {
    /* count repetitions */
    for(n = 0; i+n+1 < sz && b[i] == b[i+n+1]; n++)
      ;

    /* encode using a rle-enabled function */
    rsp_encode_char_rle(t, b[i], n, rle);
  }
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

