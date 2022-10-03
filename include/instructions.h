#pragma once

#include "machine.h"

void adjust_zero_and_negative_flag(Machine *m, u8 value);

void page_cross_behavior(Machine *m, AddrMode addr_mode, u16 addr);

void add(Machine *m, u8 value);

void bcd_add(Machine *m, u8 value);

void bcd_sub(Machine *m, u8 value);

// ADC - Add Memory to Accumulator with Carry
// Operation: A + M + C → A, C
void adc(Machine *m, AddrMode addr_mode);

// AND - "AND" Memory with Accumulator
// Operation: A ∧ M → A
void and(Machine *m, AddrMode addr_mode);

// ASL - Arithmetic Shift Left
// Operation: C ← /M7...M0/ ← 0
void asl(Machine *m, AddrMode addr_mode);

// BCC - Branch on Carry Clear
// Operation: Branch on C = 0
void bcc(Machine *m, AddrMode addr_mode);

// BCS - Branch on Carry Set
// Operation: Branch on C = 1
void bcs(Machine *m, AddrMode addr_mode);

// BIT - Test Bits in Memory with Accumulator
// Operation: A ∧ M, M7 → N, M6 → V
void bit(Machine *m, AddrMode addr_mode);

// BMI - Branch on Result Minus
// Operation: Branch on N = 1
void bmi(Machine *m, AddrMode addr_mode);

// BNE - Branch on Result Not Zero
// Operation: Branch on Z = 0
void bne(Machine *m, AddrMode addr_mode);

// BEQ - Branch on Result Zero
// Operation: Branch on Z = 1
void beq(Machine *m, AddrMode addr_mode);

// BPL - Branch on Result Plus
// Operation: Branch on N = 0
void bpl(Machine *m, AddrMode addr_mode);

// BRK - Break Command
// Operation: PC + 2↓, [FFFE] → PCL, [FFFF] → PCH
void brk(Machine *m, AddrMode addr_mode);

// BVC - Branch on Overflow Clear
// Operation: Branch on V = 0
void bvc(Machine *m, AddrMode addr_mode);

// BVS - Branch on Overflow Set
// Operation: Branch on V = 1
void bvs(Machine *m, AddrMode addr_mode);

// CLC - Clear Carry Flag
// Operation: 0 → C
void clc(Machine *m, AddrMode addr_mode);

// CLD - Clear Decimal Flag
// Operation: 0 → D
void cld(Machine *m, AddrMode addr_mode);

// CLI - Clear Interrupt Disable
// Operation: 0 → I
void cli(Machine *m, AddrMode addr_mode);

// CLV - Clear Overflow Flag
// Operation: 0 → V
void clv(Machine *m, AddrMode addr_mode);

// CMP - Compare Memory and Accumulator
// Operation: A - M
void cmp(Machine *m, AddrMode addr_mode);

// CPX - Compare Index Register X To Memory
// Operation: X - M
void cpx(Machine *m, AddrMode addr_mode);

// CPY - Compare Index Register Y To Memory
// Operation: Y - M
void cpy(Machine *m, AddrMode addr_mode);

// DEC - Decrement Memory By One
// Operation: M - 1 → M
void dec(Machine *m, AddrMode addr_mode);

// DEX - Decrement Index Register X By One
// Operation: X - 1 → X
void dex(Machine *m, AddrMode addr_mode);

// DEY - Decrement Index Register Y By One
// Operation: Y - 1 → Y
void dey(Machine *m, AddrMode addr_mode);

// EOR - "Exclusive OR" Memory with Accumulator
// Operation: A ⊻ M → A
void eor(Machine *m, AddrMode addr_mode);

// HLT - Halt the CPU
// Operation: Stop execution
void hlt(Machine *m, AddrMode addr_mode);

// INC - Increment Memory By One
// Operation: M + 1 → M
void inc(Machine *m, AddrMode addr_mode);

// INX - Increment Index Register X By One
// Operation: X + 1 → X
void inx(Machine *m, AddrMode addr_mode);

