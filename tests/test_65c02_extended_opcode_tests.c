#include "debug.h"
#include "disassembly.h"
#include "machine.h"
#include "seaio.h"
#include "seatest.h"
#include <stdlib.h>

int main(void) {
    Tester tester = create_tester("Klaus's Extended Functional Tests");

    test_section("Extended Functional Tests");

    Machine machine = machine_create();
    machine.cpu.cpu_type = WDC65C02;

    Binary data = read_binary("../tests/bins/65C02_extended_opcode_test.bin");
    for (size_t i = 0; i < data.size; ++i) {
        machine.ram[i] = data.data[i];
    }

    machine.cpu.PC = 0x0400; // this address is where the test program starts

    while (true) {
        u16 pc = machine.cpu.PC;
        // u8 opcode_num = machine.ram[pc];
        // Opcode opcode = fetch_opcode(opcode_num, WDC65C02);
        machine_tick(&machine);

        // disassemble_instruction(&machine, pc, opcode_num, opcode);

        if (pc == machine.cpu.PC) {
            cpu_error_marker(&machine, __FILE__, __LINE__);
            cpu_error(&machine, "machine trapped at %04X", pc);
        }

        pc = machine.cpu.PC;
        if (pc == 0x24f1) {
            test_true(&tester, true, "Extended tests completed!");
            break;
        }
    }

    test_summary(&tester);

    exit(tester.fail);
}
