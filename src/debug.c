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

    for (int seg = 0; seg < 3; ++seg) {
        printf("\x1b[0;31m[$%04X] ", segment);
        for (int i = 0; i < 16; ++i) {
            u32 addr = segment + i;
            if (addr == machine->cpu.PC) {
                printf("\x1b[1;91m");
            }
            printf("%02X ", machine->ram[addr]);
            if (addr == machine->cpu.PC) {
                printf("\x1b[0;31m");
            }
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

    printf("\n");
    cpu_stack_contents(machine);

    printf("\x1b[0;31mFailure after %llu instructions.", machine->cpu.tick_count);
    printf("\x1b[0m\n");
    exit(1);
}

void cpu_error_marker(Machine *machine, char *file, int line) {
    printf("\x1b[0;33m");
    printf("[$%04X] Note: %s:%d", machine->cpu.PC, file, line);
    printf("\x1b[0m\n");
}

void cpu_stack_contents(Machine *machine) {
    printf("\x1b[0;31m");
    printf("[stack]\n");
    u8 sp = 0xFF;
    for (int row = 0; row < 16; ++row) {
        printf("[$%04X] ", 0x100 + sp);
        for (int col = 0; col < 16; ++col) {
            if (sp == machine->cpu.S) {
                printf("\x1b[1;91m");
            }
            printf("%02X ", machine->ram[0x100 + sp]);
            if (sp == machine->cpu.S) {
                printf("\x1b[0;31m");
            }
            sp--;
        }
        printf("\n");
        if (sp < machine->cpu.S) {
            break;
        }
    }

    printf("\x1b[0m\n");
}


void error_marker(char *file, int line) {
    printf("\x1b[0;33m");
    printf("Note: %s:%d", file, line);
    printf("\x1b[0m\n");
}

NO_RETURN void error(char *message, ...) {
    va_list argp;
    printf("\x1b[0;31m");

    printf("Error: ");
    va_start(argp, message);
    vprintf(message, argp);
    va_end(argp);
    printf("\x1b[0m\n");

    exit(1);
}

void warning(char *message, ...) {
    va_list argp;
    printf("\x1b[0;33m");

    printf("Warning: ");
    va_start(argp, message);
    vprintf(message, argp);
    va_end(argp);
    printf("\x1b[0m\n");
}
