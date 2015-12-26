QT += core widgets
TARGET = fem
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++11
INCLUDEPATH += ../util
SOURCES += data.cpp populate.cpp calculate.cpp rotate.cpp fem.cpp
HEADERS += paint.hpp
