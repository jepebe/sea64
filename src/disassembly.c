#include "disassembly.h"
#include "debug.h"
#include "stdio.h"

static char addr_mode_buffer[20];
static char register_buffer[26];
static char flags_buffer[9];

static char *decode_addr_mode(Machine *machine, u16 addr, AddrMode mode) {
    switch (mode) {
        case Absolute: {
            u8 low = machine->ram[addr + 1];
            u8 high = machine->ram[addr + 2];
            u16 abs_addr = (high << 8) | low;
            sprintf(addr_mode_buffer, "$%04X ", abs_addr);
            break;
        }
        case Accumulator: {
            sprintf(addr_mode_buffer, "A ");
            break;
        }
        case AbsoluteIndirect: {
            u8 low = machine->ram[addr + 1];
            u8 high = machine->ram[addr + 2];
            u16 ind_addr = (high << 8) | low;

            low = machine->ram[ind_addr];
            high = machine->ram[(ind_addr & 0xFF00) | ((ind_addr & 0x00FF) + 1)];
            u16 abs_addr = (high << 8) | low;

            sprintf(addr_mode_buffer, "($%04X)->$%04X ", ind_addr, abs_addr);
            break;
        }
        case Immediate: {
            u8 value = machine->ram[addr + 1];
            // sprintf(addr_mode_buffer, "#$%02X (%d) ", value, value);
            sprintf(addr_mode_buffer, "#%02X (%d) ", value, value);
            break;
        }
        case Implied: {
            addr_mode_buffer[0] = '\0';
            break;
        }
        case Relative: {
            s8 rel = (s8) machine->ram[addr + 1];
            u16 abs_addr = addr + 2 + rel;
            sprintf(addr_mode_buffer, "$%04X ", abs_addr);
            break;
        }
        case XIndexedAbsolute: {
            u8 low = machine->ram[addr + 1];
            u8 high = machine->ram[addr + 2];
            u16 abs_addr = (high << 8) | low;
            // u16 index_addr = abs_addr + machine->cpu.X;
            // sprintf(addr_mode_buffer, "$%04X,X -> $%04X ", abs_addr, index_addr);
            sprintf(addr_mode_buffer, "$%04X,X ", abs_addr);
            break;
        }
        case XIndexedAbsoluteIndirect: {
            u8 low = machine->ram[addr + 1];
            u8 high = machine->ram[addr + 2];
            u16 abs_addr = (high << 8) | low;
            sprintf(addr_mode_buffer, "($%04X,X) ", abs_addr);
            break;
        }
        case XIndexedZeroPage: {
            u8 zero_page_addr = machine->ram[addr + 1];
            sprintf(addr_mode_buffer, "$%02X,X ", zero_page_addr);
            break;
        }
        case XIndexedZeroPageIndirect: {
            u8 zero_page_addr = machine->ram[addr + 1];
            sprintf(addr_mode_buffer, "($%02X,X) ", zero_page_addr);
            break;
        }
        case YIndexedAbsolute: {
            u8 low = machine->ram[addr + 1];
            u8 high = machine->ram[addr + 2];
            u16 abs_addr = (high << 8) | low;
            // u16 index_addr = abs_addr + machine->cpu.Y;
            // sprintf(addr_mode_buffer, "$%04X,Y -> $%04X ", abs_addr, index_addr);
            sprintf(addr_mode_buffer, "$%04X,Y ", abs_addr);
            break;
        }
        case YIndexedZeroPage: {
            u8 zero_page_addr = machine->ram[addr + 1];
            sprintf(addr_mode_buffer, "$%02X,Y ", zero_page_addr);
            break;
        }
        case ZeroPage: {
            u8 zero_page_addr = machine->ram[addr + 1];
            sprintf(addr_mode_buffer, "$%02X ", zero_page_addr);
            break;
        }
        case ZeroPageIndirect: {
            u8 zero_page_addr = machine->ram[addr + 1];
            sprintf(addr_mode_buffer, "($%02X) ", zero_page_addr);
            break;
        }
        case ZeroPageIndirectYIndexed: {
            u8 zero_page_addr = machine->ram[addr + 1];
            sprintf(addr_mode_buffer, "($%02X),Y ", zero_page_addr);
            break;
        }
        case ZeroPageRelative: {
            u8 zero_page_addr = machine->ram[addr + 1];
            s8 rel = (s8) machine->ram[addr + 2];
            u16 abs_addr = addr + 2 + rel;
            sprintf(addr_mode_buffer, "$%02X,$%04X ", zero_page_addr, abs_addr);
            break;
        }
        default:
            cpu_error_marker(machine, __FILE__, __LINE__);
            cpu_error(machine, "addressing mode %d not implemented", mode);
    }
    return addr_mode_buffer;
}

static char *decode_registers(Machine *machine) {
    u8 a = machine->cpu.A;
    u8 x = machine->cpu.X;
    u8 y = machine->cpu.Y;
    u8 s = machine->cpu.S;
    u8 p = machine->cpu.P.status;
    sprintf(register_buffer, "A:%02X X:%02X Y:%02X S:%02X P:%02X ", a, x, y, s, p);
    return register_buffer;
}

static char *decode_flags(Machine *machine) {
    flags_buffer[0] = machine->cpu.P.N ? 'N' : '.';
    flags_buffer[1] = machine->cpu.P.V ? 'V' : '.';
    flags_buffer[2] = machine->cpu.P.U ? 'U' : '.';
    flags_buffer[3] = machine->cpu.P.B ? 'B' : '.';
    flags_buffer[4] = machine->cpu.P.D ? 'D' : '.';
    flags_buffer[5] = machine->cpu.P.I ? 'I' : '.';
    flags_buffer[6] = machine->cpu.P.Z ? 'Z' : '.';
    flags_buffer[7] = machine->cpu.P.C ? 'C' : '.';
    flags_buffer[8] = 0;
    return flags_buffer;
}

void disassemble_instruction(Machine *machine, u16 addr, u8 opc, Opcode opcode) {
    printf("[$%04X] ", addr);
    printf("[%02X] ", opc);     // Opcode number
    printf("%-4s ", opcode.name); // opcode mnemonic

    printf("%-20s", decode_addr_mode(machine, addr, opcode.addr_mode));
    printf("%-20s", decode_registers(machine));
    printf("%-8s", decode_flags(machine));

    printf("\n");
}
