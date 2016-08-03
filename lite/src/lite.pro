TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += test/test.cpp

HEADERS += \
    core/convert.hpp \
    core/core.hpp \
    core/format.hpp \
    core/hashmap.hpp \
    core/hashset.hpp \
    core/index.hpp \
    core/list.hpp \
    core/manage.hpp \
    core/queue.hpp \
    core/seq.hpp \
    core/share.hpp \
    core/string.hpp \
    core/treemap.hpp \
    core/treeset.hpp \
    env/console.hpp \
    env/cs.hpp \
    env/device.hpp \
    env/env.hpp \
    env/exception.hpp \
    env/os.hpp \
    env/runtime.hpp \
    env/time.hpp \
    io/cs.hpp \
    io/file.hpp \
    io/input.hpp \
    io/output.hpp \
    test/cs.hpp \
    test/testcore.hpp \
    test/testenv.hpp \
    test/testio.hpp

LIBS += -lbfd
