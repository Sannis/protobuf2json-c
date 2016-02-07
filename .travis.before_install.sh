#!/bin/sh

# Copyright (c) 2014-2015 Oleg Efimov <efimovov@gmail.com>
#
# protobuf2json-c is free software; you can redistribute it
# and/or modify it under the terms of the MIT license.
# See LICENSE for details.

echo TRAVIS_OS_NAME=$TRAVIS_OS_NAME

PROTOBUF_VERSION=2.6.1
echo PROTOBUF_VERSION=$PROTOBUF_VERSION

PROTOBUF_C_VERSION=1.2.1
echo PROTOBUF_C_VERSION=$PROTOBUF_C_VERSION

echo "Installing protobuf..."
wget https://github.com/google/protobuf/releases/download/v$PROTOBUF_VERSION/protobuf-$PROTOBUF_VERSION.tar.gz
tar xf protobuf-$PROTOBUF_VERSION.tar.gz
cd protobuf-$PROTOBUF_VERSION && ./configure && make -j2 && sudo make install && sudo ldconfig

echo "Installing protobuf-c..."
wget https://github.com/protobuf-c/protobuf-c/releases/download/v$PROTOBUF_C_VERSION/protobuf-c-$PROTOBUF_C_VERSION.tar.gz
tar xf protobuf-c-$PROTOBUF_C_VERSION.tar.gz
cd protobuf-c-$PROTOBUF_C_VERSION && ./configure && make -j2 && sudo make install && sudo ldconfig

echo "Installing jansson and some tools..."
if [ "x$TRAVIS_OS_NAME" = "xosx" ]; then
	brew update
	brew install jansson
	brew install cppcheck
	if [ "x$MY_VALGRIND" = "x1" ]; then brew install valgrind; fi
else # linux
	sudo add-apt-repository --yes ppa:pi-rho/security
	sudo apt-get update
	sudo apt-get install libjansson4-dev libjansson4
	sudo apt-get install cppcheck
	if [ "x$MY_VALGRIND" = "x1" ]; then sudo apt-get install valgrind; fi
	if [ "x$MY_COVERAGE" = "x1" ]; then sudo apt-get install lcov; fi
	if [ "x$MY_COVERAGE" = "x1" ]; then sudo pip install cpp-coveralls; fi
fi
