#include "instructions_wdc65c02.h"
#include "debug.h"
#include "instructions.h"
#include "seaio.h"

static void bcd_add_wcd(Machine *m, u8 value) {
    u8 carry = m->cpu.P.C;
    u8 a = m->cpu.A;

    u8 low = (a & 0x0F) + (value & 0x0F) + carry;
    u8 half_carry = 0;
    if (low > 9) {
        low = (low + 6) & 0x0F;
        half_carry = 1;
    }
    u8 high = ((a & 0xF0) >> 4) + ((value & 0xF0) >> 4) + half_carry;

    u8 overflow = ((high << 4) ^ a) & ~(a ^ value);
    m->cpu.P.V = (overflow & 0x80) == 0x80;

    if (high > 9) {
        high = (high + 6);
    }

    m->cpu.P.C = high > 0x09;
    m->cpu.A = (high << 4) | low;
    m->cpu.P.N = (m->cpu.A & 0x80) == 0x80;
    m->cpu.P.Z = m->cpu.A == 0x00;
}

static void bcd_sub_wdc(Machine *m, u8 value) {
    u8 borrow = !m->cpu.P.C;
    u8 a = m->cpu.A;

    u8 bin = a - value - borrow;

    u8 low = (a & 0x0F) - (value & 0x0F) - borrow;
    u8 half_borrow = 0;
    if ((low & 0x10) == 0x10) {
        low = (low - 6) & 0x0F;
        half_borrow = 1;
    }

    u8 high = ((a & 0xF0) >> 4) - ((value & 0xF0) >> 4) - half_borrow;

    u8 overflow = (value ^ a) & ~(bin ^ value);
    m->cpu.P.V = (overflow & 0x80) == 0x80;

    m->cpu.P.C = 1;
    if ((high & 0x10) == 0x10) {
        high = (high - 6);
        m->cpu.P.C = 0;
    }

    m->cpu.A = (high << 4) | low;
    m->cpu.P.N = (m->cpu.A & 0x80) == 0x80;
    m->cpu.P.Z = m->cpu.A == 0x00;
}

void adc_wdc(Machine *m, AddrMode addr_mode) {
    u16 addr = machine_fetch_address(m, addr_mode);
    u8 value = machine_read_byte(m, addr);
    if (m->cpu.P.D) {
        bcd_add_wcd(m, value);
        if (addr_mode == Immediate) {
            addr = 0x7F;
        }
        machine_read_byte(m, addr); // cycle correct behavior
    } else {
        add(m, value);
    }
}
void asla(Machine *m, AddrMode UNUSED addr_mode) {
    u8 value = m->cpu.A;
    machine_read_byte(m, m->cpu.PC); // cycle correct behavior

    m->cpu.P.C = (value & 0x80) == 0x80;
    value = value << 1;
    adjust_zero_and_negative_flag(m, value);

    m->cpu.A = value;
}

void asl_wdc(Machine *m, AddrMode addr_mode) {
    u16 addr = machine_fetch_address(m, addr_mode);
    u8 value = machine_read_byte(m, addr);
    machine_read_byte(m, addr); // cycle correct behavior

    m->cpu.P.C = (value & 0x80) == 0x80;
    value = value << 1;
    adjust_zero_and_negative_flag(m, value);

    machine_write_byte(m, addr, value);
}

static void bbr(Machine *m, u8 bit) {
    u8 zp = machine_read_immediate_byte(m);
    u8 mem = machine_read_byte(m, zp);
    machine_write_byte(m, zp, mem); // cycle correct behavior
    bool should_branch = ((mem >> bit) & 0x01) == 0x00;

    u8 rel = machine_read_immediate_byte(m);
    u16 addr = m->cpu.PC + (s8) rel;

    machine_read_byte(m, (m->cpu.PC & 0xFF00) | (addr & 0x00FF)); // cycle correct behavior

    if (should_branch) {
        m->cpu.PC = addr;
    }
}

static void bbs(Machine *m, u8 bit) {
    u8 zp = machine_read_immediate_byte(m);
    u8 mem = machine_read_byte(m, zp);
    machine_write_byte(m, zp, mem); // cycle correct behavior
    bool should_branch = ((mem >> bit) & 0x01) == 0x01;

    u8 rel = machine_read_immediate_byte(m);
    u16 addr = m->cpu.PC + (s8) rel;

    machine_read_byte(m, (m->cpu.PC & 0xFF00) | (addr & 0x00FF)); // cycle correct behavior

    if (should_branch) {
        m->cpu.PC = addr;
    }
}

