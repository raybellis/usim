//
// hd6309.cpp
// Hitachi HD6309 emulation: mc6809 superset.
// (C) R.P.Bellis 2026 -
// vim: ts=8 sw=8 noet:
//

#include <cstdio>
#include <algorithm>

#include "hd6309.h"

hd6309::hd6309()
{
}

hd6309::~hd6309()
{
}

void hd6309::reset()
{
	w = 0;
	v = 0;
	md.value = 0;	// emulation mode after reset; no traps pending

	mc6809::reset();
}

void hd6309::print_regs()
{
	mc6809::print_regs();
	fprintf(stderr, " E:%02X F:%02X W:%04X V:%04X MD:%02X\r\n",
		e, f, w, v, md.value);
}

Word& hd6309::wordrefreg(int r)
{
	switch (r) {
		case 6: return w;
		case 7: return v;
	}
	return mc6809::wordrefreg(r);
}

Byte& hd6309::byterefreg(int r)
{
	switch (r) {
		case 12: case 13:
			// 0 register: reads as 0, writes discarded
			zero_byte = 0;
			return zero_byte;
		case 14: return e;
		case 15: return f;
	}
	return mc6809::byterefreg(r);
}

// TFR/EXG postbyte size classification: the high bit of each 4-bit code
// distinguishes 16-bit registers (bit clear) from 8-bit (bit set).
static inline bool is_word_code(int r)
{
	return (r & 0x08) == 0;
}

void hd6309::tfr()
{
	insn = "TFR";
	Byte post = fetch_operand();
	int r1 = (post & 0xf0) >> 4;
	int r2 = (post & 0x0f);

	bool w1 = is_word_code(r1);
	bool w2 = is_word_code(r2);

	if (w1 && w2) {
		wordrefreg(r2) = wordrefreg(r1);
	} else if (!w1 && !w2) {
		byterefreg(r2) = byterefreg(r1);
	} else if (w1 && !w2) {
		// 16 -> 8: low byte of source
		byterefreg(r2) = (Byte)(wordrefreg(r1) & 0xff);
	} else {
		// 8 -> 16: high byte filled with $FF (per HD6309 datasheet)
		wordrefreg(r2) = 0xff00 | byterefreg(r1);
	}

	cycles += 4;
}

void hd6309::exg()
{
	insn = "EXG";
	Byte post = fetch_operand();
	int r1 = (post & 0xf0) >> 4;
	int r2 = (post & 0x0f);

	bool w1 = is_word_code(r1);
	bool w2 = is_word_code(r2);

	if (w1 && w2) {
		std::swap(wordrefreg(r1), wordrefreg(r2));
	} else if (!w1 && !w2) {
		std::swap(byterefreg(r1), byterefreg(r2));
	} else {
		// Mismatched-size EXG. Snapshot both operands as 16-bit values
		// (extending 8-bit ones per the TFR 8->16 rule) before writing,
		// then write each side back per its native size.
		Word v1 = w1 ? wordrefreg(r1) : (Word)(0xff00 | byterefreg(r1));
		Word v2 = w2 ? wordrefreg(r2) : (Word)(0xff00 | byterefreg(r2));
		if (w1) {
			wordrefreg(r1) = v2;
		} else {
			byterefreg(r1) = (Byte)(v2 & 0xff);
		}
		if (w2) {
			wordrefreg(r2) = v1;
		} else {
			byterefreg(r2) = (Byte)(v1 & 0xff);
		}
	}

	cycles += 8;
}

//----------------------------------------------------------------------------
// Instruction dispatch — handle 6309-specific opcodes, delegate the rest.
//----------------------------------------------------------------------------

