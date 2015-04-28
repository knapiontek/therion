
all : bin/spike/spike
	@printf "done.\n"
bin/spike/spike.o : src/spike/spike.cpp src/core/core/string.hpp src/core/env/console.hpp src/core/env/runtime.hpp src/core/core/seq.hpp src/core/core/index.hpp src/core/core/share.hpp src/core/env/device.hpp src/core/core/format.hpp src/core/env/env.hpp src/core/core/core.hpp src/laygen/cs.hpp src/core/core/manage.hpp src/core/env/cs.hpp src/core/core/convert.hpp src/core/thread/thread.hpp src/core/env/exception.hpp
	@printf "spike/spike.cpp\n"
	@mkdir -p bin/spike
	@g++ -c -Wall -ggdb -O0 -Isrc -Igen src/spike/spike.cpp -o bin/spike/spike.o
bin/spike/spike : bin/spike/spike.o
	@mkdir -p bin/spike
	@g++ -Wall -ggdb -rdynamic -O0 -lpthread -lbfd bin/spike/spike.o -o bin/spike/spike
clean:
	@rm -f bin/spike/spike bin/spike/spike.o
