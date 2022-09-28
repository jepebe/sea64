#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include "seaio.h"
#include "seatypes.h"

NO_RETURN void error(char *message, ...) {
    va_list argp;
    printf("\x1b[0;31m");

    printf("Error: ");
    va_start(argp, message);
    vprintf(message, argp);
    va_end(argp);
    printf("\x1b[0m\n");

    exit(1);
}

void warning(char *message, ...) {
    va_list argp;
    printf("\x1b[0;33m");

    printf("Warning: ");
    va_start(argp, message);
    vprintf(message, argp);
    va_end(argp);
    printf("\x1b[0m\n");
}


bool process_record(FILE *hex, FILE *bin, size_t line);

void check_record_marker(FILE *hex, size_t line);

u8 parse_byte(FILE *hex, size_t line);

u16 parse_load_address(FILE *p_file, size_t line);

char *basename(char *path, char *default_basename) {
    char *name = strrchr(path, '/');

    // check for at least one char in string
    if (!name && !(name + 1)) {
        return default_basename;
    }
    return name + 1;
}

void usage(char *program) {
    char *prog_name = basename(program, "basename");
    printf("Usage: %s <intel_hex_file> <output_bin_file>\n", prog_name);
}

int main(int argc, char *argv[argc]) {
    if (argc < 3) {
        usage(argv[0]);
        exit(1);
    }

    char *hex_path = argv[1];
    char *bin_path = argv[2];

    if (access(hex_path, F_OK) != 0) {
        error("File '%s' does not exist: %s\n", hex_path, strerror(errno));
    }

    FILE *hex_file = fopen(hex_path, "rb");

    if (!hex_file) {
        error("Could not open file '%s': %s\n", hex_path, strerror(errno));
    }

    FILE *bin_file = fopen(bin_path, "wb");

    if (!bin_file) {
        error("Could not open file '%s': %s\n", bin_path, strerror(errno));
    }

    size_t line = 0;
    while (process_record(hex_file, bin_file, line)) {
        line++;
    }

    if (fclose(hex_file) != 0) {
        warning("Failed to close file '%s': %s\n", hex_path, strerror(errno));
    }

    if (fclose(bin_file) != 0) {
        warning("Failed to close file '%s': %s\n", bin_path, strerror(errno));
    }

}

void check_record_marker(FILE *hex, size_t line) {
    char marker;
    do {
        fread(&marker, sizeof(char), 1, hex);
    } while (marker == '\t' || marker == '\n' || marker == '\r');

    if (marker != ':') {
        error("Expected ':' but found '%c' at line %llu", marker, line);
    }
}

static u8 hex2u8(char ch, size_t line) {
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    error("Expected hexadecimal character value but found '%c' at line %zu", ch, line);
}


u8 parse_byte(FILE *hex, size_t line) {
    char record_length[2];
    fread(&record_length, sizeof(char), 2, hex);

    u8 value = hex2u8(record_length[0], line) << 4;
    value |= hex2u8(record_length[1], line);
    return value;
}

u16 parse_load_address(FILE *hex, size_t line) {
    char record_length[4];
    fread(&record_length, sizeof(char), 4, hex);

    u16 addr = hex2u8(record_length[0], line) << 12;
    addr |= hex2u8(record_length[1], line) << 8;
    addr |= hex2u8(record_length[2], line) << 4;
    addr |= hex2u8(record_length[3], line);
    return addr;
}

bool process_record(FILE *hex, FILE *bin, size_t line) {
    check_record_marker(hex, line);
    u8 record_length = parse_byte(hex, line);
    u16 load_address = parse_load_address(hex, line);
    u8 record_type = parse_byte(hex, line);

    if (record_type != 0 && record_type != 1) {
        error("Unhandled record type '%i' at line %zu", record_type, line);
    }

    if (fseek(bin, load_address, SEEK_SET) != 0) {
        error("fseek() to $%04x failed in file %s", load_address);
    }

    u8 sum = record_length + (load_address >> 8) + (load_address & 0xFF) + record_type;
    for (int i = 0; i < record_length; ++i) {
        u8 value = parse_byte(hex, line);

        fwrite(&value, sizeof(u8), 1, bin);
        sum += value;
    }

    sum = (~sum) + 1;  // twos complement
    u8 checksum = parse_byte(hex, line);

    if (checksum != sum) {
        error("Checksum failed at line %zu, %02X != %02X", record_type, line, checksum, sum);
    }

//    printf("Record length 0x%02X\n", record_length);
//    printf("Load address $%04X\n", load_address);
//    printf("Record type 0x%02X\n", record_type);
//    printf("Checksum 0x%02X == 0x%02X\n", checksum, sum);

    return record_type != 1; // end of file record
}
