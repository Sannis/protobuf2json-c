/*
 * Copyright (c) 2014-2016 Oleg Efimov <efimovov@gmail.com>
 *
 * protobuf2json-c is free software; you can redistribute it
 * and/or modify it under the terms of the MIT license.
 * See LICENSE for details.
 */

#ifndef BITMAP_H
#define BITMAP_H 1

typedef unsigned char bitmap_word_t;
typedef bitmap_word_t* bitmap_t;

#define bitmap_word_t_bits (8 * sizeof(bitmap_word_t))
#define bitmap_words_needed(size)  (((size) + (bitmap_word_t_bits - 1)) / bitmap_word_t_bits)

bitmap_t bitmap_alloc(int size)
{
  return (bitmap_t)calloc(bitmap_words_needed(size), sizeof(bitmap_word_t));
}

void bitmap_free(bitmap_t bitmap)
{
  free(bitmap);
}

void bitmap_set(bitmap_t bitmap, unsigned int i) {
    bitmap[i / bitmap_word_t_bits] |= (1 << (i & (bitmap_word_t_bits - 1)));
}

int bitmap_get(bitmap_t bitmap, unsigned int i) {
    return (bitmap[i / bitmap_word_t_bits] & (1 << (i & (bitmap_word_t_bits - 1)))) ? 1 : 0;
}

#endif /* BITMAP_H */
