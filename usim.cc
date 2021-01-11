//
//
//	usim.cc
//
//	(C) R.P.Bellis 1994
//
//

#include <cstdlib>
#include <cstdio>
#include "usim.h"

//----------------------------------------------------------------------------
// Generic processor run state routines
//----------------------------------------------------------------------------
void USim::run(void)
{
	halted = false;
	while (!halted) {
		tick();
	}
}

void USim::tick(void)
{
	// update all attached devices
	for (auto& d : devices) {
		d.device->tick();
	}
}

void USim::halt(void)
{
	halted = true;
}

Byte USim::fetch(void)
{
	Byte		val = read(pc);
	pc += 1;

	return val;
}

Word USim::fetch_word(void)
{
	Word		val = read_word(pc);
	pc += 2;

	return val;
}

void USim::invalid(const char *msg)
{
	fprintf(stderr, "\r\ninvalid %s : pc = [%04x], ir = [%04x]\r\n",
		msg ? msg : "",
		pc, ir);
	halt();
}

//----------------------------------------------------------------------------
// Device handling
//----------------------------------------------------------------------------

void USim::attach(Device& dev, Word base, Word mask)
{
	devices.push_back(DeviceEntry { &dev, base, mask });
}

// Single byte read
Byte USim::read(Word offset)
{
	for (auto& d : devices) {
		if ((offset & d.mask) == d.base) {
			return d.device->read(offset - d.base);
		}
	}
	return 0xff;
}

// Single byte write
void USim::write(Word offset, Byte val)
{
	for (auto& d : devices) {
		if ((offset & d.mask) == d.base) {
			d.device->write(offset - d.base, val);
			break;
		}
	}
}

//----------------------------------------------------------------------------
// Processor loading routines
//----------------------------------------------------------------------------
static Byte fread_byte(FILE *fp)
{
	char			str[3];
	long			l;

	str[0] = fgetc(fp);
	str[1] = fgetc(fp);
	str[2] = '\0';

	l = strtol(str, NULL, 16);
	return (Byte)(l & 0xff);
}

static Word fread_word(FILE *fp)
{
	Word		ret;

	ret = fread_byte(fp);
	ret <<= 8;
	ret |= fread_byte(fp);

	return ret;
}

void USim::load_intelhex(const char *filename)
{
	FILE		*fp;
	int		done = 0;

	fp = fopen(filename, "r");
	if (!fp) {
		perror("filename");
		exit(EXIT_FAILURE);
	}

	while (!done) {
		Byte		n, t;
		Word		addr;
		Byte		b;

		(void)fgetc(fp);
		n = fread_byte(fp);
		addr = fread_word(fp);
		t = fread_byte(fp);
		if (t == 0x00) {
			while (n--) {
				b = fread_byte(fp);
				write(addr++, b);
			}
		} else if (t == 0x01) {
			pc = addr;
			done = 1;
		}
		// Read and discard checksum byte
		(void)fread_byte(fp);
		if (fgetc(fp) == '\r') (void)fgetc(fp);
	}
}

//----------------------------------------------------------------------------
// Word memory access routines for big-endian (Motorola type)
//----------------------------------------------------------------------------

Word USimMotorola::read_word(Word offset)
{
	Word		tmp;

	tmp = read(offset++);
	tmp <<= 8;
	tmp |= read(offset);

	return tmp;
}

void USimMotorola::write_word(Word offset, Word val)
{
	write(offset++, (Byte)(val >> 8));
	write(offset, (Byte)val);
}

//----------------------------------------------------------------------------
// Word memory access routines for little-endian (Intel type)
//----------------------------------------------------------------------------

Word USimIntel::read_word(Word offset)
{
	Word		tmp;

	tmp = read(offset++);
	tmp |= (read(offset) << 8);

	return tmp;
}

void USimIntel::write_word(Word offset, Word val)
{
	write(offset++, (Byte)val);
	write(offset, (Byte)(val >> 8));
}
