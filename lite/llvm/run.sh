clang-3.6 -g test.c -o test
clang++-3.6 -std=c++14 -S -emit-llvm test.cpp -o test
clang++-3.8 -g HowToUseJIT.cpp `llvm-config-3.8 --cxxflags --ldflags --system-libs --libs all` -O0 -lffi  -o HowToUseJIT
