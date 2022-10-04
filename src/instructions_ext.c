#include "instructions_ext.h"
#include "instructions.h"
#include "seaio.h"

void anc(Machine *m, AddrMode addr_mode) {
    and(m, addr_mode);
    m->cpu.P.C = m->cpu.P.N;
}

void arr(Machine *m, AddrMode addr_mode) {
    // description here: http://www.6502.org/users/andre/petindex/local/64doc.txt
    u8 mem = machine_read_byte_with_mode(m, addr_mode);
    u8 result = m->cpu.A & mem;

    m->cpu.A = (result >> 1) | (m->cpu.P.C << 7);

    m->cpu.P.N = m->cpu.P.C;
    m->cpu.P.Z = !(m->cpu.A);
    m->cpu.P.V = ((result ^ m->cpu.A) & 0x40) == 0x40;

    if (m->cpu.P.D) {
        u8 ah = (result >> 4) & 0xF;
        u8 al = result & 0xF;

        if ((al + (al & 1)) > 5) {
            m->cpu.A = (m->cpu.A & 0xF0) | ((m->cpu.A + 0x06) & 0x0F);
        }

        m->cpu.P.C = (ah + (ah & 0x01)) > 0x05;
        if (m->cpu.P.C) {
            m->cpu.A = (m->cpu.A + 0x60) & 0xFF;
        }
    } else {
        m->cpu.P.C = (m->cpu.A & 0x40) == 0x40;
    }
}

void asr(Machine *m, AddrMode addr_mode) {
    u8 mem = machine_read_byte_with_mode(m, addr_mode);
    u8 result = m->cpu.A & mem;
    m->cpu.P.C = result & 0x01;
    result >>= 1;
    adjust_zero_and_negative_flag(m, result);
    m->cpu.A = result;
}

void dcp(Machine *m, AddrMode addr_mode) {
    u16 addr = machine_fetch_address(m, addr_mode);
    u8 mem = machine_read_byte(m, addr);
    page_cross_behavior(m, addr_mode, addr);
    machine_write_byte(m, addr, mem); // cycle correct behavior

    mem -= 1;
    machine_write_byte(m, addr, mem);
    u8 value = m->cpu.A - mem;
    adjust_zero_and_negative_flag(m, value);
    m->cpu.P.C = value <= m->cpu.A;
}

void isc(Machine *m, AddrMode addr_mode) {
    u16 addr = machine_fetch_address(m, addr_mode);
    u8 mem = machine_read_byte(m, addr);
    page_cross_behavior(m, addr_mode, addr);
    machine_write_byte(m, addr, mem); // cycle correct behavior

    mem += 1;
    machine_write_byte(m, addr, mem);

    if (m->cpu.P.D) {
        bcd_sub(m, mem);
    } else {
        add(m, ~mem);
    }
}

void las(Machine *m, AddrMode addr_mode) {
    u8 mem = machine_read_byte_with_mode(m, addr_mode);
    m->cpu.A = mem & m->cpu.S;
    m->cpu.X = m->cpu.A;
    m->cpu.S = m->cpu.A;
    adjust_zero_and_negative_flag(m, m->cpu.A);
}

void lax(Machine *m, AddrMode addr_mode) {
    u8 mem = machine_read_byte_with_mode(m, addr_mode);
    m->cpu.A = mem;
    m->cpu.X = mem;
    adjust_zero_and_negative_flag(m, m->cpu.A);
}

void lxa(Machine *m, AddrMode addr_mode) {
    u8 mem = machine_read_byte_with_mode(m, addr_mode);
    m->cpu.A = (m->cpu.A | 0xEE) & mem;
    m->cpu.X = m->cpu.A;
    adjust_zero_and_negative_flag(m, m->cpu.A);
}

void nop_ext(Machine *m, AddrMode addr_mode) {
    if (addr_mode == Implied) {
        machine_read_byte(m, m->cpu.PC); // cycle correct behavior
    } else {
        machine_read_byte_with_mode(m, addr_mode);
        if (m->page_cross) {
            m->cycle_count--; // to match expectation of Processor Tests
        }
        if (addr_mode == Absolute) {
            m->cycle_count--; // to match expectation of Processor Tests
        }
    }
}

void rla(Machine *m, AddrMode addr_mode) {
    u16 addr = machine_fetch_address(m, addr_mode);
    u8 value = machine_read_byte(m, addr);
    page_cross_behavior(m, addr_mode, addr);
    machine_write_byte(m, addr, value); // cycle correct behavior

    u8 carry = m->cpu.P.C;
    m->cpu.P.C = (value & 0x80) == 0x80;
    value = value << 1;
    value = value | carry;
    m->cpu.A = m->cpu.A & value;

    adjust_zero_and_negative_flag(m, m->cpu.A);
    machine_write_byte(m, addr, value);
}

