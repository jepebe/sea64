#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "seaio.h"

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

void error_marker(char *file, int line) {
    printf("\x1b[0;33m");
    printf("Note: %s:%d", file, line);
    printf("\x1b[0m\n");
}

Binary read_binary(const char *path) {
    if (access(path, F_OK) != 0) {
        printf("File '%s' does not exist!\n", path);
        exit(1);
    }

    FILE *fd = fopen(path, "rb");

    if (!fd) {
        perror("fopen");
        exit(1);
    }

    Binary binary;
    binary.path = path;

    // get filesize
    fseek(fd, 0, SEEK_END);
    binary.size = ftell(fd);
    rewind(fd);

    if (binary.size > 0x10000) {
        printf("File limit exceeded!\n");
        exit(1);
    }

    binary.data = malloc(sizeof(char) * binary.size);
    size_t read = fread(binary.data, sizeof(char), binary.size, fd);

    if (read < binary.size) {
        if (ferror(fd)) {
            perror("fopen");
            exit(1);
        }
    }
    fclose(fd);

    return binary;
}
