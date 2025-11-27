//
// r6502.cpp
// 6502 CPU core emulation
// (C) R.P.Bellis 2025 -
// vim: ts=8 sw=8 noet:
//

#include "r6502.h"

void r6502::pre_exec()
{
        insn_pc = pc;
}

void r6502::post_exec()
{
        // Placeholder for future use
}

void r6502::do_nmi()
{
        do_psh(pc);
	r6502_status saved = p;
	saved.i = true;
	saved.b = false;
	do_psh((Byte)saved);
        pc = read_word(vector_nmi);
        cycles += 7;
}

void r6502::do_irq()
{
        if (!p.i) {
                do_psh(pc);
		r6502_status saved = p;
		saved.i = true;
		saved.b = false;
		do_psh((Byte)saved);
                pc = read_word(vector_irq);
                cycles += 7;
        }
}

void r6502::do_brk()
{
	do_psh((Word)(pc + 1));
	r6502_status saved = p;
	saved.i = true;
	saved.b = true;
	do_psh((Byte)saved);
	pc = read_word(vector_irq);
	cycles += 7;
}

void r6502::reset()
{
	a = 0;
	x = 0;
	y = 0;
	s = 0xfd;
	p.value = 0;
	p.i = true;	// Disable interrupts
	p.d = false;	// Decimal mode off
	pc = read_word(vector_reset);
	cycles = 7;
}

void r6502::tick()
{
	fetch_instruction();
	execute_instruction();
}

void r6502::print_regs()
{
	printf("A:%02X X:%02X Y:%02X S:%02X P:%02X PC:%04X\n",
		a, x, y, s, (uint8_t)p, pc);
}

void r6502::fetch_instruction()
{
	ir = fetch();
	decode_mode();
}

Byte r6502::fetch_operand()
{
	Word m = fetch_effective_address();
	return read(m);
}

Word r6502::fetch_effective_address()
{
	Word m;
	if (mode == absolute) {
		m = fetch_word();
	} else if (mode == zeropage) {
		m = fetch() & 0x00ff;
	} else if (mode == zpindexed) {
		m = (fetch() + x) & 0x00ff;
	} else if (mode == xindexed) {
		m = fetch_word() + x;
	} else if (mode == yindexed) {
		m = fetch_word() + y;
	} else if (mode == absindirect) {
		Word addr = fetch_word();
		// 6502 indirect JMP bug emulation
		if ((addr & 0x00ff) == 0x00ff) {
			m = read(addr) | (read(addr & 0xff00) << 8);
		} else {
			m = read_word(addr);
		}
	} else if (mode == xindirect) {
		Byte zp_addr = (fetch() + x) & 0x00ff;
		m = read(zp_addr) | (read((zp_addr + 1) & 0x00ff) << 8);
	} else if (mode == yindirect) {
		Byte zp_addr = fetch() & 0x00ff;
		m = (read(zp_addr) | (read((zp_addr + 1) & 0x00ff) << 8)) + y;
	} else if (mode == immediate) {
		m = pc++;
	} else if (mode == relative) {
		m = pc + extend8(fetch());
	} else {
		// implied or accumulator mode
		m = 0; // not used
	}

	return m;
}

void r6502::decode_mode()
{
	// Handle special cases first
	if (ir == 0x20) {		// JSR Absolute
		mode = absolute;
		return;
	} else if (ir == 0x6c) {	// JMP (indirect)
		mode = absindirect;
		return;
	} else if (ir == 0xbe)	{
		mode = yindexed;	// LDX abs,Y
		return;
	}

	// Decode based on opcode bits
	switch (ir & 0x0f) {
	case 0x00:
		if (ir & 0x10) {
			mode = relative;
		} else {
			mode = (ir & 0x80) ? immediate : implied;
		}
		break;
	case 0x01:
		mode = (ir & 0x10) ? yindirect : xindirect;
		break;
	case 0x02:
		mode = immediate;
		break;
	case 0x04:
	case 0x05:
	case 0x06:
		mode = (ir & 0x10) ? zpindexed : zeropage;
		break;
	case 0x08:
		mode = implied;
		break;
	case 0x09:
		mode = (ir & 0x10) ? yindexed : immediate;
		break;
	case 0x0a:
		mode = (ir & 0x80) ? implied : accumulator;
		break;
	case 0x0c:
	case 0x0d:
	case 0x0e:
		mode = (ir & 0x10) ? xindexed : absolute;
		break;
	}
}

