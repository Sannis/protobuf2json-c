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
  AC_ARG_WITH([libjansson], AC_HELP_STRING([--with-libjansson=DIR], [libjansson install directory]))

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
