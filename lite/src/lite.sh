srcdir=`dirname $0`
bindir=/tmp/bin
printf "src: $srcdir\nbin: $bindir\n"
rm -rf $bindir && mkdir $bindir
g++ -c -m64 -pipe -std=c++14 -g -Wall -W -fPIE -I$srcdir -o $bindir/test.o $srcdir/test/test.cpp
g++ -m64 -o $bindir/test $bindir/test.o -lbfd && $bindir/test
printf "done\n"
