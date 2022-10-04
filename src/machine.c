#include "machine.h"
#include "debug.h"
#include "opcodes.h"
#include "stdio.h"

Machine machine_create(void) {
    Machine result = {0};
    cpu_reset(&result.cpu);
    result.halt = false;
    return result;
}

void machine_tick(Machine *machine) {
    if (machine->halt) {
        return;
    }
    machine->cycle_count = 0;
    u8 opcode_num = machine_read_byte(machine, machine->cpu.PC++);
    Opcode opcode = fetch_opcode(opcode_num, machine->cpu.cpu_type);

    if (opcode.op_fn != NULL) {
        opcode.op_fn(machine, opcode.addr_mode);
    } else {
        machine->cpu.PC--; // revert to the address that has the erroneous opcode
        cpu_error_marker(machine, __FILE__, __LINE__);
        cpu_error(machine, "opcode 0x%02X not implemented", opcode_num);
    }
    machine->cpu.tick_count++;
}

u8 machine_read_byte(Machine *machine, u16 addr) {
    u8 data = machine->ram[addr];

    machine->cycles[machine->cycle_count].addr = addr;
    machine->cycles[machine->cycle_count].value = data;
    machine->cycles[machine->cycle_count].activity = READ_CYCLE;
    machine->cycle_count++;

    return data;
}

void machine_write_byte(Machine *machine, u16 addr, u8 value) {
    machine->ram[addr] = value;

    machine->cycles[machine->cycle_count].addr = addr;
    machine->cycles[machine->cycle_count].value = value;
    machine->cycles[machine->cycle_count].activity = WRITE_CYCLE;
    machine->cycle_count++;
}

u16 machine_fetch_address(Machine *machine, AddrMode mode) {
    machine->page_cross = false;
    u16 addr;
    switch (mode) {
        case Absolute: {
            addr = machine_read_immediate_word(machine);
            break;
        }
        case Immediate:
            addr = machine->cpu.PC++;
            break;
        case XIndexedAbsolute: {
            addr = machine_read_immediate_word(machine);
            u16 indexed_addr = addr + machine->cpu.X;
            if ((addr & 0xFF00) != (indexed_addr & 0xFF00)) {
                // extra cycle for crossing pages
                machine_read_byte(machine, (addr & 0xFF00) | (indexed_addr & 0x00FF));
                machine->page_cross = true;
            }
            addr += machine->cpu.X;
            break;
        }
        case XIndexedZeroPage: {
            u8 zero_page_addr = machine_read_immediate_byte(machine);
            machine_read_byte(machine, zero_page_addr); // cycle correct behavior
            addr = (zero_page_addr + machine->cpu.X) & 0x00FF;
            break;
        }
        case XIndexedZeroPageIndirect: {
            u8 zero_page_addr = machine_read_immediate_byte(machine);
            machine_read_byte(machine, zero_page_addr); // cycle correct behavior
            zero_page_addr += machine->cpu.X;
            u8 low = machine_read_byte(machine, zero_page_addr & 0x00FF);
            u8 high = machine_read_byte(machine, (zero_page_addr + 1) & 0x00FF);
            addr = (high << 8) | low;
            break;
        }
        case YIndexedAbsolute: {
            addr = machine_read_immediate_word(machine);
            u16 indexed_addr = addr + machine->cpu.Y;
            if ((addr & 0xFF00) != (indexed_addr & 0xFF00)) {
                // extra cycle for crossing pages
                machine_read_byte(machine, (addr & 0xFF00) | (indexed_addr & 0x00FF));
                machine->page_cross = true;
            }
            addr += machine->cpu.Y;
            break;
        }
        case YIndexedZeroPage: {
            u8 zero_page_addr = machine_read_immediate_byte(machine);
            machine_read_byte(machine, zero_page_addr); // cycle correct behavior
            addr = (zero_page_addr + machine->cpu.Y) & 0x00FF;
            break;
        }
        case ZeroPage: {
            addr = machine_read_immediate_byte(machine) & 0x00FF;
            break;
        }
        case ZeroPageIndirectYIndexed: {
            u8 zero_page_addr = machine_read_immediate_byte(machine);
            u8 low = machine_read_byte(machine, zero_page_addr & 0x00FF);
            u8 high = machine_read_byte(machine, (zero_page_addr + 1) & 0x00FF);
            addr = (high << 8) | low;
            u16 indexed_addr = addr + machine->cpu.Y;
            if ((addr & 0xFF00) != (indexed_addr & 0xFF00)) {
                // extra cycle for crossing pages
                u16 adr = (addr & 0xFF00) | (indexed_addr & 0x00FF);
                machine_read_byte(machine, adr); // cycle correct behavior
                machine->page_cross = true;
            }
            addr = indexed_addr;
            break;
        }
        default:
            cpu_error_marker(machine, __FILE__, __LINE__);
            cpu_error(machine, "addressing mode %d not implemented", mode);
    }
    return addr;
}

