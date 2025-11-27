#include "r6502.h"

void r6502::adc()
{
        insn = "ADC";
        auto val = fetch_operand();
        uint16_t sum = (uint16_t)a + (uint16_t)val + (uint16_t)(p.c ? 1 : 0);
        p.c = (sum > 0xff);
        Byte result = (Byte)(sum & 0xff);
        p.z = (result == 0);
        p.n = (result & 0x80) != 0;
        p.v = (~((uint16_t)a ^ (uint16_t)val) & ((uint16_t)a ^ (uint16_t)result) & 0x80) != 0;
        a = result;
}

void r6502::and_()
{
        insn = "AND";
        Byte val = fetch_operand();
        a = a & val;
        p.z = (a == 0);
        p.n = (a & 0x80) != 0;
}

void r6502::asl()
{
        insn = "ASL";
        auto m = fetch_effective_address();
        Byte val = read(m);
        p.c = (val & 0x80) != 0;
        val <<= 1;
        write(m, val);
        p.z = (val == 0);
        p.n = (val & 0x80) != 0;
}

void r6502::bcc()
{
        insn = "BCC";
        do_br("BCC", !p.c);
}

void r6502::bcs()
{
        insn = "BCS";
        do_br("BCS", p.c);
}

void r6502::beq()
{
        insn = "BEQ";
        do_br("BEQ", p.z);
}

void r6502::bit()
{
        insn = "BIT";
        auto m = fetch_effective_address();
        Byte val = read(m);
        Byte result = a & val;
        p.z = (result == 0);
        p.n = (val & 0x80) != 0;
        p.v = (val & 0x40) != 0;
}

void r6502::bmi()
{
        insn = "BMI";
        do_br("BMI", p.n);
}

void r6502::bne()
{
        insn = "BNE";
        do_br("BNE", !p.z);
}

void r6502::bpl()
{
        insn = "BPL";
        do_br("BPL", !p.n);
}

void r6502::brk()
{
        insn = "BRK";
        // Placeholder for BRK instruction implementation
}

void r6502::bvc()
{
        insn = "BVC";
        do_br("BVC", !p.v);
}

void r6502::bvs()
{
        insn = "BVS";
        do_br("BVS", p.v);
}

void r6502::clc()
{
        insn = "CLC";
        p.c = false;
}

void r6502::cld()
{
        insn = "CLD";
        p.d = false;
}

void r6502::cli()
{
        insn = "CLI";
        p.i = false;
}

void r6502::clv()
{
        insn = "CLV";
        p.v = false;
}

void r6502::help_cmp(Byte reg, Byte val)
{
    uint16_t diff = (uint16_t)reg - (uint16_t)val;
    p.c = (reg >= val);
    Byte result = (Byte)(diff & 0xff);
    p.z = (result == 0);
    p.n = (result & 0x80) != 0;
    p.v = (((uint16_t)reg ^ (uint16_t)val) & ((uint16_t)reg ^ (uint16_t)result) & 0x80) != 0;
}

void r6502::cmp()
{
        insn = "CMP";
        help_cmp(a, fetch_operand());
}

void r6502::cpx()
{
        insn = "CPX";
        help_cmp(x, fetch_operand());
}

void r6502::cpy()
{
        insn = "CPY";
        help_cmp(y, fetch_operand());
}

void r6502::dec()
{
        insn = "DEC";
        auto m = fetch_effective_address();
        Byte val = read(m);
        --val;
        write(m, val);
        p.z = (val == 0);
        p.n = (val & 0x80) != 0;
        // cycles += ...;
}

void r6502::dex()
{
        insn = "DEX";
        --x;
        p.z = (x == 0);
        p.n = (x & 0x80) != 0;
}

void r6502::dey()
{
        insn = "DEY";
        --y;
        p.z = (y == 0);
        p.n = (y & 0x80) != 0;
}

void r6502::eor()
{
        insn = "EOR";
        Byte val = fetch_operand();
        a = a ^ val;
        p.z = (a == 0);
        p.n = (a & 0x80) != 0;
}

void r6502::inc()
{
        insn = "INC";
        auto m = fetch_effective_address();
        Byte val = read(m);
        ++val;
        write(m, val);
        p.z = (val == 0);
        p.n = (val & 0x80) != 0;
}

void r6502::inx()
{
        insn = "INX";
        ++x;
        p.z = (x == 0);
        p.n = (x & 0x80) != 0;
}

void r6502::iny()
{
        insn = "INY";
        ++y;
        p.z = (y == 0);
        p.n = (y & 0x80) != 0;
}

