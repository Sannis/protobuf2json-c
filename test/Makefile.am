# libprotobuf2json Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
#
# protobuf2json-c is free software; you can redistribute it
# and/or modify it under the terms of the MIT license.
# See LICENSE for details.

TESTS = run-tests

check_PROGRAMS = run-tests

run_tests_SOURCES = run-tests.c \
                    runner.c \
                    runner.h \
                    task.h \
                    test-list.h \
                    test-dummy.c

if WINNT
  run_tests_SOURCES += runner-win.c \
                       runner-win.h
else
  run_tests_SOURCES += runner-unix.c \
                       runner-unix.h
endif

run_tests_CFLAGS =
run_tests_LDADD = $(top_srcdir)/src/libprotobuf2json.la
