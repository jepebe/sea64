#pragma once

#include "machine.h"

// ANC - "AND" Memory with Accumulator then Move Negative Flag to Carry Flag
// Operation: A ∧ M → A, N → C
void anc(Machine *m, AddrMode addr_mode);

// ARR - "AND" Accumulator then Rotate Right
// Operation: (A ∧ M) / 2 → A
void arr(Machine *m, AddrMode addr_mode);

// ASR - "AND" then Logical Shift Right
// Operation: (A ∧ M) / 2 → A
void asr(Machine *m, AddrMode addr_mode);

// DCP - Decrement Memory By One then Compare with Accumulator
// Operation: M - 1 → M, A - M
void dcp(Machine *m, AddrMode addr_mode);

// ISC - Increment Memory By One then Subtract Memory from Accumulator with Borrow
// Operation: M + 1 → M, A - M → A
void isc(Machine *m, AddrMode addr_mode);

// LAS - "AND" Memory with Stack Pointer
// Operation: M ∧ S → A, X, S
void las(Machine *m, AddrMode addr_mode);

// LAX - Load Accumulator and Index Register X From Memory
// Operation: M → A, X
void lax(Machine *m, AddrMode addr_mode);

// LAX - Load Accumulator and Index Register X From Memory
// Store * AND oper in A and X
// Operation: M → M ∧ A, M → M ∧ X
void lxa(Machine *m, AddrMode addr_mode);

// NOP - No Operation
// Operation: No operation
void nop_ext(Machine *m, AddrMode addr_mode);

// RLA - Rotate Left then "AND" with Accumulator
// Operation: C ← /M7...M0/ ← C, A ∧ M → A
void rla(Machine *m, AddrMode addr_mode);

// RRA - Rotate Right and Add Memory to Accumulator
// Operation: C → /M7...M0/ → C, A + M + C → A
void rra(Machine *m, AddrMode addr_mode);

// SAX - Store Accumulator "AND" Index Register X in Memory
// Operation: A ∧ X → M
void sax(Machine *m, AddrMode addr_mode);

// SBX - Subtract Memory from Accumulator "AND" Index Register X
// Operation: (A ∧ X) - M → X
void sbx(Machine *m, AddrMode addr_mode);

// SHA - Store Accumulator "AND" Index Register X "AND" Value
// Operation: A ∧ X ∧ V → M
void sha(Machine *m, AddrMode addr_mode);

// SHS - Transfer Accumulator "AND" Index Register X to Stack Pointer then
// Store Stack Pointer "AND" Hi-Byte In Memory
// Operation: A ∧ X → S, S ∧ (H + 1) → M
void shs(Machine *m, AddrMode addr_mode);

// SHX - Store Index Register X "AND" Value
// Operation: X ∧ (H + 1) → M
void shx(Machine *m, AddrMode addr_mode);

// SHY - Store Index Register Y "AND" Value
// Operation: Y ∧ (H + 1) → M
void shy(Machine *m, AddrMode addr_mode);

// SRE - Logical Shift Right then "Exclusive OR" Memory with Accumulator
// Operation: M / 2 → M, A ⊻ M → A
void sre(Machine *m, AddrMode addr_mode);

// XAA - Non-deterministic Operation of Accumulator, Index Register X,
// Memory and Bus Contents
// Operation: (A ∨ V) ∧ X ∧ M → A
void xxa(Machine *m, AddrMode addr_mode);
