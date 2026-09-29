# Complete Spanish breakup effect

`func_80146760`, `80146760..80147B18`, adds a complete 5,048-byte lifecycle
under `gcc_2_8_1_g0_split`. The source, private header, canonical image-input
header and six paired dispatcher/helper files are reused verbatim from
contributor commit `f325e3ed3c35aeb7663f0bc68caf3c1f7b8f8723`.
No pending history or French function registration is imported.

The independent candidate reproduced the actual Spanish 90,112-byte bank,
SHA-256 `a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`,
before promotion. All 65 accepted Spanish entries remain unchanged.
The resulting bank has 66/85 C functions / 32,196 bytes and 19 explicit
assembly boundaries. Configured overlays total 190/209 C instances /
88,048 bytes; other pending Spanish batches are excluded.

After additive integration of accepted effects 8/12 from `3d43de926`,
all 67 accepted entries remain unchanged. Final branch totals are
68/85 C functions / 34,924 bytes, 17 bank assembly boundaries, and
192/209 configured C instances / 90,776 bytes. The shared lifecycle,
union and paired contracts remain verbatim.

## Real overlapping data ownership

The dispatcher reads 21 mode halfwords at `8015A430`. Effect 11 reads five
28-byte `GsIMAGE` records at `8015A458`. Spanish retail bytes independently
confirm that mode 20 overlaps image zero's `pmode` low halfword, value nine.
The single 180-byte `DuelEffectImageInputs` union exposes both views:
21 halfwords at zero, five images at offset 40. There is no separate
`D_8015A458` alias and no reduction of the dispatcher's 21 iterations.

Both French and Spanish maps describe this same complete generated owner
as `0xB4` bytes. The shared dispatcher uses `.modes`; the new lifecycle
retains a pointer to its chosen `.variant.images` element.
The dispatcher's complete object text, function extent and relocations
are identical to accepted master.

| Generated owner | Bytes | Role |
| --- | ---: | --- |
| `D_80146004` | 16 | Initial scale |
| `D_8015A430` | 180 | Overlapping mode/image input |
| `D_8015A4E4` | 48 | Single configuration |
| `D_8015B748` | 84 | Existing texture output pairs |
| `D_8015B7F8` | 8 | Existing dispatcher-provided origin |

## Layout, callers and lifecycle

Target GCC preserves the `0x1F28` work view and 48-byte configuration.
The work contains 28 grid pieces, 16 fragments per piece, 32 rays,
64 dust positions/velocities and three 32-vector rings. Nonnegative phase
uses its low four bits for five image variants; larger variants select
the crossed-line fallback. The separate `(phase >> 7) == 1` mode test
is not normalized to a generic boolean.

Actual Spanish configuration values include fragment size eight, radial
speed four, lift 16, initial random denominator eight, spread 16, 32 rays
and 32 dust particles. The unused halfword at `+0xA` is 360; no duration
meaning is claimed. The glow is `(128,96,32)`, and ring radii are 8/16/32.

Branch-local gravity increments, the zero dust-gravity read, signed rotation
remainders, carried ring scale and final unread unsigned fragment-size wrap
are intentional. A failed random choice reduces its denominator, but at
one the remainder is necessarily zero and starts fragmentation. Completion
requires all 28 pieces and zero glow rather than a guessed timeout.

Caller-backed helper refinements pair signed ring height and unsigned ray
width declarations with their definitions. Both complete grouped objects,
including relocations and function extents, remain identical.
The actual Spanish function has three calls to `80089928`, the canonical
372-byte `ratan2` SDK boundary, and calls the accepted 12-byte resident
`Model_GetLightSourceMatrix` at `8005C328`. No SDK code is promoted.

Production acceptance checks the actual Spanish archive and all seven
terrain copies, complete C objects and unique generated input/final data
owners, not a hash-equal foreign archive substituted for Spanish inputs.
Other regional image checks guard the shared dispatcher/helper refinements.
No profile or hash gate changes, inline assembly or retail/generated uploads
are involved. Boot ownership, MODEL/SU loads and overworld-tail coverage
remain unresolved; configured totals do not imply exhaustive completion.

## Accepted effect 9 integration

Additive rebase onto accepted `308ad56d2` preserves all 68 accepted Spanish
entries, effect 9's signed-number behavior, real owners and tests.
The previously reviewed lifecycle source/header files remain byte-identical.
The combined branch has 69/85 bank C functions / 36,720 bytes,
16 explicit assembly boundaries and 193/209 configured C
instances / 92,572 bytes. Complete production Spanish images,
linked ownership, metadata and duel regressions are checked on this head.
