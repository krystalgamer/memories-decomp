#!/bin/sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname "$0")/../../.." && pwd)
cd "$ROOT"

OUT=tmp/maspsx_bugs/post-mfhi-small-extern-store-nop
SOURCE=tools/maspsx_bugs/post-mfhi-small-extern-store-nop/func_8004E7B0.c
COMPILER=tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc
MASPSX=tools/vendor/maspsx/maspsx.py
PYTHON=tools/environments/python/bin/python
ASSEMBLER=tools/toolchains/binutils-2.42/bin/mipsel-none-elf-as
OBJDUMP=tools/toolchains/binutils-2.42/bin/mipsel-none-elf-objdump

mkdir -p "$OUT"

"$COMPILER" \
    -S -O2 -G8 -mel -mips1 -mcpu=R3000 -msoft-float -fno-builtin \
    -Dpsx -D__psx__ -D__psx \
    -o "$OUT/compiler.s" "$SOURCE"

"$PYTHON" "$MASPSX" \
    --aspsx-version=2.81 --expand-div -G8 \
    < "$OUT/compiler.s" \
    > "$OUT/maspsx.s"

"$ASSEMBLER" -EL -mips1 -G8 \
    -o "$OUT/candidate.o" "$OUT/maspsx.s"

"$OBJDUMP" -drz "$OUT/candidate.o" > "$OUT/candidate.dump"

printf 'wrote %s\n' \
    "$OUT/compiler.s" \
    "$OUT/maspsx.s" \
    "$OUT/candidate.o" \
    "$OUT/candidate.dump"
