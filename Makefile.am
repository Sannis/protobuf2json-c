# Copyright (c) 2014-2015 Oleg Efimov <efimovov@gmail.com>
#
# protobuf2json-c is free software; you can redistribute it
# and/or modify it under the terms of the MIT license.
# See LICENSE for details.

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src test

cppcheck:
	cppcheck --enable=style src/protobuf2json.c include/protobuf2json.h --error-exitcode=2

test: check
	@MY_VALGRIND_EXEC_PREFIX@./test/run-tests

benchmark: check
	@MY_VALGRIND_EXEC_PREFIX@./test/run-benchmarks

MY_COVERAGE_DIRECTORY = $(abs_top_builddir)/src
@MY_COVERAGE_RULES@