void hd6309::execute_instruction()
{
	switch (ir) {
		// LDE / LDF / LDW / LDQ
		case 0x1186: case 0x1196: case 0x11a6: case 0x11b6:
			lde(); break;
		case 0x11c6: case 0x11d6: case 0x11e6: case 0x11f6:
			ldf(); break;
		case 0x1086: case 0x1096: case 0x10a6: case 0x10b6:
			ldw(); break;
		case 0x00cd: case 0x10dc: case 0x10ec: case 0x10fc:
			// $CD is a no-prefix 1-byte opcode + 32-bit immediate
			ldq(); break;

		// STE / STF / STW / STQ (no immediate forms)
		case 0x1197: case 0x11a7: case 0x11b7:
			ste(); break;
		case 0x11d7: case 0x11e7: case 0x11f7:
			stf(); break;
		case 0x1097: case 0x10a7: case 0x10b7:
			stw(); break;
		case 0x10dd: case 0x10ed: case 0x10fd:
			stq(); break;

		// ADDE / ADDF / ADDW
		case 0x118b: case 0x119b: case 0x11ab: case 0x11bb:
			adde(); break;
		case 0x11cb: case 0x11db: case 0x11eb: case 0x11fb:
			addf(); break;
		case 0x108b: case 0x109b: case 0x10ab: case 0x10bb:
			addw(); break;

		// SUBE / SUBF / SUBW
		case 0x1180: case 0x1190: case 0x11a0: case 0x11b0:
			sube(); break;
		case 0x11c0: case 0x11d0: case 0x11e0: case 0x11f0:
			subf(); break;
		case 0x1080: case 0x1090: case 0x10a0: case 0x10b0:
			subw(); break;

		// CMPE / CMPF / CMPW
		case 0x1181: case 0x1191: case 0x11a1: case 0x11b1:
			cmpe(); break;
		case 0x11c1: case 0x11d1: case 0x11e1: case 0x11f1:
			cmpf(); break;
		case 0x1081: case 0x1091: case 0x10a1: case 0x10b1:
			cmpw(); break;

		// INC / DEC / NEG / COM / TST / CLR on accumulators
		case 0x104c: incd(); break;
		case 0x114c: ince(); break;
		case 0x115c: incf(); break;
		case 0x105c: incw(); break;

		case 0x104a: decd(); break;
		case 0x114a: dece(); break;
		case 0x115a: decf(); break;
		case 0x105a: decw(); break;

		case 0x1040: negd(); break;

		case 0x1043: comd(); break;
		case 0x1143: come(); break;
		case 0x1153: comf(); break;
		case 0x1053: comw(); break;

		case 0x104d: tstd(); break;
		case 0x114d: tste(); break;
		case 0x115d: tstf(); break;
		case 0x105d: tstw(); break;

		case 0x104f: clrd(); break;
		case 0x114f: clre(); break;
		case 0x115f: clrf(); break;
		case 0x105f: clrw(); break;

		// Shifts and rotates on D (ASLD/LSLD share opcode)
		case 0x1047: asrd(); break;
		case 0x1044: lsrd(); break;
		case 0x1048: asld(); break;
		case 0x1049: rold(); break;
		case 0x1046: rord(); break;

		// Shifts and rotates on W (ASLW/LSLW share opcode)
		case 0x1057: asrw(); break;
		case 0x1054: lsrw(); break;
		case 0x1058: aslw(); break;
		case 0x1059: rolw(); break;
		case 0x1056: rorw(); break;

		// Sign-extend W into Q
		case 0x0014: sexw(); break;

		// Push/pull W on the S and U stacks
		case 0x1038: pshsw(); break;
		case 0x1039: pulsw(); break;
		case 0x103a: pshuw(); break;
		case 0x103b: puluw(); break;

		// In-memory bit logic. The base class treats some of these
		// opcodes (e.g. $0B, $62, $6B) as undocumented NMOS variants
		// of DEC/COM/etc.; on the 6309 they are AIM/OIM/EIM/TIM and
		// must be caught here before falling through to mc6809.
		case 0x0001: case 0x0061: case 0x0071:
			oim(); break;
		case 0x0002: case 0x0062: case 0x0072:
			aim(); break;
		case 0x0005: case 0x0065: case 0x0075:
			eim(); break;
		case 0x000b: case 0x006b: case 0x007b:
			tim(); break;

		// Multiply and divide
		case 0x118f: case 0x119f: case 0x11af: case 0x11bf:
			muld(); break;
		case 0x118d: case 0x119d: case 0x11ad: case 0x11bd:
			divd(); break;
		case 0x118e: case 0x119e: case 0x11ae: case 0x11be:
			divq(); break;

		// Block transfer (interruptible: one byte per dispatch)
		case 0x1138: case 0x1139: case 0x113a: case 0x113b:
			tfm(); break;

		// CC/memory single-bit transfers (BAND/BIAND/BOR/BIOR/BEOR/
		// BIEOR/LDBT/STBT) — all share postbyte format and dispatch.
		case 0x1130: case 0x1131: case 0x1132: case 0x1133:
		case 0x1134: case 0x1135: case 0x1136: case 0x1137:
			bit_transfer(); break;

		// MD register access
		case 0x113c: bitmd(); break;
		case 0x113d: ldmd(); break;

		default:
			mc6809::execute_instruction();
			break;
	}
}

