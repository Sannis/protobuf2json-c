/*
 * Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
 *
 * protobuf2json-c is free software; you can redistribute it
 * and/or modify it under the terms of the MIT license.
 * See LICENSE for details.
 */

#ifndef JANSSON_COMPAT_H
#define JANSSON_COMPAT_H 1

#include <jansson.h>

#ifndef json_boolean
#define json_boolean(val)      ((val) ? json_true() : json_false())
#endif

#ifndef json_object_foreach
#define json_object_foreach(object, key, value) \
  void *iter; for (iter = json_object_iter(object); \
       iter && (key = json_object_iter_key(iter)) && (value = json_object_iter_value(iter)); \
       iter = json_object_iter_next(object, iter) \
  )
#endif

#ifndef json_array_foreach
#define json_array_foreach(array, index, value) \
  for (index = 0; \
       index < json_array_size(array) && (value = json_array_get(array, index)); \
       index++ \
  )
#endif

#endif
