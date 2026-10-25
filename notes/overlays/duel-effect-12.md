# Complete Spanish duel effect 12

## Canonical Stop Defense burst

`DuelEffect_RunStopDefenseBurst` names the complete native `func_80150E00`
owner: PAL `80150E00`, North American `801519D8`, Japanese `8015EE94`,
1,048 instruction bytes. The actual resident `DuelEffect_ApplyStopDefense`
allocates request `0xC` for each of five ground positions in sequence and
tracks its request. When the handler raises `field_1D`, the resident caller
tests the corresponding occupied defense-position card, starts its alpha
transition and clears its defense flag. The caller then waits for the request's
active flag to clear before advancing. The overlay renders the burst and
signals progress/completion; it does not own card eligibility or position rules.

The complete handler and dispatcher use an extern-only semantic header.
`gDuelEffect_StopDefenseBurstConfig` binds the one 16-byte native
`D_8015AEE4` owner (NA `8015AFB0`, JP `80168F70`).
`gDuelEffect_StopDefenseBurstInitialScale` binds separate native
`D_80146188` (NA `801461D0`, JP `801541E0`), retaining its 16-byte SDK
VECTOR and padding. These remain two real generated objects, not new
definitions, absolute data aliases or an invented variant table.

Every nonnegative phase still selects that same record. Preserve all three
32-point rings and 32 particle position/velocity slots, captured frame step,
signed-halfword ring-height contract, two differently scaled ring passes,
half-color copy and the final three scale assignments without adding an
unobserved matrix update before the sprite. Keep the `0x4000` threshold,
progress store before fade-by-15, separate black-color completion, frame
advance and matrix restoration. Private layouts, profiles, maps and gameplay
callers remain unchanged; no exclusive artwork or new #418 runtime evidence
is claimed.

The dispatcher-selected effect-12 routine `func_80150E00` at
`80150E00..80151218` is now a complete 1,048-byte C translation unit using
`gcc_2_8_1_g0_split`. Recovery used the Spanish bank instructions and established
local declarations. The first compiled candidate matched every instruction
word; an independent complete-bank link then reproduced all 90,112 bytes and
SHA-256 `a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.

Together with effect 8, this independent batch adds 2,728 C bytes while retaining
all 65 accepted Spanish manifest entries unchanged. It reaches 67/85 C
functions, 29,876 C bytes and 18 explicit assembly boundaries. Pending effects
2 and 21 in #6569 are not included or stacked into this batch.

## Storage and behavior

The work view is `0x518` bytes:

| Offset | Field |
| --- | --- |
| `000` | Configuration pointer |
| `004` | Three rings of 32 vectors |
| `304` / `404` | 32 particle positions / velocities |
| `504` | Origin vector |
| `50C` / `510` | Unsigned scale / frame counter |
| `514` | Color |

The single 16-byte configuration at `8015AEE4` contains the color, three
halfword radii at `+4`, ring-height step at `+A`, sprite size at `+C` and
particle speed at `+E`. The 16-byte initial scale remains at `80146188`.
These are independently sized real generated data owners, not linker aliases.
Every nonnegative phase selects the same record; no variant table or artificial
six-phase limit is introduced.

Initialization creates three fixed 32-point rings and 32 particles. The signed
halfword height argument independently corroborates effect 8's correction to
the shared radial-generator contract. Rendering preserves the captured resident
matrix, frame-step override, particle movement, half-color copy and two ring
passes. The final three half-scale assignments before the sprite call are
retained without inventing a third matrix-update call absent from the target.

Scale grows by `0x1000` until `0x4000`; later frames mark the canonical request's
`field_1D` and fade by 15. The color-completion predicate separately raises the
resident completion byte. The existing request type and matrix getter remain
the declaration owners.

Candidate snapshots, profile fingerprints and the whole-bank receipt remain
under local `tmp/`. Production acceptance additionally checks target-GCC field
offsets, real data bytes/owners and executable function extents, rather than
treating relocation-masked instruction equality as the final gate.

After incorporating accepted French work through `9b68de492`, both new effects
pass production Spanish matching, duplicate-copy and metadata checks, target-GCC
layout/data/callee ownership checks and the accepted dispatcher/texture ownership
gate. French, English PAL, Japanese and North American overlay images also remain
exact. All 109 duel-focused tests pass.
