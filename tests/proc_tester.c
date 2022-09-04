#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "proc_tester.h"
#include "debug.h"

void read_processor_test(ProcTester *proc_tester, char *path) {
    if (access(path, F_OK) != 0) {
        printf("File '%s' does not exist!\n", path);
        proc_tester->error = ProcessorTesterFileError;
        return;
    }

    FILE *fd = fopen(path, "rb");

    if (!fd) {
        perror("fopen");
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
            perror("fopen");
            proc_tester->error = ProcessorTesterFileError;
            return;
        }
    }
    fclose(fd);

    proc_tester->cursor = 0;
    proc_tester->test_count = 0;

    proc_tester->error = ProcessorTesterOK;
}

static bool is_whitespace(ProcTester *proc_tester) {
    switch (proc_tester->data_buffer[proc_tester->cursor]) {
        case '\f':
        case '\n':
        case '\r':
        case '\t':
        case '\v':
        case ' ':
            return true;
        default:
            return false;
    }
}

static void advance_cursor(ProcTester *proc_tester) {
    if (proc_tester->data_buffer[proc_tester->cursor] == '\n') {
        proc_tester->error_line++;
        proc_tester->error_loc = 0;
    }
    proc_tester->error_loc++;
    proc_tester->cursor++;
}

static char *create_string(StringView *sv) {
    char *dst = (char *) malloc(sizeof(char) * sv->size);
    strncpy(dst, sv->string, sv->size);
    dst[sv->size] = '\0';
    return dst;
}

static bool string_view_is_equal(StringView *lhs, char *rhs) {
    size_t rhs_size = strlen(rhs);
    if (lhs->size != rhs_size) {
        return false;
    }
    return strncmp(rhs, lhs->string, lhs->size) == 0;
}

static bool is_open_bracket(ProcTester *proc_tester) {
    return proc_tester->data_buffer[proc_tester->cursor] == '[';
}

static bool is_closed_bracket(ProcTester *proc_tester) {
    return proc_tester->data_buffer[proc_tester->cursor] == ']';
}

static bool is_open_brace(ProcTester *proc_tester) {
    return proc_tester->data_buffer[proc_tester->cursor] == '{';
}

static bool is_closed_brace(ProcTester *proc_tester) {
    return proc_tester->data_buffer[proc_tester->cursor] == '}';
}

static bool is_quotation_mark(ProcTester *proc_tester) {
    return proc_tester->data_buffer[proc_tester->cursor] == '"';
}

static bool is_comma(ProcTester *proc_tester) {
    return proc_tester->data_buffer[proc_tester->cursor] == ',';
}

static bool is_digit(ProcTester *proc_tester) {
    u8 c = proc_tester->data_buffer[proc_tester->cursor];
    return '0' <= c && c <= '9';
}

static void skip_whitespace(ProcTester *proc_tester) {
    while (is_whitespace(proc_tester)) {
        advance_cursor(proc_tester);
    }
}

static void skip_token(ProcTester *proc_tester) {
    advance_cursor(proc_tester);
    skip_whitespace(proc_tester);
}

static bool skip_colon(ProcTester *proc_tester) {
    skip_whitespace(proc_tester);
    if (proc_tester->data_buffer[proc_tester->cursor] != ':') {
        proc_tester->error = ProcessorTesterExpectedTokenColon;
        return false;
    }
    advance_cursor(proc_tester);
    skip_whitespace(proc_tester);
    return true;
}

static StringView parse_string(ProcTester *proc_tester) {
    StringView sv = {0};
    if (!is_quotation_mark(proc_tester)) {
        proc_tester->error = ProcessorTesterExpectedTokenQuotationMark;
        return sv;
    }
    advance_cursor(proc_tester);
    u64 start = proc_tester->cursor;
    sv.string = (char *) &proc_tester->data_buffer[start];
    while (!is_quotation_mark(proc_tester)) {
        advance_cursor(proc_tester);
    }
    sv.size = proc_tester->cursor - start;
    advance_cursor(proc_tester);

    return sv;
}

static StringView parse_property_name(ProcTester *proc_tester) {
    StringView property_name = parse_string(proc_tester);
    if (proc_tester->error == ProcessorTesterOK) {
        skip_colon(proc_tester);
    }

    return property_name;
}

