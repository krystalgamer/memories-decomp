# Spanish MODEL446 Gouraud lines

The game-owned helper at image `+0xE00..+0x12F8` is independently recovered in
`src/overlays/spanish_model_variant/variant446_lines.c`. Its 1,272 instruction
bytes match at `0x8013BE00` and `0x8017BE00` using the existing
`gcc_2_8_1_g0_split` profile: GCC 2.8.1 and MASPSX 2.81.

A fresh seven-region screen found no accepted normalized C shape among
5,957 configured C entries, including sixteen same-size comparisons.
This is retail-derived recovery, not a French or other regional source port.

## Physical coverage and ownership

The [instance ledger](spanish-model-variant446-instances.csv) exhaustively covers
MODEL146, record 146, stages 9/10, sectors 40496/40506, headers 446/596.
The actual command is 612000. Entry selects the 76-byte descriptor at
image `+0x37D0`; its duration at descriptor `+0x30` is 184.

Both complete 20KiB images match. With the independently recovered
[billboard strip](spanish-model-variant446-strip.md), each has two C helpers
and five retained assembly functions, with boundaries
`4, E00, 12F8, 195C, 1FE8, 24BC, 2CAC, 36D4`.
All seven functions have closed, fully covered control flow; entry directly
calls all six helpers. The four-byte header and `+0x36D4..+0x5000` raw tail,
including the descriptor, retain their original non-executable data owners.
All 35 resident binding addresses are verified; six additional SDK aliases
support the strip without changing any existing binding.

## Original context and phase caveat

Entry saves original `a0` in `s2` at `+0xC`. Although initialization also uses
`s2` for point storage, reaching-definition analysis proves that only the
original definition reaches the actual helper call at `+0xCF0`, with
`s2 -> a0` in its delay slot.

**The observed entry call is gated by phase <= 0.** The helper also contains
phase-1 and later-phase behavior, which remains intact. Those paths are not
claimed to execute through the observed entry call; no alternative caller
has been established.

## Local layout and initialization

| Storage | Offset / extent |
| --- | --- |
| Three line groups | context `0xB80..0x1060`, stride `0x1A0` |
| Two point sets per group | `2 x 4 x 6` SVECTORs, group `0..0x180` |
| Group color / signed size / completion | `0x180` / `0x194` / `0x198` |
| GsGLINE packet | context `0x1B08..0x1B1C` |
| Word-coordinate origin | context `0x1B30..0x1B3C` |
| Short-coordinate target | context `0x1B5C..0x1B64` |
| Time / step / descriptor pointer / phase | `0x1B94` / `0x1B9C` / `0x1BA4` / `0x1BDC` |
| Recovered helper context extent | `0x1BE0` |

Entry initializes both point sets, using eight-byte point increments,
six columns, four rows, `0x30` row increments and `0x1A0` group increments.
It initializes all three group colors and sizes and clears completion.
Thirty target-compiled constants verify local/SDK layouts and packet bounds.

The helper resets `GsGLINE.attribute` to `0x50000000` inside every inner
iteration. It projects each endpoint twice using `RotTransPers4`, deliberately
passing the same two output coordinate addresses for the repeated endpoints.
Both cached addresses remain stable, lie within the 20-byte packet, and are
disjoint from its two RGB triplets. The context and packet registers remain
stable until their epilogue restores.

## Preserved behavior

Each group has four rows of six lines. Four initial `ratan2` calls are retained
even though their results are unused. Rotation is zero; phases below 2 use
the word-coordinate origin, while later phases use the signed-short target.

Phase 0 uses group size below 4096, otherwise zero scale; color fades above
2048. Phase 1 uses scale 4096 and black endpoints. Later phases clamp negative
scale to zero and fade color above 6144 toward the 8192 threshold. The colored
endpoint changes at phase 2.

Submission requires **strictly positive depth**, narrowed to `u16`.
There is **no GTE-flag test**. These rules differ from several other renderers
and are preserved rather than normalized.

Phase-0 size updates use unsigned time/duration division followed by signed
progress subtraction and group-index staggering; nonpositive results add
4096. Phase 1 assigns negative `index * 8192 / 3`. Later size growth uses
`step * 256`, wraps below phase 5, and otherwise clamps to 8192 and marks
completion.

## Exact-match evidence

The [attempt ledger](spanish-model-variant446-lines-attempts.csv) records
eight distinct nonmatching experiments, exact candidate nine, independent
slot-one compilation, and both complete-image terminals.

The successful source separates meaningful fade and normalized-progress
intermediates. This preserves the shared fade block, signed arithmetic
boundary, and multiplication operand order. Terminal branch order also
recovers the retail size-field induction base. The penultimate candidate
already had the exact extent and differed only in three reversed `MULT`
operands; those differences were resolved in C, not patched in assembly.
There are no forced registers, padding, dummy work, new compiler flags or
foreign declarations.

Regression coverage verifies original-context invocation, initialization,
all selected and retained object owners, complete images, thirty layouts,
resident callee/loader/context owners, and every selected relocation:
eleven calls to eight resident functions and ten local jumps. Relocating the
compiler object reproduces all 1,272 retail instruction bytes independently
of the complete-image hash.