void bbr0(Machine *m, AddrMode UNUSED addr_mode) {
    bbr(m, 0);
}

void bbr1(Machine *m, AddrMode UNUSED addr_mode) {
    bbr(m, 1);
}

void bbr2(Machine *m, AddrMode UNUSED addr_mode) {
    bbr(m, 2);
}

void bbr3(Machine *m, AddrMode UNUSED addr_mode) {
    bbr(m, 3);
}

void bbr4(Machine *m, AddrMode UNUSED addr_mode) {
    bbr(m, 4);
}

void bbr5(Machine *m, AddrMode UNUSED addr_mode) {
    bbr(m, 5);
}

void bbr6(Machine *m, AddrMode UNUSED addr_mode) {
    bbr(m, 6);
}

void bbr7(Machine *m, AddrMode UNUSED addr_mode) {
    bbr(m, 7);
}

void bbs0(Machine *m, AddrMode UNUSED addr_mode) {
    bbs(m, 0);
}

void bbs1(Machine *m, AddrMode UNUSED addr_mode) {
    bbs(m, 1);
}

void bbs2(Machine *m, AddrMode UNUSED addr_mode) {
    bbs(m, 2);
}

void bbs3(Machine *m, AddrMode UNUSED addr_mode) {
    bbs(m, 3);
}

void bbs4(Machine *m, AddrMode UNUSED addr_mode) {
    bbs(m, 4);
}

void bbs5(Machine *m, AddrMode UNUSED addr_mode) {
    bbs(m, 5);
}

void bbs6(Machine *m, AddrMode UNUSED addr_mode) {
    bbs(m, 6);
}

void bbs7(Machine *m, AddrMode UNUSED addr_mode) {
    bbs(m, 7);
}

void bra(Machine *m, AddrMode UNUSED addr_mode) {
    u8 rel = machine_read_immediate_byte(m);
    u16 addr = m->cpu.PC + (s8) rel;

    machine_read_byte(m, m->cpu.PC); // cycle correct behavior

    if ((addr & 0xFF00) != (m->cpu.PC & 0xFF00)) {
        // extra cycle for crossing pages
        machine_read_byte(m, (m->cpu.PC & 0xFF00) | (addr & 0x00FF));
    }

    m->cpu.PC = addr;
}

void brk_wdc(Machine *m, AddrMode addr_mode) {
    brk(m, addr_mode);
    m->cpu.P.D = false;
}

void deca(Machine *m, AddrMode UNUSED addr_mode) {
    machine_read_byte(m, m->cpu.PC); // cycle correct behavior
    u8 value = m->cpu.A;
    value -= 1;
    adjust_zero_and_negative_flag(m, value);
    m->cpu.A = value;
}

void dec_wdc(Machine *m, AddrMode addr_mode) {
    u16 addr = machine_fetch_address(m, addr_mode);

    page_cross_behavior(m, addr_mode, m->cpu.PC - 1); // cycle correct behavior
    machine_read_byte(m, addr);                       // cycle correct behavior

    u8 value = machine_read_byte(m, addr);

    value -= 1;
    adjust_zero_and_negative_flag(m, value);
    machine_write_byte(m, addr, value);
}

void inca(Machine *m, AddrMode UNUSED addr_mode) {
    machine_read_byte(m, m->cpu.PC); // cycle correct behavior
    u8 value = m->cpu.A;
    value += 1;
    adjust_zero_and_negative_flag(m, value);
    m->cpu.A = value;
}

void inc_wdc(Machine *m, AddrMode addr_mode) {
    u16 addr = machine_fetch_address(m, addr_mode);

    page_cross_behavior(m, addr_mode, m->cpu.PC - 1); // cycle correct behavior
    machine_read_byte(m, addr);                       // cycle correct behavior

    u8 value = machine_read_byte(m, addr);

    value += 1;
    adjust_zero_and_negative_flag(m, value);

    machine_write_byte(m, addr, value);
}

