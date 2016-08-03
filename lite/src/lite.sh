rm -rf ../bin && mkdir ../bin
g++ -c -m64 -pipe -std=c++14 -g -Wall -W -fPIE -I. -I. -o ../bin/test.o test/test.cpp
g++ -m64 -o ../bin/test ../bin/test.o -lbfd
