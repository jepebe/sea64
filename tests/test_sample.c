#include <stdlib.h>
#include "seatest.h"

int main(void) {
    Tester tester = create_tester("Sample Testcase");

    test_section("Sample Section");


    test_summary(&tester);

    exit(tester.fail);
}
