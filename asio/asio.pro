QT += core widgets
TARGET = asio
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++11
HEADERS += point.hpp \
    paint.hpp \
    diagram.hpp
SOURCES += asio.cpp
