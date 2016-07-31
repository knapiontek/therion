clang-3.6 -g test.c -o test
clang++-3.6 -std=c++14 -S -emit-llvm test.cpp -o test
