/*
 * Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
 *
 * protobuf2json-c is free software; you can redistribute it
 * and/or modify it under the terms of the MIT license.
 * See LICENSE for details.
 */

#include "task.h"

#include <stdio.h>
#include <stdlib.h>


BENCHMARK_IMPL(dummy) {
  uint64_t start_time;
  double duration;
  uv_thread_t tid;
  int r = 0;

  start_time = uv_hrtime();

  for (i = 0; i < NUM_THREADS; i++) {
    r = uv_thread_create(&tid, thread_entry, (void *) 42);
    ASSERT(r == 0);

    r = uv_thread_join(&tid);
    ASSERT(r == 0);
  }

  duration = (uv_hrtime() - start_time) / 1e9;

  ASSERT(num_threads == NUM_THREADS);

  printf("%d threads created in %.2f seconds (%.0f/s)\n",
      NUM_THREADS, duration, NUM_THREADS / duration);

  RETURN_OK();
}
