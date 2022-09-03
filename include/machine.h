#pragma once

#include "cpu.h"

typedef struct {
    CPU cpu;
    u8 ram[0x10000];
} Machine;

Machine machine_create(void);

void machine_tick(Machine *machine);

u8 machine_read_immediate_byte(Machine *machine);

u16 machine_read_immediate_word(Machine *machine);

u8 machine_read_byte(Machine *machine, u16 addr);

void machine_write_byte(Machine *machine, u16 addr, u8 value);

u8 machine_read_byte_with_mode(Machine *machine, AddrMode mode);

u16 machine_read_word_with_mode(Machine *machine, AddrMode mode);

void machine_write_byte_with_mode(Machine *machine, AddrMode mode, u8 value);

void machine_push_byte_on_stack(Machine *machine, u8 value);

u8 machine_pop_byte_from_stack(Machine *machine);

void machine_push_program_counter_on_stack(Machine *machine);

void machine_pop_program_counter_from_stack(Machine *machine);

u16 machine_fetch_address(Machine *machine, AddrMode mode);
