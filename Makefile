DEBUG		= -O3
CXX			= g++ --std=c++17 -Wall -Wextra -Werror -flto
CC			= gcc --std=c9x -Wall -Werror
CCFLAGS		= $(DEBUG)
CPPFLAGS	= -D_POSIX_SOURCE
LDFLAGS		= -flto=auto

LIB_SRCS	= usim.cpp mc6809.cpp mc6809in.cpp mc6850.cpp wd1770.cpp memory.cpp

SRCS		= ${LIB_SRCS} main.cpp term.cpp
OBJS		= $(SRCS:.cpp=.o)
BIN			= usim

LIBS		=

$(BIN):		$(OBJS)
	$(CXX) -o $(@) $(CCFLAGS) $(LDFLAGS) $(OBJS) $(LIBS)

.SUFFIXES:	.cpp

.cpp.o:
	$(CXX) $(CPPFLAGS) $(CCFLAGS) -c $<

$(OBJS):	machdep.h

machdep:	machdep.o
	$(CC) -o $(@) $(CCFLAGS) $(LDFLAGS) machdep.o

machdep.h:	machdep
	./machdep $(@)

clean:
	$(RM) -f machdep.h machdep.o machdep $(BIN) $(OBJS)

depend:		machdep.h
	makedepend $(SRCS)

# DO NOT DELETE THIS LINE -- make depend depends on it.

usim.o: usim.h device.h typedefs.h machdep.h
mc6809.o: mc6809.h usim.h device.h wiring.h typedefs.h machdep.h bits.h
mc6809in.o: mc6809.h usim.h device.h typedefs.h machdep.h bits.h
memory.o: memory.h device.h typedefs.h
mc6850.o: mc6850.h device.h wiring.h typedefs.h term.h bits.h
term.o: mc6850.h term.h typedefs.h
main.o: mc6809.h usim.h device.h typedefs.h machdep.h mc6850.h term.h
main.o: memory.h
