# Spanish MODEL441 framebuffer rings

`func_8013E060` / `func_8017E060`, `+3060..+3630`, is independently recovered
game-owned C: 1,488 bytes in each of the ten physical MODEL441 images.
The refreshed pre-integration screen covered 6,037 configured regional C
entries, including newly accepted tube and French MODEL177 work, and found
no same-size body. No regional C was ported.

The named `gcc_2_8_1_g0_split` GCC 2.8.1 / MASPSX 2.81 profile matches both
slots and every full 20 KiB image. This adds ten C instances / 14,880 bytes,
preserving accepted line and tube C, their ledgers, all module records and
the instance CSV. The family now has thirty C instances / 45,920 bytes,
three unique routines, and sixty remaining physical ASM functions.
The later [retained-ribbon recovery](spanish-model-variant441-ribbon.md)
preserves these rings and adds a fourth C function per image, leaving fifty
physical ASM functions.

## Caller and private layout

Entry `+1054` calls this helper outside the descriptor iteration loop when
phase is at least two. Reaching-definition analysis proves original `a0`,
captured in `s2` at entry `+C`, reaches that call's delay-slot argument.
This does not change the earlier line helper's missing-caller limitation.

Five `1A8`-byte groups start at context `14D4` and end at `1D1C`. Each has
three banks of seventeen `SVECTOR` points at offsets `0/88/110`, total `198`
bytes, and signed progress at `1A0`. Other bytes remain opaque; no completion
field is invented.

The `POLY_GT4` packet occupies `25D4..2608`. Target is `269C`, direction
`26A4`, signed frame `26D0`, step `26DC`, and phase `271C`. The private view
ends at `2720`, not necessarily the complete context. Thirty-five target
sizes/offsets and all packet stores are checked. The helper captures context
in `s3` at `+3068`; the packet pointer in `s1` at `+30E4` is stable.

## Two deliberately different projections

For each positive-progress group, sine-derived uniform scale and direction-
dependent rotation are applied around target plus
`direction*(progress-1024)/2048`. The natural frame is 280 bytes.

Each of sixteen intervals first projects banks zero and two to obtain
framebuffer sample coordinates. Its depth and GTE flag are not tested.
The signed first screen x coordinate selects a texture page at threshold 160:

| First x | Active buffer zero | Other active buffer | UV x adjustment |
|---|---|---|---|
| `< 160` | page x 320 | page x 0 | none |
| `>= 160` | page x 448 | page x 128 | subtract 128 |

`GetTPage(2,1,x,0)` supplies the page; `SetPolyGT4` initializes the packet.
The first projection's coordinates become byte UVs, including original
narrowing/wrapping, before a second projection overwrites screen coordinates.
That second projection uses banks zero and one.

Semitransparency and raw-texture mode are then disabled. Two vertices are
white; the other two cycle through eight colors using **signed** `frame % 8`.
Submission requires **strictly positive second-pass depth**, narrows it to
`u16`, and calls `GsSortPoly`. There is no GTE-flag rejection: both calls
receive a flag pointer but the helper never loads the flag. Do not replace
this rule with the nonnegative-depth-and-flag gate of the other renderers.

## Progress and completion

Every group below 2048 clears a local completion flag and advances by
`step*32`, even if it was not rendered. Crossing 2048 clamps at phases five
and later; earlier phases subtract 2048 once.

After the fifth group, phase five advances to six only if the local flag
remains one. Thus a group that first reaches 2048 during this invocation
still prevents that transition. The flag is not a stored per-group field,
nor is it cleared only on wrapping. The entry gate itself does not stop
calling this helper merely because phase reaches six.

## Exact recovery and preservation

The first candidate already had the correct 1,488-byte extent and natural
frame, differing only at two texture-page return promotions. A full-width
`u32` temporary preserves the promotion of the existing SDK's `u16`
`GetTPage` return, restoring both `andi` instructions. The authoritative
SDK declaration was not changed, and no casts, forced registers, dummy
locals, padding or inline assembly were added to manufacture the match.

The thirteen-row attempt ledger records those two candidates, the independent
slot-one compilation and ten whole-image terminal matches. Six additional
SDK aliases preserve all original names and all 37 resident addresses.

Regressions cover original context, three coexisting private headers,
thirty-five layouts, both projection banks, texture-page selection and
promotion, UV/RGB bounds, signed palette behavior, completion-flag writes,
all compiler/ASM/data input and final owners, eighteen call relocations to
fourteen resident addresses, and eleven local jumps. Retail bytes, generated
assembly, toolchains and scratch evidence remain untracked.
