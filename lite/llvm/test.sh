

FILE=malloc.cpp

clang++-3.8 -O0 -S -emit-llvm $FILE -o $FILE.ll
llc-3.8 -march=cpp $FILE.ll -o $FILE.ll.cpp
uncrustify -c default.cfg $FILE.ll.cpp
mv $FILE.ll.cpp.uncrustify $FILE.ll.cpp
clang++-3.8 $FILE.ll.cpp -g -O0 -lffi -std=c++14 $(llvm-config-3.8 --cxxflags --ldflags --system-libs --libs all) -o $FILE.exe
