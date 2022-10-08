#pragma once

#include "seatypes.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

NO_RETURN void error(char *message, ...);

void warning(char *message, ...);

void error_marker(char *file, int line);

#define ERROR(message, ...) error_marker(__FILE__, __LINE__); error((message), __VA_ARGS__);

typedef struct {
    const char *path;
    size_t size;
    char *data;
} Binary;

Binary read_binary(const char *path);
