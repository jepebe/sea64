#include <stdarg.h>
#include <stdlib.h>
#include "debug.h"
#include "stdio.h"

void cpu_instruction_context(Machine *machine) {
    u16 segment = machine->cpu.PC & 0xFFF0;
    if (segment > 0x10) {
        segment -= 0x10;
    }

    printf("\x1b[0;31m        ");
    for (int i = 0; i < 16; ++i) {
        printf("%2X ", i);
    }
    printf("\x1b[0m\n");

    for(int seg = 0; seg < 3; ++seg) {
        printf("\x1b[0;31m[$%04X] ", segment);
        for (int i = 0; i < 16; ++i) {
            u32 addr = segment + i;
            printf("%02X ", machine->ram[addr]);
        }
        printf("\x1b[0m\n");
        segment += 0x10;
    }
}

NO_RETURN void cpu_error(Machine *machine, char *message, ...) {
    va_list argp;
    printf("\x1b[0;31m");
    printf("[$%04X] Error: ", machine->cpu.PC);
    va_start(argp, message);
    vprintf(message, argp);
    va_end(argp);
    printf("\x1b[0m\n");
    cpu_instruction_context(machine);
    exit(1);
}

void cpu_error_marker(Machine *machine, char *file, int line) {
    printf("\x1b[0;33m");
    printf("[$%04X] Note: %s:%d", machine->cpu.PC, file, line);
    printf("\x1b[0m\n");
}
