#!/bin/bash

if [ -d "build" ]; then
    echo "Removing build directory"
    rm -rf build
else
    echo "No existing build directory found"
fi

mkdir build
cd build

cmake ..
cmake --build .

echo "Running tests"
ctest  -C Debug --output-on-failure

cd ..