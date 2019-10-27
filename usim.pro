TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES +=\
    main.cc \
    mc6809.cc \
    mc6809_X.cc \
    mc6809in.cc \
    mc6850.cc \
    misc.cc \
    term.cc \
    usim.cc \


HEADERS += \
    mc6809.h \
    mc6809_X.h \
    mc6850.h \
    misc.h \
    term.h \
    typedefs.h \
    usim.h

LIBS += -lX11
