#include <assert.h>

#include "decoding.h"
#include "helper.h"

int orsp_decode_string(BUFFER *b, char *p, int sz, char *terminators, int tmandatory)
{
  int n;

  /* count bytes num */
  if(!(n = chars_in_buffer(terminators, p, sz)))
  {
    if(tmandatory)
    {
      ERR("Terminator not found in msg %s (expected terminators [%s]).",
          p, terminators);
      return -1;
    }
    n = sz;
  }

  /* copy chars */
  buffer_copy(b, p, n);

  return n;
}

int orsp_decode_hexdata(BUFFER *b, char *p, int sz, char *terminators, int tmandatory)
{
  int i, n;

  /* count bytes num */
  if(!(n = chars_in_buffer(terminators, p, sz)))
  {
    if(tmandatory)
    {
      ERR("Terminator not found in msg %s (expected terminators [%s]).",
          p, terminators);
      return -1;
    }
    n = sz;
  }
  if(n & 0x01)
  {
    ERR("Odd number of hexdigits -- THIS IS MADNESS.");
    return -1;
  }

  /* this is not madness, IT IS SPARTA */
  buffer_reset(b);
  for(i = 0; i < n; i += 2)
    buffer_concatc(b, (from_xdigit(p[i]) << 4) | from_xdigit(p[i+1]));

  return n;
}

int orsp_decode_hexnumber(long *n, char *p, int sz, char *terminators, int tmandatory)
{
  int e, i;

  /* count bytes num */
  for(i = *n = e = 0; p[i] && char_in_str(p[i], terminators) < 0; i++, e++)
    if(e >= sizeof(long) * 2)
    {
      ERR("Overflow: Hex number/address too big (%s).", p);
      return -1;
    } else
      *n = (*n << 4) | from_xdigit(p[i]);

  if(tmandatory
  && ((terminators && !p[i]) || (!terminators && i < sz)))
  {
    ERR("Terminator not found in msg %s (expected terminators [%s]).",
        p, terminators);
    return -1;
  }

  return i;
}

int orsp_decode_intnumber(long *n, char *p, int sz, char *terminators, int tmandatory)
{
  int s, i;

  i = 0;

  /* fetch sign */
  if((s = (p[i] == '-')))
    i++;

  /* count bytes num */
  for(*n = 0; p[i] && char_in_str(p[i], terminators) < 0; i++)
    if(p[i] <= '0' || p[i] >= '9')
    {
      ERR("Invalid decimal digit '%c' in number '%s'.", p[i], p);
      return -1;
    } else
      *n = (*n * 10) + ((p[i]) - '0');
// XXX #warning "would be nice to have a decent overflow check here."

  if(tmandatory
  && ((terminators && !p[i]) || (!terminators && i < sz)))
  {
    ERR("Terminator not found in msg %s (expected terminators [%s]).",
        p, terminators);
    return -1;
  }

  return i;
}

