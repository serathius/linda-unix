#!/bin/sh
if [ ! -e gtest-1.7.0.zip ]; then
    wget https://googletest.googlecode.com/files/gtest-1.7.0.zip
fi
if [ ! -e gmock-1.7.0.zip ]; then
    wget https://googlemock.googlecode.com/files/gmock-1.7.0.zip
fi
if [ ! -d gtest-1.7.0 ]; then
    unzip gtest-1.7.0.zip
fi
if [ ! -d gmock-1.7.0 ]; then
    unzip gmock-1.7.0.zip
fi
make test
./test.o
