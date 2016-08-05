TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++14
TARGET = test
LIBS += -lbfd

SOURCES += test/test.cpp

HEADERS += \
    core/core.hpp \
    core/util.hpp \
    core/convert.hpp \
    core/iterator.hpp \
    core/string.hpp \
    core/format.hpp \
    core/hashmap.hpp \
    core/hashset.hpp \
    core/index.hpp \
    core/list.hpp \
    core/manage.hpp \
    core/queue.hpp \
    core/seq.hpp \
    core/share.hpp \
    core/treemap.hpp \
    core/treeset.hpp \
    env/env.hpp \
    env/util.hpp \
    env/console.hpp \
    env/device.hpp \
    env/exception.hpp \
    env/os.hpp \
    env/runtime.hpp \
    env/time.hpp \
    io/io.hpp \
    io/file.hpp \
    io/input.hpp \
    io/output.hpp \
    test/testcore.hpp \
    test/testenv.hpp \
    test/testio.hpp
