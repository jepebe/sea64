import json
import os.path
import sys
from typing import IO


# This script converts Tom Harte 6502 Processor Tests
# to a compressed binary format
# (https://github.com/TomHarte/ProcessorTests/)

def write_string(bin_file: IO, string: str, max_length: int = 10):
    arr = bytearray([0] * max_length)
    for i in range(max_length):
        if i < len(string):
            c = string[i]
            if not c.isascii():
                c = '?'
        else:
            c = '\x00'
        arr[i] = ord(c)
    arr[max_length - 1] = ord('\x00')  # Ensure string is null terminated
    bin_file.write(arr)


def write_u16(bin_file: IO, value: int):
    low = value & 0xFF
    high = (value >> 8) & 0xFF
    bin_file.write(bytearray([high, low]))


def write_u8(bin_file: IO, value: int):
    # print(f'{value} {value:02X}')
    bin_file.write(bytearray([value & 0xFF]))


def write_state(bin_file: IO, state: dict):
    write_u16(bin_file, state["pc"])
    write_u8(bin_file, state["s"])
    write_u8(bin_file, state["a"])
    write_u8(bin_file, state["x"])
    write_u8(bin_file, state["y"])
    write_u8(bin_file, state["p"])

    write_u8(bin_file, len(state["ram"]))  # write number of ram entries

    for ram in state["ram"]:
        write_u16(bin_file, ram[0])
        write_u8(bin_file, ram[1])


def write_cycles(bin_file: IO, cycles: list):
    write_u8(bin_file, len(cycles))

    for cycle in cycles:
        write_u16(bin_file, cycle[0])
        write_u8(bin_file, cycle[1])

        if cycle[2] == 'read':
            write_u8(bin_file, ord('r'))
        elif cycle[2] == 'write':
            write_u8(bin_file, ord('w'))
        else:
            raise RuntimeError(f"Unknown cycle activity {cycle[2]}")


if __name__ == '__main__':
    max_name_len = 0
    max_ram_count = 0
    max_cycle_count = 0

    print()
    for opcode in range(256):
        print("\x1b[2K", end='\r')
        print(f"Processing {opcode + 1}/256", end='')
        sys.stdout.flush()

        json_file = f"../tests/processor_tests/wdc65c02/{opcode:02x}.json"

        json_file_size = os.path.getsize(json_file)
        if json_file_size == 0:
            continue

        with open(json_file) as input_file:
            data = json.load(input_file)

        with open(f"../tests/processor_tests/wdc65c02/{opcode:02x}.bin", "wb") as output_file:
            for test in data:

                if len(test["name"]) > max_name_len:
                    max_name_len = len(test["name"])

                if len(test["initial"]["ram"]) > max_ram_count:
                    max_ram_count = len(test["initial"]["ram"])

                if len(test["final"]["ram"]) > max_ram_count:
                    max_ram_count = len(test["final"]["ram"])

                if len(test["cycles"]) > max_cycle_count:
                    max_cycle_count = len(test["cycles"])

                write_string(output_file, test["name"])
                write_state(output_file, test["initial"])
                write_state(output_file, test["final"])
                write_cycles(output_file, test["cycles"])

    print()
    print(f"{max_name_len=}")
    print(f"{max_ram_count=}")
    print(f"{max_cycle_count=}")
