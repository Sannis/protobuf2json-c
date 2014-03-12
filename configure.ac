dnl Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
dnl
dnl protobuf2json-c is free software; you can redistribute it
dnl and/or modify it under the terms of the MIT license.
dnl See LICENSE for details.

AC_PREREQ([2.59])
AC_INIT([libprotobuf2json], [0.0.1], [efimovov@gmail.com])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.11 foreign -Wall])

AC_ENABLE_SHARED
AC_ENABLE_STATIC

AC_PROG_CC
AM_PROG_AR

LT_INIT

AC_CONFIG_FILES([Makefile src/Makefile])
AC_CONFIG_SRCDIR([src])

AC_MSG_RESULT([])

AX_PROTOBUF_C
AX_LIBJANSSON

AC_MSG_RESULT([])

AC_OUTPUT
