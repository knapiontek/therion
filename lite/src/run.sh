g++ -c -m64 -pipe -g -Wall -W -fPIE -I. -I. -o ../bin/test.o test/test.cpp
g++ -m64 -o ../bin/test ../bin/test.o -lbfd
