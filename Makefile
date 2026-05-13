#
# usim (C) R.P.Bellis 1993 -
# vim: set ts=8 sw=8 noet:
#
DEBUG		= -g
CXX		= g++ --std=c++20 -Wall -Wextra -Werror
CC		= gcc --std=c9x -Wall -Werror
CCFLAGS		= $(DEBUG)
CPPFLAGS	= -D_POSIX_SOURCE -I. -Icore -Icpu/6809 -Icpu/65xx -Iperipherals
LDFLAGS		=

LIB_SRCS	= core/usim.cpp core/memory.cpp \
		  cpu/6809/mc6809.cpp cpu/6809/mc6809in.cpp \
		  cpu/65xx/mos6502.cpp cpu/65xx/mos6502in.cpp \
		  cpu/65xx/base65c02.cpp \
		  peripherals/mc6850.cpp

OBJS		= $(LIB_SRCS:.cpp=.o)
BIN		= usim09 usim02 usim65c02 tests/test6502 tests/test6809

LIB		= libusim.a

all: $(BIN)

$(LIB): $(OBJS)
	ar crs $(@) $^
	ranlib $(@)

usim09:	$(LIB) cpu/6809/main09.o peripherals/term.o
	$(CXX) $(CCFLAGS) $(LDFLAGS) cpu/6809/main09.o peripherals/term.o -L. -lusim -o $(@)

usim02:	$(LIB) cpu/65xx/main02.o peripherals/term.o
	$(CXX) $(CCFLAGS) $(LDFLAGS) cpu/65xx/main02.o peripherals/term.o -L. -lusim -o $(@)

usim65c02: $(LIB) cpu/65xx/main65c02.o peripherals/term.o
	$(CXX) $(CCFLAGS) $(LDFLAGS) cpu/65xx/main65c02.o peripherals/term.o -L. -lusim -o $(@)

tests/test6502: $(LIB) cpu/65xx/test6502.o
	$(CXX) $(CCFLAGS) $(LDFLAGS) cpu/65xx/test6502.o -L. -lusim -o $(@)

cpu/65xx/test6502.o: cpu/65xx/test6502.cpp cpu/65xx/mos6502.h core/usim.h core/memory.h core/wiring.h \
		  core/device.h core/typedefs.h core/bits.h
	$(CXX) $(CPPFLAGS) $(CCFLAGS) -c cpu/65xx/test6502.cpp -o $(@)

tests/test6809: $(LIB) cpu/6809/test6809.o
	$(CXX) $(CCFLAGS) $(LDFLAGS) cpu/6809/test6809.o -L. -lusim -o $(@)

cpu/6809/test6809.o: cpu/6809/test6809.cpp cpu/6809/mc6809.h core/usim.h core/memory.h core/wiring.h \
		  core/device.h core/typedefs.h core/bits.h
	$(CXX) $(CPPFLAGS) $(CCFLAGS) -c cpu/6809/test6809.cpp -o $(@)

tests/test6809.bin: cpu/6809/test6809.asm
	asm6809 -B -o $(@) $(<)

.PHONY: test
test: tests/test6502 tests/test6809 tests/test6809.bin
	tests/test6502
	tests/test6809

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CCFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(RM) $(BIN) $(LIB) *.o core/*.o cpu/6809/*.o cpu/65xx/*.o peripherals/*.o tests/*.o tests/test6809.bin

.PHONY: depend
depend:
	makedepend 	$(LIB_SRCS)

# Manually defined dependencies

core/usim.o: core/usim.h core/device.h core/typedefs.h core/memory.h core/wiring.h
core/usim.o: core/bits.h
core/memory.o: core/memory.h core/device.h core/typedefs.h
cpu/6809/mc6809.o: cpu/6809/mc6809.h core/wiring.h core/usim.h core/device.h core/typedefs.h
cpu/6809/mc6809.o: core/memory.h core/bits.h
cpu/6809/mc6809in.o: cpu/6809/mc6809.h core/wiring.h core/usim.h core/device.h core/typedefs.h
cpu/6809/mc6809in.o: core/memory.h core/bits.h
cpu/65xx/mos6502.o: cpu/65xx/mos6502.h core/wiring.h core/usim.h core/device.h core/typedefs.h
cpu/65xx/mos6502.o: core/memory.h core/bits.h
cpu/65xx/mos6502in.o: cpu/65xx/mos6502.h core/wiring.h core/usim.h core/device.h core/typedefs.h
cpu/65xx/mos6502in.o: core/memory.h core/bits.h
cpu/65xx/base65c02.o: cpu/65xx/base65c02.h cpu/65xx/mos6502.h core/wiring.h core/usim.h core/device.h core/typedefs.h
cpu/65xx/base65c02.o: core/memory.h core/bits.h
peripherals/mc6850.o: peripherals/mc6850.h core/device.h core/typedefs.h core/wiring.h core/bits.h
cpu/6809/main09.o: cpu/6809/mc6809.h core/wiring.h core/usim.h core/device.h
cpu/6809/main09.o: core/typedefs.h core/memory.h core/bits.h peripherals/mc6850.h
cpu/6809/main09.o: peripherals/term.h
cpu/65xx/main02.o: cpu/65xx/mos6502.h core/wiring.h core/usim.h core/device.h
cpu/65xx/main02.o: core/typedefs.h core/memory.h core/bits.h peripherals/mc6850.h
cpu/65xx/main02.o: peripherals/term.h
cpu/65xx/main65c02.o: cpu/65xx/base65c02.h cpu/65xx/mos6502.h core/wiring.h core/usim.h core/device.h
cpu/65xx/main65c02.o: core/typedefs.h core/memory.h core/bits.h peripherals/mc6850.h
cpu/65xx/main65c02.o: peripherals/term.h
peripherals/term.o: peripherals/term.h peripherals/mc6850.h core/device.h core/typedefs.h core/wiring.h

# DO NOT DELETE THIS LINE -- make depend depends on it.

