#ifndef __LIBORSP_ENCODING_H__
#define __LIBORSP_ENCODING_H__

#include "buffer.h"

int rsp_decode_string(BUFFER *b, char *p, int sz, char *terminators, int tmandatory);
int rsp_decode_hexdata(BUFFER *b, char *p, int sz, char *terminators, int tmandatory);
int rsp_decode_hexnumber(long long *n, char *p, int sz, char *terminators, int tmandatory);
int rsp_decode_intnumber(long long *n, char *p, int sz, char *terminators, int tmandatory);

#endif
