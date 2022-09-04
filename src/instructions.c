#include "instructions.h"
#include "debug.h"

static inline void adjust_zero_and_negative_flag(Machine *m, u8 value) {
    m->cpu.P.Z = value == 0;
    m->cpu.P.N = (value & 0x80) == 0x80;
}

static void relative_branch(Machine *m, bool should_branch) {
    u8 rel = machine_read_byte(m, m->cpu.PC++);
    if (should_branch) {
        m->cpu.PC = m->cpu.PC + (s8) rel;
    }
}

static void add(Machine *m, u8 value) {
    if(m->cpu.P.D) {
        cpu_error_marker(m, __FILE__, __LINE__);
        cpu_error(m, "decimal mode not supported");
    }

    u8 carry = m->cpu.P.C;
    u8 a = m->cpu.A;
    u16 result = a + value + carry;

    m->cpu.P.C = result > 0xFF;
    adjust_zero_and_negative_flag(m, result & 0x00FF);
    u16 overflow = (~(a ^ value) & (a ^ result)) & 0x0080;
    m->cpu.P.V = overflow == 0x0080;
    m->cpu.A = result & 0xFF;
}

void adc(Machine *m, AddrMode addr_mode) {
    u8 value = machine_read_byte_with_mode(m, addr_mode);
    add(m, value);
}

void and(Machine *m, AddrMode addr_mode) {
    u8 mem = machine_read_byte_with_mode(m, addr_mode);
    u8 result = m->cpu.A & mem;
    adjust_zero_and_negative_flag(m, result);
    m->cpu.A = result;
}

void asl(Machine *m, AddrMode addr_mode) {
    u16 addr;
    u8 value;
    if (addr_mode == Accumulator) {
        value = m->cpu.A;
    } else {
        addr = machine_fetch_address(m, addr_mode);
        value = machine_read_byte(m, addr);
    }

    m->cpu.P.C = (value & 0x80) == 0x80;
    value = value << 1;
    adjust_zero_and_negative_flag(m, value);

    if (addr_mode == Accumulator) {
        m->cpu.A = value;
    } else {
        machine_write_byte(m, addr, value);
    }
}

void bcc(Machine *m, AddrMode UNUSED addr_mode) {
    relative_branch(m, !m->cpu.P.C);
}

void bcs(Machine *m, AddrMode UNUSED addr_mode) {
    relative_branch(m, m->cpu.P.C);
}

void bit(Machine *m, AddrMode addr_mode) {
    u8 mem = machine_read_byte_with_mode(m, addr_mode);
    m->cpu.P.N = (mem & 0x80) == 0x80;
    m->cpu.P.V = (mem & 0x40) == 0x40;

    m->cpu.P.Z = (m->cpu.A & mem) == 0x00;
}

void bmi(Machine *m, AddrMode UNUSED addr_mode) {
    relative_branch(m, m->cpu.P.N);
}

void beq(Machine *m, AddrMode UNUSED addr_mode) {
    relative_branch(m, m->cpu.P.Z);
}

void bne(Machine *m, AddrMode UNUSED addr_mode) {
    relative_branch(m, !m->cpu.P.Z);
}

void bpl(Machine *m, AddrMode UNUSED addr_mode) {
    relative_branch(m, !m->cpu.P.N);
}

void brk(Machine *m, AddrMode UNUSED addr_mode) {
    u8 low = machine_read_byte(m, 0xFFFE);
    u8 high = machine_read_byte(m, 0xFFFF);

    m->cpu.PC++; // BRK is a two byte command
    machine_push_program_counter_on_stack(m);

    php(m, Implied);

    m->cpu.P.I = true;
    m->cpu.PC = (high << 8) | low;
}

void bvc(Machine *m, AddrMode UNUSED addr_mode) {
    relative_branch(m, !m->cpu.P.V);
}

void bvs(Machine *m, AddrMode UNUSED addr_mode) {
    relative_branch(m, m->cpu.P.V);
}

void clc(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.P.C = 0;
}

void cld(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.P.D = 0;
}

void cli(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.P.I = 0;
}

void clv(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.P.V = 0;
}