//----------------------------------------------------------------------------
// Load/store on the 6309 accumulators (E, F, W, Q).
//----------------------------------------------------------------------------

void hd6309::lde()
{
	insn = "LDE";
	help_ld(e);
}

void hd6309::ldf()
{
	insn = "LDF";
	help_ld(f);
}

void hd6309::ldw()
{
	insn = "LDW";
	help_ld(w);
}

void hd6309::ldq()
{
	insn = "LDQ";
	DWord val;
	if (mode == immediate) {
		val = fetch_dword();
	} else {
		val = read_dword(fetch_effective_address());
	}
	setq(val);
	cc.n = (val & 0x80000000u) != 0;
	cc.z = (val == 0);
	cc.v = 0;
}

void hd6309::ste()
{
	insn = "STE";
	help_st(e);
}

void hd6309::stf()
{
	insn = "STF";
	help_st(f);
}

void hd6309::stw()
{
	insn = "STW";
	help_st(w);
}

void hd6309::stq()
{
	insn = "STQ";
	DWord val = getq();
	write_dword(fetch_effective_address(), val);
	cc.n = (val & 0x80000000u) != 0;
	cc.z = (val == 0);
	cc.v = 0;
}

//----------------------------------------------------------------------------
// ADD/SUB/CMP on the 6309 accumulators (E, F, W).
//----------------------------------------------------------------------------

void hd6309::adde()
{
	insn = "ADDE";
	help_add(e);
}

void hd6309::addf()
{
	insn = "ADDF";
	help_add(f);
}

void hd6309::addw()
{
	insn = "ADDW";
	help_add(w);
}

void hd6309::sube()
{
	insn = "SUBE";
	help_sub(e);
}

void hd6309::subf()
{
	insn = "SUBF";
	help_sub(f);
}

void hd6309::subw()
{
	insn = "SUBW";
	help_sub(w);
}

void hd6309::cmpe()
{
	insn = "CMPE";
	help_cmp(e);
}

void hd6309::cmpf()
{
	insn = "CMPF";
	help_cmp(f);
}

void hd6309::cmpw()
{
	insn = "CMPW";
	help_cmp(w);
}

//----------------------------------------------------------------------------
// 16-bit forms of the unary CC-setting helpers. The 6809 has byte-only
// versions in mc6809; the 6309 needs word versions for the new D/W ops.
//----------------------------------------------------------------------------

void hd6309::help_clr(Word& x)
{
	cc.value &= 0xf0;
	cc.value |= 0x04;
	x = 0;
	++cycles;
}

void hd6309::help_com(Word& x)
{
	x = ~x;
	cc.c = 1;
	cc.v = 0;
	cc.n = btst(x, 15);
	cc.z = !x;
	++cycles;
}

void hd6309::help_dec(Word& x)
{
	cc.v = (x == 0x8000);
	x = x - 1;
	cc.n = btst(x, 15);
	cc.z = !x;
	++cycles;
}

