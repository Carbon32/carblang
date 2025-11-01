@echo off

IF EXIST build (
    echo Removing build directory
    rmdir /s /q build
) ELSE (
    echo No existing build directory found
)

mkdir build
cd build

cmake ..
cmake --build .

echo "Running tests"
ctest  -C Debug --output-on-failure

cd ..