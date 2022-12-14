#include "opcodes_6502.h"
#include "instructions.h"

const Opcode opcodes_6502[] = {
    [0x00] = {brk, Implied, "BRK"},
    [0x01] = {ora, XIndexedZeroPageIndirect, "ORA"},
    [0x05] = {ora, ZeroPage, "ORA"},
    [0x06] = {asl, ZeroPage, "ASL"},
    [0x08] = {php, Implied, "PHP"},
    [0x09] = {ora, Immediate, "ORA"},
    [0x0A] = {asl, Accumulator, "ASL"},
    [0x0D] = {ora, Absolute, "ORA"},
    [0x0E] = {asl, Absolute, "ASL"},

    [0x10] = {bpl, Relative, "BPL"},
    [0x11] = {ora, ZeroPageIndirectYIndexed, "ORA"},
    [0x15] = {ora, XIndexedZeroPage, "ORA"},
    [0x16] = {asl, XIndexedZeroPage, "ASL"},
    [0x18] = {clc, Implied, "CLC"},
    [0x19] = {ora, YIndexedAbsolute, "ORA"},
    [0x1D] = {ora, XIndexedAbsolute, "ORA"},
    [0x1E] = {asl, XIndexedAbsolute, "ASL"},

    [0x20] = {jsr, Absolute, "JSR"},
    [0x21] = {and, XIndexedZeroPageIndirect, "AND"},
    [0x24] = {bit, ZeroPage, "BIT"},
    [0x25] = {and, ZeroPage, "AND"},
    [0x26] = {rol, ZeroPage, "ROL"},
    [0x28] = {plp, Implied, "PLP"},
    [0x29] = {and, Immediate, "AND"},
    [0x2A] = {rol, Accumulator, "ROL"},
    [0x2C] = {bit, Absolute, "BIT"},
    [0x2D] = {and, Absolute, "AND"},
    [0x2E] = {rol, Absolute, "ROL"},

    [0x30] = {bmi, Relative, "BMI"},
    [0x31] = {and, ZeroPageIndirectYIndexed, "AND"},
    [0x35] = {and, XIndexedZeroPage, "AND"},
    [0x36] = {rol, XIndexedZeroPage, "ROL"},
    [0x38] = {sec, Implied, "SEC"},
    [0x39] = {and, YIndexedAbsolute, "AND"},
    [0x3D] = {and, XIndexedAbsolute, "AND"},
    [0x3E] = {rol, XIndexedAbsolute, "ROL"},

    [0x40] = {rti, Implied, "RTI"},
    [0x41] = {eor, XIndexedZeroPageIndirect, "EOR"},
    [0x45] = {eor, ZeroPage, "EOR"},
    [0x46] = {lsr, ZeroPage, "LSR"},
    [0x48] = {pha, Implied, "PHA"},
    [0x49] = {eor, Immediate, "EOR"},
    [0x4A] = {lsr, Accumulator, "LSR"},
    [0x4C] = {jmp, Absolute, "JMP"},
    [0x4D] = {eor, Absolute, "EOR"},
    [0x4E] = {lsr, Absolute, "LSR"},

    [0x50] = {bvc, Relative, "BVC"},
    [0x51] = {eor, ZeroPageIndirectYIndexed, "EOR"},
    [0x55] = {eor, XIndexedZeroPage, "EOR"},
    [0x56] = {lsr, XIndexedZeroPage, "LSR"},
    [0x58] = {cli, Implied, "CLI"},
    [0x59] = {eor, YIndexedAbsolute, "EOR"},
    [0x5D] = {eor, XIndexedAbsolute, "EOR"},
    [0x5E] = {lsr, XIndexedAbsolute, "LSR"},

    [0x60] = {rts, Implied, "RTS"},
    [0x61] = {adc, XIndexedZeroPageIndirect, "ADC"},
    [0x65] = {adc, ZeroPage, "ADC"},
    [0x66] = {ror, ZeroPage, "ROR"},
    [0x68] = {pla, Implied, "PLA"},
    [0x69] = {adc, Immediate, "ADC"},
    [0x6A] = {ror, Accumulator, "ROR"},
    [0x6C] = {jmp, AbsoluteIndirect, "JMP"},
    [0x6D] = {adc, Absolute, "ADC"},
    [0x6E] = {ror, Absolute, "ROR"},

    [0x70] = {bvs, Relative, "BVS"},
    [0x71] = {adc, ZeroPageIndirectYIndexed, "ADC"},
    [0x75] = {adc, XIndexedZeroPage, "ADC"},
    [0x76] = {ror, XIndexedZeroPage, "ROR"},
    [0x78] = {sei, Implied, "SEI"},
    [0x79] = {adc, YIndexedAbsolute, "ADC"},
    [0x7D] = {adc, XIndexedAbsolute, "ADC"},
    [0x7E] = {ror, XIndexedAbsolute, "ROR"},

    [0x81] = {sta, XIndexedZeroPageIndirect, "STA"},
    [0x84] = {sty, ZeroPage, "STY"},
    [0x85] = {sta, ZeroPage, "STA"},
    [0x86] = {stx, ZeroPage, "STX"},
    [0x88] = {dey, Implied, "DEY"},
    [0x8A] = {txa, Implied, "TXA"},
    [0x8C] = {sty, Absolute, "STY"},
    [0x8D] = {sta, Absolute, "STA"},
    [0x8E] = {stx, Absolute, "STX"},

    [0x90] = {bcc, Relative, "BCC"},
    [0x91] = {sta, ZeroPageIndirectYIndexed, "STA"},
    [0x94] = {sty, XIndexedZeroPage, "STY"},
    [0x95] = {sta, XIndexedZeroPage, "STA"},
    [0x96] = {stx, YIndexedZeroPage, "STX"},
    [0x98] = {tya, Implied, "TYA"},
    [0x99] = {sta, YIndexedAbsolute, "STA"},
    [0x9A] = {txs, Implied, "TXS"},
    [0x9D] = {sta, XIndexedAbsolute, "STA"},

    [0xA0] = {ldy, Immediate, "LDY"},
    [0xA1] = {lda, XIndexedZeroPageIndirect, "LDA"},
    [0xA2] = {ldx, Immediate, "LDX"},
    [0xA4] = {ldy, ZeroPage, "LDY"},
    [0xA5] = {lda, ZeroPage, "LDA"},
    [0xA6] = {ldx, ZeroPage, "LDX"},
    [0xA8] = {tay, Implied, "TAY"},
    [0xA9] = {lda, Immediate, "LDA"},
    [0xAA] = {tax, Implied, "TAX"},
    [0xAC] = {ldy, Absolute, "LDY"},
    [0xAD] = {lda, Absolute, "LDA"},
    [0xAE] = {ldx, Absolute, "LDX"},

    [0xB0] = {bcs, Relative, "BCS"},
    [0xB1] = {lda, ZeroPageIndirectYIndexed, "LDA"},
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
    [0xC1] = {cmp, XIndexedZeroPageIndirect, "CMP"},
    [0xC4] = {cpy, ZeroPage, "CPY"},
    [0xC5] = {cmp, ZeroPage, "CMP"},
    [0xC6] = {dec, ZeroPage, "DEC"},
    [0xC8] = {iny, Implied, "INY"},
    [0xC9] = {cmp, Immediate, "CMP"},
    [0xCA] = {dex, Implied, "DEX"},
    [0xCC] = {cpy, Absolute, "CPY"},
    [0xCD] = {cmp, Absolute, "CMP"},
    [0xCE] = {dec, Absolute, "DEC"},

    [0xD0] = {bne, Relative, "BNE"},
    [0xD1] = {cmp, ZeroPageIndirectYIndexed, "CMP"},
    [0xD5] = {cmp, XIndexedZeroPage, "CMP"},
    [0xD6] = {dec, XIndexedZeroPage, "DEC"},
    [0xD8] = {cld, Implied, "CLD"},
    [0xD9] = {cmp, YIndexedAbsolute, "CMP"},
    [0xDD] = {cmp, XIndexedAbsolute, "CMP"},
    [0xDE] = {dec, XIndexedAbsolute, "DEC"},

    [0xE0] = {cpx, Immediate, "CPX"},
    [0xE1] = {sbc, XIndexedZeroPageIndirect, "SBC"},
    [0xE4] = {cpx, ZeroPage, "CPX"},
    [0xE5] = {sbc, ZeroPage, "SBC"},
    [0xE6] = {inc, ZeroPage, "INC"},
    [0xE8] = {inx, Implied, "INX"},
    [0xE9] = {sbc, Immediate, "SBC"},
    [0xEA] = {nop, Implied, "NOP"},
    [0xEC] = {cpx, Absolute, "CPX"},
    [0xED] = {sbc, Absolute, "SBC"},
    [0xEE] = {inc, Absolute, "INC"},

    [0xF0] = {beq, Relative, "BEQ"},
    [0xF1] = {sbc, ZeroPageIndirectYIndexed, "SBC"},
    [0xF5] = {sbc, XIndexedZeroPage, "SBC"},
    [0xF6] = {inc, XIndexedZeroPage, "INC"},
    [0xF8] = {sed, Implied, "SED"},
    [0xF9] = {sbc, YIndexedAbsolute, "SBC"},
    [0xFD] = {sbc, XIndexedAbsolute, "SBC"},
    [0xFE] = {inc, XIndexedAbsolute, "INC"},
};