void jmp_wdc(Machine *m, AddrMode addr_mode) {
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
            machine_read_byte(m, (addr & 0xFF00) | ((addr + 1) & 0x00FF)); // cycle correct behavior
            u8 high = machine_read_byte(m, addr + 1);
            m->cpu.PC = (high << 8) | low;
            break;
        }
        default:
            cpu_error_marker(m, __FILE__, __LINE__);
            cpu_error(m, "addressing mode %d not implemented", addr_mode);
    }
}
void lsra(Machine *m, AddrMode UNUSED addr_mode) {
    u8 value = m->cpu.A;
    machine_read_byte(m, m->cpu.PC); // cycle correct behavior
    m->cpu.P.C = (value & 0x01) == 0x01;
    value = value >> 1;
    adjust_zero_and_negative_flag(m, value);
    m->cpu.A = value;
}

void lsr_wdc(Machine *m, AddrMode addr_mode) {
    u16 addr = machine_fetch_address(m, addr_mode);
    u8 value = machine_read_byte(m, addr);
    machine_read_byte(m, addr); // cycle correct behavior

    m->cpu.P.C = (value & 0x01) == 0x01;
    value = value >> 1;
    adjust_zero_and_negative_flag(m, value);

    machine_write_byte(m, addr, value);
}

void nop_wdc(Machine *m, AddrMode addr_mode) {
    switch (addr_mode) {
        case Immediate:
        case ZeroPage:
        case XIndexedZeroPage:
            machine_read_byte_with_mode(m, addr_mode);
            break;
        case XIndexedAbsolute:
            machine_read_immediate_word(m);
            machine_read_byte(m, m->cpu.PC - 1);
            break;
        case Implied:
        default:
            break;
    }
}

void phx(Machine *m, AddrMode UNUSED addr_mode) {
    machine_read_byte(m, m->cpu.PC); // cycle correct behavior
    machine_push_byte_on_stack(m, m->cpu.X);
}

void phy(Machine *m, AddrMode UNUSED addr_mode) {
    machine_read_byte(m, m->cpu.PC); // cycle correct behavior
    machine_push_byte_on_stack(m, m->cpu.Y);
}

void plx(Machine *m, AddrMode UNUSED addr_mode) {
    machine_read_byte(m, m->cpu.PC);         // cycle correct behavior
    machine_read_byte(m, 0x0100 | m->cpu.S); // cycle correct behavior
    m->cpu.X = machine_pop_byte_from_stack(m);
    adjust_zero_and_negative_flag(m, m->cpu.X);
}

void ply(Machine *m, AddrMode UNUSED addr_mode) {
    machine_read_byte(m, m->cpu.PC);         // cycle correct behavior
    machine_read_byte(m, 0x0100 | m->cpu.S); // cycle correct behavior
    m->cpu.Y = machine_pop_byte_from_stack(m);
    adjust_zero_and_negative_flag(m, m->cpu.Y);
}

static void rmb(Machine *m, AddrMode addr_mode, u8 bit) {
    u16 addr = machine_fetch_address(m, addr_mode);
    u8 value = machine_read_byte(m, addr);
    machine_read_byte(m, addr); // cycle correct behavior

    value = value & ~(0x01 << bit);
    machine_write_byte(m, addr, value);
}

void rmb0(Machine *m, AddrMode addr_mode) {
    rmb(m, addr_mode, 0);
}

void rmb1(Machine *m, AddrMode addr_mode) {
    rmb(m, addr_mode, 1);
}

void rmb2(Machine *m, AddrMode addr_mode) {
    rmb(m, addr_mode, 2);
}

void rmb3(Machine *m, AddrMode addr_mode) {
    rmb(m, addr_mode, 3);
}

void rmb4(Machine *m, AddrMode addr_mode) {
    rmb(m, addr_mode, 4);
}

void rmb5(Machine *m, AddrMode addr_mode) {
    rmb(m, addr_mode, 5);
}

void rmb6(Machine *m, AddrMode addr_mode) {
    rmb(m, addr_mode, 6);
}

void rmb7(Machine *m, AddrMode addr_mode) {
    rmb(m, addr_mode, 7);
}

void rola(Machine *m, AddrMode UNUSED addr_mode) {
    u8 value = m->cpu.A;
    machine_read_byte(m, m->cpu.PC); // cycle correct behavior
    u8 carry = m->cpu.P.C;
    m->cpu.P.C = (value & 0x80) == 0x80;
    value = value << 1;
    value = value | carry;
    adjust_zero_and_negative_flag(m, value);

    m->cpu.A = value;
}

