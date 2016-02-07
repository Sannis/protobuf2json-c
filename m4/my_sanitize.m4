dnl Copyright (c) 2014-2016 Oleg Efimov <efimovov@gmail.com>
dnl
dnl protobuf2json-c is free software; you can redistribute it
dnl and/or modify it under the terms of the MIT license.
dnl See LICENSE for details.

AC_DEFUN([MY_SANITIZE],
[
  dnl Check for --enable-my-sanitize
  AC_MSG_CHECKING([whether to build with sanitizers])
  AC_ARG_ENABLE(
    [my-sanitize],
    AS_HELP_STRING(
      [--enable-my-sanitize=SANITIZE],
      [enable -fsanitize=SANITIZE with some extra flags, SANITIZE should be one of: address, undefined, integer, memory or thread]
    ),
    enable_my_sanitize=$enableval,
    enable_my_sanitize=no
  )
  AC_MSG_RESULT($enable_my_sanitize)

  MY_SANITIZE_CFLAGS=""
  MY_SANITIZE_LIBS=""

  # AddressSanitizer
  if test "x$enable_my_sanitize" = "xaddress" ; then
    MY_SANITIZE_CFLAGS+="-fsanitize=address"
    MY_SANITIZE_LIBS+="-fsanitize=address"

    AC_MSG_RESULT([sanitize: -fsanitize=address])
  fi

  # -fsanitize=undefined
  if test "x$enable_my_sanitize" = "xundefined" ; then
    MY_SANITIZE_CFLAGS+="-fsanitize=undefined"
    MY_SANITIZE_LIBS+="-fsanitize=undefined"

    AC_MSG_RESULT([sanitize: -fsanitize=undefined])
  fi

  # -fsanitize=integer
  # -fsanitize=integer-divide-by-zero
  # -fsanitize=signed-integer-overflow
  if test "x$enable_my_sanitize" = "xinteger" ; then
    MY_SANITIZE_CFLAGS+="-fsanitize=integer -fsanitize=integer-divide-by-zero -fsanitize=signed-integer-overflow"
    MY_SANITIZE_LIBS+="-fsanitize=integer -fsanitize=integer-divide-by-zero -fsanitize=signed-integer-overflow"

    AC_MSG_RESULT([sanitize: -fsanitize=integer -fsanitize=integer-divide-by-zero -fsanitize=signed-integer-overflow])
  fi

  # MemorySanitizer
  # MemorySanitizer requires that all program code is instrumented.
  # This also includes any libraries that the program depends on, even libc.
  # Failing to achieve this may result in false reports.
  if test "x$enable_my_sanitize" = "xmemory" ; then
    MY_SANITIZE_CFLAGS+="-fsanitize=memory -fsanitize-memory-track-origins"
    MY_SANITIZE_LIBS+="-fsanitize=memory -fsanitize-memory-track-origins"

    AC_MSG_RESULT([sanitize: -fsanitize=memory  -fsanitize-memory-track-origins])
  fi

  # ThreadSanitizer
  if test "x$enable_my_sanitize" = "xthread" ; then
    MY_SANITIZE_CFLAGS+="-fsanitize=thread"
    MY_SANITIZE_LIBS+="-fsanitize=thread"

    AC_MSG_RESULT([sanitize: -fsanitize=thread])
  fi

  if test "x$enable_my_sanitize" != "xno" ; then
    if test "x$MY_SANITIZE_CFLAGS" = "x"; then
      AC_MSG_ERROR([sanitize: unknown option '$enable_my_sanitize'])
    else
      # To get a reasonable performance add -O1 or higher.
      # Use -g to get file names and line numbers in the warning messages.
      MY_SANITIZE_CFLAGS+=" -g -O1"
      MY_SANITIZE_LIBS+=" -g"

      # To get meaningful stack traces in error messages add -fno-omit-frame-pointer.
      # To get perfect stack traces you may need to disable inlining (just use -O1)
      # and tail call elimination (-fno-optimize-sibling-calls).
      MY_SANITIZE_CFLAGS+=" -fno-omit-frame-pointer -fno-optimize-sibling-calls"
    fi
  fi

  AC_SUBST([MY_SANITIZE_CFLAGS])
  AC_SUBST([MY_SANITIZE_LIBS])
])
