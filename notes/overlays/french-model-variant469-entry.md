# French MODEL469 loader entries

The header-469 family used by French physical MODEL129 and MODEL582, in
stage 9/slot 0 and stage 10/slot 1, now has matching C for its complete
six-function inventory. All four complete 20,480-byte images reproduce their
immutable retail inputs. This is part of #6460.

The new entry is `8013B004..8013C560` in slot 0 and
`8017B004..8017C560` in slot 1: 5,468 instruction bytes, 1,367 words and a
240-byte stack frame. The two physical models have identical same-slot entry
bytes. One C implementation and the existing slot-wrapper convention cover
both physical pairs, using the named `gcc_2_8_1_g0_split` profile.

## Evidence and retained ownership

All 1,367 native entry words were read before reconstruction. Declarations
come from the measured native accesses, existing local component headers
and authoritative SDK/game headers, not reference-project declarations.
The first entry recovery has thirty measured, fingerprinted source/header
or named-profile experiments in
`french-model-variant469-entry-attempts.csv`, followed by four production
full-image terminals.

Each image retains these five existing helper owners, profiles and sizes:

| Slot-0 address | Bytes | Existing C owner |
|---|---:|---|
| `8013C560` | `638` | French `variant582_arcs.c` |
| `8013CB98` | `65C` | French `variant582_sheets.c` |
| `8013D1F4` | `93C` | French `variant469_ribbons.c` |
| `8013DB30` | `538` | Spanish `variant469_sheets.c` |
| `8013E068` | `454` | Spanish `variant469_rings.c` |

Slot-1 addresses use the corresponding `8017` prefix. Production ELFs
independently confirm the original-sized entry and all five original-sized
helper functions in each image; their existing manifest entries are unchanged.

The entry directly calls the ring, arcs and five-sheet helpers, in that order,
behind the native elapsed/phase gates. It does not directly call the ribbon
or retained-sheet helper. Initializing their storage does not establish a
direct call edge; no global dead-code conclusion is drawn.

## Measured constructor view

The local context view has a minimum observed extent of `39EC`, not a claim
about allocation capacity. Configuration records have a `40`-byte stride at
the image suffix plus `FC`. Three explicit part lookups are retained, but
only the first part's matrix is refreshed.

| Context offset | Measured storage |
|---|---|
| `0000` | Four `D0`-byte, two-by-five-point arc groups |
| `0340` | Five `174`-byte, three-row, thirteen-point sheets |
| `0A84` | Five `250`-byte ribbon records |
| `1614` | Extended `5B4`-byte, twenty-four-quad ring |
| `1BC8` | Sixteen `A8`-byte trail records |
| `2648` | One `A0`-byte two-color-pair band |
| `26E8` | Ten `9C`-byte quadrant sheets |
| `2D00` | Six `90`-byte eight-point rings |
| `3060` | Four `90`-byte eight-point rings |
| `3330` | Triangle, quad, mask and textured packet area |
| `34C0` | First-part matrix |
| `3508` | Five orbit positions and three five-vector path arrays |
| `366C` | Twenty-four delayed ring positions and directions |
| `3984` | Frame, timing, configuration and controller state |

The existing ring renderer's `434`-byte prefix is not the initialized ring's
stride: the constructor initializes four additional twenty-four-word arrays
through `5B4`. No following component is placed using the partial prefix's
`sizeof`. The extra GT4 at `33F0`, GsGLINE at `349C`, packet holes and other
unwritten storage remain untouched by the entry.

Native details retained include the arc delay decrement of **381**, unsigned
trigonometric shifts, negative-numerator `/24` ring timing, signed projection
Y values, an unsigned matrix-refresh deadline and delayed per-ring position
capture. The five-point paths preserve their separate target, origin and
direction arrays. The opposite-slot copy has no target-Y clear.

Three expression/lifetime distinctions were necessary for exact code:

- Natural signed remainder parity (`n % 2 == 1`) reproduces the native
  quotient comparison without introducing an extra ribbon-delay induction.
- The trail progress is its own actual `0, -256, ...` working induction,
  separate from the negative color seed and color decrement.
- The raw `4096` reset value is produced full-width in the last four-ring
  initializer block and narrowed only at the subsequent `39CC` store.
  The native producer is `8013BE74` and consumer `8013BEAC`. Moving its
  definition outside that block changes scheduling. Its value is a literal,
  not derived from a completed loop index.

No inline assembly, forced registers, source-local externs, volatile tricks,
fabricated memory dependencies or index-derived reset anchors are used.

## Complete production image identities

| Physical model / stage / slot | SHA-256 |
|---|---|
| 129 / 9 / 0 | `d95eb39f7d8ec7e211e7e5d733f47940398dfb833de3d20f64b544b71b59fd7d` |
| 129 / 10 / 1 | `099d2da46e36044e8d38180072ab257388ca67ae0afb11c5da854fe5da1671f9` |
| 582 / 9 / 0 | `6132878d19bfbc59dc3250faf71a5c1232d8ed33d9e952f7b2edb605453289c5` |
| 582 / 10 / 1 | `bd5af8998e6db72de975ec0b53d0c4562a70f4193604e826244074643dfed648` |
