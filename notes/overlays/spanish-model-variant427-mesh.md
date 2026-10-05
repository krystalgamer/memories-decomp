# Spanish MODEL427 three-mesh renderer

The game-owned helper at image `+0xFB4..+0x157C` is independently recovered in
`src/overlays/spanish_model_variant/variant427_mesh.c`. It is 1,480 instruction
bytes at `0x8013BFB4` or `0x8017BFB4`, using the existing
`gcc_2_8_1_g0_split` profile (GCC 2.8.1 / MASPSX 2.81).

The local seven-region screen found no accepted normalized C shape among
5,955 configured C entries, including four same-size comparisons. This is
retail-derived recovery, not a regional source port.

## Physical coverage and preservation

The exhaustive [existing instance ledger](spanish-model-variant427-instances.csv)
covers MODEL379, record 329, stages 9/10, sectors 91004/91014, headers 427/577.
The descriptor selects command 593000. Both complete 20KiB images remain exact.
Each image now has three C helpers and four assembly functions. The accepted
panels and curtains, their private headers and historical attempt records,
all 34 resident bindings, raw four-byte headers and `+0x3ACC..+0x5000` tails
are unchanged.

The [five-row attempt ledger](spanish-model-variant427-mesh-attempts.csv) records
the initial mismatch, corrected slot-zero source, independent slot-one
compilation and two full-image terminals. The initial source introduced a
duplicate row induction and a spill: 1,496 bytes, 271 differing words and a
360-byte frame. Ordinary indexed traversal, companion declaration order and
the existing word-packet argument contract recover every byte with the retail
352-byte frame. There is no register forcing, padding, dummy work or new flag.

## Original context and initialization

The entry saves original `a0` in `s5` at `+0xC`. Its phase-3-through-5 path calls
the helper at `+0xE0C`, passing that same original context in the `+0xE10`
delay slot. Control-flow reaching-definition analysis establishes `+0xC` as
the only reaching definition of `s5` at that call.

Entry `+0x4C/+0x58` forms and saves the context `+0x3450` GT4 address.
The stack slot has one direct write before its reload at `+0x3E4` and actual
`SetPolyGT4` call at `+0x3E8`. Texture-page/UV/CLUT setup follows, then
`SetSemiTrans(1)` at `+0x430` and `SetShadeTex(0)` at `+0x43C`.
The helper's context and packet registers remain stable until their epilogue
restores. Its twelve packet writes are only the four RGB triplets.

Entry initializes three mesh records: seventeen vertices per row, nine rows,
eight-byte point and `0x88` row increments, RGB rows at `+0x4C8`, zero size
at `+0x4EC`, and `0x4F0` record increments. It separately initializes the
companion storage beginning at `+0xED0`, with progress at record `+0x240`
and `0x3A8` strides. This helper consumes only the first three companions;
the existing panel view of later overlapping context storage is preserved.

## Recovered local layout

| Storage | Offset / extent |
| --- | --- |
| Three meshes | `0..0xED0`, stride `0x4F0` |
| Mesh vertices | `9 x 17` SVECTORs, `0..0x4C8` |
| Mesh RGB rows / signed size | `0x4C8..0x4EC` / `0x4EC` |
| First three companion records | `0xED0..0x19C8`, stride `0x3A8` |
| Companion progress | record `+0x240` |
| GT4 packet | `0x3450..0x3484` |
| Three word-coordinate origins | `0x3690..0x36C0`, stride 16 |
| Frame / step | `0x373C` / `0x3748` |
| Shared size / intensity / phase | `0x3770` / `0x3780` / `0x37C4` |
| Recovered context extent | `0x37C8` |

Twenty-four target-compiled constants cover these layouts and the SDK packet
coordinate offsets. Repeated inclusion of the mesh, panel and curtain
headers in both orders is checked.

## Behavior and ownership

Each mesh uses zero rotation, its own word-coordinate origin and uniform
size, with an even-frame pulse of shared size divided by 32. Phase 5 or later
scales each RGB row by signed intensity divided by 1024. Eight row pairs and
sixteen columns produce 128 candidate quads per mesh.

Projection still occurs before checking companion progress. Submission
requires progress at least 1024, nonnegative depth and nonnegative GTE flag.
The draw-mode packet helper receives the depth narrowed to `u16` and flag 1.
Its existing `u32 *` contract is used for the initialized, word-aligned GT4.

An eligible mesh grows by `step * 256` to 4096 while phase is below 5; later it
grows by `step * 64` to 16384. Shared size grows by `step * 256` to 4096 from phase 3.
During phase 5, positive intensity becomes
`1024 - (first_mesh_size - 4096) / 12`; nonpositive results clamp to zero
and advance phase 6.

The regression suite verifies every input/final code and raw-data owner in
both images, selected compiler symbol sizes and executable sections, full
image bytes, the existing resident callee/context owners, and each selected
relocation. This helper has seven calls to seven resident addresses and
three local jumps: `+0xA8 -> +0xBC`, `+0x21C -> +0x270`, and
`+0x464 -> +0x4AC`. Relocating the compiler object reproduces the entire
retail helper, rather than relying on a complete-image hash alone.
