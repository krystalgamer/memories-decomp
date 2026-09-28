# Duel-effect projected gradient lines

`func_801570B0` occupies all 760 bytes of `0x801570B0..0x801573A8`.
`gradient_lines.c` reproduces that complete interval with the unchanged
`gcc_2_8_1_g0_split` profile.

The routine uses the existing SDK `GsGLINE` record. For each adjacent pair of
SVECTOR vertices it computes two RGB intensities using signed integer
division, projects both endpoints, subtracts the unsigned-halfword depth bias
from the second projection result, and sorts only nonnegative adjusted depth.
The priority is shifted by two and narrowed to `u16`. Projection flags are
written but are not an additional rejection condition.

Mode one also captures the first next vertex whose Y coordinate is positive:
copy X/Z, set Y to zero, add the supplied offset, and change the mode to two.
That capture is independent of whether the segment was submitted. Zero count
does not enter the loop and returns the original mode; the output and offset
are not accessed. Loop denominators remain positive for all executed
iterations, while the original signed-division instruction sequence and traps
are preserved rather than replaced with unsigned division.

## Source experiments

The first candidate had the correct 760-byte size with only two differing
instructions at offsets `0x74` and `0x7C`: GCC reassociated `count + 1 - i`
as `count - (i - 1)`. A two-statement divisor update preserved the arithmetic
order but selected `$a0` instead of `$v1`, producing 21 register differences.
Keeping only `divisor = count + 1` as a separate statement and using
`divisor - i` in the three first-endpoint expressions reproduced all target
instructions. All three source/header snapshots and diffs remain local.

## SDK identity and ownership

The two external calls are independently identified against already named
North American SDK functions, not guessed from overlay completion:

| SDK routine | Spanish address | North American address | Bytes |
|---|---|---|---:|
| `RotTransPers` | `0x80087868` | `0x800878E0` | 44 |
| `GsSortGLine` | `0x800840B8` | `0x80084130` | 264 |

The 44-byte projection routines are byte-identical. The line sorters differ
only in seven address relocations at relative offsets `0x2C`, `0x70`, `0x88`,
`0xC4`, `0xDC`, `0xE8` and `0xF4`; every other word and the instruction/register
bits of those relocations agree. The Spanish resident inventory already marks
both complete extents as `sdk_asm`. New overlay bindings use those existing
owners and the SDK header signatures; no resident function is renamed,
reclassified or counted as new game C.

A fresh private full-bank link verifies this function alongside all 43
accepted Spanish entries, reaching 44 C functions / 10,020 bytes and retaining
41 assembly boundaries. The entire 90,112-byte bank retains SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
Configured Spanish overlays contain 168/209 matching instances / 65,872 bytes
at this independent cutoff. Production acceptance also checks all module
images and terrain copies, exact C object/ELF ownership, and real preserved
ordering-table/vector/texture data. No partial number-renderer candidate or
unmerged display/projection group is included.

Before publication, the now-accepted display/projection batch and subsequent
French/Japanese shared-source consolidation were integrated additively.
All 50 accepted Spanish entries remain unchanged. The combined bank contains
51 C functions / 12,352 bytes and 34 assembly boundaries; configured Spanish
overlays contain 175/209 matching instances / 68,204 bytes. The new game C
addition remains exactly one complete 760-byte routine.
