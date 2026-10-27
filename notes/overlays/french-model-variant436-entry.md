# French MODEL436 entry

The loader entry `func_8013B004` / `func_8017B004`, image offset `0x4`,
owns 3,476 bytes in MODEL707 stages 9/10. It now uses pure C with the
existing `gcc_2_8_1_g0_split` profile (GCC 2.8.1 and MASPSX 2.81).
The four earlier helper bodies and their ownership remain unchanged.

## Recovered context and initialization

The local MODEL437 entry research supplied structural comparisons only.
All 869 MODEL436 instructions were read independently; the accepted
MODEL436 sheet and streamer views corroborate their layouts.
The entry view establishes an observed extent of `0x1650`, not allocation
capacity. Opaque gaps remain uninterpreted.

Four `0x200` ribbons occupy `0x000..0x800`; one `0x88` sheet follows.
One `0xB4` three-point band begins at `0x888`, followed by three
`0x274` streamers at `0x93C`. Packet templates begin at `0x13B0` (G3),
`0x13CC` (G4), `0x13F0` (two GT4s), `0x1458` and `0x14A8`
(two FT4s each). The matrix is at `0x1508`, target at `0x1538`,
signed clock at `0x15E0`, configuration pointer at `0x15F4`,
three part pointers at `0x15FC`, phase at `0x163C`, tint at `0x1648`,
and signed slot/command halfwords at `0x164C`/`0x164E`.

Configuration records have stride `0x30`, with part bytes at 4/5/6,
signed start at `0x0C`, and signed scale interval at `0x10`/`0x14`.
Their base is the image data plus `0xFC`. The parts are `GsCOORDUNIT`
pointers and all matrix queries call `GsGetLwUnit`, not `GsGetLw`.
Initialization returns 6 and targets `(0,-300,-450)` in slot 0 or
`(0,-300,450)` in slot 1.

Both page/clut queries, including the unused second pair, are preserved.
The page uses `(1,1,896,0)` and palette `(640,244)`. Six texture uploads
use data offsets `0xC4`, `0x8C`, `0x1C`, `0x38`, `0x54`, `0xE0`.
The second GT4 runs from U64 to U127, unlike the related MODEL437 probe.
Bands and the sheet have white outer and `(0,64,255)` inner colors;
their progress/corners use 128-unit steps. Ribbons start with alternating
signed ranges, and streamer colors are `(128,128,128)`.
The native unreachable second sheet-plane initializer is retained.

## Update and reachability

Camera-view angles and projected target deltas update each frame.
The moving origin lowers Y by signed `lift/4`. The two-part direction
updates only below phase 2; later phases query the root part instead.
Once the signed start deadline is reached, ribbons run below phase 4.
Streamers and sheets additionally require the strict scale-start deadline.
The band is initialized but never called by this entry; its accepted,
non-entry-reachable body is not changed.

The animation clock advances only when the slot animation frame changes.
Ambient tint decays by 128 below phase 11; later it uses `fade << 5`.
Phases 5/6 return 4; phase 7 returns 1 and enters 11; phase 11 advances
fade to 64 and enters 12; phase 12 returns 2.

## Exact recovery and integration

The measured 224-byte frame includes two unused matrix locals.
A sheet-local index with a shared band/streamer index preserves native
allocation without dummy operations or artificial dependencies.
Writing band colors before indexed progress restores the final three
hoisted constant positions while preserving the native store schedule.
The [attempt ledger](french-model-variant436-entry-attempts.csv) records
all ten measured experiments and both production terminals.

Both complete 20,480-byte French images rebuild byte-for-byte, with
3,476-byte `STT_FUNC` owners at their original entry addresses:

| Image | SHA-256 |
|---|---|
| MODEL707 stage 9 slot 0 | `e27d2ab6352960331b9f355d23ec29d545bd5bf68e6bc032baef66f6de382ce1` |
| MODEL707 stage 10 slot 1 | `51886844345e30aa42c63f6708a1afb850d6d31b140ac1c4a6da8385502aefa1` |

This adds 6,952 C instruction bytes across both images and completes all
five functions in each. Spanish registrations remain unchanged; sharing
some helper bodies does not by itself establish an exact Spanish entry.
