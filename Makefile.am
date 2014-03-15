# libprotobuf2json Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
#
# protobuf2json-c is free software; you can redistribute it
# and/or modify it under the terms of the MIT license.
# See LICENSE for details.

ACLOCAL_AMFLAGS = -I m4

#AUTOMAKE_OPTIONS = subdir-objects

VALGRIND = valgrind --leak-check=full --smc-check=all # --error-exitcode=1

SUBDIRS = src test

test: check
	@./test/run-tests

benchmark: check
	@./test/run-benchmarks

test-valgrind: check
	@${VALGRIND} ./test/run-tests

benchmark-valgrind: check
	@${VALGRIND} ./test/run-benchmarks
