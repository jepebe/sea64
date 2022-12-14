#include "seatest.h"

Tester create_tester(char *name) {
    Tester tester;
    tester.name = name;
    timespec_get(&tester.time, TIME_UTC);
    tester.count = 0;
    tester.success = 0;
    tester.fail = 0;

    printf("\x1b[33m--== %s ==--\x1b[0m\n", name);
    return tester;
}

double timespec_diff(struct timespec *a) {
    struct timespec result;
    struct timespec b;
    timespec_get(&b, TIME_UTC);
    result.tv_sec = b.tv_sec - a->tv_sec;
    result.tv_nsec = b.tv_nsec - a->tv_nsec;
    if (result.tv_nsec < 0) {
        --result.tv_sec;
        result.tv_nsec += 1000000000L;
    }
    double t = result.tv_sec * 1000000000.0;
    t += result.tv_nsec;
    t /= 1000000000.0;

    return t;
}

void test_fail(Tester *tester, const char *message) {
    u16 count = ++tester->count;
    tester->fail++;
    printf("%s  Test #%02d Error! %s", CROSS, count, message);
    printf("%s\n", END_ANSI);
}

void test_success(Tester *tester, const char *message) {
    u16 count = ++tester->count;

    tester->success++;
    if (message) {
        printf("%s  Test #%02d OK! %s", CHECK, count, message);
    }
    printf("%s\n", END_ANSI);
}

void test_false(Tester *tester, bool test_state, const char *message) {
    u16 count = ++tester->count;

    if (!test_state) {
        tester->success++;
        if (message) {
            printf("%s  Test #%02d OK! %s", CHECK, count, message);
        }
    } else {
        tester->fail++;

        printf("%s  Test #%02d Error! Expected '%s' to be false", CROSS, count, message);
    }
    printf("%s\n", END_ANSI);
}

void test_true(Tester *tester, bool test_state, const char *message) {
    u16 count = ++tester->count;

    if (test_state) {
        tester->success++;
        if (message) {
            printf("%s  Test #%02d OK! %s", CHECK, count, message);
        }
    } else {
        tester->fail++;

        printf("%s  Test #%02d Error! Expected '%s' to be true", CROSS, count, message);
    }
    printf("%s\n", END_ANSI);
}

void test_section(const char *name) {
    printf("\n\x1b[33m[%s]\x1b[0m\n", name);
}

int test_summary(Tester *tester) {
    printf("\n");
    if (tester->fail > 0) {
        u16 fail_count = tester->fail;
        u16 count = tester->count;
        printf("\x1b[0;31mError! %d of %d test(s) failed!\x1b[0m\n", fail_count, count);
    } else {
        u16 success = tester->success;
        printf("\x1b[0;32mSuccess! %d test(s) ran successfully!\x1b[0m\n", success);
    }
    double diff = timespec_diff(&tester->time);
    printf("\x1b[33mRunning time: %0.5f s.\x1b[0m\n", diff);

    return tester->fail;
}
