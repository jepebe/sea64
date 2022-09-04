#include "machine.h"

NO_RETURN void cpu_error(Machine *machine, char *message, ...);

void cpu_error_marker(Machine *machine, char *file, int line);

void cpu_stack_contents(Machine *machine);

void cpu_instruction_context(Machine *machine);

void error_marker(char *file, int line);

void error(char *message, ...);

void warning(char *message, ...);
