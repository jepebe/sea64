#include <stdlib.h>
#include "seatest.h"
#include "machine.h"
#include "opcodes_6502.h"
#include "debug.h"
#include "proc_tester.h"
#include "disassembly.h"

static char name_buffer[100];
static char path_buffer[100];

void load_state(Machine *machine, ProcState *state) {
    machine->cpu.PC = state->pc;
    machine->cpu.S = state->s;
    machine->cpu.A = state->a;
    machine->cpu.X = state->x;
    machine->cpu.Y = state->y;
    machine->cpu.P.status = state->p;

    for (u8 i = 0; i < state->ram_size; ++i) {
        machine->ram[state->ram[i].addr] = state->ram[i].value;
    }
}

static char flags_buffer_1[9];
static char flags_buffer_2[9];

void print_failure_warning(char *path, ProcTester *proc_tester) {
    u64 error_line = proc_tester->error_line - 1;
    u64 error_loc = proc_tester->error_loc;
    warning("test #%d failed - %s", proc_tester->test_count, proc_tester->proc_test.name);
    warning("test failed at '%s:%d:%d'", path, error_line, error_loc);
}

bool compare_cycles_equals(Machine *machine, ProcTest *proc_test) {
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

            if(i < fail_index) {
                warning("#%d  $%04X,%02X %s == $%04X,%02X %s", i + 1, e_addr, e_value, e_act, r_addr, r_value, r_act);
            } else {
                warning("#%d  $%04X,%02X %s != \x1b[0;31m$%04X,%02X %s", i + 1, e_addr, e_value, e_act, r_addr, r_value, r_act);
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

bool compare_state_equal(Machine *machine, ProcState *state) {
    bool equal = true;
    if (machine->cpu.PC != state->pc) {
        printf("PC $%04X != $%04X\n", machine->cpu.PC, state->pc);
        equal = false;
    }
    if (machine->cpu.S != state->s) {
        printf("S 0x%02X != 0x%02X\n", machine->cpu.S, state->s);
        equal = false;
    }

    if (machine->cpu.A != state->a) {
        printf("A 0x%02X != 0x%02X\n", machine->cpu.A, state->a);
        equal = false;
    }

    if (machine->cpu.X != state->x) {
        printf("X 0x%02X != 0x%02X\n", machine->cpu.X, state->x);
        equal = false;
    }

    if (machine->cpu.Y != state->y) {
        printf("Y 0x%02X != 0x%02X\n", machine->cpu.Y, state->y);
        equal = false;
    }

    if (machine->cpu.P.status != state->p) {
        CPUFlags final_flags = {.status=state->p};
        char *flags_m = decode_flags(machine->cpu.P, flags_buffer_1);
        char *flags_f = decode_flags(final_flags, flags_buffer_2);
        printf("P %s != %s A=%02X\n", flags_m, flags_f, machine->cpu.A);
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

char *addr_mode_name(AddrMode mode) {
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


int main(void) {
    Tester tester = create_tester("Processor Tests");

    test_section("Processor Tests");

    Machine machine = machine_create();

    char *path_prefix = "../tests/processor_tests";
    ProcTester proc_tester = {0};
    clear_processor_tester(&proc_tester);

    for (u16 opc = 0x00; opc < 0xff; ++opc) {
        Opcode opcode = fetch_opcode(opc);
        if (opcode.op_fn != NULL) {
            sprintf(path_buffer, "%s/%02x.json", path_prefix, opc);

            read_processor_test(&proc_tester, path_buffer);
            if (proc_tester.error != ProcessorTesterOK) {
                error_marker(__FILE__, __LINE__);
                warning("failed to open '%s'", path_buffer);

                sprintf(name_buffer, "%s %s", opcode.name, addr_mode_name(opcode.addr_mode));
                test_fail(&tester, name_buffer);
            } else {
                bool error = false;
                u64 tests = 0;
                u64 failures = 0;
                while (!error) {
                    parse_next_processor_test(&proc_tester);

                    if (proc_tester.error == ProcessorTesterEOF) {
                        break; // done
                    } else if (proc_tester.error != ProcessorTesterOK) {
                        error = true;
                    } else {
                        tests++;

                        ProcTest proc_test = proc_tester.proc_test;
                        ProcState initial = proc_test.initial;
                        load_state(&machine, &initial);

                        machine_tick(&machine);

                        ProcState final = proc_test.final;
                        if (!compare_state_equal(&machine, &final)) {
                            u16 pc = initial.pc;
                            machine.cpu.PC = pc;
                            disassemble_instruction(&machine, pc, opc, opcode);
                            cpu_instruction_context(&machine);
                            print_failure_warning(path_buffer, &proc_tester);
                            failures++;
                            break; // break to limit number of error messages per test
                        } else if (machine.cycle_count != proc_test.cycle_count) {
                            warning("Cycle count mismatch expected: %d got: %d", proc_test.cycle_count, machine.cycle_count);
                            compare_cycles_equals(&machine, &proc_test);
                            print_failure_warning(path_buffer, &proc_tester);
                            failures++;
                            break;
                        } else if (!compare_cycles_equals(&machine, &proc_test)) {
                            print_failure_warning(path_buffer, &proc_tester);
                            failures++;
                            break;
                        }
                    }
                }

                if (error) {
                    error_marker(__FILE__, __LINE__);
                    u64 error_line = proc_tester.error_line;
                    u64 error_loc = proc_tester.error_loc;
                    warning("failed to parse token at '%s:%d:%d'", path_buffer, error_line, error_loc);
                    warning("failure code '%d'", proc_tester.error);
                    test_fail(&tester, "Parsing error");
                } else if (failures) {
                    char *amn = addr_mode_name(opcode.addr_mode);
                    sprintf(name_buffer, "#%02X %s %s %llu/%llu tests failed", opc, opcode.name, amn, failures, tests);
                    test_fail(&tester, name_buffer);
                } else {
                    sprintf(name_buffer, "#%02X %s %s", opc, opcode.name, addr_mode_name(opcode.addr_mode));
                    test_success(&tester, name_buffer);
                }
            }

            clear_processor_tester(&proc_tester);
        }
    }

    test_summary(&tester);

    exit(tester.fail);
}
