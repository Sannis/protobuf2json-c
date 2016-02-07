/*
 * Copyright (c) 2014-2016 Oleg Efimov <efimovov@gmail.com>
 *
 * protobuf2json-c is free software; you can redistribute it
 * and/or modify it under the terms of the MIT license.
 * See LICENSE for details.
 */

#include "task.h"
#include "getrusage-helper.h"


BENCHMARK_IMPL(dummy) {
  double ru_stime = 0, ru_utime = 0;
  int i = 0, r = 0;

  if (getrusage_helper(&ru_stime, &ru_utime)) {
    FATAL("getrusage_helper failed");
  }

  for (i = 0; i < 1000000; i++) {
    ASSERT(r == 0);
  }

  if (getrusage_helper_sub(&ru_stime, &ru_utime, ru_stime, ru_utime)) {
    FATAL("getrusage_helper_sub failed");
  }

  getrusage_helper_printf("Dummy", ru_stime, ru_utime);

  RETURN_OK();
}
