
source = src/SafeList.cs \
	src/Cimply.cs \
	src/AST.cs \
	src/Printer.cs \
	src/Format.cs \
	src/CLang.cs \
	bin/Parser.cs \
	bin/Scanner.cs

all: cimply.exe
	@echo "mono ..."
	@mono --debug bin/cimply.exe hello sample/hello.cy bin/hello

cimply.exe: $(source)
	@echo "gmcs ..."
	@gmcs -debug $(source) -out:bin/cimply.exe

bin/Parser.cs: src/syntax
	@echo "coco ..."
	@coco/Coco.exe -namespace coco -frames coco -o bin src/syntax

clean:
	@echo "cleaning ..."
	@rm -rf bin/* `find . -type f -name "*~"` cimply.pidb cimply.userprefs
