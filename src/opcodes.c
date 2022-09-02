#include "opcodes.h"
#include "instructions.h"

const Opcode opcodes[] = {
        [0x10] = {bpl, Relative, "BPL"},
        [0x4C] = {jmp, Absolute, "JMP"},
        [0x88] = {dey, Implied, "DEY"},
        [0x8D] = {sta, Absolute, "STA"},
        [0x98] = {tya, Implied, "TYA"},
        [0x9A] = {txs, Implied, "TXS"},
        [0xA0] = {ldy, Immediate, "LDY"},
        [0xA2] = {ldx, Immediate, "LDX"},
        [0xA9] = {lda, Immediate, "LDA"},
        [0xAA] = {tax, Implied, "TAX"},
        [0xAD] = {lda, Absolute, "LDA"},
        [0xC9] = {cmp, Immediate, "CMP"},
        [0xCA] = {dex, Implied, "DEX"},
        [0xD0] = {bne, Relative, "BNE"},
        [0xD8] = {cld, Implied, "CLD"},
        [0xEA] = {nop, Implied, "NOP"},
        [0xF0] = {beq, Relative, "BEQ"},
};

Opcode fetch_opcode(u8 opcode) {
    return opcodes[opcode];
}
