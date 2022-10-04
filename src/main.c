#include "seaio.h"
#include <stdio.h>

int main(void) {
    Binary bin_file = read_binary("sea64");
    printf("Hello, World! %zu bytes\n", bin_file.size);
    return 0;
}
