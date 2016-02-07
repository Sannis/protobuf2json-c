dnl Copyright (c) 2014-2016 Oleg Efimov <efimovov@gmail.com>
dnl
dnl protobuf2json-c is free software; you can redistribute it
dnl and/or modify it under the terms of the MIT license.
dnl See LICENSE for details.

AC_DEFUN([MY_VALGRIND],
[
  dnl Check for --enable-my-valgrind
  AC_MSG_CHECKING([whether to build with valgrind checks])
  AC_ARG_ENABLE(
    [my-valgrind],
    AS_HELP_STRING([--enable-my-valgrind], [enable valgrind checks]),
    enable_my_valgrind=yes,
    enable_my_valgrind=no
  )
  AC_MSG_RESULT($enable_my_valgrind)

  MY_VALGRIND_CFLAGS=""
  MY_VALGRIND_LIBS=""
  MY_VALGRIND_EXEC_PREFIX=""

  if test "x$enable_my_valgrind" = "xyes"; then
    # To get a reasonable performance add -O1 or higher.
    # Use -g to get file names and line numbers in the warning messages.
    MY_VALGRIND_CFLAGS+=" -g -O1"
    MY_VALGRIND_LIBS+=" -g"

    MY_VALGRIND_EXEC_PREFIX="valgrind --leak-check=full --leak-resolution=med --trace-children=yes "

    AC_MSG_RESULT([valgrind: $MY_VALGRIND_EXEC_PREFIX])
  fi

  AC_SUBST([MY_VALGRIND_CFLAGS])
  AC_SUBST([MY_VALGRIND_LIBS])
  AC_SUBST([MY_VALGRIND_EXEC_PREFIX])
])
