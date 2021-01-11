DEBUG		= -O0 -g
CXX		= g++ --std=c++14 -Wall -Werror -flto
CC		= gcc --std=c9x -Wall -Werror
CCFLAGS		= $(DEBUG)
CPPFLAGS	= -D_POSIX_SOURCE
LDFLAGS		= -flto

SRCS		= usim.cc misc.cc memory.cc \
		  mc6809.cc mc6809in.cc \
		  mc6850.cc term.cc \
		  main.cc
OBJS		= $(SRCS:.cc=.o)
BIN		= usim
LIBS		=

$(BIN):		$(OBJS)
	$(CXX) -o $(@) $(CCFLAGS) $(LDFLAGS) $(OBJS) $(LIBS)

.SUFFIXES:	.cc

.cc.o:
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
