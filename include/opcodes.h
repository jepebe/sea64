#pragma once

#include "machine.h"

typedef struct {
    void (*op_fn)(Machine *m, AddrMode addr_mode); // operation
    AddrMode addr_mode;
    const char *name;                                      // opcode mnemonic
} Opcode;

extern const Opcode opcodes[256];

Opcode fetch_opcode(u8 opcode);
