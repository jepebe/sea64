cmake_minimum_required(VERSION 3.23)
project(sea64 C)

set(CMAKE_C_STANDARD 23)

add_compile_options(-Wall -Wextra -pedantic -Werror)

include_directories(include/)

set(SOURCES
        src/seaio.c
        src/cpu.c
        src/machine.c
        src/opcodes_6502.c
        src/instructions.c
        src/debug.c
        src/disassembly.c)


add_library(sea64 STATIC ${SOURCES})

add_executable(sea64bin src/main.c)
target_link_libraries(sea64bin sea64)
set_target_properties(sea64bin PROPERTIES OUTPUT_NAME sea64)

add_executable(hex2bin tools/hex2bin.c)
target_link_libraries(hex2bin sea64)

enable_testing()

add_executable(test_sample tests/test_sample.c)
target_link_libraries(test_sample sea64)
add_test("Sample Testcase" test_sample)

add_executable(test_cpu_structs tests/test_cpu_structs.c)
target_link_libraries(test_cpu_structs sea64)
add_test("CPU Structs" test_cpu_structs)

add_executable(test_functional_tests tests/test_functional_tests.c)
target_link_libraries(test_functional_tests sea64)
add_test("Klaus's Functional Tests" test_functional_tests)

add_executable(test_processor_tests tests/test_processor_tests.c tests/proc_tester.c tests/proc_tester.c)
target_link_libraries(test_processor_tests sea64)
add_test("Processor Tests" test_processor_tests)
