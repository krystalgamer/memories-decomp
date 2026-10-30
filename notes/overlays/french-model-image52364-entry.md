# French MODEL52364 / MODEL52374 entry

Refs #6460. Both game-owned `0x2300`-byte entries are matching pure C under
`gcc_2_8_1_g0_split` (GCC 2.8.1 and MASPSX 2.81). The second slot uses a
preprocessor wrapper around the same body, with its own image-tail symbols.
Each normal production build reproduces its complete original 20,480-byte image.
The four-byte header and raw suffix beginning at image offset `0x2304` are
unchanged.

| Sector | Entry | Complete image SHA-256 |
|---|---|---|
| 52364 | `0x8013B004` | `d0ec5280973304ce9ea60447cd0eb8001ec161af4b3a308c756124d09a48824a` |
| 52374 | `0x8017B004` | `6f3d7413891497c999aacca93d1267bf169c7496a3ca40fc37b133be34068dc9` |

Archive: `game/france/DATA/MODEL.MRG`, SHA-256
`0c3f90cf4a3b4776d1188054a6cd5d89c5bc364215dac15c4730ef74edbc8da3`.
Each entry retains the native 1,760-byte frame and all 118 resident calls.

## Family and local structural evidence

The two French entry bodies differ in exactly 16 words: three LUI producers
for image-tail data and 13 entry-bounded direct jumps, relocated by `0x40000`.
The other 2,224 words and all resident calls are identical. This proves body
equivalence, not identity of the raw suffixes. Header values 786 and 883 are
locators, not family identifiers.

The accepted local `src/overlays/model_variant/variant783_entry.c` was located
independently through the three 128-SVECTOR walks at state offsets 20, 1,044
and 2,068, the three entry memsets, the frame, and the ordered calls.
Its USA image is `model_variant_400_stage7_slot0`, sector 96780. Of the 118
ordered calls, 117 have identical authoritative resident names. The remaining
French OT getter is explicitly mapped by
`config/sles_03948/link_symbols.ld` from canonical `func_80058F10` to
`0x8005C018`; its `GsOT *` declaration is in `src/psyq/sdk_internal.h`.

The local source supplied structure only. French layouts and behavioral
differences were recovered from the complete native entry before adaptation.
No external reference types or compiler claims were used.

## Recovered regional differences

The French command stride is 60 bytes, not the local USA command's 64.
Wing RGB occupies bytes 0..2, ring RGB 3..5, the main slot is byte 6, and the
independently sampled ring slot is byte 7. Half-size/height are at 8/10,
particle count/radius/speed at 12/14/16, rise speed at 18, travel divisor at 20,
and wing count at 22. Geometry fields continue through spray count at 42.
Start/release/launch are signed halfwords at 44/46/48; burst time is a word at
52. The trailing word at 56 is not accessed by this entry and retains an
address-based unknown field name; its purpose is not claimed.

The constructor starts below the slot. Between release and launch, it captures
the slot position once and raises the relative position toward zero. At launch,
it computes travel velocity toward the signed 450-unit endpoint; arrival uses
the absolute signed-short z coordinate. The apparently repeated initialization
of `particles[0]` in the wing-count loop is present in the native instructions.

There is no retained centre vector or centre-set flag. Spray screen coordinates
begin at 3,860; textures are at 3,988/3,992; ambient/elapsed/growth at
3,996/4,000/4,004; state/shown at 4,008/4,010; spray depths at 4,012. One fade
byte at 4,076 replaces three separate color channels, followed by flash,
burst intensity and ring RGB through 4,081.

Main quads use texture 0 and the scalar fade for all RGB channels. Fade grows
by the frame step below 122 and otherwise becomes 128. Flash decays by three
steps; burst/fade by one; ring RGB by two. The ring uses texture 1, UV bounds
0..127 and 64..191, and samples the second command slot every frame rather
than retaining and advancing a centre. Ring submission tests depth only.
Completion outside states 3/4 tests the scalar fade, not flash intensity.

## Attempts and acceptance

`french-model-image52364-entry-attempts.csv` records all six source experiments
for both images and both normal production proofs: 14 rows. Every experiment
passed 49 independently recovered target layout assertions. All 4,485 original
inputs were frozen before the two native baselines; only the seven intended
metadata files subsequently changed.

The first candidate was four bytes short, with 1,493 differing aligned words
per image. Calling out-of-line `abs` was rejected by both ordinary links:
the named profile disables builtins, and the native entry has no such call.
The actual Psy-Q `ABS` expression restored the exact extent, leaving 33 words.
Using the original z velocity directly in the launch subtraction removed eight
store-scheduling differences. Moving only the ring cursor canonicalized to the
same remaining 25 words. Restoring both natural walker initializations before
the live-slot lookup matched both full images exactly.

No failed link was forced, no raw suffix was moved, and binary diagnostics were
deferred until both ordinary full-image links completed. Final acceptance used
the tracked manifests and normal `overlay_build.py build` and `verify` for
both images, with one original-sized C owner in each ELF. No generated output,
retail data, assembly tricks, pinned registers or fabricated accesses are
included in the change.
