
srcdir=`dirname $0`
bindir=/tmp/bin

all: lemon
	printf "src: $srcdir\nbin: $bindir\n"
	rm -rf $bindir && mkdir -p $bindir/lang
	cd $srcdir

$bindir/test:

$srcdir/lang/grammar.lem: $bindir/lang/grammar.h
	lemon lang/grammar.lem $bindir \
	clang++-3.8 -c -m64 -pipe -std=c++14 -g -Wall -W -fPIE -I. -I$bindir -o $bindir/test.o test/test.cpp \
	clang++-3.8 -m64 -o $bindir/test $bindir/test.o -lbfd \
	$bindir/test ~/therion/lite

$bindir/lang:
