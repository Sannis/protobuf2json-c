#!/bin/sh

echo $TRAVIS_OS_NAME

if [ "x$TRAVIS_OS_NAME" = "xosx" ]; then
	echo "Do nothing for now on OSX"
else # linux
	sudo add-apt-repository --yes ppa:pi-rho/security
	sudo apt-get update
	sudo apt-get install libprotobuf-c0-dev libprotobuf-c0 protobuf-c-compiler
	sudo apt-get install libjansson4-dev libjansson4
	sudo apt-get install cppcheck
	if [ "x$MY_VALGRIND" = "x1" ]; then sudo apt-get install valgrind; fi
	if [ "x$MY_COVERAGE" = "x1" ]; then sudo apt-get install lcov; fi
	if [ "x$MY_COVERAGE" = "x1" ]; then sudo pip install cpp-coveralls; fi
fi


