# French MODEL446 entry

The loader entries `func_8013B004` / `func_8017B004`, at image offset
`0x4`, own 3,580 bytes each in MODEL146 stages 9/10. Both now use pure C
with `gcc_2_8_1_g0_split` (GCC 2.8.1 and MASPSX 2.81). The six accepted
helper bodies and their ownership are unchanged. This is French family
446, not French module 446 or the North American family with that number.

## Context and initialization

All 895 native instruction words supplied the entry's layout, arithmetic,
call order and store order. Existing local French and Spanish helper
views corroborate the geometry records and SDK/game declarations.
The recovered context extends through `0x1BE8`; this is an accessed view,
not an allocation-capacity claim. Entry-only views remain local.

| Offset | Initialized family |
|---|---|
| `0x0000` | Sixteen `0xB8` rays |
| `0x0B80` | Three `0x1A0` two-layer line spheres |
| `0x1060` | One `0xD0` strip |
| `0x1130` | Two `0x90` pulses |
| `0x1510` | Four `0x118` seventeen-point rings |

Packet templates occupy `0x1970..0x1AD0`; the line packet is at `0x1B08`,
two matrices at `0x1B1C` / `0x1B3C`, target at `0x1B5C`, direction at
`0x1B64`, screen deltas at `0x1B74`, camera vector at `0x1B78`, unsigned
elapsed time at `0x1B94`, unsigned step at `0x1B9C`, signed fade at
`0x1BA0`, configuration pointer at `0x1BA4`, part index at `0x1BB8`,
phase at `0x1BDC`, and signed slot/command halfwords at `0x1BE4`/`0x1BE6`.
The three-word initializer containers at line offset `0x184` and pulse
offset `0x80` leave their fourth words untouched; their broader roles
are not inferred from these zero stores.

Configuration records have stride `0x4C` at image data plus `0xFC`,
with three part bytes at `4..6`, unsigned part count at `0xC`,
radii at `0x14`/`0x18`, signed pulse extent at `0x28` and unsigned
timings at `0x2C..0x44`. The first two part coordinates supply two matrices.
The initial target is `(0,-300,-450)` in slot 0 and `(0,-300,450)` otherwise.

The first page/palette queries use `(1,1,896,0)` and `(640,244)`;
the unused second query pair remains. Five uploads use data offsets
`0xC4`, `0x8C`, `0x1C`, `0x38`, `0x54`. The first result supplies the
flat pair and the third supplies the ring packet. The last call's real
return overwrites the packed-result local, separating the earlier
page/palette snapshots and recovering native register allocation.
The extra GT4 receives its tag and flags but no invented UV/page writes.

Ray endpoint 2 is black; the other two endpoints have magenta outer and
gray inner colors. The line spheres use radii 256/192, three azimuths
0/300/600, four latitude rows and six longitudes. Each pulse corner loads
one actual extent scalar before its stores. Rings use configuration radii,
seventeen points and outer z=-64; the final ring starts at zero while
the other three have negative initial progress.

## Update and reachability

Camera direction is view reference minus viewpoint. The opposite-slot
target, both matrices, direction and projected deltas update each frame.
The projected unsigned x is captured before clearing the stored part index;
the signed y delta follows the x store. An explicit nonzero-count gate
precedes the stored-index do/while loop.

Only indices 0..2 select a part matrix. Higher indices deliberately retain
the previous matrix while continuing direction and helper processing.
Per part, pulses run at `draw_start <= elapsed`, rays at
`rays_start <= elapsed`, and ribbons then strip when phase is above 0.
After the loop, rings run at `draw_start <= elapsed`, with lines additionally
requiring phase at most 0. All six helpers are therefore entry-reachable.

Animation changes advance elapsed time and cached step with separate
frame-step calls. Phases 2..5 return 4; phase 6 returns 1 and enters 7;
phase 7 adds `unsigned_step >> 1` only while signed fade is below 64,
then resets fade to zero and enters 8 when the result reaches 64.
Phase 8 returns 2. This entry does not submit ambient state.

## Exact recovery

The [attempt ledger](french-model-variant446-entry-attempts.csv) records
all nine measured source experiments and both production terminals.
Actual radius scalars, counter ownership, conditional point indices,
extent snapshots, texture-result lifetimes and the native projection/count
sequence recover the 240-byte frame and complete entry. No inline assembly,
dummy accesses, fabricated arithmetic dependencies or new compiler profiles
were introduced.

Both full 20,480-byte images match retail with original-sized C `STT_FUNC`
owners and all six helper owners preserved:

| Image | SHA-256 |
|---|---|
| MODEL146 stage 9 slot 0 | `b18a7100da6117a0b01350bfbb4b9d5742e3ce619e6e38ae0a19f54bbe2223bd` |
| MODEL146 stage 10 slot 1 | `d027809b51bbb68c09fd741c882fafda9b6f03a02243e0cffdb775cf7b9db9bf` |

This adds 7,160 matching C instruction bytes across both images and completes
all seven functions in each. Spanish registrations are unchanged.
