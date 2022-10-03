#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "seaio.h"
#include "proc_tester.h"
#include "debug.h"
#include "opcodes.h"
#include "disassembly.h"

void read_processor_tests(ProcTester *proc_tester, char *path) {
    if (access(path, F_OK) != 0) {
        error_marker(__FILE__, __LINE__);
        error("File '%s' does not exist!\n", path);
    }

    FILE *fd = fopen(path, "rb");

    if (!fd) {
        error_marker(__FILE__, __LINE__);
        error("Could not open file '%s': %s\n", path, strerror(errno));
    }

    // get filesize
    fseek(fd, 0, SEEK_END);
    proc_tester->buffer_size = ftell(fd);
    rewind(fd);

    proc_tester->data_buffer = malloc(sizeof(char) * proc_tester->buffer_size);
    size_t read = fread(proc_tester->data_buffer, sizeof(char), proc_tester->buffer_size, fd);

    if (read < proc_tester->buffer_size) {
        if (ferror(fd)) {
            error_marker(__FILE__, __LINE__);
            error("Failed to read file '%s': %s\n", path, strerror(errno));
        }
    }

    if (fclose(fd) != 0) {
        error_marker(__FILE__, __LINE__);
        warning("Failed to close file '%s': %s\n", path, strerror(errno));
    }

    proc_tester->cursor = 0;
    proc_tester->test_count = 0;

    proc_tester->error = ProcessorTesterOK;
}

static u8 read_u8(ProcTester *proc_tester) {
    return (u8) *(proc_tester->data_buffer + proc_tester->cursor++);
}

static u16 read_u16(ProcTester *proc_tester) {
    u8 high = (u8) *(proc_tester->data_buffer + proc_tester->cursor++);
    u8 low = (u8) *(proc_tester->data_buffer + proc_tester->cursor++);
    return (high << 8) | low;
}

static void read_state(ProcTester *proc_tester, ProcState *state) {
    state->pc = read_u16(proc_tester);

    state->s = read_u8(proc_tester);
    state->a = read_u8(proc_tester);
    state->x = read_u8(proc_tester);
    state->y = read_u8(proc_tester);
    state->p = read_u8(proc_tester);

    state->ram_size = read_u8(proc_tester);
    for (int i = 0; i < state->ram_size; ++i) {
        state->ram[i].addr = read_u16(proc_tester);
        state->ram[i].value = read_u8(proc_tester);
    }
}

static void read_cycles(ProcTester *proc_tester) {
    proc_tester->proc_test.cycle_count = read_u8(proc_tester);
    for (int i = 0; i < proc_tester->proc_test.cycle_count; ++i) {
        proc_tester->proc_test.cycles[i].addr = read_u16(proc_tester);
        proc_tester->proc_test.cycles[i].value = read_u8(proc_tester);
        char activity = (char) read_u8(proc_tester);
        switch (activity) {
            case 'r': {
                proc_tester->proc_test.cycles[i].activity = READ_CYCLE;
                break;
            }
            case 'w': {
                proc_tester->proc_test.cycles[i].activity = WRITE_CYCLE;
                break;
            }
            default:
                error("Unknown activity type for cycle: %c", activity);
        }
    }
}

bool read_next_processor_test(ProcTester *proc_tester) {
    if (proc_tester->cursor == proc_tester->buffer_size) {
        proc_tester->error = ProcessorTesterEOF;
        return false;
    }

    proc_tester->proc_test.name = (char *) (proc_tester->data_buffer + proc_tester->cursor);
    proc_tester->cursor += 10;

    read_state(proc_tester, &proc_tester->proc_test.initial);
    read_state(proc_tester, &proc_tester->proc_test.final);
    read_cycles(proc_tester);

    proc_tester->error = ProcessorTesterOK;
    proc_tester->test_count++;
    return true;
}

void clear_processor_tester(ProcTester *proc_tester) {
    proc_tester->cursor = 0;
    proc_tester->test_count = 0;

    if (proc_tester->buffer_size > 0) {
        free(proc_tester->data_buffer);
        proc_tester->data_buffer = NULL;
        proc_tester->buffer_size = 0;
    }

    if (proc_tester->proc_test.name != NULL) {
        proc_tester->proc_test.name = NULL;
    }
    proc_tester->proc_test.cycle_count = 0;
    proc_tester->proc_test.initial.ram_size = 0;
    proc_tester->proc_test.final.ram_size = 0;

    proc_tester->error = ProcessorTesterOK;
}

