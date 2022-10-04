#include "debug.h"
#include "machine.h"
#include "proc_tester.h"
#include <stdlib.h>

int main(void) {
    Tester tester = create_tester("Processor Tests 6502 Extended");
    test_section("Processor Tests 6502 Extended");

    Machine machine = machine_create();
    machine.cpu.cpu_type = MOS6502EXT;

    char *path_prefix = "../tests/processor_tests";
    tester = run_processor_tests(&tester, &machine, path_prefix);
    test_summary(&tester);
    exit(tester.fail);
}
