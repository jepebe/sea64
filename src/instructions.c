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

void adc(Machine *m, AddrMode addr_mode) {
    u8 value = machine_read_byte_with_mode(m, addr_mode);
    u8 carry = m->cpu.P.C;
    u16 result = m->cpu.A + value + carry;

    m->cpu.P.C = result > 0xFF;
    adjust_zero_and_negative_flag(m, result & 0xFF);
    m->cpu.P.V = (~(m->cpu.A ^ value) & (m->cpu.A ^ result)) & 0x0080;
    m->cpu.A = result & 0xFF;
}

void bcc(Machine *m, AddrMode UNUSED addr_mode) {
    relative_branch(m, !m->cpu.P.C);
}

void bcs(Machine *m, AddrMode UNUSED addr_mode) {
    relative_branch(m, m->cpu.P.C);
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

void inx(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.X++;
    adjust_zero_and_negative_flag(m, m->cpu.X);
}

void iny(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.Y++;
    adjust_zero_and_negative_flag(m, m->cpu.Y);
}

void jmp(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.PC = machine_read_word_with_mode(m, addr_mode);
}

void jsr(Machine *m, AddrMode addr_mode) {
    u16 dst = machine_read_word_with_mode(m, addr_mode);
    m->cpu.PC--;  // stack should contain the last address of the instruction
    machine_push_program_counter_on_stack(m);
    m->cpu.PC = dst;
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

void rti(Machine *m, AddrMode UNUSED addr_mode) {
    plp(m, Implied);
    machine_pop_program_counter_from_stack(m);
}

void rts(Machine *m, AddrMode UNUSED addr_mode) {
    machine_pop_program_counter_from_stack(m);
    m->cpu.PC++;
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
