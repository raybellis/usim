#
# usim (C) R.P.Bellis 1993 -
# vim: set ts=8 sw=8 noet:
#
DEBUG		= -g
CXX		= g++ --std=c++20 -Wall -Wextra -Werror
CC		= gcc --std=c9x -Wall -Werror
CCFLAGS		= $(DEBUG)
CPPFLAGS	= -D_POSIX_SOURCE -I.
LDFLAGS		=

LIB_SRCS	= usim.cpp memory.cpp \
		  mc6809.cpp mc6809in.cpp \
		  mos6502.cpp mos6502in.cpp \
		  mc6850.cpp

OBJS		= $(LIB_SRCS:.cpp=.o)
BIN		= usim usim02

LIB		= libusim.a

all: $(BIN)

$(LIB): $(OBJS)
	ar crs $(@) $^
	ranlib $(@)

usim:	$(LIB) main.o term.o
	$(CXX) $(CCFLAGS) $(LDFLAGS) main.o term.o -L. -lusim -o $(@)

usim02:	$(LIB) main02.o term.o
	$(CXX) $(CCFLAGS) $(LDFLAGS) main02.o term.o -L. -lusim -o $(@)

.SUFFIXES: .cpp

.cpp.o:
	$(CXX) $(CPPFLAGS) $(CCFLAGS) -c $<

.PHONY: clean
clean:
	$(RM) $(BIN) $(OBJS) main.o term.o $(LIB)

.PHONY: depend
depend:
	makedepend 	$(LIB_SRCS) main.cpp term.cpp

# Manually defined dependencies

usim.o: usim.h device.h typedefs.h memory.h wiring.h
usim.o: bits.h
mc6809.o: mc6809.h wiring.h usim.h device.h typedefs.h
mc6809.o: memory.h bits.h
mc6809in.o: mc6809.h wiring.h usim.h device.h typedefs.h
mc6809in.o: memory.h bits.h
mc6850.o: mc6850.h device.h typedefs.h wiring.h bits.h
memory.o: memory.h device.h typedefs.h
main.o: mc6809.h wiring.h usim.h device.h
main.o: typedefs.h memory.h bits.h mc6850.h
main.o: term.h
term.o: term.h mc6850.h device.h typedefs.h wiring.h

# DO NOT DELETE THIS LINE -- make depend depends on it.

