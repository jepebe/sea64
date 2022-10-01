cmake_minimum_required(VERSION 3.23)
project(sea64 C)

set(CMAKE_C_STANDARD 23)

add_compile_options(-Wall -Wextra -pedantic -Werror)

include_directories(include/)

set(SOURCES
        src/seaio.c
        src/cpu.c
        src/machine.c
        src/opcodes.c
        src/opcodes_6502.c
        src/opcodes_6502_ext.c
        src/instructions.c
        src/debug.c
        src/disassembly.c)


add_library(sea64 STATIC ${SOURCES})

set(TEST_SOURCES
        tests/seatest.c
        tests/proc_tester.c)

add_library(sea64test STATIC ${TEST_SOURCES})

add_executable(sea64bin src/main.c)
target_link_libraries(sea64bin sea64)
set_target_properties(sea64bin PROPERTIES OUTPUT_NAME sea64)

add_executable(hex2bin tools/hex2bin.c)
target_link_libraries(hex2bin sea64)

add_executable(optrix tools/optrix.c)
target_link_libraries(optrix sea64)

enable_testing()

add_executable(test_sample tests/test_sample.c)
target_link_libraries(test_sample sea64 sea64test)
add_test("Sample Testcase" test_sample)

add_executable(test_cpu_structs tests/test_cpu_structs.c)
target_link_libraries(test_cpu_structs sea64 sea64test)
add_test("CPU Structs" test_cpu_structs)

add_executable(test_functional_tests tests/test_functional_tests.c)
target_link_libraries(test_functional_tests sea64 sea64test)
add_test("Klaus's Functional Tests" test_functional_tests)

add_executable(test_interrupt_tests tests/test_interrupt_tests.c)
target_link_libraries(test_interrupt_tests sea64 sea64test)
add_test("Klaus's Interrupt Tests" test_interrupt_tests)

add_executable(test_processor_tests_6502 tests/test_processor_tests_6502.c)
target_link_libraries(test_processor_tests_6502 sea64 sea64test)
add_test("Processor Tests 6502" test_processor_tests_6502)

add_executable(test_processor_tests_6502_ext tests/test_processor_tests_6502_ext.c)
target_link_libraries(test_processor_tests_6502_ext sea64 sea64test)
add_test("Processor Tests 6502 Extended" test_processor_tests_6502_ext)