void hd6309::help_inc(Word& x)
{
	cc.v = (x == 0x7fff);
	x = x + 1;
	cc.n = btst(x, 15);
	cc.z = !x;
	++cycles;
}

void hd6309::help_neg(Word& x)
{
	int	t = 0 - x;

	cc.v = btst((DWord)(x ^ t ^ (t >> 1)), 15);
	cc.c = btst((DWord)t, 16);
	cc.n = btst((DWord)t, 15);
	x = (Word)(t & 0xffff);
	cc.z = !x;
	++cycles;
}

void hd6309::help_tst(Word x)
{
	cc.v = 0;
	cc.n = btst(x, 15);
	cc.z = !x;
	++cycles;
}

//----------------------------------------------------------------------------
// INC/DEC/NEG/COM/TST/CLR on the 6309 accumulators.
//----------------------------------------------------------------------------

void hd6309::incd()
{
	insn = "INCD";
	help_inc(d);
}

void hd6309::ince()
{
	insn = "INCE";
	help_inc(e);
}

void hd6309::incf()
{
	insn = "INCF";
	help_inc(f);
}

void hd6309::incw()
{
	insn = "INCW";
	help_inc(w);
}

void hd6309::decd()
{
	insn = "DECD";
	help_dec(d);
}

void hd6309::dece()
{
	insn = "DECE";
	help_dec(e);
}

void hd6309::decf()
{
	insn = "DECF";
	help_dec(f);
}

void hd6309::decw()
{
	insn = "DECW";
	help_dec(w);
}

void hd6309::negd()
{
	insn = "NEGD";
	help_neg(d);
}

void hd6309::comd()
{
	insn = "COMD";
	help_com(d);
}

void hd6309::come()
{
	insn = "COME";
	help_com(e);
}

void hd6309::comf()
{
	insn = "COMF";
	help_com(f);
}

void hd6309::comw()
{
	insn = "COMW";
	help_com(w);
}

void hd6309::tstd()
{
	insn = "TSTD";
	help_tst(d);
}

void hd6309::tste()
{
	insn = "TSTE";
	help_tst(e);
}

void hd6309::tstf()
{
	insn = "TSTF";
	help_tst(f);
}

void hd6309::tstw()
{
	insn = "TSTW";
	help_tst(w);
}

void hd6309::clrd()
{
	insn = "CLRD";
	help_clr(d);
}

void hd6309::clre()
{
	insn = "CLRE";
	help_clr(e);
}

void hd6309::clrf()
{
	insn = "CLRF";
	help_clr(f);
}

void hd6309::clrw()
{
	insn = "CLRW";
	help_clr(w);
}

//----------------------------------------------------------------------------
// 16-bit forms of the shift/rotate helpers (6309-only).
//----------------------------------------------------------------------------

void hd6309::help_asr(Word& x)
{
	cc.c = btst(x, 0);
	x >>= 1;
	if ((cc.n = btst(x, 14)) != 0) {
		bset(x, 15);
	}
	cc.z = !x;
	++cycles;
}

void hd6309::help_lsl(Word& x)
{
	cc.c = btst(x, 15);
	cc.v = btst(x, 15) ^ btst(x, 14);
	x <<= 1;
	cc.n = btst(x, 15);
	cc.z = !x;
	++cycles;
}

void hd6309::help_lsr(Word& x)
{
	cc.c = btst(x, 0);
	x >>= 1;
	cc.n = 0;
	cc.z = !x;
	++cycles;
}

void hd6309::help_rol(Word& x)
{
	int oc = cc.c;
	cc.v = btst(x, 15) ^ btst(x, 14);
	cc.c = btst(x, 15);
	x = x << 1;
	if (oc) {
		bset(x, 0);
	}
	cc.n = btst(x, 15);
	cc.z = !x;
	++cycles;
}

void hd6309::help_ror(Word& x)
{
	int oc = cc.c;
	cc.c = btst(x, 0);
	x = x >> 1;
	if (oc) {
		bset(x, 15);
	}
	cc.n = btst(x, 15);
	cc.z = !x;
	++cycles;
}

