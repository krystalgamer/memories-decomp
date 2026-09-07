#!/bin/sh
# Build func_80012DB4.c the way the project does and compare it with the
# original game assembly in func_80012DB4.s.
#
# Profile gcc_2_8_1_g8_split_comm, from config/slus_01411/compiler_profiles.json.
# All intermediates go to tmp/; nothing is written into this directory.
set -e
cd "$(dirname "$0")/../../.."
D=tools/maspsx_bugs/reorder-branch-delay-slot
O=tmp/maspsx_bug_reorder
mkdir -p "$O"
GCC=tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc
AS=tools/toolchains/binutils-2.42/bin/mipsel-none-elf-as
OBJDUMP=tools/toolchains/binutils-2.42/bin/mipsel-none-elf-objdump

# 1. compile
$GCC -S -O2 -G8 -mel -mips1 -mcpu=R3000 -msoft-float -fno-builtin \
     -mgas -msplit-addresses -Dpsx -D__psx__ -D__psx \
     -o "$O/out.s" "$D/func_80012DB4.c"

# 2. normalise through maspsx  <-- the step under test
python3 tools/vendor/maspsx/maspsx.py \
     --aspsx-version=2.81 --expand-div -G8 --use-comm-section \
     < "$O/out.s" > "$O/out.maspsx.s"

# 3. assemble
$AS -EL -mips1 -G8 -o "$O/out.o" "$O/out.maspsx.s"

SIZE=$($OBJDUMP -t "$O/out.o" | awk '/F .text/ {print strtonum("0x" $5)}')
echo "built $SIZE bytes; original game function is 168 (0xA8, 42 instructions)"
echo
echo "=== what GCC handed the assembler (the input under test) ==="
sed -n '/andi\t\$2,\$2,0x00ff/,/^\$L7:/p' "$O/out.s"
echo
echo "=== what maspsx produced ==="
sed -n '/andi\t\$2,\$2,0x00ff/,/^\$L7:/p' "$O/out.maspsx.s"
echo
echo "=== built ==="
$OBJDUMP -d "$O/out.o" | sed -n '/<func_80012DB4>:/,$p'
echo
echo "=== original game assembly: $D/func_80012DB4.s ==="
cat "$D/func_80012DB4.s"
