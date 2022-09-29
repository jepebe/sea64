#pragma once

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include "seatypes.h"

NO_RETURN void error(char *message, ...);

void warning(char *message, ...);

typedef struct {
    const char *path;
    size_t size;
    char *data;
} Binary;

Binary read_binary(const char *path);
