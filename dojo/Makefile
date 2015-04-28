
source = src/NilList.cs \
	src/Dojo.cs \
	src/AST.cs \
	src/Printer.cs \
	src/Iterator.cs \
	src/Solver.cs \
	src/Mixer.cs \
	src/HTML.cs \
	src/JS.cs \
	bin/Parser.cs \
	bin/Scanner.cs

all: dojo.exe
	@echo "mono ..."
	@mono --debug bin/dojo.exe hello sample/hello.dojo bin/hello

dojo.exe: $(source)
	@echo "gmcs ..."
	@gmcs -debug $(source) -out:bin/dojo.exe

bin/Parser.cs: src/syntax
	@echo "coco ..."
	@coco/Coco.exe -namespace coco -frames coco -o bin src/syntax

clean:
	@echo "cleaning ..."
	@rm -rf bin/* `find . -type f -name "*~"` dojo.pidb dojo.userprefs
