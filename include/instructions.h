#pragma once

#include "machine.h"


// BNE - Branch on Result Not Zero
// Operation: Branch on Z = 0
void bne(Machine *m, AddrMode addr_mode);

// BEQ - Branch on Result Zero
// Operation: Branch on Z = 1
void beq(Machine *m, AddrMode addr_mode);

// BPL - Branch on Result Plus
// Operation: Branch on N = 0
void bpl(Machine *m, AddrMode addr_mode);

// CLD - Clear Decimal Flag
// Operation: 0 → D
void cld(Machine *m, AddrMode addr_mode);

// CMP - Compare Memory and Accumulator
// Operation: A - M
void cmp(Machine *m, AddrMode addr_mode);

// DEX - Decrement Index Register X By One
// Operation: X - 1 → X
void dex(Machine *m, AddrMode addr_mode);

// DEY - Decrement Index Register Y By One
// Operation: Y - 1 → Y
void dey(Machine *m, AddrMode addr_mode);

// JMP - JMP Indirect
// Operation: [PC + 1] → PCL, [PC + 2] → PCH
void jmp(Machine *m, AddrMode addr_mode);

// LDA - Load Accumulator with Memory
// Operation: M → A
void lda(Machine *m, AddrMode addr_mode);

// LDY - Load Index Register Y From Memory
// Operation: M → Y
void ldy(Machine *m, AddrMode addr_mode);

// Load Index Register X From Memory
// Operation: M → X
void ldx(Machine *m, AddrMode addr_mode);

// NOP - No Operation
// Operation: No operation
void nop(Machine *m, AddrMode addr_mode);

// STA - Store Accumulator in Memory
// Operation: A → M
void sta(Machine *m, AddrMode addr_mode);

// TAX - Transfer Accumulator To Index X
// Operation: A → X
void tax(Machine *m, AddrMode addr_mode);

// TXS - Transfer Index X To Stack Pointer
// Operation: X → S
void txs(Machine *m, AddrMode addr_mode);

// TYA - Transfer Index Y To Accumulator
// Operation: Y → A
void tya(Machine *m, AddrMode addr_mode);



