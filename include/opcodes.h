#pragma once

#include "machine.h"
#include "seaio.h"

typedef struct {
    void (*op_fn)(Machine *m, AddrMode addr_mode); // operation
    AddrMode addr_mode;   // addressing mode
    const char *name;    // opcode mnemonic
} Opcode;

Opcode fetch_opcode(u8 opcode_num, CPUType cpu_type);
