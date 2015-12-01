/*
 * Copyright (c) 2014-2015 Oleg Efimov <efimovov@gmail.com>
 *
 * protobuf2json-c is free software; you can redistribute it
 * and/or modify it under the terms of the MIT license.
 * See LICENSE for details.
 */

#ifndef FAILED_ALLOC_HELPER_H_
#define FAILED_ALLOC_HELPER_H_

size_t failed_malloc_size = 0;
int failed_malloc_count = 0;

static void* failed_malloc(size_t size) {
  if (failed_malloc_count < 0) {
    if (size == failed_malloc_size) {
      return NULL;
    }
  } else if (failed_malloc_count == 0) {
    return NULL;
  } else {
    failed_malloc_count--;
  }

  return malloc(size);
}

static void failed_alloc_json_set_by_size(size_t new_failed_malloc_size) {
  failed_malloc_size = new_failed_malloc_size;
  failed_malloc_count = -1;

  json_set_alloc_funcs(failed_malloc, free);
}

static void failed_alloc_json_set_by_count(int new_failed_malloc_count) {
  failed_malloc_count = new_failed_malloc_count;

  json_set_alloc_funcs(failed_malloc, free);
}

static void failed_alloc_json_unset() {
  json_set_alloc_funcs(malloc, free);
}

#endif /* FAILED_ALLOC_HELPER_H_ */