void r6502::execute_instruction()
{
	switch (ir) {
		// Miscellaneous Instructions
		case 0x00:
			brk(); break;
		case 0x20:
			jsr(); break;
		case 0x4c: case 0x6c:
			jmp(); break;
		case 0xea:
			nop(); break;

		// Accumulator operations
		case 0x01: case 0x05: case 0x09: case 0x0d:
		case 0x11: case 0x15: case 0x19: case 0x1d:
			ora(); break;
		case 0x21: case 0x25: case 0x29: case 0x2d:
		case 0x31: case 0x35: case 0x39: case 0x3d:
			and_(); break;
		case 0x41: case 0x45: case 0x49: case 0x4d:
		case 0x51: case 0x55: case 0x59: case 0x5d:
			eor(); break;
		case 0x61: case 0x65: case 0x69: case 0x6d:
		case 0x71: case 0x75: case 0x79: case 0x7d:
			adc(); break;
		case 0x81: case 0x85: case 0x8d:
		case 0x91: case 0x95: case 0x99: case 0x9d:
			sta(); break;
		case 0xa1: case 0xa5: case 0xa9: case 0xad:
		case 0xb1: case 0xb5: case 0xb9: case 0xbd:
			lda(); break;
		case 0xc1: case 0xc5: case 0xc9: case 0xcd:
		case 0xd1: case 0xd5: case 0xd9: case 0xdd:
			cmp(); break;
		case 0xe1: case 0xe5: case 0xe9: case 0xed:
		case 0xf1: case 0xf5: case 0xf9: case 0xfd:
			sbc(); break;

		// Flag operations
		case 0x18:
			clc(); break;
		case 0x38:
			sec(); break;
		case 0x58:
			cli(); break;
		case 0x78:
			sei(); break;
		case 0xb8:
			clv(); break;
		case 0xd8:
			cld(); break;
		case 0xf8:
			sed(); break;

		// Rotate and Shift Instructions
		case 0x06: case 0x0a: case 0x0e:
		case 0x16: case 0x1e:
			asl(); break;
		case 0x26: case 0x2a: case 0x2e:
		case 0x36: case 0x3e:
			rol(); break;
		case 0x46: case 0x4a: case 0x4e:
		case 0x56: case 0x5e:
			lsr(); break;
		case 0x66: case 0x6a: case 0x6e:
		case 0x76: case 0x7e:
			ror(); break;

		// Bit Instructions
		case 0x24: case 0x2c:
			bit(); break;

		// Branch Instructions
		case 0x10:
			bpl(); break;
		case 0x30:
			bmi(); break;
		case 0x50:
			bvc(); break;
		case 0x70:
			bvs(); break;
		case 0x90:
			bcc(); break;
		case 0xb0:
			bcs(); break;
		case 0xd0:
			bne(); break;
		case 0xf0:
			beq(); break;

		// Compare X/Y Instructions
		case 0xc0: case 0xc4: case 0xcc:
			cpy(); break;
		case 0xe0: case 0xe4: case 0xec:
			cpx(); break;

		// Increment and Decrement Memory Instructions
		case 0xc6: case 0xce: case 0xd6: case 0xde:
			dec(); break;
		case 0xe6: case 0xee: case 0xf6: case 0xfe:
			inc(); break;

		// Increment and Decrement Register Instructions
		case 0x88:
			dey(); break;
		case 0xc8:
			iny(); break;
		case 0xca:
			dex(); break;
		case 0xe8:
			inx(); break;

		// Transfer Instructions
		case 0x8a:
			txa(); break;
		case 0x98:
			tya(); break;
		case 0x9a:
			txs(); break;
		case 0xa8:
			tay(); break;
		case 0xaa:
			tax(); break;
		case 0xba:
			tsx(); break;

		// Stack Operations
		case 0x08:
			php(); break;
		case 0x28:
			plp(); break;
		case 0x48:
			pha(); break;
		case 0x68:
			pla(); break;

		default:
			invalid("invalid instruction");

	}
}
