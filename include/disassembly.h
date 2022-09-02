#pragma once

#include "machine.h"
#include "opcodes.h"

void disassemble_instruction(Machine *machine, u16 addr, u8 opc, Opcode opcode);
