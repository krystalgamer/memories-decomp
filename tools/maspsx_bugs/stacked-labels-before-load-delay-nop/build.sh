#!/bin/sh
# Profile: gcc_2_7_2_cdk_g0 in config/slus_01411/compiler_profiles.json.
# Run from the repository root.  MASPSX=<path to maspsx.py> overrides the pinned
# checkout, which is how maspsx.patch is tried without touching tools/vendor.
set -eu

if [ ! -f config/slus_01411/compiler_profiles.json ]; then
    echo "run from the repository root" >&2
    exit 1
fi

here=$(dirname "$0")
out=tmp/maspsx_bugs/stacked-labels-before-load-delay-nop
source=$here/func_8013C864.c
python=tools/environments/python/bin/python
gcc=tools/toolchains/gcc-2.7.2-cdk/bin/mips-sony-psx-gcc
maspsx=${MASPSX:-tools/vendor/maspsx/maspsx.py}
as=tools/toolchains/binutils-2.42/bin/mipsel-none-elf-as
objdump=tools/toolchains/binutils-2.42/bin/mipsel-none-elf-objdump

mkdir -p "$out"

"$gcc" -S -O2 -G0 -mel -mips1 -mcpu=R3000 -msoft-float -fno-builtin \
    -Dpsx -D__psx__ -D__psx \
    -o "$out/compiler.s" "$source"

"$python" "$maspsx" --aspsx-version=2.81 --expand-div -G0 \
    < "$out/compiler.s" > "$out/maspsx.s"

"$as" -EL -mips1 -G0 -o "$out/candidate.o" "$out/maspsx.s"
"$objdump" -drz --no-show-raw-insn "$out/candidate.o" > "$out/candidate.dump"

echo "--- gcc: the two stacked labels"
grep -n -B2 -A2 '^\$L78:' "$out/compiler.s"
echo "--- maspsx: where the nop lands relative to them"
grep -n -B2 -A4 '^\$L78:' "$out/maspsx.s"
echo "--- candidate: the branch and the jump (retail: both 0x104)"
grep -E '^ +(d8|e4):' "$out/candidate.dump"
