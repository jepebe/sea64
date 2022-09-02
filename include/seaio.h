#pragma once

#include <stdint.h>

typedef struct {
    const char *path;
    size_t size;
    char *data;
} Binary;

Binary read_binary(const char *path);
