cmake_minimum_required(VERSION 3.17)
project(SIMDVectors)

set(CMAKE_CXX_STANDARD 14)

add_executable(SIMDVectors main.cpp src/SIMDArray.h src/AVXArray.cpp src/AVXArray.h src/alignedAllocator.h)
target_compile_options (SIMDVectors PRIVATE -mavx)
#target_compile_options (SIMDVectors PRIVATE -O3)