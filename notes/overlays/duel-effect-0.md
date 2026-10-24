# Spanish duel-bank effect 0

`func_80154688` (`80154688..80154B30`, 1,192 bytes) is complete matching C
under `gcc_2_8_1_g0_split`. The numeric identity comes from dispatcher case
zero, without assigning an exclusive card or runtime artwork identity.

Phases zero through 29 select 30 configuration records and initialize three
rings, two four-vertex polygons, 16 randomized rotations, scale and color.
Phase 30 or greater starts the distinct crossed-line path. Negative phase
either advances that 180-frame path or runs the configuration-driven draw,
delay, expansion and fade lifecycle. The routine preserves the saved frame
step, both matrix copies, the halfword color predicates and completion paths.
Scale increases only below `0x7000`; no new unconditional clamp is introduced.

## Shared contour-burst family and semantic bindings

`DuelEffect_RunContourBurst` names the complete native `func_80154688`
owner from its actual rendering sequence: a quad contour, configured delay,
expanding gradient ring, layered tile and 16 rotating gradient strips,
followed by fade completion. The source has no particle array; effect 18
adds that distinct path. Both complete objects remain separate.
`duel_draw_resolution.c` creates effect 0 with phase 9 during Exodia
presentation, so this must not be called an ordinary-spell-only animation
or have its 30 configurations reduced to that single observed caller value.
This is static code evidence, not a new human artwork observation.

The actual NA function entry is `80155104`, JP is `8016271C`, and PAL
keeps `80154688`; every complete owner is `4A8` bytes.
`gDuelEffect_ContourBurstConfigs[30]` binds the existing 600-byte
`D_8015B0B4` owner (NA `8015B180`, JP `80169140`, PAL `8015B0B4`).
`gDuelEffect_ContourBurstInitialScale` binds the existing 16-byte SDK
VECTOR `D_801461E8` (NA `80146230`, JP `80154240`, PAL `801461E8`).
Its role is the entry copy to local scale, not a new padding or
identity-value contract. The semantic header reuses the original private
types and retains the original native function compatibility declaration.

Preserve all 30 records, the unaligned color, delay/request-field handshake,
full matrix copies, SDK rotation-store order, frame-step accumulation and
the phase-at-least-30 cross fallback. Names introduce no storage, const
qualification, image identity or exclusive gameplay ownership.

## Independently recovered layout

The effect-specific work view is `0x3D4` bytes:

| Offset | Member |
| --- | --- |
| `000` | Configuration pointer |
| `004` | Three rings of 32 SDK vectors |
| `304` | 16 rotation vectors |
| `384` | Two four-vertex polygons |
| `3C4` / `3C8` | Unsigned scale / frame words |
| `3CC` | Crossed-line frame halfword |
| `3CE` | SDK color record |

The color intentionally starts at the unaligned offset `3CE`. No artificial
padding or packed-structure workaround is needed: the SDK color has byte
alignment. A target-GCC probe checks this and every accessed field offset.

The configuration has size 20: color at zero, sprite size at 4, three radii
at 6, two widths at `C`, height at `10` and delay at `12`. The selector bounds
and 20-byte address stride establish `D_8015B0B4[30]`, a 600-byte real data
object. The initial 16-byte scale is copied from `D_801461E8`.
Both symbols remain generated input-data definitions with exact linked sizes
and retail bytes; neither is replaced with C storage or an absolute alias.

## Exact-match evidence

The first complete candidate matches all 1,192 instruction bytes. The
independent whole-bank link then matches all 90,112 bytes, SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
Private candidate/profile/header snapshots and receipts are under
`tmp/effect-0-probe/` and `tmp/effect-0-full/`.

It uses the same independently verified caller contracts as effect 18:
crossed lines receive an intentionally ignored mode, and the gradient-quad
height is unsigned. The existing SDK vector-update macro preserves the
original rotation-store ordering. No inline assembly, forced register or
ad hoc compiler option is used.

Production Spanish images, all seven bank copies, metadata and targeted
tests pass. Exact object/final-ELF C extents, the two real data owners and all
15 distinct overlay callees are checked independently of the image hash.
The initial integration against `b9dd81974` preserves all 58 prior entries,
reaching 59/85 bank C functions / 16,996 bytes. Other runtime scope remains
open; configured inventory progress is not exhaustive completion.

The final batch also includes independently verified PAL reuse of the
[gather and tile routines](duel-effect-pal-reuse.md). After incorporating
accepted effect 18, the three additions total 6,136 bytes and reach 62/85
bank C functions / 23,480 bytes, preserving all 59 previously accepted
Spanish entries.