static char name_buffer[100];
static char path_buffer[100];
static char flags_buffer_1[9];
static char flags_buffer_2[9];

static void load_state(Machine *machine, ProcState *state) {
    machine->cpu.PC = state->pc;
    machine->cpu.S = state->s;
    machine->cpu.A = state->a;
    machine->cpu.X = state->x;
    machine->cpu.Y = state->y;
    machine->cpu.P.status = state->p;

    for (u8 i = 0; i < state->ram_size; ++i) {
        machine->ram[state->ram[i].addr] = state->ram[i].value;
    }

    machine->halt = false;
}

static void print_failure_warning(char *path, ProcTester *proc_tester) {
    warning("test #%d failed - %s", proc_tester->test_count, proc_tester->proc_test.name);
    warning("test failed at '%s:%d'", path, proc_tester->cursor);
}

static bool compare_cycles_equals(Machine *machine, ProcTest *proc_test) {
    s8 fail_index = -1;

    for (u8 i = 0; i < proc_test->cycle_count; ++i) {
        if (machine->cycles[i].addr != proc_test->cycles[i].addr) {
            fail_index = (s8) i;
            break;
        }

        if (machine->cycles[i].value != proc_test->cycles[i].value) {
            fail_index = (s8) i;
            break;
        }

        CycleActivity machine_activity = machine->cycles[i].activity;
        if (machine_activity == IDLE_CYCLE) {
            machine_activity = READ_CYCLE;
        }
        if (machine_activity != proc_test->cycles[i].activity) {
            fail_index = (s8) i;
            break;
        }
    }

    if (fail_index >= 0) {
        warning("cycle #%d mismatch", fail_index + 1);

        for (u8 i = 0; i <= fail_index; ++i) {
            u16 e_addr = proc_test->cycles[i].addr;
            u8 e_value = proc_test->cycles[i].value;
            CycleActivity e_cyc_act = proc_test->cycles[i].activity;
            char *e_act = e_cyc_act == WRITE_CYCLE ? "Write" : "Read";

            u16 r_addr = machine->cycles[i].addr;
            u8 r_value = machine->cycles[i].value;
            CycleActivity r_cyc_act = machine->cycles[i].activity;
            char *r_act = r_cyc_act == WRITE_CYCLE ? "Write" : "Read";

            if (i < fail_index) {
                char *msg = "#%d  $%04X = %02X %s == $%04X = %02X %s";
                warning(msg, i + 1, e_addr, e_value, e_act, r_addr, r_value, r_act);
            } else {
                char *msg = "#%d  $%04X = %02X %s != \x1b[0;31m$%04X = %02X %s";
                warning(msg, i + 1, e_addr, e_value, e_act, r_addr, r_value, r_act);
            }
        }

        return false;
    }

    return true;
}

static char *decode_flags(CPUFlags flags, char *buf) {
    buf[0] = flags.N ? 'N' : '.';
    buf[1] = flags.V ? 'V' : '.';
    buf[2] = flags.U ? 'U' : '.';
    buf[3] = flags.B ? 'B' : '.';
    buf[4] = flags.D ? 'D' : '.';
    buf[5] = flags.I ? 'I' : '.';
    buf[6] = flags.Z ? 'Z' : '.';
    buf[7] = flags.C ? 'C' : '.';
    buf[8] = 0;
    return buf;
}

static bool compare_state_equal(Machine *machine, ProcState *state) {
    bool equal = true;
    CPU cpu = machine->cpu;
    if (cpu.PC != state->pc) {
        printf("PC $%04X != $%04X [%d] (expected)\n", cpu.PC, state->pc, state->pc);
        equal = false;
    }
    if (cpu.S != state->s) {
        printf("S 0x%02X != 0x%02X (expected)\n", cpu.S, state->s);
        equal = false;
    }

    if (cpu.A != state->a) {
        printf("A 0x%02X != 0x%02X (expected)\n", cpu.A, state->a);
        equal = false;
    }

    if (cpu.X != state->x) {
        printf("X 0x%02X != 0x%02X (expected)\n", cpu.X, state->x);
        equal = false;
    }

    if (cpu.Y != state->y) {
        printf("Y 0x%02X != 0x%02X (expected)\n", cpu.Y, state->y);
        equal = false;
    }

    if (cpu.P.status != state->p) {
        CPUFlags final_flags = {.status=state->p};
        char *flags_m = decode_flags(cpu.P, flags_buffer_1);
        char *flags_f = decode_flags(final_flags, flags_buffer_2);
        printf("P %s != %s (expected)\n", flags_m, flags_f);
        equal = false;
    }

    for (u8 i = 0; i < state->ram_size; ++i) {
        u16 ram_addr = state->ram[i].addr;
        u8 ram_value = machine->ram[ram_addr];
        if (ram_value != state->ram[i].value) {
            printf("RAM [$%04X]=0x%02X != [%d]=0x%02X\n", ram_addr, ram_value, i, state->ram[i].value);
            equal = false;
        }
    }
    return equal;
}

