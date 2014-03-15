# libprotobuf2json Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
#
# protobuf2json-c is free software; you can redistribute it
# and/or modify it under the terms of the MIT license.
# See LICENSE for details.

# protoc

person.pb-c.c: person.proto
	$(PROTOBUF_C_COMPILER) --c_out=$(top_srcdir)/test person.proto

# check

check_PROGRAMS = run-tests run-benchmarks

AM_CFLAGS = -I$(top_srcdir)/include
AM_CFLAGS += $(PROTOBUF_C_INCLUDES)
AM_CFLAGS += $(PROTOBUF_C_CFLAGS)
AM_CFLAGS += $(LIBJANSSON_INCLUDES)

# run-tests

run_tests_SOURCES = run-tests.c \
                    test-list.h \
                    test-protobuf2json.c \
                    runner.c \
                    runner.h \
                    task.h \
                    person.pb-c.c \
                    getrusage-helper.h

if WINNT
  run_tests_SOURCES += runner-win.c \
                       runner-win.h
else
  run_tests_SOURCES += runner-unix.c \
                       runner-unix.h
endif

run_tests_LDADD =
run_tests_LDADD += $(PROTOBUF_C_LIBS)
run_tests_LDADD += $(LIBJANSSON_LIBS)

run_tests_LDADD = $(top_srcdir)/src/libprotobuf2json.la

# run-benchmarks

run_benchmarks_SOURCES = run-benchmarks.c \
                         benchmarks-list.h \
                         benchmark-dummy.c \
                         runner.c \
                         runner.h \
                         task.h \
                         person.pb-c.c \
                         getrusage-helper.h

if WINNT
  run_benchmarks_SOURCES += runner-win.c \
                            runner-win.h
else
  run_benchmarks_SOURCES += runner-unix.c \
                            runner-unix.h
endif

run_benchmarks_LDADD =
run_benchmarks_LDADD += $(PROTOBUF_C_LIBS)
run_benchmarks_LDADD += $(LIBJANSSON_LIBS)

run_benchmarks_LDADD = $(top_srcdir)/src/libprotobuf2json.la
