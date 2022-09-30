#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "seaio.h"
#include "proc_tester.h"
#include "debug.h"

void read_processor_tests(ProcTester *proc_tester, char *path) {
    if (access(path, F_OK) != 0) {
        warning("File '%s' does not exist!\n", path);
        proc_tester->error = ProcessorTesterFileError;
        return;
    }

    FILE *fd = fopen(path, "rb");

    if (!fd) {
        warning("Could not open file '%s': %s\n", path, strerror(errno));
        proc_tester->error = ProcessorTesterFileError;
        return;
    }

    // get filesize
    fseek(fd, 0, SEEK_END);
    proc_tester->buffer_size = ftell(fd);
    rewind(fd);

    proc_tester->data_buffer = malloc(sizeof(char) * proc_tester->buffer_size);
    size_t read = fread(proc_tester->data_buffer, sizeof(char), proc_tester->buffer_size, fd);

    if (read < proc_tester->buffer_size) {
        if (ferror(fd)) {
            warning("Failed to read file '%s': %s\n", path, strerror(errno));
            proc_tester->error = ProcessorTesterFileError;
            return;
        }
    }

    if (fclose(fd) != 0) {
        warning("Failed to close file '%s': %s\n", path, strerror(errno));
    }

    proc_tester->cursor = 0;
    proc_tester->test_count = 0;

    proc_tester->error = ProcessorTesterOK;
}

static u8 read_u8(ProcTester *proc_tester) {
    return (u8) *(proc_tester->data_buffer + proc_tester->cursor++);
}

static u16 read_u16(ProcTester *proc_tester) {
    u8 high = (u8) *(proc_tester->data_buffer + proc_tester->cursor++);
    u8 low = (u8) *(proc_tester->data_buffer + proc_tester->cursor++);
    return (high << 8) | low;
}

static void read_state(ProcTester *proc_tester, ProcState *state) {
    state->pc = read_u16(proc_tester);

    state->s = read_u8(proc_tester);
    state->a = read_u8(proc_tester);
    state->x = read_u8(proc_tester);
    state->y = read_u8(proc_tester);
    state->p = read_u8(proc_tester);

    state->ram_size = read_u8(proc_tester);
    for (int i = 0; i < state->ram_size; ++i) {
        state->ram[i].addr = read_u16(proc_tester);
        state->ram[i].value = read_u8(proc_tester);
    }
}

static void read_cycles(ProcTester *proc_tester) {
    proc_tester->proc_test.cycle_count = read_u8(proc_tester);
    for (int i = 0; i < proc_tester->proc_test.cycle_count; ++i) {
        proc_tester->proc_test.cycles[i].addr = read_u16(proc_tester);
        proc_tester->proc_test.cycles[i].value = read_u8(proc_tester);
        char activity = (char) read_u8(proc_tester);
        switch (activity) {
            case 'r': {
                proc_tester->proc_test.cycles[i].activity = READ_CYCLE;
                break;
            }
            case 'w': {
                proc_tester->proc_test.cycles[i].activity = WRITE_CYCLE;
                break;
            }
            default:
                error("Unknown activity type for cycle: %c", activity);
        }
    }
}

bool read_next_processor_test(ProcTester *proc_tester) {
    if (proc_tester->cursor == proc_tester->buffer_size) {
        proc_tester->error = ProcessorTesterEOF;
        return false;
    }

    proc_tester->proc_test.name = (char *) (proc_tester->data_buffer + proc_tester->cursor);
    proc_tester->cursor += 10;

    read_state(proc_tester, &proc_tester->proc_test.initial);
    read_state(proc_tester, &proc_tester->proc_test.final);
    read_cycles(proc_tester);

    proc_tester->error = ProcessorTesterOK;
    proc_tester->test_count++;
    return true;
}

void clear_processor_tester(ProcTester *proc_tester) {
    proc_tester->cursor = 0;
    proc_tester->test_count = 0;

    if (proc_tester->buffer_size > 0) {
        free(proc_tester->data_buffer);
        proc_tester->data_buffer = NULL;
        proc_tester->buffer_size = 0;
    }

    if (proc_tester->proc_test.name != NULL) {
        proc_tester->proc_test.name = NULL;
    }
    proc_tester->proc_test.cycle_count = 0;
    proc_tester->proc_test.initial.ram_size = 0;
    proc_tester->proc_test.final.ram_size = 0;

    proc_tester->error = ProcessorTesterOK;
    proc_tester->error_loc = 1;
    proc_tester->error_line = 1;
}
