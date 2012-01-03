#ifndef __LIBRST_ENCODING_H__
#define __LIBRST_ENCODING_H__

#include "buffer.h"

void rsp_encode_char(BUFFER *t, char c);
void rsp_encode_hexdata(BUFFER *t, char *b, int sz);
void rsp_encode_data(BUFFER *t, char *b, int sz);
void rsp_encode_oke(BUFFER *t, int oke);

#endif
