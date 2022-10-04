#pragma once

#include "seatypes.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#define CHECK "\x1b[0;32m\u2705"
#define CROSS "\x1b[0;31m\u274C"
#define END_ANSI "\x1b[0m"

typedef struct {
    char *name;
    struct timespec time;
    u16 count;
    u16 success;
    u16 fail;
} Tester;

Tester create_tester(char *name);

double timespec_diff(struct timespec *a);

void test_fail(Tester *tester, const char *message);

void test_success(Tester *tester, const char *message);

void test_false(Tester *tester, bool test_state, const char *message);

void test_true(Tester *tester, bool test_state, const char *message);

void test_section(const char *name);

int test_summary(Tester *tester);