u8 machine_read_byte_with_mode(Machine *machine, AddrMode mode) {
    u16 addr = machine_fetch_address(machine, mode);
    return machine_read_byte(machine, addr);
}

void machine_write_byte_with_mode(Machine *machine, AddrMode mode, u8 value) {
    u16 addr = machine_fetch_address(machine, mode);
    machine_write_byte(machine, addr, value);
}

u8 machine_read_immediate_byte(Machine *machine) {
    return machine_read_byte(machine, machine->cpu.PC++);
}

u16 machine_read_immediate_word(Machine *machine) {
    u8 low = machine_read_byte(machine, machine->cpu.PC++);
    u8 high = machine_read_byte(machine, machine->cpu.PC++);
    return (high << 8) | low;
}

void machine_push_byte_on_stack(Machine *machine, u8 value) {
    u16 stack_pointer = 0x100 + machine->cpu.S;
    machine_write_byte(machine, stack_pointer, value);
    machine->cpu.S--;
}

u8 machine_pop_byte_from_stack(Machine *machine) {
    machine->cpu.S++;
    u16 stack_pointer = 0x100 + machine->cpu.S;
    u8 value = machine_read_byte(machine, stack_pointer);
    return value;
}

void machine_push_program_counter_on_stack(Machine *machine) {
    u16 pc = machine->cpu.PC;
    machine_push_byte_on_stack(machine, (pc >> 8) & 0xFF); // high
    machine_push_byte_on_stack(machine, pc & 0xFF);        // low
}

void machine_pop_program_counter_from_stack(Machine *machine) {
    u8 low = machine_pop_byte_from_stack(machine);
    u8 high = machine_pop_byte_from_stack(machine);
    machine->cpu.PC = ((high << 8) | low);
}

void machine_push_cpu_flags_on_stack(Machine *machine) {
    CPUFlags flags = machine->cpu.P;
    flags.U = true;
    flags.B = true;
    machine_push_byte_on_stack(machine, flags.status);
}

void machine_irq(Machine *machine) {
    if (!machine->cpu.P.I) {
        machine_push_program_counter_on_stack(machine);
        machine->cpu.P.B = false;
        machine->cpu.P.U = true;
        machine_push_byte_on_stack(machine, machine->cpu.P.status);

        machine->cpu.P.I = true;

        u8 low = machine_read_byte(machine, 0xFFFE);
        u8 high = machine_read_byte(machine, 0xFFFF);
        u16 addr = (high << 8) | low;
        machine->cpu.PC = addr;
    }
}

void machine_nmi(Machine *machine) {
    machine_push_program_counter_on_stack(machine);
    machine->cpu.P.B = false;
    machine->cpu.P.U = true;
    machine_push_byte_on_stack(machine, machine->cpu.P.status);

    machine->cpu.P.I = true;

    u8 low = machine_read_byte(machine, 0xFFFA);
    u8 high = machine_read_byte(machine, 0xFFFB);
    u16 addr = (high << 8) | low;
    machine->cpu.PC = addr;
}

void machine_halt(Machine *machine) {
    machine->halt = true;
}
