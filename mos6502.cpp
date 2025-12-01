//
// mos6502.cpp
// 6502 CPU core emulation
// (C) R.P.Bellis 2025 -
// vim: ts=8 sw=8 noet:
//

#include <iostream>

#include "mos6502.h"

mos6502::mos6502()
{
}

mos6502::~mos6502()
{
}

void mos6502::pre_exec()
{
        insn_pc = pc;
	// print_regs();
}

void mos6502::post_exec()
{
        // Placeholder for future use
}

void mos6502::do_nmi()
{
        do_psh(pc);
	mos6502_status saved = p;
	saved.i = true;
	saved.b = false;
	do_psh((Byte)saved);
        pc = read_word(vector_nmi);
        cycles += 7;
}

void mos6502::do_irq()
{
	do_psh(pc);
	mos6502_status saved = p;
	p.i = true;
	p.b = false;
	do_psh((Byte)saved);
	pc = read_word(vector_irq);
	cycles += 7;
}

void mos6502::do_brk()
{
	do_psh((Word)(pc + 1));
	mos6502_status saved = p;
	p.i = true;
	p.b = true;
	do_psh((Byte)saved);
	pc = read_word(vector_irq);
	cycles += 7;
}

void mos6502::reset()
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

void mos6502::tick()
{
	// handle the attached devices
	USim::tick();

	// get interrupt pin states
	bool c_nmi = NMI;
	bool c_irq = IRQ;

	// check for NMI falling edge
	bool nmi_triggered = !c_nmi && nmi_previous;
	nmi_previous = c_nmi;

	// look for external interrupts
	if (nmi_triggered) {
		do_nmi();
	} else if (!c_irq && !p.i) {
                do_irq();
        }

	// remember current instruction address
	insn_pc = pc;

	// hook
	pre_exec();

	// fetch the next instruction
	fetch_instruction();

	// and process it
	execute_instruction();

	// hook
	post_exec();

	// deduct a cycle to account for the one added in USim::tick
	--cycles;
}

void mos6502::print_regs()
{
	printf("A:%02X X:%02X Y:%02X S:%02X P:%02X PC:%04X\r\n",
		a, x, y, s, (uint8_t)p, pc);
}

void mos6502::fetch_instruction()
{
	ir = fetch();
	decode_mode();
}

Byte mos6502::fetch_operand()
{
	Word m = fetch_effective_address();
	return read(m);
}

Word mos6502::fetch_effective_address()
{
	Word m;
	switch (mode) {
	case absolute:
		m = fetch_word();
		break;
	case zeropage:
		m = fetch() & 0x00ff;
		break;
	case zpindexed:
		m = (fetch() + x) & 0x00ff;
		break;
	case xindexed:
		m = fetch_word() + x;
		break;
	case yindexed:
		m = fetch_word() + y;
		break;
	case absindirect:
		{
			Word addr = fetch_word();
			// 6502 indirect JMP bug emulation
			if ((addr & 0x00ff) == 0x00ff) {
				m = read(addr) | (read(addr & 0xff00) << 8);
			} else {
				m = read_word(addr);
			}
		}
		break;
	case xindirect:
		{
			Byte zp_addr = (fetch() + x) & 0x00ff;
			m = read(zp_addr) | (read((zp_addr + 1) & 0x00ff) << 8);
		}
		break;
	case yindirect:
		{
			Byte zp_addr = fetch() & 0x00ff;
			m = (read(zp_addr) | (read((zp_addr + 1) & 0x00ff) << 8)) + y;
		}
		break;
	case immediate:
		m = pc++;
		break;
	case relative:
		{
			Word offset = extend8(fetch());
			m = pc + offset;
		}
		break;
	default:
		// implied or accumulator mode
		m = 0; // not used
		break;
	}

	return m;
}

void mos6502::decode_mode()
{
	// Decode based on opcode bits
	switch (ir & 0x1f) {
	case 0x00:
		mode = (ir & 0x80) ? immediate : (ir == 0x20 ? absolute : implied); // JSR special case
		break;
	case 0x10:
		mode = relative;
		break;
	case 0x01:
		mode = xindirect;
		break;
	case 0x11:
		mode = yindirect;
		break;
	case 0x02:
		mode = immediate;
		break;
	case 0x04: case 0x05: case 0x06:
		mode = zeropage;
		break;
	case 0x14: case 0x15: case 0x16:
		mode = zpindexed;
		break;
	case 0x08: case 0x18:
		mode = implied;
		break;
	case 0x09:
		mode = immediate;
		break;
	case 0x19:
		mode = yindexed;
		break;
	case 0x0a: case 0x1a:
		mode = (ir & 0x80) ? implied : accumulator;
		break;
	case 0x0c: case 0x0d: case 0x0e:
		mode = (ir == 0x6c) ? absindirect : absolute;	// JMP indirect special case
		break;
	case 0x1c: case 0x1d: case 0x1e:
		mode = (ir == 0xbe) ? yindexed : xindexed;	// LDX ABS,Y special case
		break;
	default:
		mode = implied;
		break;
	}
}

