
clang++-3.8 -O0 -S -emit-llvm $1 -o $1.ll
llc-3.8 -march=cpp $1.ll -o $1.ll.cpp
#uncrustify -c default.cfg $1.llc.cpp
clang++-3.8 $1.ll.cpp -g -O0 -lffi -std=c++14 $(llvm-config-3.8 --cxxflags --ldflags --system-libs --libs all) -o $1.exe
