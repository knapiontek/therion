QT += core widgets
TARGET = cgm
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++11
HEADERS += point.hpp \
    paint.hpp \
    sparse.hpp \
    data.hpp \
    convert.hpp \
    tetrahedron.hpp \
    solve-tetrahedron.hpp \
    pipe.hpp \
    solve-pipe.hpp \
    test.hpp \
    doc.hpp \
    gen-pipe.hpp \
    clip.hpp
SOURCES += cgm.cpp
