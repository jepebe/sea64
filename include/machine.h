#pragma once

#include "cpu.h"

typedef enum {
    READ_CYCLE,
    WRITE_CYCLE,
    IDLE_CYCLE
} CycleActivity;

typedef struct {
    u16 addr;
    u8 value;
    CycleActivity activity;
} CycleInfo;

typedef struct {
    CPU cpu;
    u8 ram[0x10000];
    bool halt;
    bool page_cross;
    u8 cycle_count;
    CycleInfo cycles[10];
} Machine;

Machine machine_create(void);

void machine_halt(Machine *machine);

void machine_irq(Machine *machine);

void machine_nmi(Machine *machine);

void machine_tick(Machine *machine);

u8 machine_read_immediate_byte(Machine *machine);

u16 machine_read_immediate_word(Machine *machine);

u8 machine_read_byte(Machine *machine, u16 addr);

void machine_write_byte(Machine *machine, u16 addr, u8 value);

u8 machine_read_byte_with_mode(Machine *machine, AddrMode mode);

void machine_write_byte_with_mode(Machine *machine, AddrMode mode, u8 value);

void machine_push_byte_on_stack(Machine *machine, u8 value);

u8 machine_pop_byte_from_stack(Machine *machine);

void machine_push_cpu_flags_on_stack(Machine *machine);

void machine_push_program_counter_on_stack(Machine *machine);

void machine_pop_program_counter_from_stack(Machine *machine);

u16 machine_fetch_address(Machine *machine, AddrMode mode);
