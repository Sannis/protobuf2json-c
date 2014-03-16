dnl Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
dnl
dnl protobuf2json-c is free software; you can redistribute it
dnl and/or modify it under the terms of the MIT license.
dnl See LICENSE for details.

AC_DEFUN([AX_PROTOBUF_C],
[
  AC_ARG_WITH([protobuf_c], AC_HELP_STRING([--with-protobuf-c=DIR], [protobuf-c install directory]))

  PROTOBUF_C_INCLUDES=""
  PROTOBUF_C_CFLAGS=""
  PROTOBUF_C_LIBS=""
  PROTOBUF_C_COMPILER=""

  have_protobuf_c=false
  if test "$1" != "optional" -o \( "$with_protobuf_c" != "no" -a -n "$with_protobuf_c" \) ; then
    AC_MSG_CHECKING([for protobuf-c headers])

    protobuf_c_path=""
    for path in "$with_protobuf_c" /usr/local /usr /opt/local /opt ; do
      if test -r "$path/include/google/protobuf-c/protobuf-c.h" -a "$path/bin/protoc-c" ; then
        protobuf_c_path=$path
        break
      fi
    done

    if test -n "$protobuf_c_path" ; then
      have_protobuf_c=true
      AC_MSG_RESULT([$protobuf_c_path])
      PROTOBUF_C_INCLUDES="-I$protobuf_c_path/include"
      PROTOBUF_C_LIBS="-L$protobuf_c_path/lib -lprotobuf-c"
      PROTOBUF_C_CFLAGS="-DPRINT_UNPACK_ERRORS=0"
      PROTOBUF_C_COMPILER="$path/bin/protoc-c"
    else
      AC_MSG_ERROR([protobuf_c headers not found])
    fi
  fi

  AM_CONDITIONAL([ENABLE_PROTOBUF_C], [test "x$have_protobuf_c" = "xtrue"])

  if test "x$have_protobuf_c" = "xtrue" ; then
    AC_MSG_RESULT([protobuf_c: INCLUDES=$PROTOBUF_C_INCLUDES, LIBS=$PROTOBUF_C_LIBS, CFLAGS=$PROTOBUF_C_CFLAGS, PROTOC-C=$PROTOBUF_C_COMPILER])
  fi

  AC_SUBST([PROTOBUF_C_INCLUDES])
  AC_SUBST([PROTOBUF_C_LIBS])
  AC_SUBST([PROTOBUF_C_CFLAGS])
  AC_SUBST([PROTOBUF_C_COMPILER])
])

AC_DEFUN([AX_LIBJANSSON],
[
  AC_ARG_WITH([libjanson], AC_HELP_STRING([--with-libjansson=DIR], [libjansson install directory]))

  LIBJANSSON_INCLUDES=""
  LIBJANSSON_LIBS=""

  have_libjansson=false
  if test "$1" != "optional" -o \( "$with_libjansson" != "no" -a -n "$with_libjansson" \) ; then
    AC_MSG_CHECKING([for libjansson headers])

    libjansson_path=""
    for dir in "$with_libjansson" /usr/local /usr /opt/local /opt ; do
      if test -r "$dir/include/jansson.h" ; then
        libjansson_path="$dir"
        break
      fi
    done

    if test -n "$libjansson_path" ; then
      have_libjansson=true
      AC_MSG_RESULT([$libjansson_path])
      LIBJANSSON_INCLUDES="-I$libjansson_path/include"
      LIBJANSSON_LIBS="-L$libjansson_path/lib -ljansson"
    else
      AC_MSG_ERROR([libjansson headers not found])
    fi
  fi

  AM_CONDITIONAL([ENABLE_LIBJANSSON], [test "x$have_libjansson" = "xtrue"])

  if test "x$have_libjansson" = "xtrue" ; then
    AC_MSG_RESULT([libjansson: INCLUDES=$LIBJANSSON_INCLUDES, LIBS=$LIBJANSSON_LIBS])
  fi

  AC_SUBST([LIBJANSSON_INCLUDES])
  AC_SUBST([LIBJANSSON_LIBS])
])

AC_ARG_VAR(SANITIZE, [Enable -fsanitize=..., should be one of address, thread, memory or undefined])
AC_DEFUN([AX_SANITIZE],
[

  SANITIZE_CFLAGS=""
  SANITIZE_LIBS=""

  # ASAN
  if test "x$SANITIZE" = "xaddress" ; then
    SANITIZE_CFLAGS+="-fsanitize=address"
    SANITIZE_LIBS+="-fsanitize=address"

    AC_MSG_RESULT([sanitize: -fsanitize=address])
  fi

  # TSAN (not now)
  if test "x$SANITIZE" = "xthread" ; then
    SANITIZE_CFLAGS+="-fsanitize=thread"
    SANITIZE_LIBS+="-fsanitize=thread"

    AC_MSG_RESULT([sanitize: -fsanitize=thread])
  fi

  # MSAN
  if test "x$SANITIZE" = "xmemory" ; then
    SANITIZE_CFLAGS+="-fsanitize=memory -fsanitize-memory-track-origins"
    SANITIZE_LIBS+="-fsanitize=memory -fsanitize-memory-track-origins"

    AC_MSG_RESULT([sanitize: -fsanitize=memory])
  fi

  # UBSAN
  if test "x$SANITIZE" = "xundefined" ; then
    SANITIZE_CFLAGS+="-fsanitize=undefined"
    SANITIZE_LIBS+="-fsanitize=undefined"

    AC_MSG_RESULT([sanitize: -fsanitize=undefined])
  fi

  if test -n "$SANITIZE"; then
    if test "x$SANITIZE_CFLAGS" = "x"; then
      AC_MSG_ERROR([sanitize: unknown option $SANITIZE $SANITIZE_CFLAGS])
    else
      # To get a reasonable performance add -O1 or higher.
      # Use -g to get file names and line numbers in the warning messages.
      SANITIZE_CFLAGS+=" -g -O1"
      SANITIZE_LIBS+=" -g"

      # To get meaningful stack traces in error messages add -fno-omit-frame-pointer.
      # To get perfect stack traces you may need to disable inlining (just use -O1)
      # and tail call elimination (-fno-optimize-sibling-calls).
      SANITIZE_CFLAGS+=" -fno-omit-frame-pointer -fno-optimize-sibling-calls"
    fi
  fi

  AC_SUBST([SANITIZE_CFLAGS])
  AC_SUBST([SANITIZE_LIBS])
])
