dnl Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
dnl
dnl protobuf2json-c is free software; you can redistribute it
dnl and/or modify it under the terms of the MIT license.
dnl See LICENSE for details.

AC_PREREQ([2.59])
AC_INIT([libprotobuf2json], [0.0.2], [efimovov@gmail.com])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign -Wall])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

AC_ENABLE_SHARED
AC_ENABLE_STATIC

AC_PROG_CC
AC_PROG_MAKE_SET

LT_INIT

AC_CHECK_LIB([pthread], [pthread_create])
AC_CHECK_LIB([pthread], [pthread_join])

AM_CONDITIONAL([WINNT],   [AS_CASE([$host_os], [mingw*],   [true], [false])])

AC_MSG_RESULT([])

AX_PROTOBUF_C
AX_LIBJANSSON

AC_MSG_RESULT([])

AX_SANITIZE

AC_MSG_RESULT([])

AC_CONFIG_FILES([Makefile src/Makefile test/Makefile])

AC_OUTPUT
