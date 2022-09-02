#include "machine.h"

NO_RETURN void cpu_error(Machine *machine, char *message, ...);

void cpu_error_marker(Machine *machine, char *file, int line);

void cpu_instruction_context(Machine *machine);
