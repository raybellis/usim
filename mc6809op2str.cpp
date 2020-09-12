
#include "string.h"
#include "mc6809.h"

void Instruction::decode(char* string) {
	const char* name;

	Word ir = bytes[0];
	if (ir == 0x10) {
		ir = (ir << 8) | bytes[1];
	}

	switch (ir) {
		case 0x13:
			name = "sync"; break;
		case 0x16:
			name = "lbra"; break;
		case 0x1c:
			name = "andcc"; break;
		case 0x1f:
			name = "tfr"; break;
		case 0x21:
			name = "brn"; break;
		case 0x24:
			name = "bcc"; break;
		case 0x25:
			name = "bcs"; break;
		case 0x27:
			name = "beq"; break;
		case 0x2c:
			name = "bge"; break;
		case 0x2e:
			name = "bgt"; break;
		case 0x3a:
			name = "abx"; break;
		case 0x47:
			name = "asra"; break;
		case 0x4d:
			name = "tsta"; break;
		case 0x5d:
			name = "tstb"; break;
		case 0x0d: case 0x6d: case 0x7d:
			name = "tst"; break;
		case 0x57:
			name = "asrb"; break;
		case 0x07: case 0x67: case 0x77:
			name = "asr"; break;
		case 0x22:
			name = "bhi"; break;
		case 0x2f:
			name = "ble"; break;
		case 0x23:
			name = "bls"; break;
		case 0x2d:
			name = "blt"; break;
		case 0x2b:
			name = "bmi"; break;
		case 0x26:
			name = "bne"; break;
		case 0x2a:
			name = "bpl"; break;
		case 0x20:
			name = "bra"; break;
		case 0x8d:
			name = "bsr"; break;
		case 0x17:
			name = "lbsr"; break;
		case 0x28:
			name = "bvc"; break;
		case 0x29:
			name = "bvs"; break;
		case 0x4e: case 0x4f:
			// 0x4e undocumented
			name = "clra"; break;
		case 0x5e: case 0x5f:
			// 0x5e undocumented
			name = "clrb"; break;
		case 0x0f: case 0x6f: case 0x7f:
			name = "clr"; break;
		case 0x81: case 0x91: case 0xa1: case 0xb1:
			name = "cmpa"; break;
		case 0xc1: case 0xd1: case 0xe1: case 0xf1:
			name = "cmpb"; break;
		case 0x1083: case 0x1093: case 0x10a3: case 0x10b3:
			name = "cmpd"; break;
		case 0x118c: case 0x119c: case 0x11ac: case 0x11bc:
			name = "cmps"; break;
		case 0x8c: case 0x9c: case 0xac: case 0xbc:
			name = "cmpx"; break;
		case 0x1183: case 0x1193: case 0x11a3: case 0x11b3:
			name = "cmpu"; break;
		case 0x108c: case 0x109c: case 0x10ac: case 0x10bc:
			name = "cmpy"; break;
		case 0x42: case 0x43: case 0x1042:
			// 0x42 / 0x1042 undocumented
			name = "coma"; break;
		case 0x52: case 0x53:
			// 0x52 undocumented
			name = "comb"; break;
		case 0x03: case 0x62: case 0x63: case 0x73:
			// 0x62 undocumented
			name = "com"; break;
		case 0x19:
			name = "daa"; break;
		case 0x4a: case 0x4b:
			// 0x4b undocumented
			name = "deca"; break;
		case 0x5a: case 0x5b:
			// 0x5b undocumented
			name = "decb"; break;
		case 0x0a: case 0x0b:
		case 0x6a: case 0x6b:
		case 0x7a: case 0x7b:
			// 0x0b, 0x6b, 0x7b undocumented
			name = "dec"; break;
		case 0x88: case 0x98: case 0xa8: case 0xb8:
			name = "eora"; break;
		case 0xc8: case 0xd8: case 0xe8: case 0xf8:
			name = "eorb"; break;
		case 0x1e:
			name = "exg"; break;
		case 0x4c:
			name = "inca"; break;
		case 0x5c:
			name = "incb"; break;
		case 0x0c: case 0x6c: case 0x7c:
			name = "inc"; break;
		case 0x0e: case 0x6e: case 0x7e:
			name = "jmp"; break;
		case 0x9d: case 0xad: case 0xbd:
			name = "jsr"; break;
		case 0x86: case 0x96: case 0xa6: case 0xb6:
			name = "lda"; break;
		case 0xc6: case 0xd6: case 0xe6: case 0xf6:
			name = "ldb"; break;
		case 0xcc: case 0xdc: case 0xec: case 0xfc:
			name = "ldd"; break;
		case 0x10ce: case 0x10de: case 0x10ee: case 0x10fe:
			name = "lds"; break;
		case 0xce: case 0xde: case 0xee: case 0xfe:
			name = "ldu"; break;
		case 0x8e: case 0x9e: case 0xae: case 0xbe:
			name = "ldx"; break;
		case 0x108e: case 0x109e: case 0x10ae: case 0x10be:
			name = "ldy"; break;
		case 0x32:
			name = "leas"; break;
		case 0x33:
			name = "leau"; break;
		case 0x30:
			name = "leax"; break;
		case 0x31:
			name = "leay"; break;
		case 0x48:
			name = "lsla"; break;
		case 0x58:
			name = "lslb"; break;
		case 0x08: case 0x68: case 0x78:
			name = "lsl"; break;
		case 0x44: case 0x45:
			// 0x45 undocumented
			name = "lsra"; break;
		case 0x54: case 0x55:
			// 0x55 undocumented
			name = "lsrb"; break;
		case 0x04: case 0x05:
		case 0x64: case 0x65:
		case 0x74: case 0x75:
			// 0x05, 0x65, 0x75 undocumented
			name = "lsr"; break;
		case 0x3d:
			name = "mul"; break;
		case 0x40: case 0x41:
			// 0x41 undocumented
			name = "nega"; break;
		case 0x50: case 0x51:
			// 0x51 undocumented
			name = "negb"; break;
		case 0x00: case 0x01:
		case 0x60: case 0x61:
		case 0x70: case 0x71:
			// 0x01, 0x61, 0x71 undocumented
			name = "neg"; break;
		case 0x12:
			name = "nop"; break;
		case 0x8a: case 0x9a: case 0xaa: case 0xba:
			name = "ora"; break;
		case 0xca: case 0xda: case 0xea: case 0xfa:
			name = "orb"; break;
		case 0x1a:
			name = "orcc"; break;
		case 0x34:
			name = "pshs"; break;
		case 0x36:
			name = "pshu"; break;
		case 0x35:
			name = "puls"; break;
		case 0x37:
			name = "pulu"; break;
		case 0x49:
			name = "rola"; break;	
		case 0x59:
			name = "rolb"; break;
		case 0x09: case 0x69: case 0x79:
			name = "rol"; break;
		case 0x46:
			name = "rora"; break;	
		case 0x56:
			name = "rorb"; break;
		case 0x06: case 0x66: case 0x76:
			name = "ror"; break;
		case 0x3b:
			name = "rti"; break;
		case 0x39:
			name = "rts"; break;
		case 0x82: case 0x92: case 0xa2: case 0xb2: 
			name = "sbca"; break;
		case 0xc2: case 0xd2: case 0xe2: case 0xf2: 
			name = "sbcb"; break;
		case 0x1d:
			name = "sex"; break;
		case 0x97: case 0xa7: case 0xb7:
			name = "sta"; break;
		case 0xd7: case 0xe7: case 0xf7:
			name = "stb"; break;
		case 0xdd: case 0xed: case 0xfd:
			name = "std"; break;
		case 0x10df: case 0x10ef: case 0x10ff:
			name = "sts"; break;
		case 0xdf: case 0xef: case 0xff:
			name = "stu"; break;
		case 0x9f: case 0xaf: case 0xbf:
			name = "stx"; break;
		case 0x109f: case 0x10af: case 0x10bf:
			name = "sty"; break;
		case 0x80: case 0x90: case 0xa0: case 0xb0:
			name = "suba"; break;
		case 0xc0: case 0xd0: case 0xe0: case 0xf0:
			name = "subb"; break;
		case 0x83: case 0x93: case 0xa3: case 0xb3:
			name = "subd"; break;
		case 0x3f:
			name = "swi"; break;
		case 0x89: case 0x99: case 0xa9: case 0xb9:
			name = "adca"; break;
		case 0xc9: case 0xd9: case 0xe9: case 0xf9:
			name = "adcb"; break;
		case 0x8b: case 0x9b: case 0xab: case 0xbb:
			name = "adda"; break;
		case 0xcb: case 0xdb: case 0xeb: case 0xfb:
			name = "addb"; break;
		case 0xc3: case 0xd3: case 0xe3: case 0xf3:
			name = "addd"; break;
		case 0x84: case 0x94: case 0xa4: case 0xb4:
			name = "anda"; break;
		case 0xc4: case 0xd4: case 0xe4: case 0xf4:
			name = "andb"; break;
		case 0x85: case 0x95: case 0xa5: case 0xb5:
			name = "bita"; break;
		case 0xc5: case 0xd5: case 0xe5: case 0xf5:
			name = "bitb"; break;
		case 0x1021:
			name = "lbrn"; break;
		case 0x1022:
			name = "lbhi"; break;
		case 0x1023:
			name = "lbls"; break;
		case 0x1024:
			name = "lbcc"; break;
		case 0x1025:
			name = "lbcs"; break;
		case 0x1026:
			name = "lbne"; break;
		case 0x1027:
			name = "lbeq"; break;
		case 0x1028:
			name = "lbvc"; break;
		case 0x1029:
			name = "lbvs"; break;
		case 0x102a:
			name = "lbpl"; break;
		case 0x102b:
			name = "lbmi"; break;
		case 0x102c:
			name = "lbge"; break;
		case 0x102d:
			name = "lblt"; break;
		case 0x102e:
			name = "lbgt"; break;
		case 0x102f:
			name = "lble"; break;
		case 0x103f:
			name = "swi2"; break;
		case 0x113f:
			name = "swi3"; break;
		default:
			name = "INVALID"; break;
	};
	strcpy(string, name);
}