//----------------------------------------------------------------------------
// Shift/rotate on D and W. ASL/LSL share opcodes so each has just one
// implementation; alias mnemonics can be picked up by the assembler.
//----------------------------------------------------------------------------

void hd6309::asld()
{
	insn = "ASLD";
	help_lsl(d);
}

void hd6309::asrd()
{
	insn = "ASRD";
	help_asr(d);
}

void hd6309::lsrd()
{
	insn = "LSRD";
	help_lsr(d);
}

void hd6309::rold()
{
	insn = "ROLD";
	help_rol(d);
}

void hd6309::rord()
{
	insn = "RORD";
	help_ror(d);
}

void hd6309::aslw()
{
	insn = "ASLW";
	help_lsl(w);
}

void hd6309::asrw()
{
	insn = "ASRW";
	help_asr(w);
}

void hd6309::lsrw()
{
	insn = "LSRW";
	help_lsr(w);
}

void hd6309::rolw()
{
	insn = "ROLW";
	help_rol(w);
}

void hd6309::rorw()
{
	insn = "RORW";
	help_ror(w);
}

//----------------------------------------------------------------------------
// Sign-extend W into Q: D becomes 0x0000 or 0xFFFF mirroring W's sign bit.
// W is left unchanged. Z and N reflect the resulting Q value (since the
// high word of Q is fully determined by W's MSB, Z = (W == 0) and N = W[15]).
//----------------------------------------------------------------------------

void hd6309::sexw()
{
	insn = "SEXW";
	d = btst(w, 15) ? 0xffff : 0x0000;
	cc.n = btst(w, 15);
	cc.z = !w;
	cycles += 4;
}

//----------------------------------------------------------------------------
// Push/pull the W register on either stack. No postbyte: the register set
// is fixed (just W), so these are simpler than the 6809 PSHS/PSHU.
//----------------------------------------------------------------------------

void hd6309::pshsw()
{
	insn = "PSHSW";
	do_psh(s, w);
	cycles += 2;
}

void hd6309::pulsw()
{
	insn = "PULSW";
	do_pul(s, w);
	cycles += 2;
}

void hd6309::pshuw()
{
	insn = "PSHUW";
	do_psh(u, w);
	cycles += 2;
}

void hd6309::puluw()
{
	insn = "PULUW";
	do_pul(u, w);
	cycles += 2;
}

//----------------------------------------------------------------------------
// In-memory bit logic: AIM, OIM, EIM, TIM.
//
// Encoding: opcode, mask byte, then a standard direct / indexed / extended
// address. The mask is fetched first; fetch_effective_address resolves the
// memory operand afterwards. AIM/OIM/EIM read-modify-write the location;
// TIM only sets flags from (mem & mask) without writing back. All four
// clear V and update N and Z from the masked result.
//----------------------------------------------------------------------------

void hd6309::aim()
{
	insn = "AIM";
	Byte mask = fetch();
	Word addr = fetch_effective_address();
	Byte val = read(addr) & mask;
	cc.n = btst(val, 7);
	cc.z = !val;
	cc.v = 0;
	write(addr, val);
}

void hd6309::oim()
{
	insn = "OIM";
	Byte mask = fetch();
	Word addr = fetch_effective_address();
	Byte val = read(addr) | mask;
	cc.n = btst(val, 7);
	cc.z = !val;
	cc.v = 0;
	write(addr, val);
}

void hd6309::eim()
{
	insn = "EIM";
	Byte mask = fetch();
	Word addr = fetch_effective_address();
	Byte val = read(addr) ^ mask;
	cc.n = btst(val, 7);
	cc.z = !val;
	cc.v = 0;
	write(addr, val);
}

void hd6309::tim()
{
	insn = "TIM";
	Byte mask = fetch();
	Word addr = fetch_effective_address();
	Byte val = read(addr) & mask;
	cc.n = btst(val, 7);
	cc.z = !val;
	cc.v = 0;
}

