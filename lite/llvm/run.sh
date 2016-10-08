clang-3.6 -g test.c -o test
clang++-3.6 -std=c++14 -S -emit-llvm test.cpp -o test
clang++-3.8 -g HowToUseJIT.cpp `llvm-config-3.8 --cxxflags --ldflags --system-libs --libs all` -O0 -lffi  -o HowToUseJIT

clang++-3.8 -O0 -std=c++14 -S -emit-llvm malloc.cpp -o malloc.ll
llc -march=cpp malloc.cpp -o malloc.llc.cpp
llc -march=cpp test.ll
uncrustify -c default.cfg malloc.llc.cpp

clang++-3.8 -O0 -std=c++14 -S -emit-llvm float.cpp -o float.ll
llc-3.8 -march=cpp float.ll -o float.llc.cpp
clang++-3.8 float.llc.cpp -O0 -std=c++14 $(llvm-config-3.8 --cxxflags --ldflags --system-libs --libs all) -o float.llc
