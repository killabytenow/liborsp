#ifndef __LIBORSP_ENCODING_H__
#define __LIBORSP_ENCODING_H__

#include "buffer.h"

int orsp_decode_string(BUFFER *b, char *p, int sz, char *terminators, int tmandatory);
int orsp_decode_hexdata(BUFFER *b, char *p, int sz, char *terminators, int tmandatory);
int orsp_decode_hexnumber(long *n, char *p, int sz, char *terminators, int tmandatory);
int orsp_decode_intnumber(long *n, char *p, int sz, char *terminators, int tmandatory);

#endif
