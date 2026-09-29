# Complete Spanish vortex effect

`func_80148BA4`, `80148BA4..80149F90`, adds 5,100 exact C bytes under
`gcc_2_8_1_g0_split`. The source/private header and paired brightness
helper files are reused verbatim from contributor commit
`c68c2410fda3d6730c61ff7876a47ad9a7301367`, without pending history or
French registrations. The dispatcher includes the canonical private header
rather than repeating the lifecycle prototype.

Independent preflight reproduces the actual Spanish 90,112-byte bank,
SHA-256 `a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
All 67 accepted Spanish entries, including effects 8/12, remain unchanged.
The resulting bank has 68/85 C functions / 34,976 bytes, 17 explicit
assembly boundaries and 192/209 configured C instances / 90,828 bytes.
Other pending matching batches are excluded.

## Work, configuration and real owners

The target work view is `0x2020` bytes, not an assumed 8 KiB allocation.
It contains 20 card vectors/rotation increments, 20-by-16 burst vectors,
64 inward particles/velocities and 48 eight-vector paths. The configuration
size is 16 bytes; the canonical `DisplayObject` and `POLY_GT4` sizes are
112 and 52 bytes.

Actual Spanish records independently confirm:

| Field | Variant 0 | Variant 1 |
| --- | --- | --- |
| Primary RGB | 96,128,96 | 128,64,64 |
| Burst RGB | 192,255,192 | 255,160,160 |
| Radius / spin / height | 160 / 64 / 64 | 80 / 96 / 96 |
| Fan spin / mode | 8 / 0 | 16 / 1 |

Seven complete generated owners remain real section-defined data:
`D_80146034/16`, `D_8015A60C/32`, `D_8015B7A0/84`, `D_8015B748/84`,
`D_8015B7F4/4`, `D_8015B7F8/8` and `D_8015B800/2`.
The collector output has 21 words including its terminator. Mode zero uses
selector -1 for up to 20 field objects; mode one uses selector zero for up
to five selected-row objects. Neither is truncated to a guessed shared bound.

Spanish instruction calls and the resident inventory independently agree on
`Duel_CollectMatchingFieldCardObjects = 8002CB88`,
`Model_GetLightSourceMatrix = 8005C328` and three `ratan2` calls at
`80089928`. The first two remain resident C; `ratan2` remains SDK assembly.
The complete 836-byte curve helper at `8014FABC` remains generated assembly.

## Preserved contracts and lifetimes

This caller loads halfword brightness. Both the shared declaration and
definition accept `u16`, while explicitly truncating each packed channel to
`u8`. The entire accepted color-transition object, including function extents
and relocations, remains unchanged. The dispatcher object also stays exact.

Only a nonempty collection activates inward particles, guarding the
last-card lookup. Burst color becomes nonzero only after a real card
completes, guarding `card_particles[completed-1]`. Each card moves 17 times
before its visibility flag is cleared. Paths retain state when restarted.
Red-as-blue swirl color, branch order, unsigned random doubling followed by
signed conversion, halfword scale wrapping and per-layer transitions remain
target behavior, not simplified approximations.

Acceptance uses actual Spanish archive/copy proofs, complete code extents,
unique input/final data owners and target-compiled layout constants.
Regional overlay image checks cover the shared contract change. No new
compiler profile, inline assembly, weakened hash or retail upload is used.
Boot, MODEL/SU loads and the overworld tail remain open scope; configured
counts do not claim exhaustive runtime completion.

## Accepted effect 9 integration

Additive rebase onto accepted `308ad56d2` preserves all 68 accepted Spanish
entries, effect 9's signed-number behavior, real owners and tests.
The previously reviewed lifecycle source/header files remain byte-identical.
The combined branch has 69/85 bank C functions / 36,772 bytes,
16 explicit assembly boundaries and 193/209 configured C
instances / 92,624 bytes. Complete production Spanish images,
linked ownership, metadata and duel regressions are checked on this head.
