# French MODEL437 entry

The loader entry `func_8013B004` / `func_8017B004`, image offset `0x4`,
owns 3,400 bytes in MODEL103 stages 9/10. Both entries now use pure C
with the existing `gcc_2_8_1_g0_split` profile (GCC 2.8.1 and MASPSX 2.81).
The four accepted helper bodies and their ownership remain unchanged.

## Recovered context and initialization

The complete 850-instruction native entry establishes the offsets,
arithmetic and call order. Existing local MODEL402 API/projection structure,
the accepted Spanish MODEL437 sheet, and the shared MODEL437 streamer
layout supplied independently corroborated declarations. The observed
context extent is `0x1650`, not allocation capacity.

Four `0x200` ribbons occupy `0x000..0x800`; one `0x88` sheet follows.
One `0xB4` three-point band begins at `0x888`, followed by three
`0x274` streamers at `0x93C`. Packet templates begin at `0x13B0` (G3),
`0x13CC` (G4), `0x13F0` (two GT4s), `0x1458` and `0x14A8`
(two FT4s each). The matrix is at `0x1508`, target at `0x1538`,
unsigned clock at `0x15E0`, configuration pointer at `0x15F4`,
three part pointers at `0x15FC`, phase at `0x163C`, tint at `0x1648`,
and signed slot/command halfwords at `0x164C`/`0x164E`.

Configuration records have stride `0x30`, with part bytes at 4/5/6,
unsigned start at `0x0C`, and unsigned scale interval at `0x10`/`0x14`.
Their base is the image data plus `0xFC`. The parts are `GsCOORDUNIT`
pointers; the actual native matrix target is `GsGetLwUnit`. An earlier
unmatched scratch linker alias labeled that address `GsGetLw`; production
uses the authoritative SDK declaration instead.

Initialization returns 6 and targets `(0,-300,-450)` in slot 0 or
`(0,-300,450)` in slot 1. Both page/clut queries, including the unused
second pair, remain. The first page uses `(1,2,896,0)` and palette
`(640,244)`. Six uploads consume data offsets
`0xC4`, `0x8C`, `0x1C`, `0x38`, `0x54`, `0xE0`.
The second GT4 mirrors U127 to U64. The band has white outer and
`(0,64,255)` inner colors; the sheet instead has gray `(8,8,8)` inner
colors. The native unreachable second sheet-plane initializer is retained.

## Update and reachability

Camera-view angles and projected target deltas update each frame.
The origin copies the root translation without MODEL436's `lift/4` drop.
The two-part direction updates below phase 3; later phases query the
root part instead. Streamers and sheet are directly called, in that order,
only when unsigned `start <= elapsed` and `scale_start < elapsed`.
This entry initializes but never calls the bands or ribbons. That is a
local reachability observation, not a claim about all callers.

The animation clock advances only when the slot animation frame changes,
using separate frame-step calls for elapsed time and cached step.
Ambient tint decays by 128 below phase 11; later it uses `fade << 5`.
Phases 5/6 return 4; phase 7 returns 1 and enters 11; phase 11 advances
unsigned fade, clamps above 64 and enters 12; phase 12 returns 2.

## Exact recovery and integration

The measured 224-byte frame includes two unused matrix locals.
The initial near-match already reproduced the entire packet prefix and
update/return path. Applying the independently verified MODEL436 index
lifetimes and band color/progress store order recovered the remaining
initialization allocation and constant setup. No dummy operations,
artificial dependencies, inline assembly or new profiles were introduced.
The [attempt ledger](french-model-variant437-entry-attempts.csv) records
the eight original experiments, one SDK include-order compile failure,
the exact recovery and both production terminals.

Both complete 20,480-byte French images rebuild byte-for-byte, with
3,400-byte `STT_FUNC` owners at the original entry addresses:

| Image | SHA-256 |
|---|---|
| MODEL103 stage 9 slot 0 | `afa3cd8a5cc90ef290867a3185dafc5a5beb6c7468dacb3fc3744de7a52daa31` |
| MODEL103 stage 10 slot 1 | `5c57f637b21a637c274d019e6b9056c4c238a0e40ce726ccb161c1e25e1873b7` |

This adds 6,800 C instruction bytes across both images and completes all
five functions in each. Spanish registrations remain unchanged.
