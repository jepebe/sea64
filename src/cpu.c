#include "cpu.h"

void cpu_reset(CPU *cpu) {
    cpu->PC = 0xFFFC;
    cpu->A = 0x00;
    cpu->X = 0x00;
    cpu->Y = 0x00;
    cpu->S = 0xFD;
    cpu->P.status = 0x0;
    cpu->P.U = 1;
    cpu->P.I = 1;
}
