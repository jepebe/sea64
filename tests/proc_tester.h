#pragma once

#include "seatypes.h"
#include "machine.h"

typedef struct {
    u16 addr;
    u8 value;
} RamData;

typedef struct {
    u16 pc;
    u8 s;
    u8 a;
    u8 x;
    u8 y;
    u8 p;
    RamData ram[10];
    u8 ram_size;
} ProcState;

typedef struct {
    char *name;
    ProcState initial;
    ProcState final;
    u8 cycle_count;
    CycleInfo cycles[10];
} ProcTest;

typedef enum {
    ProcessorTesterOK,
    ProcessorTesterFileError,
    ProcessorTesterEOF,
} Error;

typedef struct {
    ProcTest proc_test;
    u16 test_count;
    u64 cursor;
    u64 buffer_size;
    u8 *data_buffer;
    Error error;
    u64 error_loc;
    u64 error_line;
} ProcTester;

void read_processor_tests(ProcTester *proc_tester, char *path);

void clear_processor_tester(ProcTester *proc_tester);

bool read_next_processor_test(ProcTester *proc_tester);