void rol_wdc(Machine *m, AddrMode addr_mode) {
    u16 addr = machine_fetch_address(m, addr_mode);
    u8 value = machine_read_byte(m, addr);
    machine_read_byte(m, addr); // cycle correct behavior

    u8 carry = m->cpu.P.C;
    m->cpu.P.C = (value & 0x80) == 0x80;
    value = value << 1;
    value = value | carry;
    adjust_zero_and_negative_flag(m, value);

    machine_write_byte(m, addr, value);
}
void rora(Machine *m, AddrMode UNUSED addr_mode) {
    u8 value = m->cpu.A;
    machine_read_byte(m, m->cpu.PC); // cycle correct behavior

    u8 carry = m->cpu.P.C;
    m->cpu.P.C = (value & 0x01) == 0x01;
    value = value >> 1;
    value = (carry << 7) | value;
    adjust_zero_and_negative_flag(m, value);

    m->cpu.A = value;
}

void ror_wdc(Machine *m, AddrMode addr_mode) {
    u16 addr = machine_fetch_address(m, addr_mode);
    u8 value = machine_read_byte(m, addr);
    machine_read_byte(m, addr); // cycle correct behavior

    u8 carry = m->cpu.P.C;
    m->cpu.P.C = (value & 0x01) == 0x01;
    value = value >> 1;
    value = (carry << 7) | value;
    adjust_zero_and_negative_flag(m, value);

    machine_write_byte(m, addr, value);
}

void sbc_wdc(Machine *m, AddrMode addr_mode) {
    u16 addr = machine_fetch_address(m, addr_mode);
    u8 value = machine_read_byte(m, addr);
    if (m->cpu.P.D) {
        bcd_sub_wdc(m, value);
        if (addr_mode == Immediate) {
            addr = 0xB8;
        }
        machine_read_byte(m, addr); // cycle correct behavior
    } else {
        add(m, ~value);
    }
}

static void smb(Machine *m, AddrMode addr_mode, u8 bit) {
    u16 addr = machine_fetch_address(m, addr_mode);
    u8 value = machine_read_byte(m, addr);
    machine_read_byte(m, addr); // cycle correct behavior

    value = value | (0x01 << bit);
    machine_write_byte(m, addr, value);
}

void smb0(Machine *m, AddrMode addr_mode) {
    smb(m, addr_mode, 0);
}

void smb1(Machine *m, AddrMode addr_mode) {
    smb(m, addr_mode, 1);
}

void smb2(Machine *m, AddrMode addr_mode) {
    smb(m, addr_mode, 2);
}

void smb3(Machine *m, AddrMode addr_mode) {
    smb(m, addr_mode, 3);
}

void smb4(Machine *m, AddrMode addr_mode) {
    smb(m, addr_mode, 4);
}

void smb5(Machine *m, AddrMode addr_mode) {
    smb(m, addr_mode, 5);
}

void smb6(Machine *m, AddrMode addr_mode) {
    smb(m, addr_mode, 6);
}

void smb7(Machine *m, AddrMode addr_mode) {
    smb(m, addr_mode, 7);
}

void sta_wdc(Machine *m, AddrMode addr_mode) {
    u16 addr = machine_fetch_address(m, addr_mode);
    page_cross_behavior(m, addr_mode, m->cpu.PC - 1);
    machine_write_byte(m, addr, m->cpu.A); // cycle correct behavior
}

void stp(Machine UNUSED *m, AddrMode UNUSED addr_mode) {
    ERROR("STP not implemented!", 1);
}

void stz(Machine *m, AddrMode addr_mode) {
    u16 addr = machine_fetch_address(m, addr_mode);
    page_cross_behavior(m, addr_mode, m->cpu.PC - 1);
    machine_write_byte(m, addr, 0);
}

void trb(Machine *m, AddrMode addr_mode) {
    u16 addr = machine_fetch_address(m, addr_mode);
    u8 mem = machine_read_byte(m, addr);
    machine_read_byte(m, addr); // cycle correct behavior
    m->cpu.P.Z = (m->cpu.A & mem) == 0x00;
    machine_write_byte(m, addr, (~m->cpu.A) & mem);
}

void tsb(Machine *m, AddrMode addr_mode) {
    u16 addr = machine_fetch_address(m, addr_mode);
    u8 mem = machine_read_byte(m, addr);
    machine_read_byte(m, addr); // cycle correct behavior
    m->cpu.P.Z = (m->cpu.A & mem) == 0x00;
    mem = m->cpu.A | mem;
    machine_write_byte(m, addr, mem);
}

void wai(Machine UNUSED *m, AddrMode UNUSED addr_mode) {
    ERROR("WAI not implemented!", 1);
}
