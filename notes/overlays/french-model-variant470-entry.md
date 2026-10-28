# French MODEL470 loader entries

The header-470/620 family in French physical MODEL425, stage 7/slot 0 and
stage 8/slot 1, now has matching C for all three functions. Both complete
20,480-byte images reproduce retail byte-for-byte. This completes a family
in #6460.

The entry occupies `8013B004..8013C510`, or the corresponding `8017` range:
5,388 instruction bytes, 1,347 words and a 200-byte frame. One implementation
and a slot wrapper use the named `gcc_2_8_1_g0_split` profile. All native
words were read before reconstruction. Declarations come from local game/SDK
headers and independently measured accesses, not reference-project headers,
types or compiler claims.

## Preserved ownership and reachability

| Slot-0 address | Bytes (hex) | Existing C owner |
|---|---:|---|
| `8013C510` | `4E4` | French `variant470_quad_pairs.c` |
| `8013C9F4` | `4DC` | Spanish `variant470_quads.c` |

Both helpers, their slot wrappers, profiles and inventory rows are unchanged.
Production ELFs verify all three original-sized C owners in each image.
The first helper requires `elapsed >= config->draw_start`; the second
independently requires `phase >= 2`. The entry directly reaches both.

## Measured constructor and runtime view

The minimum observed context extent is `2628`, not an allocation-capacity
claim. Configuration records have a `38`-byte stride at suffix `8013CED0`
plus `FC`. Two byte-indexed part lookups and first-part matrix refresh remain.

| Context offset | Measured storage |
|---|---|
| `0000` | Two `4C4`-byte ten-quad pair records, including matrices and captured/direction arrays |
| `0988` | One `2FC`-byte ten-quad group |
| `0C84` | Sixteen `A8`-byte nine-point trail records |
| `1704` | Three `1A0`-byte four-by-six near/far webs |
| `1BE4` | One `A0`-byte two-color-pair record |
| `1C84` | Two `98`-byte sheets |
| `1DB4` | Six `90`-byte eight-point rings |
| `2114` | Four `90`-byte eight-point rings |
| `2354` | One `90`-byte five-point fan |
| `23E4` | Triangle, quad, textured packets and line |
| `2584` | Matrix, target, direction and projection state |
| `25D8` | Timing, configuration, part pointers and controller state |

Entry-only arrays retain their measured extents rather than using a shorter
renderer prefix as a stride. Packet holes and unobserved storage stay untouched.
Native unsigned geometry shifts, byte-color stores, signed projected Y,
unsigned timing and separate frame-step calls remain.

The target starts at `(0,-300,slot0 ? -450 : 450)`. Matrix refresh uses
`elapsed < config->matrix_end`. The unsigned motion interval and division
remain distinct from pair capture. Native slot arms are retained separately
even where their predicates agree: direct capture for `k % 4 == 0`, otherwise
192-unit circular X/Y capture with a 1300-unit bearing step. Each pair uses
its own part matrix and captured-minus-anchor direction.

Camera offset is `(-512,-256,0)`. Ambient decay, `fade << 5`, phase changes
and return values remain native: phases 2-4 return 4; phase 5 returns 1 and
enters 6; fade 64 enters 7; phase 7 returns 2.

## Angular ownership recovery

The final exact experiment separates the actual small-ring point bearing
from the shared fan/web/runtime bearing. Web latitude shares the real angular
variable later used for ring phase. This removes its overlap with the web
column counter and reproduces the native latitude/far-radius reuse and reset
anchor allocation. Neither change adds a value dependency or memory access.
The same ownership recovery independently matches the MODEL477 entry.

The ledger records seventeen measured source/header or named-profile
experiments and two complete production-image terminals. Product/shift
staging, shared size outputs and literal-radius controls were retained as
failed evidence rather than integrated. The final fixed-point reset remains
a literal 4096 produced inside the last ring initializer and consumed after it.
There is no inline assembly, forced register, volatile trick, source-local
extern, fabricated access or index-derived reset arithmetic.

## Complete production image identities

| Physical model / stage / slot | SHA-256 |
|---|---|
| 425 / 7 / 0 | `7a21edf9756b25dfbfa2688984fae56e7567c93caaf7fda36aa3108ff4762202` |
| 425 / 8 / 1 | `4c4424c041c8051d3b3260c81284aaa3a61fd160b04011e1fa34229abedcffa5` |
