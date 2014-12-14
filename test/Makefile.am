# Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
#
# protobuf2json-c is free software; you can redistribute it
# and/or modify it under the terms of the MIT license.
# See LICENSE for details.

# protoc

BUILT_SOURCES =
BUILT_SOURCES += person.pb-c.c test.pb-c.c

MOSTLYCLEANFILES =
MOSTLYCLEANFILES += *.pb-c.[ch]

%.pb-c.c %.pb-c.h: %.proto
	$(PROTOBUF_C_COMPILER) --c_out=$(abs_builddir)/ -I`dirname $<` $<

# check

check_PROGRAMS = run-tests run-benchmarks run-tmp

AM_CFLAGS = -I$(top_srcdir)/include
AM_CFLAGS += $(PROTOBUF_C_INCLUDES)
AM_CFLAGS += $(PROTOBUF_C_CFLAGS)
AM_CFLAGS += $(LIBJANSSON_INCLUDES)
AM_CFLAGS += $(MY_SANITIZE_CFLAGS)
AM_CFLAGS += $(MY_VALGRIND_CFLAGS)
AM_CFLAGS += $(MY_COVERAGE_CFLAGS)

AM_LDFLAGS = -static
AM_LDFLAGS += $(MY_VALGRIND_LDFLAGS)
AM_LDFLAGS += $(MY_COVERAGE_LDFLAGS)

LDADD =
LDADD += $(PROTOBUF_C_LIBS)
LDADD += $(LIBJANSSON_LIBS)
LDADD += $(MY_SANITIZE_LIBS)
LDADD += $(top_srcdir)/src/libprotobuf2json-c.la

# run-tests

run_tests_SOURCES = run-tests.c \
                    test-list.h \
                    test-protobuf2json.c \
                    test-json2protobuf.c \
                    runner.c \
                    runner.h \
                    task.h \
                    person.pb-c.c \
                    test.pb-c.c \
                    getrusage-helper.h

if WINNT
  run_tests_SOURCES += runner-win.c \
                       runner-win.h
else
  run_tests_SOURCES += runner-unix.c \
                       runner-unix.h
endif

# run-benchmarks

run_benchmarks_SOURCES = run-benchmarks.c \
                         benchmarks-list.h \
                         benchmark-dummy.c \
                         runner.c \
                         runner.h \
                         task.h \
                         person.pb-c.c \
                         test.pb-c.c \
                         getrusage-helper.h

if WINNT
  run_benchmarks_SOURCES += runner-win.c \
                            runner-win.h
else
  run_benchmarks_SOURCES += runner-unix.c \
                            runner-unix.h
endif

# run-tmp

run_tmp_SOURCES = run-tmp.c \
                  person.pb-c.c
