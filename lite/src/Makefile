
srcdir=$(dir $(lastword $(MAKEFILE_LIST)))
bindir=/tmp/bin

cc=clang++-3.8
cxxflags=-g -c -m64 -O0 -pipe -std=c++14 -Wall -Wextra -Wno-unused-parameter -I. -I$(bindir) -I/usr/lib/llvm-3.8/include
linkflags=-g -m64 -lbfd -lffi $(shell llvm-config-3.8 --ldflags --system-libs --libs mcjit interpreter nativecodegen)

all:
	@make -C $(srcdir) build

build:
	rm -rf $(bindir) && mkdir -p $(bindir)/lang && mkdir -p $(bindir)/test
	lemon lang/grammar.lem $(bindir)
	$(cc) test/test.cpp $(cxxflags) -o $(bindir)/test/test.o
	$(cc) $(bindir)/test/test.o $(linkflags) -o $(bindir)/test/test
	#$(bindir)/test/test ~/therion/lite
