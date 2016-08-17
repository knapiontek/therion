
# settings
srcdir=`dirname $0`
bindir=/tmp/bin
printf "src: $srcdir\nbin: $bindir\n"

# preparing directories
rm -rf $bindir && mkdir -p $bindir/lang
cd $srcdir

# compile
lemon lang/grammar.lem $bindir \
    && clang++-3.6 -c -m64 -pipe -std=c++14 -g -Wall -W -fPIE -I. -I$bindir -o $bindir/test.o test/test.cpp \
    && clang++-3.6 -m64 -o $bindir/test $bindir/test.o -lbfd \
    && $bindir/test /home/knapiontek/therion/lite

# finish
printf "done\n"
