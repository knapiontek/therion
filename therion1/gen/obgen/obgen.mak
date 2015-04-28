
all : bin/obgen/obgen
	@printf "done.\n"
gen/obgen/obgen.hpp : src/obgen/obgen.pro bin/langen/langen src/core/io/output.hpp src/core/core/pager.hpp
	@printf "obgen/obgen.pro\n"
	@mkdir -p gen/obgen
	@bin/langen/langen -i src/obgen/obgen.pro -o gen/obgen/obgen.hpp
gen/obgen/db.hpp : src/obgen/db.pron bin/langen/langen src/core/io/output.hpp src/core/core/pager.hpp
	@printf "obgen/db.pron\n"
	@mkdir -p gen/obgen
	@bin/langen/langen -n -i src/obgen/db.pron -o gen/obgen/db.hpp
bin/obgen/obgen.o : src/obgen/obgen.cpp src/core/core/index.hpp src/core/env/device.hpp src/core/io/input.hpp src/core/core/manage.hpp src/core/env/cs.hpp src/core/core/string.hpp src/core/io/output.hpp src/core/core/pager.hpp src/core/env/console.hpp src/core/env/env.hpp gen/obgen/obgen.hpp src/core/core/format.hpp src/core/core/list.hpp src/obgen/cs.hpp src/core/core/seq.hpp src/core/core/share.hpp src/core/core/hashmap.hpp src/core/thread/thread.hpp src/core/env/exception.hpp src/core/env/time.hpp src/core/env/runtime.hpp src/core/io/cs.hpp src/core/io/file.hpp src/core/core/core.hpp src/core/core/convert.hpp gen/obgen/db.hpp
	@printf "obgen/obgen.cpp\n"
	@mkdir -p bin/obgen
	@g++ -c -Wall -ggdb -O0 -Isrc -Igen src/obgen/obgen.cpp -o bin/obgen/obgen.o
bin/obgen/obgen : bin/obgen/obgen.o
	@mkdir -p bin/obgen
	@g++ -Wall -ggdb -rdynamic -O0 -lpthread -lbfd bin/obgen/obgen.o -o bin/obgen/obgen
clean:
	@rm -f bin/obgen/obgen bin/obgen/obgen.o gen/obgen/obgen.hpp gen/obgen/db.hpp
