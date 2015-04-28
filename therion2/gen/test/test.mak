
all : bin/test/test
	@printf "done.\n"
bin/test/test.o : src/test/test.cpp src/core/thread/threadpool.hpp src/test/testcore.hpp src/core/core/list.hpp src/test/testio.hpp src/core/core/index.hpp src/core/env/device.hpp src/core/core/treemap.hpp src/core/io/input.hpp src/core/core/manage.hpp src/core/env/cs.hpp src/core/core/string.hpp src/core/io/output.hpp src/core/core/pager.hpp src/core/env/os.hpp src/core/env/env.hpp src/test/cs.hpp src/core/core/format.hpp src/core/env/console.hpp src/test/testenv.hpp src/core/core/seq.hpp src/core/core/share.hpp src/core/core/hashset.hpp src/core/thread/event.hpp src/core/core/queue.hpp src/core/io/inetstream.hpp src/core/core/hashmap.hpp src/core/thread/thread.hpp src/core/core/treeset.hpp src/core/env/exception.hpp src/core/thread/lockablequeue.hpp src/core/env/time.hpp src/test/testthread.hpp src/core/env/runtime.hpp src/core/io/inetaddress.hpp src/core/io/cs.hpp src/core/io/file.hpp src/core/core/core.hpp src/core/core/convert.hpp
	@printf "test/test.cpp\n"
	@mkdir -p bin/test
	@g++ -Wall -ggdb -O0 -c -Isrc -Igen src/test/test.cpp -o bin/test/test.o
bin/test/test : bin/test/test.o
	@mkdir -p bin/test
	@g++ -Wall -ggdb -O0 -rdynamic -lpthread -lbfd bin/test/test.o -o bin/test/test
clean:
	@rm -f bin/test/test bin/test/test.o
