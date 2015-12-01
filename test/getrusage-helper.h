/*
 * Copyright (c) 2014-2015 Oleg Efimov <efimovov@gmail.com>
 *
 * protobuf2json-c is free software; you can redistribute it
 * and/or modify it under the terms of the MIT license.
 * See LICENSE for details.
 */

#ifndef GETRUSAGE_HELPER_H_
#define GETRUSAGE_HELPER_H_

#include <sys/resource.h>
#include <errno.h>

static int getrusage_helper(double *ru_stime, double *ru_utime) {
  struct rusage usage;

  if (getrusage(RUSAGE_SELF, &usage))
    return -errno;

  *ru_stime = (double)usage.ru_stime.tv_sec + (double)usage.ru_stime.tv_usec / 1000000.0;
  *ru_utime = (double)usage.ru_utime.tv_sec + (double)usage.ru_utime.tv_usec / 1000000.0;

  return 0;
}

static int getrusage_helper_sub(double *ru_stime, double *ru_utime, double ru_stime_sub, double ru_utime_sub) {
  struct rusage usage;

  if (getrusage(RUSAGE_SELF, &usage))
    return -errno;

  *ru_stime = (double)usage.ru_stime.tv_sec + (double)usage.ru_stime.tv_usec / 1000000.0 - ru_stime_sub;
  *ru_utime = (double)usage.ru_utime.tv_sec + (double)usage.ru_utime.tv_usec / 1000000.0 - ru_utime_sub;

  return 0;
}

static void getrusage_helper_printf(const char *message, double ru_stime, double ru_utime) {
  printf("%s rusage: %.5f system, %.5f user\n", message, ru_stime, ru_utime);
}

#endif /* GETRUSAGE_HELPER_H_ */
