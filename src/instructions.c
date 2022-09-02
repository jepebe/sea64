#include "instructions.h"

void beq(Machine *m, AddrMode UNUSED addr_mode) {
    s8 rel = (s8) machine_read_byte(m, m->cpu.PC++);
    if (m->cpu.P.Z) {
        m->cpu.PC = m->cpu.PC + rel;
    }
}

void bne(Machine *m, AddrMode UNUSED addr_mode) {
    s8 rel = (s8) machine_read_byte(m, m->cpu.PC++);
    if (!m->cpu.P.Z) {
        m->cpu.PC = m->cpu.PC + rel;
    }
}

void bpl(Machine *m, AddrMode UNUSED addr_mode) {
    s8 rel = (s8) machine_read_byte(m, m->cpu.PC++);
    if (!m->cpu.P.N) {
        m->cpu.PC = m->cpu.PC + rel;
    }
}

void cld(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.P.D = 0;
    m->cpu.cycles++;
}

void cmp(Machine *m, AddrMode addr_mode) {
    u8 value = machine_read_byte_with_mode(m, addr_mode);
    u8 a = m->cpu.A;
    u8 diff = a - value;

    m->cpu.P.Z = !diff;
    m->cpu.P.N = diff & 0x80;
    m->cpu.P.C = value <= a;
}

void dex(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.X--;
    m->cpu.P.Z = !m->cpu.X;
    m->cpu.P.N = m->cpu.X & 0x80;
}

void dey(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.Y--;
    m->cpu.P.Z = !m->cpu.Y;
    m->cpu.P.N = m->cpu.Y & 0x80;
}

void jmp(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.PC = machine_read_immediate_word(m);
}

void lda(Machine *m, AddrMode addr_mode) {
    m->cpu.A = machine_read_byte_with_mode(m, addr_mode);

    m->cpu.P.Z = !m->cpu.A;
    m->cpu.P.N = m->cpu.A & 0x80;
}

void ldx(Machine *m, AddrMode addr_mode) {
    m->cpu.X = machine_read_byte_with_mode(m, addr_mode);

    m->cpu.P.Z = !m->cpu.X;
    m->cpu.P.N = m->cpu.X & 0x80;
}

void ldy(Machine *m, AddrMode addr_mode) {
    m->cpu.Y = machine_read_byte_with_mode(m, addr_mode);

    m->cpu.P.Z = !m->cpu.Y;
    m->cpu.P.N = m->cpu.Y & 0x80;
}

void nop(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.cycles++;
}

void sta(Machine *m, AddrMode addr_mode) {
    machine_write_byte_with_mode(m, addr_mode, m->cpu.A);
}

void tax(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.X = m->cpu.A;
    m->cpu.cycles++;

    m->cpu.P.Z = !m->cpu.X;
    m->cpu.P.N = m->cpu.X & 0x80;
}

void txs(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.S = m->cpu.X;
    m->cpu.cycles++;
}

void tya(Machine *m, AddrMode UNUSED addr_mode) {
    m->cpu.A = m->cpu.Y;
    m->cpu.cycles++;

    m->cpu.P.Z = !m->cpu.A;
    m->cpu.P.N = m->cpu.A & 0x80;
}