void mos6502::execute_instruction()
{
	switch (ir) {
		// Miscellaneous Instructions
		case 0x00:
			brk(); break;
		case 0x20:
			jsr(); break;
		case 0x40:
			rti(); break;
		case 0x60:
			rts(); break;
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
		case 0xc6: case 0xce:
		case 0xd6: case 0xde:
			dec(); break;
		case 0xe6: case 0xee:
		case 0xf6: case 0xfe:
			inc(); break;

		// Register Load and Store Instructions
		case 0x84: case 0x8c:
		case 0x94:
			sty(); break;
		case 0x86: case 0x8e:
		case 0x96:
			stx(); break;
		case 0xa0: case 0xa4: case 0xac:
		case 0xb4:
			ldy(); break;
		case 0xa2: case 0xa6: case 0xae:
		case 0xb6:
			ldx(); break;

		// Increment and Decrement Register Instructions
		case 0x88:
			dey(); break;
		case 0xc8:
			iny(); break;
		case 0xca:
			dex(); break;
		case 0xe8:
			inx(); break;

		// Register Transfer Instructions
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

const char *mos6502::disasm_opcode(Word addr)
{
	Byte ir = read(addr);
	switch (ir) {
		// Miscellaneous Instructions
		case 0x00:	return "BRK";
		case 0x20:	return "JSR";
		case 0x40:	return "RTI";
		case 0x60:	return "RTS";
		case 0x4c: case 0x6c:
				return "JMP";
		case 0xea:	return "NOP";

		// Accumulator operations
		case 0x01: case 0x05: case 0x09: case 0x0d:
		case 0x11: case 0x15: case 0x19: case 0x1d:
				return "ORA";
		case 0x21: case 0x25: case 0x29: case 0x2d:
		case 0x31: case 0x35: case 0x39: case 0x3d:
				return "AND";
		case 0x41: case 0x45: case 0x49: case 0x4d:
		case 0x51: case 0x55: case 0x59: case 0x5d:
				return "EOR";
		case 0x61: case 0x65: case 0x69: case 0x6d:
		case 0x71: case 0x75: case 0x79: case 0x7d:
				return "ADC";
		case 0x81: case 0x85: case 0x8d:
		case 0x91: case 0x95: case 0x99: case 0x9d:
				return "STA";
		case 0xa1: case 0xa5: case 0xa9: case 0xad:
		case 0xb1: case 0xb5: case 0xb9: case 0xbd:
				return "LDA";
		case 0xc1: case 0xc5: case 0xc9: case 0xcd:
		case 0xd1: case 0xd5: case 0xd9: case 0xdd:
				return "CMP";
		case 0xe1: case 0xe5: case 0xe9: case 0xed:
		case 0xf1: case 0xf5: case 0xf9: case 0xfd:
				return "SBC";

		// Flag operations
		case 0x18:	return "CLC";
		case 0x38:	return "SEC";
		case 0x58:	return "CLI";
		case 0x78:	return "SEI";
		case 0xb8:	return "CLV";
		case 0xd8:	return "CLD";
		case 0xf8:	return "SED";

		// Rotate and Shift Instructions
		case 0x06: case 0x0a: case 0x0e:
		case 0x16: case 0x1e:
				return "ASL";
		case 0x26: case 0x2a: case 0x2e:
		case 0x36: case 0x3e:
				return "ROL";
		case 0x46: case 0x4a: case 0x4e:
		case 0x56: case 0x5e:
				return "LSR";
		case 0x66: case 0x6a: case 0x6e:
		case 0x76: case 0x7e:
				return "ROR";

		// Bit Instructions
		case 0x24: case 0x2c:
				return "BIT";

		// Branch Instructions
		case 0x10:	return "BPL";
		case 0x30:	return "BMI";
		case 0x50:	return "BVC";
		case 0x70:	return "BVS";
		case 0x90:	return "BCC";
		case 0xb0:	return "BCS";
		case 0xd0:	return "BNE";
		case 0xf0:	return "BEQ";

		// Compare X/Y Instructions
		case 0xc0: case 0xc4: case 0xcc:
				return "CPY";
		case 0xe0: case 0xe4: case 0xec:
				return "CPX";

		// Increment and Decrement Memory Instructions
		case 0xc6: case 0xce:
		case 0xd6: case 0xde:
				return "DEC";
		case 0xe6: case 0xee:
		case 0xf6: case 0xfe:
				return "INC";

		// Register Load and Store Instructions
		case 0x84: case 0x8c:
		case 0x94:
				return "STX";
		case 0x86: case 0x8e:
		case 0x96:
				return "STY";
		case 0xa0: case 0xa4: case 0xac:
		case 0xb4:
				return "LDY";
		case 0xa2: case 0xa6: case 0xae:
		case 0xb6:
				return "LDX";

		// Increment and Decrement Register Instructions
		case 0x88:	return "DEY";
		case 0xc8:	return "INY";
		case 0xca:	return "DEX";
		case 0xe8:	return "INX";

		// Register Transfer Instructions
		case 0x8a:	return "TXA";
		case 0x98:	return "TYA";
		case 0x9a:	return "TXS";
		case 0xa8:	return "TAY";
		case 0xaa:	return "TAX";
		case 0xba:	return "TSX";

		// Stack Operations
		case 0x08:	return "PHP";
		case 0x28:	return "PLP";
		case 0x48:	return "PHA";
		case 0x68:	return "PLA";

		default:	return "???";
	}
}
