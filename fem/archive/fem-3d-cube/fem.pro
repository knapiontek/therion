
QT += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fem
TEMPLATE = app


SOURCES += main.cpp\
	mainwindow.cpp \
	input.cpp \
	render.cpp \
	engine.cpp

HEADERS += mainwindow.h\
	input.h \
	render.h \
	engine.h

QMAKE_CXXFLAGS += -std=c++11