//----------------------------------------------------------------------------
// Signed multiply and divide.
//
// MULD: 16-bit signed D * M -> 32-bit signed result in Q (D high, W low).
// DIVD: 16-bit signed D / 8-bit signed M -> 8-bit quotient in B, remainder
//       in A. V is set on /0 or quotient overflow; registers are untouched
//       in those cases.
// DIVQ: 32-bit signed Q / 16-bit signed M -> 16-bit quotient in W, remainder
//       in D. Same overflow / divide-by-zero handling.
//
// Divide-by-zero also sets MD.dz; the actual trap dispatch is wired up in
// the illegal-opcode-trap phase.
//----------------------------------------------------------------------------

void hd6309::muld()
{
	insn = "MULD";
	int16_t multiplier = (int16_t)fetch_word_operand();
	int32_t result = (int32_t)(int16_t)d * multiplier;
	setq((DWord)(uint32_t)result);
	cc.n = (result < 0);
	cc.z = (result == 0);
	cycles += 10;
}

void hd6309::divd()
{
	insn = "DIVD";
	int8_t divisor = (int8_t)fetch_operand();
	if (divisor == 0) {
		md.dz = 1;
		cc.v = 1;
		take_trap();
		return;
	}
	int16_t dividend = (int16_t)d;
	int32_t quotient = dividend / divisor;
	if (quotient > 127 || quotient < -128) {
		// Range overflow: per datasheet, leave registers unchanged
		// and signal via V.
		cc.v = 1;
	} else {
		int8_t q = (int8_t)quotient;
		int8_t r = (int8_t)(dividend % divisor);
		b = (Byte)q;
		a = (Byte)r;
		cc.n = (q < 0);
		cc.z = (q == 0);
		cc.c = btst((Byte)q, 0);
		cc.v = 0;
	}
	cycles += 24;
}

void hd6309::divq()
{
	insn = "DIVQ";
	int16_t divisor = (int16_t)fetch_word_operand();
	if (divisor == 0) {
		md.dz = 1;
		cc.v = 1;
		take_trap();
		return;
	}
	int32_t dividend = (int32_t)getq();
	int64_t quotient = (int64_t)dividend / divisor;
	if (quotient > 32767 || quotient < -32768) {
		cc.v = 1;
	} else {
		int16_t q = (int16_t)quotient;
		int16_t r = (int16_t)(dividend % divisor);
		w = (Word)q;
		d = (Word)r;
		cc.n = (q < 0);
		cc.z = (q == 0);
		cc.c = btst((Word)q, 0);
		cc.v = 0;
	}
	cycles += 32;
}

//----------------------------------------------------------------------------
// Block transfer: TFM r+,r+ / r-,r- / r+,r / r,r+.
//
// W is the byte count; src and dst are encoded by the postbyte using the
// standard 16-bit register codes (0..7). The instruction is interruptible:
// we transfer one byte per dispatch and back PC up to the TFM opcode while
// W != 0, so any interrupt naturally lands between bytes and RTI resumes
// the loop. When W reaches 0 we leave PC pointing at the next instruction
// and the loop exits.
//----------------------------------------------------------------------------

void hd6309::tfm()
{
	insn = "TFM";
	// fetch() directly: mc6809::decode_mode classifies $113x as inherent
	// rather than immediate, so fetch_operand would route through
	// fetch_effective_address and trap.
	Byte post = fetch();
	int r1_code = (post & 0xf0) >> 4;
	int r2_code = (post & 0x0f);

	// TFM operands must be 16-bit registers (codes 0..7).
	if (r1_code > 7 || r2_code > 7) {
		invalid("invalid TFM operand");
		return;
	}

	Word& src = wordrefreg(r1_code);
	Word& dst = wordrefreg(r2_code);

	if (w == 0) {
		// Empty transfer: nothing to do but consume cycles.
		cycles += 6;
		return;
	}

	// Transfer one byte
	write(dst, read(src));

	// Adjust src/dst per variant.
	switch (ir) {
		case 0x1138:	// r+, r+
			++src; ++dst;
			break;
		case 0x1139:	// r-, r-
			--src; --dst;
			break;
		case 0x113a:	// r+, r (dst constant)
			++src;
			break;
		case 0x113b:	// r, r+ (src constant)
			++dst;
			break;
	}

	--w;

	if (w != 0) {
		// More bytes pending: rewind PC to re-fetch the TFM opcode
		// and postbyte on the next tick. The instruction is 3 bytes
		// (prefix $11, opcode, postbyte).
		pc -= 3;
	}
	cycles += 3;
}

