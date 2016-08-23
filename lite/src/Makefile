
srcdir=$(dir $(lastword $(MAKEFILE_LIST)))
bindir=/tmp/bin
cc=clang++-3.8

all:
	@make -C $(srcdir) build

build:
	rm -rf $(bindir) && mkdir -p $(bindir)/lang
	lemon lang/grammar.lem $(bindir)
	$(cc) -g -c -m64 -pipe -std=c++14 -Wall -Wextra -I. -I$(bindir) -o $(bindir)/test.o test/test.cpp
	$(cc) -g -m64 -lbfd -o $(bindir)/test $(bindir)/test.o
	$(bindir)/test ~/therion/lite
