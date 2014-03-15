/*
 * Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
 *
 * protobuf2json-c is free software; you can redistribute it
 * and/or modify it under the terms of the MIT license.
 * See LICENSE for details.
 */

#include "task.h"


TEST_IMPL(dummy_ok) {
  int r = 0;

  ASSERT(r == 0);

  RETURN_OK();
}


TEST_IMPL(dummy_todo) {
  RETURN_TODO("Implement tests");
}
