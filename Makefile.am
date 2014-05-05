# libprotobuf2json-c Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
#
# protobuf2json-c is free software; you can redistribute it
# and/or modify it under the terms of the MIT license.
# See LICENSE for details.

# Autotools

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src test

test: check
	@./test/run-tests

benchmark: check
	@./test/run-benchmarks

# GYP

BUILDTYPE ?= Release

git-clone-gyp:
	@rm -rf ./gyp
	@mkdir -p ./gyp
	git clone https://chromium.googlesource.com/external/gyp.git ./gyp/gyp

gyp: git-clone-gyp
	BUILDTYPE=${BUILDTYPE} ./gyp/gyp/gyp --depth=. -Goutput_dir=. -Icommon.gypi --generator-output=./gyp/build -Dprotobuf2json_c_library=static_library -f make
	BUILDTYPE=${BUILDTYPE} make -C ./gyp/build
	cp ./gyp/build/${BUILDTYPE}/run-tests ./test/run-tests
	cp ./gyp/build/${BUILDTYPE}/run-benchmarks ./test/run-benchmarks
	cp ./gyp/build/${BUILDTYPE}/run-tmp ./test/run-tmp
