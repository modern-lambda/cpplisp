#!/bin/sh
mkdir build
cd build
cmake ..
make
./cpplisp
cd ..
rm -rf build