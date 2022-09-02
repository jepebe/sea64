#include "machine.h"
#include "stdio.h"
#include "opcodes.h"
#include "debug.h"
#include "disassembly.h"

Machine machine_create(void) {
    Machine result = {0};
    cpu_reset(&result.cpu);
    return result;
}

void machine_tick(Machine *machine) {
    u16 current_address = machine->cpu.PC;
    u8 opcode_num = machine_read_byte(machine, machine->cpu.PC++);
    Opcode opcode = fetch_opcode(opcode_num);

    if (opcode.op_fn != NULL) {
        opcode.op_fn(machine, opcode.addr_mode);
        disassemble_instruction(machine, current_address, opcode_num, opcode);
    } else {
        machine->cpu.PC--; // revert to the addr that has the erroneous opcode
        cpu_error_marker(machine, __FILE__, __LINE__);
        cpu_error(machine, "opcode 0x%02X not implemented", opcode_num);
    }
}

u8 machine_read_byte(Machine *machine, u16 addr) {
    u8 data = machine->ram[addr];
    machine->cpu.cycles++;
    return data;
}

void machine_write_byte(Machine *machine, u16 addr, u8 value) {
    machine->ram[addr] = value;
    machine->cpu.cycles++;
}

u8 machine_read_byte_with_mode(Machine *machine, AddrMode mode) {
    switch (mode) {
        case Absolute: {
            u16 addr = machine_read_immediate_word(machine);
            return machine_read_byte(machine, addr);
        }
        case Immediate:
            return machine_read_byte(machine, machine->cpu.PC++);
        default:
            cpu_error_marker(machine, __FILE__, __LINE__);
            cpu_error(machine, "addressing mode %d not implemented", mode);
    }
}

void machine_write_byte_with_mode(Machine *machine, AddrMode mode, u8 value) {
    switch (mode) {
        case Absolute: {
            u16 addr = machine_read_immediate_word(machine);
            machine_write_byte(machine, addr, value);
            break;
        }
        default:
            cpu_error_marker(machine, __FILE__, __LINE__);
            cpu_error(machine, "addressing mode %d not implemented", mode);
    }
}

u16 machine_read_immediate_word(Machine *machine) {
    u8 low = machine_read_byte(machine, machine->cpu.PC++);
    u8 high = machine_read_byte(machine, machine->cpu.PC++);
    return (high << 8) | low;
}
