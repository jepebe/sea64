#include "opcodes.h"
#include "instructions.h"

const Opcode opcodes[] = {
        [0x00] = {brk, Implied, "BRK"},
        [0x08] = {php, Implied, "PHP"},
        [0x09] = {ora, Immediate, "ORA"},

        [0x10] = {bpl, Relative, "BPL"},
        [0x18] = {clc, Implied, "CLC"},

        [0x20] = {jsr, Absolute, "JSR"},
        [0x28] = {plp, Implied, "PLP"},

        [0x30] = {bmi, Relative, "BMI"},
        [0x38] = {sec, Implied, "SEC"},

        [0x40] = {rti, Implied, "RTI"},
        [0x48] = {pha, Implied, "PHA"},
        [0x49] = {eor, Immediate, "EOR"},
        [0x4C] = {jmp, Absolute, "JMP"},

        [0x50] = {bvc, Relative, "BVC"},
        [0x58] = {cli, Implied, "CLI"},

        [0x60] = {rts, Implied, "RTS"},
        [0x68] = {pla, Implied, "PLA"},
        [0x69] = {adc, Immediate, "ADC"},
        [0x6C] = {jmp, AbsoluteIndirect, "JMP"},

        [0x70] = {bvs, Relative, "BVS"},
        [0x78] = {sei, Implied, "SEI"},

        [0x85] = {sta, ZeroPage, "STA"},
        [0x86] = {stx, ZeroPage, "STX"},
        [0x88] = {dey, Implied, "DEY"},
        [0x8A] = {txa, Implied, "TXA"},
        [0x8D] = {sta, Absolute, "STA"},

        [0x90] = {bcc, Relative, "BCC"},
        [0x94] = {sty, XIndexedZeroPage, "STY"},
        [0x96] = {stx, YIndexedZeroPage, "STX"},
        [0x98] = {tya, Implied, "TYA"},
        [0x99] = {sta, YIndexedAbsolute, "STA"},
        [0x9A] = {txs, Implied, "TXS"},
        [0x9D] = {sta, XIndexedAbsolute, "STA"},

        [0xA0] = {ldy, Immediate, "LDY"},
        [0xA2] = {ldx, Immediate, "LDX"},
        [0xA5] = {lda, ZeroPage, "LDA"},
        [0xA6] = {ldx, ZeroPage, "LDX"},
        [0xA8] = {tay, Implied, "TAY"},
        [0xA9] = {lda, Immediate, "LDA"},
        [0xAA] = {tax, Implied, "TAX"},
        [0xAD] = {lda, Absolute, "LDA"},

        [0xB0] = {bcs, Relative, "BCS"},
        [0xB4] = {ldy, XIndexedZeroPage, "LDY"},
        [0xB5] = {lda, XIndexedZeroPage, "LDA"},
        [0xB6] = {ldx, YIndexedZeroPage, "LDX"},
        [0xB8] = {clv, Implied, "CLV"},
        [0xB9] = {lda, YIndexedAbsolute, "LDA"},
        [0xBA] = {tsx, Implied, "TSX"},
        [0xBC] = {ldy, XIndexedAbsolute, "LDY"},
        [0xBE] = {ldx, YIndexedAbsolute, "LDX"},
        [0xBD] = {lda, XIndexedAbsolute, "LDA"},

        [0xC0] = {cpy, Immediate, "CPY"},
        [0xC8] = {iny, Implied, "INY"},
        [0xC9] = {cmp, Immediate, "CMP"},
        [0xCA] = {dex, Implied, "DEX"},
        [0xCD] = {cmp, Absolute, "CMP"},

        [0xD0] = {bne, Relative, "BNE"},
        [0xD8] = {cld, Implied, "CLD"},
        [0xD5] = {cmp, XIndexedZeroPage, "CMP"},
        [0xD9] = {cmp, YIndexedAbsolute, "CMP"},
        [0xDD] = {cmp, XIndexedAbsolute, "CMP"},

        [0xE0] = {cpx, Immediate, "CPX"},
        [0xE8] = {inx, Implied, "INX"},
        [0xEA] = {nop, Implied, "NOP"},

        [0xF0] = {beq, Relative, "BEQ"},
        [0xF8] = {sed, Implied, "SED"},
};

Opcode fetch_opcode(u8 opcode) {
    return opcodes[opcode];
}
