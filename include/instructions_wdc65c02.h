#pragma once

#include "machine.h"

// ADC - Add Memory to Accumulator with Carry
// Operation: A + M + C → A, C
void adc_wdc(Machine *m, AddrMode addr_mode);

// ASL - Arithmetic Shift Left
// Operation: C ← /M7...M0/ ← 0
void asla(Machine *m, AddrMode addr_mode);
void asl_wdc(Machine *m, AddrMode addr_mode);

// BRA - Branch Always
// Operation: Branch Always
void bra(Machine *m, AddrMode addr_mode);

// BBR0 - Branch on Bit 0 Reset
// Operation: Branch on M0 = 0
void bbr0(Machine *m, AddrMode addr_mode);

// BBR1 - Branch on Bit 1 Reset
// Operation: Branch on M1 = 0
void bbr1(Machine *m, AddrMode addr_mode);

// BBR2 - Branch on Bit 2 Reset
// Operation: Branch on M2 = 0
void bbr2(Machine *m, AddrMode addr_mode);

// BBR3 - Branch on Bit 3 Reset
// Operation: Branch on M3 = 0
void bbr3(Machine *m, AddrMode addr_mode);

// BBR4 - Branch on Bit 4 Reset
// Operation: Branch on M4 = 0
void bbr4(Machine *m, AddrMode addr_mode);

// BBR5 - Branch on Bit 5 Reset
// Operation: Branch on M5 = 0
void bbr5(Machine *m, AddrMode addr_mode);

// BBR6 - Branch on Bit 6 Reset
// Operation: Branch on M6 = 0
void bbr6(Machine *m, AddrMode addr_mode);

// BBR7 - Branch on Bit 7 Reset
// Operation: Branch on M7 = 0
void bbr7(Machine *m, AddrMode addr_mode);

// BBS0 - Branch on Bit 0 Set
// Operation: Branch on M0 = 1
void bbs0(Machine *m, AddrMode addr_mode);

// BBS1 - Branch on Bit 1 Set
// Operation: Branch on M1 = 1
void bbs1(Machine *m, AddrMode addr_mode);

// BBS2 - Branch on Bit 2 Set
// Operation: Branch on M2 = 1
void bbs2(Machine *m, AddrMode addr_mode);

// BBS3 - Branch on Bit 3 Set
// Operation: Branch on M3 = 1
void bbs3(Machine *m, AddrMode addr_mode);

// BBS4 - Branch on Bit 4 Set
// Operation: Branch on M4 = 1
void bbs4(Machine *m, AddrMode addr_mode);

// BBS5 - Branch on Bit 5 Set
// Operation: Branch on M5 = 1
void bbs5(Machine *m, AddrMode addr_mode);

// BBS6 - Branch on Bit 6 Set
// Operation: Branch on M6 = 1
void bbs6(Machine *m, AddrMode addr_mode);

// BBS7 - Branch on Bit 7 Set
// Operation: Branch on M7 = 1
void bbs7(Machine *m, AddrMode addr_mode);

// BRK - Break Command
// Operation: PC + 2↓, [FFFE] → PCL, [FFFF] → PCH
void brk_wdc(Machine *m, AddrMode addr_mode);

// DEC - Decrement Memory By One
// Operation: M - 1 → M
void deca(Machine *m, AddrMode addr_mode);
void dec_wdc(Machine *m, AddrMode addr_mode);

// INC - Increment Memory By One
// Operation: M + 1 → M
void inca(Machine *m, AddrMode addr_mode);
void inc_wdc(Machine *m, AddrMode addr_mode);

// JMP - JMP Indirect
// Operation: [PC + 1] → PCL, [PC + 2] → PCH
void jmp_wdc(Machine *m, AddrMode addr_mode);

// LSR - Logical Shift Right
// Operation: 0 → /M7...M0/ → C
void lsra(Machine *m, AddrMode addr_mode);
void lsr_wdc(Machine *m, AddrMode addr_mode);

// NOP - No Operation
// Operation: No operation
void nop_wdc(Machine *m, AddrMode addr_mode);

