# libprotobuf2json Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
#
# protobuf2json-c is free software; you can redistribute it
# and/or modify it under the terms of the MIT license.
# See LICENSE for details.

lib_LTLIBRARIES = libprotobuf2json.la

libprotobuf2json_la_SOURCES = protobuf2json.c

AM_CFLAGS = -I$(top_srcdir)/include
libprotobuf2json_la_LIBADD =

AM_CFLAGS += $(PROTOBUF_C_INCLUDES)
libprotobuf2json_la_LIBADD += $(PROTOBUF_C_LIBS)
AM_CFLAGS += $(PROTOBUF_C_CFLAGS)

AM_CFLAGS += $(LIBJANSSON_INCLUDES)
libprotobuf2json_la_LIBADD += $(LIBJANSSON_LIBS)

AM_CFLAGS += $(SANITIZE_CFLAGS)
libprotobuf2json_la_LIBADD += $(SANITIZE_LIBS)
