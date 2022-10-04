#include "seatest.h"
#include <stdlib.h>

int main(void) {
    Tester tester = create_tester("Sample Testcase");

    test_section("Sample Section");

    test_summary(&tester);

    exit(tester.fail);
}
