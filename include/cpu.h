#pragma once

#include "seatypes.h"

typedef enum {
    Absolute,
    AbsoluteIndirect,
    Accumulator,
    Immediate,
    Implied,
    Relative,
    XIndexedAbsolute,
    XIndexedAbsoluteIndirect,
    XIndexedZeroPage,
    XIndexedZeroPageIndirect,
    YIndexedAbsolute,
    YIndexedZeroPage,
    ZeroPage,
    ZeroPageIndirect,
    ZeroPageIndirectYIndexed,
    ZeroPageRelative,
} AddrMode;

typedef union {
    u8 status;
    struct {
        u8 C : 1; // Carry flag
        u8 Z : 1; // Zero flag
        u8 I : 1; // Interrupt Disable flag
        u8 D : 1; // Decimal flag
        u8 B : 1; // Break Command flag
        u8 U : 1; // Unused
        u8 V : 1; // Overflow flag
        u8 N : 1; // Negative flag
    };
} CPUFlags;

typedef enum {
    MOS6502 = 0,
    MOS6502EXT,
    WDC65C02,
} CPUType;

typedef struct {
    u8 A;       // Accumulator
    u8 X;       // X Index Register
    u8 Y;       // Y Index Register
    u8 S;       // Stack Pointer
    CPUFlags P; // Processor Status
    u16 PC;     // Program Counter

    u64 tick_count; // number of opcodes processed
    CPUType cpu_type;
} CPU;

void cpu_reset(CPU *cpu);
