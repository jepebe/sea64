#include <stdlib.h>
#include "seatest.h"
#include "machine.h"
#include "opcodes_6502.h"
#include "debug.h"
#include "proc_tester.h"
#include "disassembly.h"

static char name_buffer[20];
static char path[100];

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

bool compare_state_equal(Machine *machine, ProcState *state) {
    if (machine->cpu.PC != state->pc) {
        printf("PC $%04X != $%04X\n", machine->cpu.PC, state->pc);
        return false;
    } else if (machine->cpu.S != state->s) {
        printf("S $%02X != $%02X\n", machine->cpu.S, state->s);
        return false;
    } else if (machine->cpu.A != state->a) {
        printf("A ");
        return false;
    } else if (machine->cpu.X != state->x) {
        printf("X ");
        return false;
    } else if (machine->cpu.Y != state->y) {
        printf("Y ");
        return false;
    } else if (machine->cpu.P.status != state->p) {
        printf("P ");
        return false;
    } else {
        for (u8 i = 0; i < state->ram_size; ++i) {
            if (machine->ram[state->ram[i].addr] != state->ram[i].value) {
                printf("RAM %d ", i);
                return false;
            }
        }
    }
    return true;
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
        if(opc == 0x61 || opc == 0x65 || opc == 0x69 || opc == 0x6D) {
            continue;
        }

        if(opc == 0x71 || opc == 0x75 || opc == 0x79 || opc == 0x7D) {
            continue;
        }

        if(opc == 0xE1 || opc == 0xE5 || opc == 0xE9 || opc == 0xED) {
            continue;
        }

        if(opc == 0xF1 || opc == 0xF5 || opc == 0xF9 || opc == 0xFD) {
            continue;
        }


        Opcode opcode = fetch_opcode(opc);
        if (opcode.op_fn != NULL) {
            sprintf(path, "%s/%02x.json", path_prefix, opc);

            read_processor_test(&proc_tester, path);
            if (proc_tester.error != ProcessorTesterOK) {
                error_marker(__FILE__, __LINE__);
                warning("failed to open '%s'", path);

                sprintf(name_buffer, "%s %s", opcode.name, addr_mode_name(opcode.addr_mode));
                test_fail(&tester, name_buffer);
            } else {
                bool error = false;
                u64 tests = 0;
                u64 failures = 0;
                while (!error) {
                    parse_next_processor_test(&proc_tester);

                    if (proc_tester.error == ProcessorTesterEOF) {
                        break;
                    } else if (proc_tester.error != ProcessorTesterOK) {
                        error = true;
                    } else {
                        tests++;

                        ProcState initial = proc_tester.proc_test.initial;
                        load_state(&machine, &initial);

                        machine_tick(&machine);

                        ProcState final = proc_tester.proc_test.final;
                        if (!compare_state_equal(&machine, &final)) {
                            u16 pc = proc_tester.proc_test.initial.pc;
                            machine.cpu.PC = pc;
                            disassemble_instruction(&machine, pc, opc, opcode);
                            cpu_instruction_context(&machine);
                            u64 error_line = proc_tester.error_line - 1;
                            u64 error_loc = proc_tester.error_loc;
                            warning("test #%d failed - %s", proc_tester.test_count, proc_tester.proc_test.name);
                            warning("test failed at '%s:%d:%d'", path, error_line, error_loc);
                            failures++;
                        }
                    }
                }

                if (error) {
                    error_marker(__FILE__, __LINE__);
                    u64 error_line = proc_tester.error_line;
                    u64 error_loc = proc_tester.error_loc;
                    warning("failed to parse token at '%s:%d:%d'", path, error_line, error_loc);
                    warning("failure code '%d'", proc_tester.error);

                    test_fail(&tester, "Parsing error");
                } else if(failures) {
                    sprintf(name_buffer, "%s %s", opcode.name, addr_mode_name(opcode.addr_mode));

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
