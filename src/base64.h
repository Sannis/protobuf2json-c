/*
 * Copyright (c) 2014-2016 Oleg Efimov <efimovov@gmail.com>
 *
 * Code based on nginx 1.5.5, extracted by Anton Povarov <anton.povarov@gmail.com>
 *
 * protobuf2json-c is free software; you can redistribute it
 * and/or modify it under the terms of the MIT license.
 * See LICENSE for details.
 */

#ifndef BASE64_H
#define BASE64_H 1

#define base64_encoded_len(len) (((len + 2) / 3) * 4)
#define base64_decoded_len(len) (((len + 3) / 4) * 3)

typedef struct base64_tables {
  unsigned char encode[64];
  unsigned char decode[256];
  unsigned char padding;
} base64_tables_t;

static const base64_tables_t base64_default_tables = {
  .encode = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
  .decode =  {
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 62, 77, 77, 77, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 77, 77, 77, 77, 77, 77,
    77,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 77, 77, 77, 77, 77,
    77, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 77, 77, 77, 77, 77,

    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77
  },
  .padding = '='
};

size_t base64_encode(char *dst, const char *src, size_t src_len)
{
  char                *d = dst;
  const unsigned char *s = (void*)src;
  const unsigned char *basis64 = base64_default_tables.encode;

  while (src_len > 2) {
    *d++ = basis64[(s[0] >> 2) & 0x3f];
    *d++ = basis64[((s[0] & 3) << 4) | (s[1] >> 4)];
    *d++ = basis64[((s[1] & 0x0f) << 2) | (s[2] >> 6)];
    *d++ = basis64[s[2] & 0x3f];

    s += 3;
    src_len -= 3;
  }

  if (src_len) {
    *d++ = basis64[(s[0] >> 2) & 0x3f];

    if (src_len == 1) {
      *d++ = basis64[(s[0] & 3) << 4];
      *d++ = base64_default_tables.padding;
    } else {
      *d++ = basis64[((s[0] & 3) << 4) | (s[1] >> 4)];
      *d++ = basis64[(s[1] & 0x0f) << 2];
    }

    *d++ = base64_default_tables.padding;
  }

  return (d - dst);
}

size_t base64_decode(char *dst, const char *src, size_t src_len)
{
  size_t               len;
  char                *d = dst;
  const unsigned char *s = (void*)src;
  const unsigned char *basis = base64_default_tables.decode;

  for (len = 0; len < src_len; len++) {
    if (s[len] == base64_default_tables.padding) {
      break;
    }

    if (basis[s[len]] == 77) {
      return 0;
    }
  }

  if (len % 4 == 1) {
    return 0;
  }

  while (len > 3) {
    *d++ = (char) (basis[s[0]] << 2 | basis[s[1]] >> 4);
    *d++ = (char) (basis[s[1]] << 4 | basis[s[2]] >> 2);
    *d++ = (char) (basis[s[2]] << 6 | basis[s[3]]);

    s += 4;
    len -= 4;
  }

  if (len > 1) {
    *d++ = (char) (basis[s[0]] << 2 | basis[s[1]] >> 4);
  }

  if (len > 2) {
    *d++ = (char) (basis[s[1]] << 4 | basis[s[2]] >> 2);
  }

  return (d - dst);
}

#endif /* BASE64_H */
