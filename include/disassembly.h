#pragma once

#include "machine.h"
#include "opcodes_6502.h"

void disassemble_instruction(Machine *machine, u16 addr, u8 opc, Opcode opcode);