void cmp(Machine *m, AddrMode addr_mode) {
    u8 value = machine_read_byte_with_mode(m, addr_mode);
    u8 a = m->cpu.A;

    adjust_zero_and_negative_flag(m, a - value);
    m->cpu.P.C = value <= a;
}

void cpx(Machine *m, AddrMode addr_mode) {
    u8 value = machine_read_byte_with_mode(m, addr_mode);
    u8 x = m->cpu.X;

    adjust_zero_and_negative_flag(m, x - value);
    m->cpu.P.C = value <= x;
}

void cpy(Machine *m, AddrMode addr_mode) {
    u8 value = machine_read_byte_with_mode(m, addr_mode);
    u8 y = m->cpu.Y;

    adjust_zero_and_negative_flag(m, y - value);
    m->cpu.P.C = value <= y;
}

void dec(Machine *m, AddrMode addr_mode) {
    u16 addr = machine_fetch_address(m, addr_mode);
    u8 value = machine_read_byte(m, addr);
    value -= 1;
    adjust_zero_and_negative_flag(m, value);
    machine_write_byte(m, addr, value);
}

void dex(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.X--;
    adjust_zero_and_negative_flag(m, m->cpu.X);
}

void dey(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.Y--;
    adjust_zero_and_negative_flag(m, m->cpu.Y);
}

void eor(Machine *m, AddrMode addr_mode) {
    u8 mem = machine_read_byte_with_mode(m, addr_mode);
    m->cpu.A = m->cpu.A ^ mem;
    adjust_zero_and_negative_flag(m, m->cpu.A);
}

void inc(Machine *m, AddrMode addr_mode) {
    u16 addr = machine_fetch_address(m, addr_mode);
    u8 value = machine_read_byte(m, addr);
    value += 1;
    adjust_zero_and_negative_flag(m, value);
    machine_write_byte(m, addr, value);
}

void inx(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.X++;
    adjust_zero_and_negative_flag(m, m->cpu.X);
}

void iny(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.Y++;
    adjust_zero_and_negative_flag(m, m->cpu.Y);
}

void jmp(Machine *m, AddrMode addr_mode) {
    u16 addr = machine_read_immediate_word(m);

    switch (addr_mode) {
        case Absolute: {
            m->cpu.PC = addr;
            break;
        }
        case AbsoluteIndirect: {
            // The indirect jump instruction does not increment the page address
            // when the indirect pointer crosses a page boundary. JMP ($xxFF) will
            // fetch the address from $xxFF and $xx00.
            u8 low = machine_read_byte(m, addr);
            u8 high = machine_read_byte(m, (addr & 0xFF00) | ((addr + 1) & 0x00FF));
            m->cpu.PC = (high << 8) | low;
            break;
        }
        default:
            cpu_error_marker(m, __FILE__, __LINE__);
            cpu_error(m, "addressing mode %d not implemented", addr_mode);
    }
}

void jsr(Machine *m, AddrMode UNUSED addr_mode) {
    // This (probably) replicates the actual behavior of the cycles of the hardware
    u8 low = machine_read_byte_with_mode(m, Immediate);
    machine_push_program_counter_on_stack(m);
    u8 high = machine_read_byte_with_mode(m, Immediate);
    m->cpu.PC = (high << 8) | low;
}

void lda(Machine *m, AddrMode addr_mode) {
    m->cpu.A = machine_read_byte_with_mode(m, addr_mode);

    adjust_zero_and_negative_flag(m, m->cpu.A);
}

void ldx(Machine *m, AddrMode addr_mode) {
    m->cpu.X = machine_read_byte_with_mode(m, addr_mode);

    adjust_zero_and_negative_flag(m, m->cpu.X);
}

void ldy(Machine *m, AddrMode addr_mode) {
    m->cpu.Y = machine_read_byte_with_mode(m, addr_mode);

    adjust_zero_and_negative_flag(m, m->cpu.Y);
}

void lsr(Machine *m, AddrMode addr_mode) {
    u16 addr;
    u8 value;
    if (addr_mode == Accumulator) {
        value = m->cpu.A;
    } else {
        addr = machine_fetch_address(m, addr_mode);
        value = machine_read_byte(m, addr);
    }

    m->cpu.P.C = (value & 0x01) == 0x01;
    value = value >> 1;
    adjust_zero_and_negative_flag(m, value);

    if (addr_mode == Accumulator) {
        m->cpu.A = value;
    } else {
        machine_write_byte(m, addr, value);
    }
}