//----------------------------------------------------------------------------
// CC/memory single-bit transfer family.
//
// Postbyte layout:
//   bits 7..6: register selector (00 = CC, 01 = A, 10 = B; 11 = invalid)
//   bits 5..3: register bit index (0..7)
//   bits 2..0: memory bit index  (0..7)
// The address byte that follows is direct mode (combined with DP).
//
// All eight ops share the same postbyte parse and the same read-of-memory;
// only the per-bit operation and write-back target differ.
//----------------------------------------------------------------------------

void hd6309::bit_transfer()
{
	Byte post = fetch();
	int r_sel = (post >> 6) & 3;
	int r_bit = (post >> 3) & 7;
	int m_bit = post & 7;

	if (r_sel == 3) {
		invalid("invalid bit-transfer register");
		return;
	}

	Byte addr_low = fetch();
	Word addr = ((Word)dp << 8) | addr_low;

	Byte& reg = (r_sel == 0) ? cc.value
		  : (r_sel == 1) ? a
		  :                b;	// r_sel == 2

	Byte mem = read(addr);
	bool r_b = btst(reg, r_bit);
	bool m_b = btst(mem, m_bit);
	bool result = false;

	switch (ir) {
	case 0x1130:
		insn = "BAND";
		result = r_b && m_b;
		break;
	case 0x1131:
		insn = "BIAND";
		result = r_b && !m_b;
		break;
	case 0x1132:
		insn = "BOR";
		result = r_b || m_b;
		break;
	case 0x1133:
		insn = "BIOR";
		result = r_b || !m_b;
		break;
	case 0x1134:
		insn = "BEOR";
		result = r_b != m_b;
		break;
	case 0x1135:
		insn = "BIEOR";
		result = r_b == m_b;
		break;
	case 0x1136:
		insn = "LDBT";
		result = m_b;
		break;
	case 0x1137:
		insn = "STBT";
		if (r_b) {
			bset(mem, m_bit);
		} else {
			bclr(mem, m_bit);
		}
		write(addr, mem);
		cycles += 7;
		return;
	}

	if (result) {
		bset(reg, r_bit);
	} else {
		bclr(reg, r_bit);
	}
	cycles += 7;
}

//----------------------------------------------------------------------------
// Native-mode interrupt stack frames.
//
// In emulation mode the entire frame is identical to the MC6809:
//     low addr  -> CC, A, B, DP, X, Y, U, PC <- high addr
// In native mode (MD.nm = 1) the frame additionally carries E and F:
//     low addr  -> CC, A, B, E, F, DP, X, Y, U, PC <- high addr
//
// FIRQ uses the entire frame when MD.fm = 1 (FIRQ "full stack") and the
// short two-byte frame (CC, PC) otherwise, matching MC6809.
//----------------------------------------------------------------------------

void hd6309::push_entire(Word& sp)
{
	do_psh(sp, pc);
	do_psh(sp, u);
	do_psh(sp, y);
	do_psh(sp, x);
	do_psh(sp, dp);
	if (md.nm) {
		do_psh(sp, f);
		do_psh(sp, e);
	}
	do_psh(sp, b);
	do_psh(sp, a);
	do_psh(sp, cc.value);
}

void hd6309::pull_entire(Word& sp)
{
	do_pul(sp, cc.value);
	do_pul(sp, a);
	do_pul(sp, b);
	if (md.nm) {
		do_pul(sp, e);
		do_pul(sp, f);
	}
	do_pul(sp, dp);
	do_pul(sp, x);
	do_pul(sp, y);
	do_pul(sp, u);
	do_pul(sp, pc);
}

