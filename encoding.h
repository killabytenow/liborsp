#ifndef __LIBRST_ENCODING_H__
#define __LIBRST_ENCODING_H__

#include "buffer.h"

void orsp_encode_char(BUFFER *t, char c);
void orsp_encode_char_rle(BUFFER *t, char c, int n, int rle);
void orsp_encode_hexdata(BUFFER *t, char *b, int sz, int rle);
void orsp_encode_data(BUFFER *t, char *b, int sz, int rle);
void orsp_encode_oke(BUFFER *t, int oke);

#endif