// PHX - Push Index Register X On Stack
// Operation: X↓
void phx(Machine *m, AddrMode addr_mode);

// PHY - Push Index Register Y On Stack
// Operation: Y↓
void phy(Machine *m, AddrMode addr_mode);

// PLX - Pull Index Register X From Stack
// Operation: X↑
void plx(Machine *m, AddrMode addr_mode);

// PLY - Pull Index Register Y From Stack
// Operation: Y↑
void ply(Machine *m, AddrMode addr_mode);

// RMB0 - Reset Memory Bit 0
// Operation: 0 → M0
void rmb0(Machine *m, AddrMode addr_mode);

// RMB1 - Reset Memory Bit 1
// Operation: 0 → M1
void rmb1(Machine *m, AddrMode addr_mode);

// RMB2 - Reset Memory Bit 2
// Operation: 0 → M2
void rmb2(Machine *m, AddrMode addr_mode);

// RMB3 - Reset Memory Bit 3
// Operation: 0 → M3
void rmb3(Machine *m, AddrMode addr_mode);

// RMB4 - Reset Memory Bit 4
// Operation: 0 → M4
void rmb4(Machine *m, AddrMode addr_mode);

// RMB5 - Reset Memory Bit 5
// Operation: 0 → M5
void rmb5(Machine *m, AddrMode addr_mode);

// RMB6 - Reset Memory Bit 6
// Operation: 0 → M6
void rmb6(Machine *m, AddrMode addr_mode);

// RMB7 - Reset Memory Bit 7
// Operation: 0 → M7
void rmb7(Machine *m, AddrMode addr_mode);

// ROL - Rotate Left
// Operation: C ← /M7...M0/ ← C
void rola(Machine *m, AddrMode addr_mode);
void rol_wdc(Machine *m, AddrMode addr_mode);

// ROR - Rotate Right
// Operation: C → /M7...M0/ → C
void rora(Machine *m, AddrMode addr_mode);
void ror_wdc(Machine *m, AddrMode addr_mode);

// SBC - Subtract Memory from Accumulator with Borrow
// Operation: A - M - ~C → A
void sbc_wdc(Machine *m, AddrMode addr_mode);

// SMB0 - Set Memory Bit 0
// Operation: 1 → M0
void smb0(Machine *m, AddrMode addr_mode);

// SMB1 - Set Memory Bit 1
// Operation: 1 → M1
void smb1(Machine *m, AddrMode addr_mode);

// SMB2 - Set Memory Bit 2
// Operation: 1 → M2
void smb2(Machine *m, AddrMode addr_mode);

// SMB3 - Set Memory Bit 3
// Operation: 1 → M3
void smb3(Machine *m, AddrMode addr_mode);

// SMB4 - Set Memory Bit 4
// Operation: 1 → M4
void smb4(Machine *m, AddrMode addr_mode);

// SMB5 - Set Memory Bit 5
// Operation: 1 → M5
void smb5(Machine *m, AddrMode addr_mode);

// SMB6 - Set Memory Bit 6
// Operation: 1 → M6
void smb6(Machine *m, AddrMode addr_mode);

// SMB7 - Set Memory Bit 7
// Operation: 1 → M7
void smb7(Machine *m, AddrMode addr_mode);

// STA - Store Accumulator in Memory
// Operation: A → M
void sta_wdc(Machine *m, AddrMode addr_mode);

// STP - Stop the Clock
// Operation: Stop the Clock
void stp(Machine *m, AddrMode addr_mode);

// STZ - Store Zero In Memory
// Operation: 0 → M
void stz(Machine *m, AddrMode addr_mode);

// TRB - Test And Reset Memory Bits With Accumulator
// Operation: ~A ∧ M → M
void trb(Machine *m, AddrMode addr_mode);

// TSB - Test And Set Memory Bits With Accumulator
// Operation: A ∨ M → M
// Flags: (A ∧ M) == 0 → Z
void tsb(Machine *m, AddrMode addr_mode);

// WAI - Wait for Interrupt
// Operation: Wait for Interrupt
void wai(Machine *m, AddrMode addr_mode);
