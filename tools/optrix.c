#include "cpu.h"
#include "opcodes.h"
#include "seatypes.h"
#include "stdio.h"

static char *addr_mode_name(AddrMode mode) {
    switch (mode) {
        case Absolute:
            return "$nnnn";
        case AbsoluteIndirect:
            return "($nnnn)";
        case Accumulator:
            return "A";
        case Immediate:
            return "#nn"; // canonically #$nn
        case Implied:
            return "";
        case Relative:
            return "$nnnn";
        case XIndexedAbsolute:
            return "$nnnn,X";
        case XIndexedAbsoluteIndirect:
            return "($nnnn,X)";
        case XIndexedZeroPage:
            return "$nn,X";
        case XIndexedZeroPageIndirect:
            return "($nn,X)";
        case YIndexedAbsolute:
            return "$nnnn,Y";
        case YIndexedZeroPage:
            return "$nn,Y";
        case ZeroPage:
            return "$nn";
        case ZeroPageIndirect:
            return "($nn)";
        case ZeroPageIndirectYIndexed:
            return "($nn),Y";
        case ZeroPageRelative:
            return "$nn,$nnnn";;
        default:
            error_marker(__FILE__, __LINE__);
            error("addressing mode %d not implemented", mode);
    }
}

void print_line() {
    printf("----+");
    for (int col = 0; col < 16; ++col) {
        printf("-----------+");
    }
    printf("\n");
}

void print_header() {
    printf("    |");
    for (int col = 0; col < 16; ++col) {
        printf("    x%X     ", col);
        printf("|");
    }
    printf("\n");
}

int main() {
    CPUType cpu_type = WDC65C02;

    print_line();
    print_header();
    print_line();

    for (int row = 0; row < 16; ++row) {
        printf(" %Xx |", row);
        for (int col = 0; col < 16; ++col) {
            Opcode opc = fetch_opcode(row * 16 + col, cpu_type);
            if (opc.op_fn != NULL) {
                printf(" %-9s ", opc.name);
            } else {
                printf(" %-9s ", " ");
            }

            printf("|");
        }
        printf("\n");

        printf("    |");
        for (int col = 0; col < 16; ++col) {
            Opcode opc = fetch_opcode(row * 16 + col, cpu_type);

            if (opc.op_fn != NULL) {
                printf(" %-9s ", addr_mode_name(opc.addr_mode));
            } else {
                printf(" %-9s ", " ");
            }

            printf("|");
        }
        printf("\n");

        print_line();
    }
}