static char *addr_mode_name(AddrMode mode) {
    switch (mode) {
        case Absolute:
            return "$nnnn";
        case AbsoluteIndirect:
            return "($nnnn)";
        case Accumulator:
            return "A";
        case Immediate:
            return "#$nn";
        case Implied:
            return "";
        case Relative:
            return "$nnnn";
        case XIndexedAbsolute:
            return "$nnnn,X";
        case XIndexedZeroPage:
            return "$nn,X";
        case XIndexedZeroPageIndirect:
            return "($nn,X)";
        case YIndexedAbsolute:
            return "$nnnn,Y";
        case YIndexedZeroPage:
            return "$nn,Y";
        case ZeroPage:
            return "$nn";
        case ZeroPageIndirectYIndexed:
            return "($nn),Y";
        default:
            error_marker(__FILE__, __LINE__);
            error("addressing mode %d not implemented", mode);
    }
}


static bool run_opcode_tests(Machine *machine, ProcTester *proc_tester, Opcode *opcode, u16 opc) {
    while (true) {
        read_next_processor_test(proc_tester);

        if ((*proc_tester).error == ProcessorTesterEOF) {
            break; // done
        } else if ((*proc_tester).error != ProcessorTesterOK) {
            error("Unknown ProcessorTester error type %d", (*proc_tester).error);
        } else {
            ProcTest proc_test = (*proc_tester).proc_test;
            ProcState initial = proc_test.initial;
            load_state(machine, &initial);

            machine_tick(machine);

            ProcState final = proc_test.final;
            if (!compare_state_equal(machine, &final)) {
                compare_cycles_equals(machine, &proc_test);
                u16 pc = initial.pc;
                machine->cpu.PC = pc;
                disassemble_instruction(machine, pc, opc, (*opcode));
                cpu_instruction_context(machine);
                print_failure_warning(path_buffer, proc_tester);
                return false;
            } else if ((*machine).cycle_count != proc_test.cycle_count) {
                char *msg = "Cycle count mismatch expected: %d got: %d";
                warning(msg, proc_test.cycle_count, (*machine).cycle_count);
                compare_cycles_equals(machine, &proc_test);
                print_failure_warning(path_buffer, proc_tester);
                return false;
            } else if (!compare_cycles_equals(machine, &proc_test)) {
                u16 pc = initial.pc;
                machine->cpu.PC = pc;
                disassemble_instruction(machine, pc, opc, (*opcode));
                cpu_instruction_context(machine);
                print_failure_warning(path_buffer, proc_tester);
                return false;
            }
        }
    }
    return true;
}

Tester run_processor_tests(Tester *tester, Machine *machine, const char *path_prefix) {
    ProcTester proc_tester = {0};
    for (u16 opc = 0x00; opc <= 0xff; ++opc) {
        clear_processor_tester(&proc_tester);
        Opcode opcode = fetch_opcode(opc, (*machine).cpu.cpu_type);
        if (opcode.op_fn != NULL) {
            sprintf(path_buffer, "%s/%02x.bin", path_prefix, opc);

            read_processor_tests(&proc_tester, path_buffer);

            bool success = run_opcode_tests(machine, &proc_tester, &opcode, opc);

            char *amn = addr_mode_name(opcode.addr_mode);
            if (!success) {
                sprintf(name_buffer, "#%02X %s %s test failed", opc, opcode.name, amn);
                test_fail(tester, name_buffer);
            } else {
                sprintf(name_buffer, "#%02X %s %s", opc, opcode.name, amn);
                test_success(tester, name_buffer);
            }
        }
    }
    return (*tester);
}
