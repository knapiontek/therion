
all : bin/test/test
	@printf "done.\n"
gen/test/sample/match.hpp : src/test/sample/match.pro bin/langen/langen src/core/io/output.hpp src/core/core/pager.hpp
	@printf "test/sample/match.pro\n"
	@mkdir -p gen/test/sample
	@bin/langen/langen -i src/test/sample/match.pro -o gen/test/sample/match.hpp
gen/test/sample/sample.hpp : src/test/sample/sample.tho bin/obgen/obgen src/core/io/file.hpp src/core/env/env.hpp
	@printf "test/sample/sample.tho\n"
	@mkdir -p gen/test/sample
	@bin/obgen/obgen -i src/test/sample/sample.tho -o gen/test/sample/sample.hpp
bin/test/test.o : src/test/test.cpp src/core/thread/threadpool.hpp src/test/testcore.hpp gen/test/sample/floatlist.hpp gen/test/sample/sample.hpp src/test/testio.hpp src/core/core/index.hpp gen/test/sample/match.hpp src/core/env/device.hpp src/core/core/treemap.hpp src/test/testobgen.hpp src/core/io/input.hpp src/core/core/manage.hpp src/core/env/cs.hpp gen/test/sample/floatplus.hpp src/core/core/string.hpp src/core/io/output.hpp src/core/core/pager.hpp src/core/env/os.hpp src/core/env/env.hpp src/service/http.hpp src/test/cs.hpp src/core/core/format.hpp src/core/core/list.hpp src/test/testenv.hpp src/core/env/console.hpp src/test/testlangen.hpp src/core/core/seq.hpp src/core/core/share.hpp src/core/core/hashset.hpp src/core/thread/event.hpp src/test/testservice.hpp src/core/io/inetstream.hpp src/core/core/hashmap.hpp src/service/cs.hpp src/core/thread/thread.hpp src/core/core/treeset.hpp src/core/env/exception.hpp src/service/solver.hpp src/core/thread/lockablequeue.hpp gen/test/sample/conflict.hpp src/test/testthread.hpp src/core/env/runtime.hpp src/core/io/inetaddress.hpp src/core/env/time.hpp src/core/io/cs.hpp src/core/io/file.hpp src/core/core/queue.hpp src/core/core/core.hpp src/core/core/convert.hpp
	@printf "test/test.cpp\n"
	@mkdir -p bin/test
	@g++ -c -Wall -ggdb -O0 -Isrc -Igen src/test/test.cpp -o bin/test/test.o
gen/test/sample/floatplus.hpp : src/test/sample/floatplus.pro bin/langen/langen src/core/io/output.hpp src/core/core/pager.hpp
	@printf "test/sample/floatplus.pro\n"
	@mkdir -p gen/test/sample
	@bin/langen/langen -i src/test/sample/floatplus.pro -o gen/test/sample/floatplus.hpp
gen/test/sample/floatlist.hpp : src/test/sample/floatlist.pro bin/langen/langen src/core/io/output.hpp src/core/core/pager.hpp
	@printf "test/sample/floatlist.pro\n"
	@mkdir -p gen/test/sample
	@bin/langen/langen -i src/test/sample/floatlist.pro -o gen/test/sample/floatlist.hpp
gen/test/sample/conflict.hpp : src/test/sample/conflict.pro bin/langen/langen src/core/io/output.hpp src/core/core/pager.hpp
	@printf "test/sample/conflict.pro\n"
	@mkdir -p gen/test/sample
	@bin/langen/langen -i src/test/sample/conflict.pro -o gen/test/sample/conflict.hpp
bin/test/test : bin/test/test.o
	@mkdir -p bin/test
	@g++ -Wall -ggdb -rdynamic -O0 -ldb -lpthread -lbfd bin/test/test.o -o bin/test/test
clean:
	@rm -f bin/test/test bin/test/test.o gen/test/sample/match.hpp gen/test/sample/sample.hpp gen/test/sample/floatplus.hpp gen/test/sample/floatlist.hpp gen/test/sample/conflict.hpp
