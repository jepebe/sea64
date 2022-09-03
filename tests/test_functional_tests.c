#include <stdlib.h>
#include "seatest.h"
#include "seaio.h"
#include "machine.h"
#include "debug.h"

int main(void) {
    Tester tester = create_tester("Klaus's Functional Tests");

    test_section("Functional Tests");

    Machine machine = machine_create();

    Binary data = read_binary("../tests/bins/6502_functional_test.bin");
    for(size_t i = 0; i < data.size; ++i) {
        machine.ram[i] = data.data[i];
    }

    machine.cpu.PC = 0x0400;  // this address is where the test program starts

    while (true) {
        u16 pc = machine.cpu.PC;
        machine_tick(&machine);

        if(pc == machine.cpu.PC) {
            cpu_error_marker(&machine, __FILE__, __LINE__);
            cpu_error(&machine, "machine trapped at %04X", pc);
        }

        if(pc == 0x336D) {
            test_true(&tester, true, "Functional tests before BCD completed!");
            break;
        }
    }

    test_summary(&tester);

    exit(tester.fail);
}
