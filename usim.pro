TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES +=\
    diskio.cc \
    main.cc \
    mc6809.cc \
    mc6809_X.cc \
    mc6809in.cc \
    mc6850.cc \
    misc.cc \
    sidforth.cc \
    term.cc \
    ttyio.cc \
    usim.cc \
    aciadevice.cc

HEADERS += \
    aciadevice.h \
    diskio.h \
    mc6809.h \
    mc6809_X.h \
    mc6850.h \
    misc.h \
    sidforth.h \
    term.h \
    ttyio.h \
    typedefs.h \
    usim.h \


LIBS += -lX11
