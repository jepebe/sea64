#pragma once

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include "seatypes.h"

NO_RETURN void error(char *message, ...);

void warning(char *message, ...);

void error_marker(char *file, int line);

typedef struct {
    const char *path;
    size_t size;
    char *data;
} Binary;

Binary read_binary(const char *path);
