
all : bin/laygen/laygen
	@printf "done.\n"
gen/laygen/laygen.hpp : src/laygen/laygen.pro bin/langen/langen src/core/io/output.hpp src/core/core/pager.hpp
	@printf "laygen/laygen.pro\n"
	@mkdir -p gen/laygen
	@bin/langen/langen -i src/laygen/laygen.pro -o gen/laygen/laygen.hpp
bin/laygen/laygen.o : src/laygen/laygen.cpp src/core/core/index.hpp src/core/env/device.hpp gen/laygen/dojo.hpp src/core/io/input.hpp src/core/core/manage.hpp src/core/env/cs.hpp src/core/core/string.hpp src/core/io/output.hpp src/core/core/pager.hpp src/core/env/env.hpp src/core/core/format.hpp src/core/core/list.hpp src/core/env/console.hpp src/core/core/seq.hpp src/core/core/share.hpp src/core/thread/thread.hpp src/core/env/exception.hpp src/core/env/runtime.hpp src/core/io/cs.hpp src/core/io/file.hpp src/core/core/core.hpp src/laygen/cs.hpp gen/laygen/laygen.hpp src/core/core/convert.hpp
	@printf "laygen/laygen.cpp\n"
	@mkdir -p bin/laygen
	@g++ -c -Wall -ggdb -O0 -Isrc -Igen src/laygen/laygen.cpp -o bin/laygen/laygen.o
gen/laygen/dojo.hpp : src/laygen/dojo.pron bin/langen/langen src/core/io/output.hpp src/core/core/pager.hpp
	@printf "laygen/dojo.pron\n"
	@mkdir -p gen/laygen
	@bin/langen/langen -n -i src/laygen/dojo.pron -o gen/laygen/dojo.hpp
bin/laygen/laygen : bin/laygen/laygen.o
	@mkdir -p bin/laygen
	@g++ -Wall -ggdb -rdynamic -O0 -lpthread -lbfd bin/laygen/laygen.o -o bin/laygen/laygen
clean:
	@rm -f bin/laygen/laygen bin/laygen/laygen.o gen/laygen/laygen.hpp gen/laygen/dojo.hpp