void nop(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.cycles++;
}

void ora(Machine *m, AddrMode addr_mode) {
    u8 mem = machine_read_byte_with_mode(m, addr_mode);
    m->cpu.A = m->cpu.A | mem;
    adjust_zero_and_negative_flag(m, m->cpu.A);
}

void pha(Machine *m, AddrMode UNUSED addr_mode) {
    machine_push_byte_on_stack(m, m->cpu.A);
}

void php(Machine *m, AddrMode UNUSED addr_mode) {
    CPUFlags flags = m->cpu.P;
    flags.U = true;
    flags.B = true;
    machine_push_byte_on_stack(m, flags.status);
}

void pla(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.A = machine_pop_byte_from_stack(m);
    adjust_zero_and_negative_flag(m, m->cpu.A);
}

void plp(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.P.status = machine_pop_byte_from_stack(m);
    m->cpu.P.U = true;
    m->cpu.P.B = false;
}

void rol(Machine *m, AddrMode addr_mode) {
    u16 addr;
    u8 value;
    if (addr_mode == Accumulator) {
        value = m->cpu.A;
    } else {
        addr = machine_fetch_address(m, addr_mode);
        value = machine_read_byte(m, addr);
    }

    u8 carry = m->cpu.P.C;
    m->cpu.P.C = (value & 0x80) == 0x80;
    value = value << 1;
    value = value | carry;
    adjust_zero_and_negative_flag(m, value);

    if (addr_mode == Accumulator) {
        m->cpu.A = value;
    } else {
        machine_write_byte(m, addr, value);
    }
}

void ror(Machine *m, AddrMode addr_mode) {
    u16 addr;
    u8 value;
    if (addr_mode == Accumulator) {
        value = m->cpu.A;
    } else {
        addr = machine_fetch_address(m, addr_mode);
        value = machine_read_byte(m, addr);
    }

    u8 carry = m->cpu.P.C;
    m->cpu.P.C = (value & 0x01) == 0x01;
    value = value >> 1;
    value = (carry << 7) | value;
    adjust_zero_and_negative_flag(m, value);

    if (addr_mode == Accumulator) {
        m->cpu.A = value;
    } else {
        machine_write_byte(m, addr, value);
    }
}

void rti(Machine *m, AddrMode UNUSED addr_mode) {
    plp(m, Implied);
    machine_pop_program_counter_from_stack(m);
}

void rts(Machine *m, AddrMode UNUSED addr_mode) {
    machine_pop_program_counter_from_stack(m);
    m->cpu.PC++;
}

void sbc(Machine *m, AddrMode addr_mode) {
    u8 value = machine_read_byte_with_mode(m, addr_mode);
    add(m, ~value);
}

void sec(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.P.C = 1;
}

void sed(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.P.D = 1;
}

void sei(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.P.I = 1;
}

void sta(Machine *m, AddrMode addr_mode) {
    machine_write_byte_with_mode(m, addr_mode, m->cpu.A);
}

void stx(Machine *m, AddrMode addr_mode) {
    machine_write_byte_with_mode(m, addr_mode, m->cpu.X);
}

void sty(Machine *m, AddrMode addr_mode) {
    machine_write_byte_with_mode(m, addr_mode, m->cpu.Y);
}

void tax(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.X = m->cpu.A;
    m->cpu.cycles++;

    adjust_zero_and_negative_flag(m, m->cpu.X);
}

void tay(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.Y = m->cpu.A;
    m->cpu.cycles++;

    adjust_zero_and_negative_flag(m, m->cpu.Y);
}

void tsx(Machine *m, UNUSED AddrMode addr_mode) {
    m->cpu.X = m->cpu.S;
    adjust_zero_and_negative_flag(m, m->cpu.X);
}

void txa(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.A = m->cpu.X;
    m->cpu.cycles++;
    adjust_zero_and_negative_flag(m, m->cpu.X);
}

void txs(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.S = m->cpu.X;
    m->cpu.cycles++;
}

void tya(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.A = m->cpu.Y;
    m->cpu.cycles++;

    adjust_zero_and_negative_flag(m, m->cpu.A);
}
