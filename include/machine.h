#pragma once

#include "cpu.h"

typedef struct {
    CPU cpu;
    u8 ram[0x10000];
} Machine;

Machine machine_create(void);

void machine_tick(Machine *machine);

u16 machine_read_immediate_word(Machine *machine);

u8 machine_read_byte(Machine *machine, u16 addr);

void machine_write_byte(Machine *machine, u16 addr, u8 value);

u8 machine_read_byte_with_mode(Machine *machine, AddrMode mode);

void machine_write_byte_with_mode(Machine *machine, AddrMode mode, u8 value);

