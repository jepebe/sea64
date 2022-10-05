#!/usr/bin/env sh

find include -iname '*.h' -o -iname '*.c' | xargs clang-format --dry-run --Werror
find src -iname '*.h' -o -iname '*.c' | xargs clang-format --dry-run --Werror
find tests -iname '*.h' -o -iname '*.c' | xargs clang-format --dry-run --Werror
