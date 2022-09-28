#include <stdlib.h>
#include "seatest.h"
#include "seaio.h"
#include "debug.h"
#include "machine.h"
#include "disassembly.h"


int main(void) {
    Tester tester = create_tester("Klaus's Functional Tests");

    test_section("Functional Tests");

    Machine machine = machine_create();

    Binary data = read_binary("../tests/bins/6502_interrupt_test.bin");
    for (size_t i = 0; i < data.size; ++i) {
        machine.ram[i] = data.data[i];
    }

    machine.cpu.PC = 0x0400;  // this address is where the test program starts

    const u16 interrupt_port = 0xbffc;

    while (true) {
        u16 pc = machine.cpu.PC;
        u8 opcode_num = machine.ram[pc];
        Opcode opcode = fetch_opcode(opcode_num);

        machine_tick(&machine);

        disassemble_instruction(&machine, pc, opcode_num, opcode);

        if (pc == machine.cpu.PC) {
            cpu_error_marker(&machine, __FILE__, __LINE__);
            cpu_error(&machine, "machine trapped at %04X", pc);
        }

        u8 port_value = machine.ram[interrupt_port];
        if ((port_value & 0x02) == 0x02) {  // check NMI bit
            machine_nmi(&machine);
            port_value = port_value & ~0x02;
        }

        if (!machine.cpu.P.I && (port_value & 0x01) == 0x01) {  // check IRQ bit
            machine_irq(&machine);
            port_value = port_value & ~0x01;
        }

        machine.ram[interrupt_port] = port_value;

        pc = machine.cpu.PC;
        if (pc == 0x06f5) {
            test_true(&tester, true, "Interrupt test completed!");
            break;
        }
    }

    test_summary(&tester);

    exit(tester.fail);
}
