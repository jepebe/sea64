#include "disassembly.h"
#include "stdio.h"
#include "debug.h"

static char read_mode_buffer[20];
static char register_buffer[21];
static char flags_buffer[9];

static char *decode_addr_mode(Machine *machine, u16 addr, AddrMode mode) {
    switch (mode) {
        case Absolute: {
            u8 low = machine_read_byte(machine, addr + 1);
            u8 high = machine_read_byte(machine, addr + 2);
            u16 abs_addr = (high << 8) | low;
            sprintf(read_mode_buffer, "$%04X ", abs_addr);
            break;
        }
        case Immediate: {
            u8 value = machine->ram[addr + 1];
            sprintf(read_mode_buffer, "#%02X (%d) ", value, value);
            break;
        }
        case Implied: {
            read_mode_buffer[0] = '\0';
            break;
        }
        case Relative: {
            s8 rel = (s8) machine_read_byte(machine, addr + 1);
            u16 abs_addr = addr + 2 + rel;
            sprintf(read_mode_buffer, "$%04X ", abs_addr);
            break;
        }
        default:
            cpu_error_marker(machine, __FILE__, __LINE__);
            cpu_error(machine, "addressing mode %d not implemented", mode);
    }
    return read_mode_buffer;
}

static char *decode_registers(Machine *machine) {
    u8 a = machine->cpu.A;
    u8 x = machine->cpu.X;
    u8 y = machine->cpu.Y;
    u8 s = machine->cpu.S;
    sprintf(register_buffer, "A:%02X X:%02X Y:%02X S:%02X ", a, x, y, s);
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
    printf("[%02X] ", opc); // Opcode number
    printf("%s ", opcode.name); // opcode mnemonic

    printf("%-20s", decode_addr_mode(machine, addr, opcode.addr_mode));
    printf("%-20s", decode_registers(machine));
    printf("%-8s", decode_flags(machine));

    printf("\n");
}
