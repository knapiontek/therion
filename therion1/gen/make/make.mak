
all : bin/make/make
	@printf "done.\n"
bin/make/make.o : src/make/make.cpp src/core/core/index.hpp src/core/env/device.hpp src/core/io/input.hpp src/core/core/manage.hpp src/core/env/cs.hpp src/core/core/string.hpp src/core/io/output.hpp gen/make/include.hpp src/core/core/pager.hpp src/core/env/env.hpp src/core/core/format.hpp src/core/core/list.hpp src/core/env/console.hpp src/core/core/seq.hpp src/core/core/share.hpp src/core/core/hashmap.hpp src/core/thread/thread.hpp src/core/env/exception.hpp gen/make/makefile.hpp src/core/env/runtime.hpp src/core/io/cs.hpp src/core/io/file.hpp src/core/core/core.hpp src/core/core/convert.hpp src/make/cs.hpp
	@printf "make/make.cpp\n"
	@mkdir -p bin/make
	@g++ -c -Wall -ggdb -O0 -Isrc -Igen src/make/make.cpp -o bin/make/make.o
gen/make/makefile.hpp : src/make/makefile.pro bin/langen/langen src/core/io/output.hpp src/core/core/pager.hpp
	@printf "make/makefile.pro\n"
	@mkdir -p gen/make
	@bin/langen/langen -i src/make/makefile.pro -o gen/make/makefile.hpp
gen/make/include.hpp : src/make/include.pro bin/langen/langen src/core/io/output.hpp src/core/core/pager.hpp
	@printf "make/include.pro\n"
	@mkdir -p gen/make
	@bin/langen/langen -i src/make/include.pro -o gen/make/include.hpp
bin/make/make : bin/make/make.o
	@mkdir -p bin/make
	@g++ -Wall -ggdb -rdynamic -O0 -lpthread -lbfd bin/make/make.o -o bin/make/make
clean:
	@rm -f bin/make/make bin/make/make.o gen/make/makefile.hpp gen/make/include.hpp
