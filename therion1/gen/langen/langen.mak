
all : bin/langen/langen
	@printf "done.\n"
bin/langen/langen.o : src/langen/langen.cpp src/core/core/index.hpp src/core/env/device.hpp src/core/core/treemap.hpp src/core/io/input.hpp src/core/core/manage.hpp src/core/env/cs.hpp src/core/core/string.hpp src/core/io/output.hpp src/core/env/env.hpp src/langen/encode.hpp src/core/core/format.hpp src/core/core/list.hpp src/core/core/treeset.hpp src/langen/model.hpp src/core/env/console.hpp src/core/core/seq.hpp src/core/core/share.hpp src/core/core/hashset.hpp src/langen/cs.hpp src/core/core/hashmap.hpp src/core/thread/thread.hpp src/core/env/exception.hpp src/langen/mangle.hpp src/langen/prototype.hpp src/core/env/runtime.hpp src/langen/machine.hpp src/core/io/cs.hpp src/core/io/file.hpp src/core/core/core.hpp src/core/core/convert.hpp
	@printf "langen/langen.cpp\n"
	@mkdir -p bin/langen
	@g++ -c -Wall -ggdb -O0 -Isrc -Igen src/langen/langen.cpp -o bin/langen/langen.o
bin/langen/langen : bin/langen/langen.o
	@mkdir -p bin/langen
	@g++ -Wall -ggdb -rdynamic -O0 -lpthread -lbfd bin/langen/langen.o -o bin/langen/langen
clean:
	@rm -f bin/langen/langen bin/langen/langen.o
