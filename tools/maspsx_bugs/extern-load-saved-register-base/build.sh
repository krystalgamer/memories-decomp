#!/bin/sh
set -eu

if [ ! -f config/slus_01411/compiler_profiles.json ]; then
    echo "run from the repository root" >&2
    exit 1
fi

out=tmp/maspsx_bugs/extern-load-saved-register-base
source=tools/maspsx_bugs/extern-load-saved-register-base/func_8004CB0C.c
python=tools/environments/python/bin/python
gcc=tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc
as=tools/toolchains/binutils-2.42/bin/mipsel-none-elf-as
objdump=tools/toolchains/binutils-2.42/bin/mipsel-none-elf-objdump

mkdir -p "$out"

"$gcc" -S -O2 -G0 -mel -mips1 -mcpu=R3000 -msoft-float -fno-builtin \
    -Dpsx -D__psx__ -D__psx \
    -o "$out/compiler.s" "$source"

"$python" tools/vendor/maspsx/maspsx.py \
    --aspsx-version=2.81 --expand-div -G0 \
    < "$out/compiler.s" > "$out/maspsx.s"

"$as" -EL -mips1 -G0 -o "$out/candidate.o" "$out/maspsx.s"
"$objdump" -drz "$out/candidate.o" > "$out/candidate.dump"

grep -n -E 'D_800E9D98|D_800E9D9C' \
    "$out/compiler.s" "$out/maspsx.s" "$out/candidate.dump"
