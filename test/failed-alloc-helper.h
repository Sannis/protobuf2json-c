/*
 * Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
 *
 * protobuf2json-c is free software; you can redistribute it
 * and/or modify it under the terms of the MIT license.
 * See LICENSE for details.
 */

#ifndef FAILED_ALLOC_HELPER_H_
#define FAILED_ALLOC_HELPER_H_

size_t failed_malloc_size;

static void* failed_malloc(size_t size) {
  if (size == failed_malloc_size) {
    return NULL;
  }

  return malloc(size);
}

static void failed_alloc_json_set(size_t new_failed_malloc_size) {
  failed_malloc_size = new_failed_malloc_size;
  json_set_alloc_funcs(failed_malloc, free);
}

static void failed_alloc_json_unset() {
  json_set_alloc_funcs(malloc, free);
}

#endif /* FAILED_ALLOC_HELPER_H_ */