static u16 parse_u16(ProcTester *proc_tester) {
    u16 result = 0;

    while (is_digit(proc_tester)) {
        result *= 10;
        result += proc_tester->data_buffer[proc_tester->cursor] - '0';
        advance_cursor(proc_tester);
    }
    skip_whitespace(proc_tester);
    return result;
}

static bool parse_ram_data(ProcTester *proc_tester, ProcState *state) {
    if (!is_open_bracket(proc_tester)) {
        proc_tester->error = ProcessorTesterExpectedTokenOpenBracket;
        return false;
    }

    skip_token(proc_tester);

    state->ram_size = 0;

    while (!is_closed_bracket(proc_tester)) {
        if (!is_open_bracket(proc_tester)) {
            proc_tester->error = ProcessorTesterExpectedTokenOpenBracket;
            return false;
        }
        skip_token(proc_tester);
        state->ram[state->ram_size].addr = parse_u16(proc_tester);

        if (!is_comma(proc_tester)) {
            proc_tester->error = ProcessorTesterExpectedTokenComma;
            return false;
        }
        skip_token(proc_tester);

        state->ram[state->ram_size].value = (u8) parse_u16(proc_tester);

        if (!is_closed_bracket(proc_tester)) {
            proc_tester->error = ProcessorTesterExpectedTokenClosedBracket;
            return false;
        }
        skip_token(proc_tester);
        state->ram_size++;

        if (is_comma(proc_tester)) {
            skip_token(proc_tester);
        }
    }

    if (!is_closed_bracket(proc_tester)) {
        proc_tester->error = ProcessorTesterExpectedTokenClosedBracket;
        return false;
    }
    skip_token(proc_tester);

    return true;
}

static bool parse_cycles(ProcTester *proc_tester) {
    if (!is_open_bracket(proc_tester)) {
        proc_tester->error = ProcessorTesterExpectedTokenOpenBracket;
        return false;
    }

    skip_token(proc_tester);

    ProcTest *proc_test = &proc_tester->proc_test;
    proc_test->cycle_count = 0;

    while (!is_closed_bracket(proc_tester)) {
        if (!is_open_bracket(proc_tester)) {
            proc_tester->error = ProcessorTesterExpectedTokenOpenBracket;
            return false;
        }
        skip_token(proc_tester);
        proc_test->cycles[proc_test->cycle_count].data.addr = parse_u16(proc_tester);

        if (!is_comma(proc_tester)) {
            proc_tester->error = ProcessorTesterExpectedTokenComma;
            return false;
        }
        skip_token(proc_tester);
        proc_test->cycles[proc_test->cycle_count].data.value = (u8) parse_u16(proc_tester);

        if (!is_comma(proc_tester)) {
            proc_tester->error = ProcessorTesterExpectedTokenComma;
            return false;
        }

        skip_token(proc_tester);
        StringView activity = parse_string(proc_tester);

        if (proc_tester->error != ProcessorTesterOK) {
            return false;
        }

        if(string_view_is_equal(&activity, "read")) {
            proc_test->cycles[proc_test->cycle_count].rw = READ;
        } else if(string_view_is_equal(&activity, "write")) {
            proc_test->cycles[proc_test->cycle_count].rw = WRITE;
        } else {
            proc_tester->error = ProcessorTesterUnexpectedToken;
            return false;
        }

        if (!is_closed_bracket(proc_tester)) {
            proc_tester->error = ProcessorTesterExpectedTokenClosedBracket;
            return false;
        }
        skip_token(proc_tester);
        proc_test->cycle_count++;

        if (is_comma(proc_tester)) {
            skip_token(proc_tester);
        }
    }

    if (!is_closed_bracket(proc_tester)) {
        proc_tester->error = ProcessorTesterExpectedTokenClosedBracket;
        return false;
    }
    skip_token(proc_tester);

    return true;
}