void hd6309::do_nmi()
{
	if (!waiting_cwai) {
		cc.e = 1;
		push_entire(s);
	}
	cc.f = cc.i = 1;
	pc = read_word(vector_nmi);
}

void hd6309::do_irq()
{
	if (!waiting_cwai) {
		cc.e = 1;
		push_entire(s);
	}
	cc.f = cc.i = 1;
	pc = read_word(vector_irq);
}

void hd6309::do_firq()
{
	if (!waiting_cwai) {
		if (md.fm) {
			cc.e = 1;
			push_entire(s);
		} else {
			cc.e = 0;
			help_psh(0x81, s, u);
		}
	}
	cc.f = cc.i = 1;
	pc = read_word(vector_firq);
}

void hd6309::swi()
{
	insn = "SWI";
	cc.e = 1;
	push_entire(s);
	cc.f = cc.i = 1;
	pc = read_word(vector_swi);
	cycles += 4;
}

void hd6309::swi2()
{
	insn = "SWI2";
	cc.e = 1;
	push_entire(s);
	pc = read_word(vector_swi2);
	cycles += 4;
}

void hd6309::swi3()
{
	insn = "SWI3";
	cc.e = 1;
	push_entire(s);
	pc = read_word(vector_swi3);
	cycles += 4;
}

void hd6309::cwai()
{
	insn = "CWAI";
	Byte mask = fetch_operand();
	cc.value &= mask;
	cc.e = 1;
	push_entire(s);
	cycles += 2;
	waiting_cwai = true;
}

void hd6309::rti()
{
	insn = "RTI";
	// Peek at CC.e first to decide which frame to pull. The byte at
	// (s) is CC, since CC is pushed last and ends up at the lowest
	// address.
	mc6809_cc peek;
	peek.value = read(s);
	if (peek.e) {
		pull_entire(s);
	} else {
		// Short frame: CC, PC only.
		do_pul(s, cc.value);
		do_pul(s, pc);
	}
	cycles += 2;
}

//----------------------------------------------------------------------------
// MD register access.
//
// LDMD #imm  : writes the user-settable bits (NM and FM) of MD from the
//              immediate and clears the trap-status bits IL and DZ.
// BITMD #imm : tests bits in MD against the immediate. Z reflects the
//              AND; the IL and DZ bits actually tested are cleared after
//              being read (the standard mechanism for acknowledging a
//              pending trap).
//----------------------------------------------------------------------------

void hd6309::ldmd()
{
	insn = "LDMD";
	Byte v = fetch();		// immediate operand
	md.nm = btst(v, 0);
	md.fm = btst(v, 1);
	md.il = 0;
	md.dz = 0;
	cycles += 5;
}

void hd6309::bitmd()
{
	insn = "BITMD";
	Byte v = fetch();		// immediate operand
	Byte t = md.value & v;
	cc.z = (t == 0);
	// Clear the trap-status bits that were just tested.
	if (btst(v, 6)) {
		md.il = 0;
	}
	if (btst(v, 7)) {
		md.dz = 0;
	}
	cycles += 4;
}

//----------------------------------------------------------------------------
// Illegal-instruction and divide-by-zero traps.
//
// Both traps share the same dispatch: push the entire state (the trap is
// always "entire") and vector through $FFF0 with I and F set. The cause
// can be identified by the trap handler reading MD (IL=1 -> illegal,
// DZ=1 -> divide-by-zero). PC at trap entry points just past the offending
// instruction, so RTI returns to whatever follows.
//----------------------------------------------------------------------------

void hd6309::take_trap()
{
	cc.e = 1;
	push_entire(s);
	cc.f = cc.i = 1;
	pc = read_word(vector_reserved);
	cycles += 20;
}

void hd6309::invalid(const char* msg)
{
	(void)msg;
	md.il = 1;
	take_trap();
}
