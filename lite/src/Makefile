
srcdir=$(dir $(lastword $(MAKEFILE_LIST)))
bindir=/tmp/bin

cc=clang++-3.8
cxxflags=-g -c -m64 -pipe -std=c++14 -Wall -Wextra
linkflags=-g -m64 -lbfd

llvm_cxxflags=-Wno-unused-parameter
llvm_linkflags=$(shell llvm-config-3.8 --ldflags --system-libs --libs mcjit interpreter nativecodegen) -lffi

all:
	@make -C $(srcdir) build

build:
	rm -rf $(bindir) && mkdir -p $(bindir)/lang && mkdir -p $(bindir)/test
	lemon lang/grammar.lem $(bindir)
	$(cc) test/test.cpp $(cxxflags) -I. -I$(bindir) -o $(bindir)/test/test.o
	$(cc) lang/writer.cpp $(cxxflags) $(llvm_cxxflags) -I/usr/lib/llvm-3.8/include -o $(bindir)/lang/writer.o
	$(cc) $(bindir)/lang/writer.o $(bindir)/test/test.o $(linkflags) $(llvm_linkflags) -o $(bindir)/test/test
	$(bindir)/test/test ~/therion/lite