static bool parse_processor_state(ProcTester *proc_tester, ProcState *state) {
    if (!is_open_brace(proc_tester)) {
        proc_tester->error = ProcessorTesterExpectedTokenOpenBrace;
        return false;
    }
    skip_token(proc_tester);

    bool in_object = true;
    while (in_object) {
        StringView property_name = parse_property_name(proc_tester);

        if (proc_tester->error != ProcessorTesterOK) {
            return false;
        }

        if (string_view_is_equal(&property_name, "pc")) {
            state->pc = parse_u16(proc_tester);
        } else if (string_view_is_equal(&property_name, "s")) {
            state->s = (u8) parse_u16(proc_tester);
        } else if (string_view_is_equal(&property_name, "a")) {
            state->a = (u8) parse_u16(proc_tester);
        } else if (string_view_is_equal(&property_name, "x")) {
            state->x = (u8) parse_u16(proc_tester);
        } else if (string_view_is_equal(&property_name, "y")) {
            state->y = (u8) parse_u16(proc_tester);
        } else if (string_view_is_equal(&property_name, "p")) {
            state->p = (u8) parse_u16(proc_tester);
        } else if (string_view_is_equal(&property_name, "ram")) {
            if (!parse_ram_data(proc_tester, state)) {
                return false;
            }
        } else {
            warning("Unknown property name: %s", create_string(&property_name));
            proc_tester->error = ProcessorTesterUnexpectedToken;
            return false;
        }

        skip_whitespace(proc_tester);

        if (is_comma(proc_tester)) {
            skip_token(proc_tester);
        } else if (is_closed_brace(proc_tester)) {
            in_object = false;
            skip_token(proc_tester);
        } else {
            proc_tester->error = ProcessorTesterExpectedTokenCommaOrClosedBrace;
            return false;
        }
    }
    return true;
}


static bool parse_processor_test(ProcTester *proc_tester) {
    if (!is_open_brace(proc_tester)) {
        proc_tester->error = ProcessorTesterExpectedTokenOpenBrace;
        return false;
    }
    skip_token(proc_tester);

    bool in_object = true;

    while (in_object) {
        StringView property_name = parse_property_name(proc_tester);

        if (proc_tester->error != ProcessorTesterOK) {
            return false;
        }

        if (string_view_is_equal(&property_name, "name")) {
            StringView name = parse_string(proc_tester);
            if (name.string == NULL) {
                return false;
            }
            proc_tester->proc_test.name = create_string(&name);
        } else if (string_view_is_equal(&property_name, "initial")) {
            ProcState *initial = &proc_tester->proc_test.initial;
            if (!parse_processor_state(proc_tester, initial)) {
                return false;
            }
        } else if (string_view_is_equal(&property_name, "final")) {
            ProcState *final = &proc_tester->proc_test.final;
            if (!parse_processor_state(proc_tester, final)) {
                return false;
            }
        } else if (string_view_is_equal(&property_name, "cycles")) {
            if (!parse_cycles(proc_tester)) {
                return false;
            }
        } else {
            warning("Unknown property name: %s", create_string(&property_name));
            proc_tester->error = ProcessorTesterUnexpectedToken;
            return false;
        }

        skip_whitespace(proc_tester);

        if (is_comma(proc_tester)) {
            skip_token(proc_tester);
        } else if (is_closed_brace(proc_tester)) {
            in_object = false;
            skip_token(proc_tester);
        } else {
            proc_tester->error = ProcessorTesterExpectedTokenCommaOrClosedBrace;
            return false;
        }
    }

    return true;
}

bool parse_next_processor_test(ProcTester *proc_tester) {
    if (proc_tester->cursor == 0) {
        if (!is_open_bracket(proc_tester)) {
            proc_tester->error = ProcessorTesterExpectedTokenOpenBracket;
            return false;
        }
        skip_token(proc_tester);
    }
    if (parse_processor_test(proc_tester)) {
        proc_tester->error = ProcessorTesterOK;
        proc_tester->test_count++;

        if (is_comma(proc_tester)) {
            skip_token(proc_tester);
        }

        return true;
    }

    if (is_closed_bracket(proc_tester)) {
        skip_token(proc_tester);
        proc_tester->error = ProcessorTesterEOF;
        return false;
    }

    return false;
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
        free(proc_tester->proc_test.name);
        proc_tester->proc_test.name = NULL;
    }
    proc_tester->proc_test.cycle_count = 0;
    proc_tester->proc_test.initial.ram_size = 0;
    proc_tester->proc_test.final.ram_size = 0;

    proc_tester->error = ProcessorTesterOK;
    proc_tester->error_loc = 1;
    proc_tester->error_line = 1;
}
