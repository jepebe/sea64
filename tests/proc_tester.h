#pragma once

#include "seatypes.h"

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

typedef enum {
    READ,
    WRITE
} CycleActivity;

typedef struct {
    RamData data;
    CycleActivity rw;
} Cycle;

typedef struct {
    char *name;
    ProcState initial;
    ProcState final;
    u8 cycle_count;
    Cycle cycles[10];

} ProcTest;

typedef enum {
    ProcessorTesterOK,
    ProcessorTesterFileError,
    ProcessorTesterExpectedTokenOpenBracket,
    ProcessorTesterExpectedTokenClosedBracket,
    ProcessorTesterExpectedTokenOpenBrace,
    ProcessorTesterExpectedTokenQuotationMark,
    ProcessorTesterExpectedTokenColon,
    ProcessorTesterExpectedTokenComma,
    ProcessorTesterExpectedTokenCommaOrClosedBrace,
    ProcessorTesterUnexpectedToken,
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

typedef struct {
    char* string;
    u64 size;
} StringView;

void read_processor_test(ProcTester *proc_tester, char *path);

void clear_processor_tester(ProcTester *proc_tester);

bool parse_next_processor_test(ProcTester *proc_tester);
