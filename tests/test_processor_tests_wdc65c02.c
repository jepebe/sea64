#include "debug.h"
#include "machine.h"
#include "proc_tester.h"
#include <stdlib.h>

int main(void) {
    Tester tester = create_tester("Processor Tests WDC65C02");
    test_section("Processor Tests WDC65C02");

    Machine machine = machine_create();
    machine.cpu.cpu_type = WDC65C02;

    char *path_prefix = "../tests/processor_tests/wdc65c02";
    tester = run_processor_tests(&tester, &machine, path_prefix);
    test_summary(&tester);
    exit(tester.fail);
}
