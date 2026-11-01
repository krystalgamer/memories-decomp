# Duel-effect projected primitives and gradient lines

## Scoped projected-primitive C views

`primitive_semantic_views.h` names the complete two-function
`primitive_draw.c` object and the complete `gradient_lines.c` object. The
original drawing header, native declarations, SDK types, source definition
order, grouped extents and `gcc_2_8_1_g0_split` profile remain unchanged.
The symbol files bind the three names in place of their former labels;
nothing adds wrappers, native exports, data definitions or linker aliases.

| C name | Former label | NA address | JP address | UK / FR / ES address |
|---|---|---|---|---|
| `DuelEffect_DrawSharedBaseTriangles` | `func_80156E58` | `0x801490A0` | `0x80164EE8` | `0x80156E58` |
| `DuelEffect_DrawProjectedFlatQuad` | `func_80156FA4` | `0x801491EC` | `0x80165034` | `0x80156FA4` |
| `DuelEffect_DrawGradientPath` | `func_801570B0` | `0x801492F8` | `0x80165140` | `0x801570B0` |

These helpers form a projected-primitive submission cohort rather than one
generalized clipping API. Effects 8, 11, 16, 17 and 22 use the triangles;
effect 10 uses the flat quad; effects 13 and 17 use the gradient path and its
returned state. All actual bank callers use the typed views. Three scoped
registry rows use the actual NA inventory addresses, not native suffixes.
The three existing paired triangle ABI guards verify the complete typed
canonical definition, which the symbol files bind where `func_80156E58`
stood. Their unrelated helper and
regional SDK ownership/address/size assertions remain unchanged.
The five configured bank inventories change only their fifteen name cells;
addresses, sizes, profiles, manifests and non-name fields remain intact.
No resident symbols or German/Italian bank coverage are inferred.

### Shared-base triangles

For each supplied position, the complete body projects a `POLY_G3` with a
black first vertex and input-color second/third vertices. The two base
vectors are fixed at `(-width, 0, 0)` and `(width, 0, 0)` in local coordinates;
they are not translated independently to each position. Width remains `u16`
with assignment narrowing into SDK vector halfwords. The signed-halfword
bias is subtracted from `RotAverage3` depth before arithmetic shift and
`u16` priority narrowing. The resident submission always receives flags 1.

The routine does not reject on projection flags or negative depth and adds
no clamp. Zero count submits nothing but retains the original packet/base
initialization. Caller names such as rays, clouds and card particles support
the role but do not establish world-space units or one universal effect.

### Projected flat quad

The body constructs a solid input-color `POLY_F4`, enables
semitransparency and projects four supplied vertices. Only a negative
projection flag rejects submission; there is no separate negative-depth
check. Mode zero clears semitransparency and calls `GsSortPoly` with the
original shifted-depth argument. Every nonzero `u16` mode instead calls the
resident submitter with narrowed `u16` priority and caller flags. Do not
borrow mode-one-only branching, priority clamps or depth bias from other
packet helpers. Effect 10 calls this helper for two plane colors with mode
one; that does not remove the complete mode-zero contract.

### Gradient path and endpoint state

For each of `count` segments the body projects adjacent vertices and
builds a `GsGLINE` with RGB components divided by `count + 1 - i` and
`count - i`. Keep the separate divisor assignment, signed divisions and
projection order. Submission uses only the second endpoint's depth minus
the unsigned-halfword bias: nonnegative adjusted depth is quartered,
narrowed to `u16` and passed to `GsSortGLine`. Projection flags are written
but are not a rejection condition. Attribute formation retains
`0x40000000U + ((u32)flags << 28)` without flag normalization.

Mode one captures the first next vertex with positive local Y: copy X/Z,
set Y to zero, add the supplied offset and return mode two. This capture
is independent of segment submission; it is not an interpolated crossing
point or proof of a collision. Other modes remain unchanged. Zero count
returns the original mode without accessing vertices, output or offset.
No new count bound, null guard, output initialization or runtime coverage
claim is added.

### Conservation evidence

The nine affected C files retain all original bodies after reversing only
these three identifiers and removing one added include per file.
Accepted-parent compilation through the named profile preserves raw GCC
and MASPSX assembly, native symbol tables and disassembly/relocations for
all nine complete objects. Existing native headers, selectors and curve
controls remain unchanged. The paired ABI guards have only the
identifier-aware correction described above; no guard is deleted or
skipped and no parameter domain is relaxed. Full-image acceptance continues
through ordinary project matching and PR gates, not new source-string
harnesses or local execution of cloud tests.

## Original gradient-line matching evidence

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
