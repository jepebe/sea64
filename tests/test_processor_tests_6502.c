#include "debug.h"
#include "machine.h"
#include "proc_tester.h"
#include <stdlib.h>

int main(void) {
    Tester tester = create_tester("Processor Tests 6502");
    test_section("Processor Tests 6502");

    Machine machine = machine_create();
    machine.cpu.cpu_type = MOS6502;

    char *path_prefix = "../tests/processor_tests/6502";
    tester = run_processor_tests(&tester, &machine, path_prefix);
    test_summary(&tester);
    exit(tester.fail);
}