// INY - Increment Index Register Y By One
// Operation: Y + 1 → Y
void iny(Machine *m, AddrMode addr_mode);

// JMP - JMP Indirect
// Operation: [PC + 1] → PCL, [PC + 2] → PCH
void jmp(Machine *m, AddrMode addr_mode);

// JSR - Jump To Subroutine
// Operation: PC + 2↓, [PC + 1] → PCL, [PC + 2] → PCH
void jsr(Machine *m, AddrMode addr_mode);

// LDA - Load Accumulator with Memory
// Operation: M → A
void lda(Machine *m, AddrMode addr_mode);

// LDY - Load Index Register Y From Memory
// Operation: M → Y
void ldy(Machine *m, AddrMode addr_mode);

// Load Index Register X From Memory
// Operation: M → X
void ldx(Machine *m, AddrMode addr_mode);

// LSR - Logical Shift Right
// Operation: 0 → /M7...M0/ → C
void lsr(Machine *m, AddrMode addr_mode);

// NOP - No Operation
// Operation: No operation
void nop(Machine *m, AddrMode addr_mode);

// ORA - "OR" Memory with Accumulator
// Operation: A ∨ M → A
void ora(Machine *m, AddrMode addr_mode);

// PHA - Push Accumulator On Stack
// Operation: A↓
void pha(Machine *m, AddrMode addr_mode);

// PHP - Push Processor Status On Stack
// Operation: P↓
void php(Machine *m, AddrMode addr_mode);

// PLA - Pull Accumulator From Stack
// Operation: A↑
void pla(Machine *m, AddrMode addr_mode);

// PLP - Pull Processor Status From Stack
// Operation: P↑
void plp(Machine *m, AddrMode addr_mode);

// ROL - Rotate Left
// Operation: C ← /M7...M0/ ← C
void rol(Machine *m, AddrMode addr_mode);

// ROR - Rotate Right
// Operation: C → /M7...M0/ → C
void ror(Machine *m, AddrMode addr_mode);

// RTI - Return From Interrupt
// Operation: P↑ PC↑
void rti(Machine *m, AddrMode addr_mode);

// RTS - Return From Subroutine
// Operation: PC↑, PC + 1 → PC
void rts(Machine *m, AddrMode addr_mode);

// SBC - Subtract Memory from Accumulator with Borrow
// Operation: A - M - ~C → A
void sbc(Machine *m, AddrMode addr_mode);

// SEC - Set Carry Flag
// Operation: 1 → C
void sec(Machine *m, AddrMode addr_mode);

// SED - Set Decimal Mode
// Operation: 1 → D
void sed(Machine *m, AddrMode addr_mode);

// SEI - Set Interrupt Disable
// Operation: 1 → I
void sei(Machine *m, AddrMode addr_mode);

// SLO - Arithmetic Shift Left then "OR" Memory with Accumulator
// Operation: M * 2 → M, A ∨ M → A
void slo(Machine *m, AddrMode addr_mode);

// STA - Store Accumulator in Memory
// Operation: A → M
void sta(Machine *m, AddrMode addr_mode);

// STX - Store Index Register X In Memory
// Operation: X → M
void stx(Machine *m, AddrMode addr_mode);

// STY - Store Index Register Y In Memory
// Operation: Y → M
void sty(Machine *m, AddrMode addr_mode);

// TAX - Transfer Accumulator To Index X
// Operation: A → X
void tax(Machine *m, AddrMode addr_mode);

// TAY - Transfer Accumulator To Index Y
// Operation: A → Y
void tay(Machine *m, AddrMode addr_mode);

// TSX - Transfer Stack Pointer To Index X
// Operation: S → X
void tsx(Machine *m, AddrMode addr_mode);

// TXA - Transfer Index X To Accumulator
// Operation: X → A
void txa(Machine *m, AddrMode addr_mode);

// TXS - Transfer Index X To Stack Pointer
// Operation: X → S
void txs(Machine *m, AddrMode addr_mode);

// TYA - Transfer Index Y To Accumulator
// Operation: Y → A
void tya(Machine *m, AddrMode addr_mode);



