#include "opcodes.h"
#include "opcodes_6502.h"
#include "opcodes_6502_ext.h"
#include "opcodes_wdc65c02.h"

inline Opcode fetch_opcode(u8 opcode_num, CPUType cpu_type) {
    switch (cpu_type) {
        case MOS6502:
            return opcodes_6502[opcode_num];
        case MOS6502EXT:
            return opcodes_6502_ext[opcode_num];
        case WDC65C02:
            return opcodes_wdc65c02[opcode_num];
        default:
            ERROR("Unknown cpu type %d", cpu_type)
    }
}
