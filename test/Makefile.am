# libprotobuf2json Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
#
# protobuf2json-c is free software; you can redistribute it
# and/or modify it under the terms of the MIT license.
# See LICENSE for details.

check_PROGRAMS = run-tests run-benchmarks

# run-tests

run_tests_SOURCES = run-tests.c \
                    test-list.h \
                    test-dummy.c \
                    runner.c \
                    runner.h \
                    task.h \
                    getrusage-helper.h

if WINNT
  run_tests_SOURCES += runner-win.c \
                       runner-win.h
else
  run_tests_SOURCES += runner-unix.c \
                       runner-unix.h
endif

run_tests_CFLAGS =
run_tests_LDADD = $(top_srcdir)/src/libprotobuf2json.la

# run-benchmarks

run_benchmarks_SOURCES = run-benchmarks.c \
                         benchmarks-list.h \
                         benchmark-dummy.c \
                         runner.c \
                         runner.h \
                         task.h \
                         getrusage-helper.h

if WINNT
  run_benchmarks_SOURCES += runner-win.c \
                            runner-win.h
else
  run_benchmarks_SOURCES += runner-unix.c \
                            runner-unix.h
endif

run_benchmarks_CFLAGS =
run_benchmarks_LDADD = $(top_srcdir)/src/libprotobuf2json.la