void rra(Machine *m, AddrMode addr_mode) {
    u16 addr = machine_fetch_address(m, addr_mode);
    u8 value = machine_read_byte(m, addr);
    page_cross_behavior(m, addr_mode, addr);
    machine_write_byte(m, addr, value); // cycle correct behavior

    u8 carry = m->cpu.P.C;
    m->cpu.P.C = (value & 0x01) == 0x01;
    value = value >> 1;
    value = (carry << 7) | value;

    if (m->cpu.P.D) {
        bcd_add(m, value);
    } else {
        add(m, value);
    }

    machine_write_byte(m, addr, value);
}

void sax(Machine *m, AddrMode addr_mode) {
    u8 value = m->cpu.A & m->cpu.X;
    machine_write_byte_with_mode(m, addr_mode, value);
}

void sbx(Machine *m, AddrMode addr_mode) {
    u8 mem = machine_read_byte_with_mode(m, addr_mode);
    u8 anx = m->cpu.A & m->cpu.X;
    m->cpu.X = anx - mem;
    adjust_zero_and_negative_flag(m, m->cpu.X);
    m->cpu.P.C = mem <= anx;
}

void sha(Machine *m, AddrMode addr_mode) {
    u16 addr;
    if (addr_mode == ZeroPageIndirectYIndexed) {
        u8 zp = machine_read_immediate_byte(m);
        u8 low = machine_read_byte(m, zp & 0x00FF);
        u8 high = machine_read_byte(m, (zp + 1) & 0x00FF);
        addr = ((high << 8) | low);

    } else if (addr_mode == YIndexedAbsolute) {
        addr = machine_read_immediate_word(m);

    } else {
        error("Unsupported addressing mode %d", addr_mode);
    }

    u16 indexed_addr = addr + m->cpu.Y;
    // handle page cross boundary behavior
    addr = (addr & 0xFF00) | (((addr & 0x00FF) + m->cpu.Y) & 0xFF);
    machine_read_byte(m, addr); // cycle correct behavior

    u8 value = m->cpu.A & m->cpu.X & ((indexed_addr >> 8) + 1);
    machine_write_byte(m, indexed_addr, value);
}

void shs(Machine *m, AddrMode UNUSED addr_mode) {
    u16 addr = machine_read_immediate_word(m);

    m->cpu.S = m->cpu.A & m->cpu.X;

    u16 indexed_addr = addr + m->cpu.Y;
    // handle page cross boundary behavior
    addr = (addr & 0xFF00) | (((addr & 0x00FF) + m->cpu.Y) & 0xFF);
    machine_read_byte(m, addr); // cycle correct behavior

    u8 value = m->cpu.S & ((indexed_addr >> 8) + 1);
    machine_write_byte(m, indexed_addr, value);
}

void shx(Machine *m, AddrMode UNUSED addr_mode) {
    u16 addr = machine_read_immediate_word(m);

    u16 indexed_addr = addr + m->cpu.Y;
    // handle page cross boundary behavior
    addr = (addr & 0xFF00) | (((addr & 0x00FF) + m->cpu.Y) & 0xFF);
    machine_read_byte(m, addr); // cycle correct behavior

    u8 value = m->cpu.X & ((indexed_addr >> 8) + 1);
    machine_write_byte(m, indexed_addr, value);
}

void shy(Machine *m, AddrMode UNUSED addr_mode) {
    u16 addr = machine_read_immediate_word(m);

    u16 indexed_addr = addr + m->cpu.X;
    // handle page cross boundary behavior
    addr = (addr & 0xFF00) | (((addr & 0x00FF) + m->cpu.X) & 0xFF);
    machine_read_byte(m, addr); // cycle correct behavior

    u8 value = m->cpu.Y & ((indexed_addr >> 8) + 1);
    machine_write_byte(m, indexed_addr, value);
}

void sre(Machine *m, AddrMode addr_mode) {
    u16 addr = machine_fetch_address(m, addr_mode);
    u8 value = machine_read_byte(m, addr);
    page_cross_behavior(m, addr_mode, addr);
    machine_write_byte(m, addr, value); // cycle correct behavior

    m->cpu.P.C = (value & 0x01) == 0x01;
    value = value >> 1;
    m->cpu.A = m->cpu.A ^ value;
    adjust_zero_and_negative_flag(m, m->cpu.A);

    machine_write_byte(m, addr, value);
}

void xxa(Machine *m, AddrMode addr_mode) {
    u8 mem = machine_read_byte_with_mode(m, addr_mode);
    m->cpu.A = (m->cpu.A | 0xEE) & m->cpu.X & mem;
    adjust_zero_and_negative_flag(m, m->cpu.A);
}
