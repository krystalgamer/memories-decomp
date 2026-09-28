# Spanish duel-bank dispatcher

`func_80146258` (`80146258..80146760`, 1,288 bytes) is complete matching C
under `gcc_2_8_1_g0_split`. The independent Spanish integration also reuses
the unchanged 148-byte French `rect_vertices.c` accepted in #6546.

Against accepted `45c9a270f`, every prior 54 Spanish manifest entry is
unchanged. The addition is two functions / 1,436 bytes: **56/85 bank C
functions / 14,320 bytes**, 29 explicit assembly boundaries, and **180/209
configured-overlay instances / 70,172 bytes**. This is not exhaustive runtime
completion; boot and other dynamic loads remain separate work.

## Recovered dispatch and argument contracts

The entry copies three offset halfwords, preserves vector padding, and
copies a flag halfword. For nonnegative phase it initializes exactly 21
texture-page/CLUT pairs using `func_8014F564`. It then executes 25 independent
effect comparisons. Unrecognized effects still perform the preceding setup.
Negative phase skips texture initialization, not effect dispatch.

The local 20-byte context has a standard eight-byte `SVECTOR` at offset
zero, projected/screen ordering-table pointers at 8/12, flags at 16, and a
signed variant halfword at 18. Only cases 2 and 13 pass the variant:
their callees `80153200` and `801503F8` save incoming `$a2` before overwriting
argument registers.

Case 9 preserves both original writes to the active ordering-table pointer.
The `$a2` value at its call is **not a third parameter**: `801593A8` ignores
incoming `$a2`, overwriting it with 8 before its first call. Treating this
scratch register as a third parameter caused the last five mismatching
instructions. The final declaration has two parameters, and no volatile
access or register-forcing workaround is used.

Case 20 passes one for nonnegative phase and preserves a negative phase.
Keeping two explicit source calls in the `if`/`else` lets GCC merge their
common call tail into the exact target. A ternary call argument was eight
bytes short and changed saved-register allocation.

`SetGeomOffset` uses the existing Spanish resident binding `80087838`
(24 bytes, still `sdk_asm`). Its calls use 160/108. No resident SDK function
is newly classified as game C.

## Canonical data views and actual storage owners

The setup loop establishes these extents independently of guessed headers:

| Address | Extent | Evidence |
| --- | ---: | --- |
| `8015A1E4` | 588 | 21 SDK `GsIMAGE` records, stride 28 |
| `8015A430` | 42 | 21 mode halfwords, stride 2 |
| `8015B748` | 84 | 21 output pairs, stride 4 |
| `8015B7F4` | 4 | Active ordering-table pointer |
| `8015B7F8` | 8 | SDK vector, three written halfwords and retained padding |
| `8015B800` | 2 | Flags |

`DuelEffectTextureTable` is the single declaration owner of the texture
storage: a union of the accepted 48-byte named prefix and `u16 pairs[21][2]`.
Existing consumers use the named member at offset zero, without changing any
accepted field offset. This avoids separate incompatible array/struct
declarations and avoids indexing beyond the old prefix.

The six symbols remain real definitions in the generated input `.data`
object, not C definitions or absolute aliases. Explicit symbol extents let
Splat resolve interior references such as `8015B778` as `D_8015B748 + 0x30`;
the original bytes and reference operands remain unchanged. The final linked
symbols have all six exact sizes. The final module section also contains
code, so data ownership is checked against the non-executable input data
object rather than inferred from the final combined section flags.

A target-GCC layout probe verifies sizes 84/48/20/28 for the union, prefix,
context and image record; context offsets 8/12/16/18; and retained prefix
offsets 40/46 for `page0`/`clut1`.

## Experiments and acceptance

Full snapshots and per-word diagnostics are retained privately under
`tmp/dispatch-probe/`. Sizes and differences below describe complete
functions, not accepted partial ranges.

| Attempt | Change | Bytes | Differing words |
| --- | --- | ---: | ---: |
| 1 | Direct dispatch with ternary phase and incorrectly inferred third argument | 1280 | 112 |
| 2 | Buffer alias and assignment-expression argument | 1280 | 143 |
| 3 | Explicit phase branches; original parameter lifetime | 1288 | 5 |
| 4 | Projected-table local | 1288 | 5 |
| 5 | Sequenced argument expression | 1288 | 5 |
| 6 | Named no-follow-jumps profile | 1288 | 5 |
| 7 | Re-read context field as the third argument | 1292 | 190 |
| 8 | Shared screen-table local | 1288 | 5 |
| 9 | Preload both tables; first store optimized away | 1280 | 190 |
| 10 | Locally qualified volatile view; rejected | 1292 | 191 |
| 11 | Ordinary active-table pointer view | 1288 | 5 |
| 12 | Named first-scheduling-disabled profile | 1284 | 288 |
| 13 | Correct two-argument callee and direct stores | 1288 | 0 |
| 14 | Canonical union-backed texture table | 1288 | 0 |

The private complete-bank link and production Spanish build reproduce all
90,112 bytes, SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
Exact object/final-ELF function extents, all six data owners and retail bytes,
and all 25 distinct overlay callees are verified. Assembly callees retain
their inventory status and real executable-section definitions.

Every affected regional build (Spanish, French, English PAL and Japanese)
matches after the shared view change. Spanish retail copies, metadata policy
and 64 targeted tests also pass. The randomized curve, polygon generators and
number renderer remain nonmatching assembly; none is included in this change.

## Independent French reuse

At accepted-master cutoff `1aafeffd6`, French reuses `dispatch.c`, its context
and the canonical texture-table view unchanged. `SetGeomOffset = 0x80087838`
is independently named in the French resident linker map; its complete
24-byte resident function remains `sdk_asm`.

The French full-bank proof reproduces the same 90,112-byte hash above. All
25 distinct overlay callees retain their real executable definitions and
original inventory status. All six data symbols have the exact extents above
in both the final ELF and the non-executable generated input-data object;
their bytes equal the French retail slice. No callee or data storage is
replaced with an absolute alias.

Together with the independently recovered 384-byte
[height ring](duel-effect-geometry.md#height-ring-follow-up), the two additions
contribute **1,672 bytes**. All 57 previously accepted French entries remain
unchanged: **59/85 bank C functions / 15,360 bytes**, **26 assembly
boundaries**, and **183 configured C instances / 71,212 bytes**. Existing
regional implementations and profiles are unchanged. Boot, other dynamic
loads and remaining bank assembly still prevent a French completion claim.