void r6502::jmp()
{
        insn = "JMP";
        pc = fetch_effective_address();
}

void r6502::jsr()
{
        insn = "JSR";
        Word addr = fetch_effective_address();
        do_psh((Word)(pc - 1));
        pc = addr;
}

void r6502::help_ld(Byte& reg)
{
        reg = fetch_operand();
        p.z = (reg == 0);
        p.n = (reg & 0x80) != 0;
}

void r6502::lda()
{
        insn = "LDA";
        help_ld(a);
}

void r6502::ldx()
{
        insn = "LDX";
        help_ld(x);
}

void r6502::ldy()
{
        insn = "LDY";
        help_ld(y);
}

void r6502::lsr()
{
        insn = "LSR";
        auto m  = fetch_effective_address();
        Byte val = read(m);
        p.c = (val & 0x01) != 0;
        val >>= 1;
        write(m, val);
        p.z = (val == 0);
        p.n = false;
}

void r6502::nop()
{
        insn = "NOP";
}

void r6502::ora()
{
        insn = "ORA";
        Byte val = fetch_operand();
        a = a | val;
        p.z = (a == 0);
        p.n = (a & 0x80) != 0;
}

void r6502::pha()
{
        insn = "PHA";
        do_psh(a);
}

void r6502::php()
{
        insn = "PHP";
        do_psh(p.value);
}

void r6502::pla()
{
        insn = "PLA";
        do_pul(a);
        p.z = (a == 0);
        p.n = (a & 0x80) != 0;
}

void r6502::plp()
{
        insn = "PLP";
        do_pul(p.value);
}

void r6502::rol()
{
        insn = "ROL";
        auto m = fetch_effective_address();
        Byte val = read(m);
        bool old_c = p.c;
        p.c = (val & 0x80) != 0;
        val = (val << 1) | (old_c ? 0x01 : 0x00);
        write(m, val);
        p.z = (val == 0);
        p.n = (val & 0x80) != 0;
}

void r6502::ror()
{
        insn = "ROR";
        auto m = fetch_effective_address();
        Byte val = read(m);
        bool old_c = p.c;
        p.c = (val & 0x01) != 0;
        val = (val >> 1) | (old_c ? 0x80 : 0x00);
        write(m, val);
        p.z = (val == 0);
        p.n = (val & 0x80) != 0;
}

void r6502::rti()
{
        insn = "RTI";
        do_pul(p.value);
        do_pul(pc);
}

void r6502::rts()
{
        insn = "RTS";
        do_pul(pc);
        ++pc;
}

void r6502::sbc()
{
        insn = "SBC";
        auto val = fetch_operand();
        uint16_t diff = (uint16_t)a - (uint16_t)val - (uint16_t)(p.c ? 0 : 1);
        p.c = (diff < 0x100);
        Byte result = (Byte)(diff & 0xff);
        p.z = (result == 0);
        p.n = (result & 0x80) != 0;
        p.v = (((uint16_t)a ^ (uint16_t)val) & ((uint16_t)a ^ (uint16_t)result) & 0x80) != 0;
        a = result;
}

void r6502::sec()
{
        insn = "SEC";
        p.c = true;
}

void r6502::sed()
{
        insn = "SED";
        p.d = true;
}

void r6502::sei()
{
        insn = "SEI";
        p.i = true;
}

void r6502::sta()
{
        insn = "STA";
        auto m = fetch_effective_address();
        write(m, a);
}

void r6502::stx()
{
        insn = "STX";
        auto m = fetch_effective_address();
        write(m, x);
}

void r6502::sty()
{
        insn = "STY";
        auto m = fetch_effective_address();
        write(m, y);
}

void r6502::tax()
{
        insn = "TAX";
        x = a;
        p.z = (x == 0);
        p.n = (x & 0x80) != 0;
}

void r6502::tay()
{
        insn = "TAY";
        y = a;
        p.z = (y == 0);
        p.n = (y & 0x80) != 0;
}

void r6502::txa()
{
        insn = "TXA";
        a = x;
        p.z = (a == 0);
        p.n = (a & 0x80) != 0;
}

void r6502::tya()
{
        insn = "TYA";
        a = y;
        p.z = (a == 0);
        p.n = (a & 0x80) != 0;
}

void r6502::tsx()
{
        insn = "TSX";
        x = s;
        p.z = (x == 0);
        p.n = (x & 0x80) != 0;
}

void r6502::txs()
{
        // flags are not affected
        insn = "TXS";
        s = x;
}
