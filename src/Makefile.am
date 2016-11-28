# Copyright (c) 2014-2016 Oleg Efimov <efimovov@gmail.com>
#
# protobuf2json-c is free software; you can redistribute it
# and/or modify it under the terms of the MIT license.
# See LICENSE for details.

lib_LTLIBRARIES = libprotobuf2json-c.la

libprotobuf2json_c_la_SOURCES = protobuf2json.c

# 1. Programs using the previous version may use the new version as drop-in replacement,
#    and programs using the new version can also work with the previous one.
#    In other words, no recompiling nor relinking is needed.
#    In this case, bump revision only, don’t touch current nor age.
# 2. Programs using the previous version may use the new version as drop-in replacement,
#    but programs using the new version may use APIs not present in the previous one.
#    In other words, a program linking against the new version may fail with “unresolved symbols”
#    if linking against the old version at runtime: set revision to 0, bump current and age.
# 3. Programs may need to be changed, recompiled, and relinked in order to use the new version.
#    Bump current, set revision and age to 0.
#
# current[:revision[:age]]
libprotobuf2json_c_la_LDFLAGS = -version-info 3:0:0

include_HEADERS = ../include/protobuf2json.h

AM_CFLAGS = -I$(top_srcdir)/include
libprotobuf2json_c_la_LIBADD =

AM_CFLAGS += $(PROTOBUF_C_INCLUDES)
libprotobuf2json_c_la_LIBADD += $(PROTOBUF_C_LIBS)
AM_CFLAGS += $(PROTOBUF_C_CFLAGS)

AM_CFLAGS += $(LIBJANSSON_INCLUDES)
libprotobuf2json_c_la_LIBADD += $(LIBJANSSON_LIBS)

AM_CFLAGS += $(MY_SANITIZE_CFLAGS)
libprotobuf2json_c_la_LIBADD += $(MY_SANITIZE_LIBS)

AM_CFLAGS += $(MY_VALGRIND_CFLAGS)
libprotobuf2json_c_la_LIBADD += $(MY_VALGRIND_LDFLAGS)

AM_CFLAGS += $(MY_COVERAGE_CFLAGS)
libprotobuf2json_c_la_LIBADD += $(MY_COVERAGE_LDFLAGS)
