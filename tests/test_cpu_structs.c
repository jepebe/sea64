#include "cpu.h"
#include "seatest.h"
#include <stdlib.h>

int main(void) {
    Tester tester = create_tester("CPU Structs tests");

    test_section("CPU Flags");

    CPUFlags flags;
    flags.status = 0x0;

    test_false(&tester, flags.C, "CPU flag C");
    test_false(&tester, flags.Z, "CPU flag Z");
    test_false(&tester, flags.I, "CPU flag I");
    test_false(&tester, flags.D, "CPU flag D");
    test_false(&tester, flags.B, "CPU flag B");
    test_false(&tester, flags.V, "CPU flag V");
    test_false(&tester, flags.N, "CPU flag N");

    flags.status = 0x01;
    test_true(&tester, flags.C, "CPU flag C");
    flags.status = 0x02;
    test_true(&tester, flags.Z, "CPU flag Z");
    flags.status = 0x04;
    test_true(&tester, flags.I, "CPU flag I");
    flags.status = 0x08;
    test_true(&tester, flags.D, "CPU flag D");
    flags.status = 0x10;
    test_true(&tester, flags.B, "CPU flag B");
    flags.status = 0x40;
    test_true(&tester, flags.V, "CPU flag V");
    flags.status = 0x80;
    test_true(&tester, flags.N, "CPU flag N");

    flags.status = 0x0;
    flags.C = true;
    test_true(&tester, flags.C, "CPU flag C");
    test_false(&tester, flags.Z, "CPU flag Z");
    test_false(&tester, flags.I, "CPU flag I");
    test_false(&tester, flags.D, "CPU flag D");
    test_false(&tester, flags.B, "CPU flag B");
    test_false(&tester, flags.V, "CPU flag V");
    test_false(&tester, flags.N, "CPU flag N");

    flags.status = 0x0;
    flags.Z = true;
    test_false(&tester, flags.C, "CPU flag C");
    test_true(&tester, flags.Z, "CPU flag Z");
    test_false(&tester, flags.I, "CPU flag I");
    test_false(&tester, flags.D, "CPU flag D");
    test_false(&tester, flags.B, "CPU flag B");
    test_false(&tester, flags.V, "CPU flag V");
    test_false(&tester, flags.N, "CPU flag N");

    flags.status = 0x0;
    flags.N = true;
    test_false(&tester, flags.C, "CPU flag C");
    test_false(&tester, flags.Z, "CPU flag Z");
    test_false(&tester, flags.I, "CPU flag I");
    test_false(&tester, flags.D, "CPU flag D");
    test_false(&tester, flags.B, "CPU flag B");
    test_false(&tester, flags.V, "CPU flag V");
    test_true(&tester, flags.N, "CPU flag N");

    test_summary(&tester);

    exit(tester.fail);
}
