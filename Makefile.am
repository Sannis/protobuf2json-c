# Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
#
# protobuf2json-c is free software; you can redistribute it
# and/or modify it under the terms of the MIT license.
# See LICENSE for details.

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src test

cppcheck:
	cppcheck --enable=style src/protobuf2json.c include/protobuf2json.h # --error-exitcode=1

test: check
	@./test/run-tests

benchmark: check
	@./test/run-benchmarks

# Coverage targets

if HAVE_GCOV

.PHONY: clean-gcda clean-gcno
clean-gcda:
	@echo Removing old coverage results
	-find -name '*.gcda' -print | xargs -r rm

clean-gcno:
	@echo Removing old coverage sources
	-find -name '*.gcno' -print | xargs -r rm

.PHONY: generate-coverage-html clean-coverage-html
generate-coverage-html:
	@echo Generate coverage HTML
	$(LCOV) --directory $(top_builddir)/src --no-external --capture --no-checksum --compat-libtool --output-file coverage.info
	LANG=C $(GENHTML) --output-directory coverage --title "libprotobuf2json-c code coverage" --legend --show-details coverage.info

clean-coverage-html: clean-gcda
	-$(LCOV) --directory $(top_builddir) -z
	-rm -rf coverage.info coverage

clean-local: clean-coverage-html

distclean-local: clean-gcno

endif # HAVE_GCOV
