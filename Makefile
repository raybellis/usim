DEBUG		= -O3
CXX			= g++ --std=c++17 -Wall -Wextra -Werror -flto
CC			= gcc --std=c9x -Wall -Werror
CCFLAGS		= $(DEBUG)
CPPFLAGS	= -D_POSIX_SOURCE -I . -I usimdbg -o $(@)
LDFLAGS		= -flto

LIB_SRCS	= usim.cpp mc6809.cpp mc6809in.cpp mc6850.cpp memory.cpp

OBJS		= $(LIB_SRCS:.cpp=.o)
BIN			= usim

LIB			= libusim.a

all: $(BIN)

$(LIB): $(OBJS) # $(LIB)($(OBJS))
	ar crs $(@) $^
	ranlib $(@)

$(BIN):	$(LIB) main.o term.o
	$(CXX) $(CCFLAGS) $(LDFLAGS) main.o term.o -L. -lusim -o $(@) 

.SUFFIXES: .cpp

.cpp.o:
	$(CXX) $(CPPFLAGS) $(CCFLAGS) -c $<

$(OBJS): machdep.h

machdep: machdep.o
	$(CC) -o $(@) $(CCFLAGS) $(LDFLAGS) machdep.o

machdep.h: machdep
	./machdep $(@)

clean:
	$(RM) machdep.h machdep.o machdep $(BIN) $(OBJS) main.o term.o $(LIB) 

depend:	machdep.h
	makedepend 	$(LIB_SRCS) main.cpp term.cpp

# DO NOT DELETE THIS LINE -- make depend depends on it.

usim.o: usim.h device.h typedefs.h /usr/include/stdint.h memory.h wiring.h
usim.o: bits.h
mc6809.o: mc6809.h wiring.h usim.h device.h typedefs.h /usr/include/stdint.h
mc6809.o: memory.h bits.h machdep.h
mc6809in.o: mc6809.h wiring.h usim.h device.h typedefs.h
mc6809in.o: /usr/include/stdint.h memory.h bits.h machdep.h
mc6850.o: mc6850.h device.h typedefs.h /usr/include/stdint.h wiring.h bits.h
memory.o: memory.h device.h typedefs.h /usr/include/stdint.h
main.o: /usr/include/unistd.h /usr/include/features.h
main.o: /usr/include/stdc-predef.h mc6809.h wiring.h usim.h device.h
main.o: typedefs.h /usr/include/stdint.h memory.h bits.h machdep.h mc6850.h
main.o: term.h /usr/include/termios.h
term.o: term.h mc6850.h device.h typedefs.h /usr/include/stdint.h wiring.h
term.o: /usr/include/termios.h /usr/include/features.h
term.o: /usr/include/stdc-predef.h /usr/include/unistd.h
term.o: /usr/include/ncurses.h /usr/include/ncurses_dll.h
term.o: /usr/include/stdio.h /usr/include/unctrl.h /usr/include/curses.h
term.o: /usr/include/string.h /usr/include/strings.h
