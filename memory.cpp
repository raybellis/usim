//
//	memory.c
//	(C) R.P.Bellis 2021 - 2025
//
//      vim: ts=8 sw=8 noet:
//

#include <cstdio>
#include <cstdlib>
#include "memory.h"

Byte fread_hex_byte(FILE *fp)
{
	char			str[3];
	long			l;

	str[0] = fgetc(fp);
	str[1] = fgetc(fp);
	str[2] = '\0';

	l = strtol(str, NULL, 16);
	return (Byte)(l & 0xff);
}

Word fread_hex_word(FILE *fp)
{
	Word		ret;

	ret = fread_hex_byte(fp);
	ret <<= 8;
	ret |= fread_hex_byte(fp);

	return ret;
}

void ROM::load_intelhex(const char *filename, Word base)
{
	FILE		*fp;
	int		done = 0;

	fp = fopen(filename, "r");
	if (!fp) {
		perror("filename");
		exit(EXIT_FAILURE);
	}

	while (!done && !feof(fp)) {
		Byte		n, t;
		Word		addr;
		Byte		b;

		(void)fgetc(fp);		// colon
		n = fread_hex_byte(fp);		// byte count
		addr = fread_hex_word(fp);	// start memory address
		t = fread_hex_byte(fp);		// record type

		while (n--) {
			b = fread_hex_byte(fp);	// data byte
			if (t == 0x00) {
				if ((addr >= base) && (addr < ((DWord)base + size))) {
					memory[addr - base] = b;
				}
				++addr;
			} else if (t == 0x01) {
				done = 1;
			}
		}

		// Read and discard checksum byte
		(void)fread_hex_byte(fp);
		if (fgetc(fp) == '\r') (void)fgetc(fp);
	}
}
