# French MODEL417 framebuffer feedback

The `+0x30E4` helper is independently recovered as 1,644 bytes of C in
eight existing French images: MODEL134, MODEL232 and MODEL535 stages 9/10,
and MODEL354 stages 7/8. Both runtime slots use the existing
`gcc_2_8_1_g0_split` profile (GCC 2.8.1 and MASPSX 2.81). No physical
registration, archive slice, compiler profile or shared regional source changes.
The entry and the unclassified suffix remain unchanged.

## Behavior and measured view

Five 0x1A8-byte records at context `+0xF28` hold three seventeen-point
vector rows, two colours, progress, and a cycle counter. Their offsets are:

| Field | Record offset |
|---|---|
| Point rows | `0x0`, `0x88`, `0x110` |
| Inner / outer colours | `0x198` / `0x19C` |
| Progress / cycles | `0x1A0` / `0x1A4` |

The helper samples the active framebuffer once. For each positive-progress
record, it scales by `rsin(progress)`, translates along the effect direction,
and projects sixteen pairs of quads. The first projection selects a framebuffer
texture page and coordinates; the second establishes the actual quad positions
and depth. It keeps the inner/outer colours, semi-transparency, and both
nonnegative depth and flag checks.

From phase five, it regenerates point rows one and two. A size word at context
`+0xBB8` (the record view at `+0xA98`, field `+0x120`) gives signed
`width = size / 128` and `radius = size * 100 / 8192`. The regenerated radii
are `256 + radius` and `284 + radius`, with z `-width`.

Progress below 2048 advances by `step * 48` and wraps on crossing 2048.
Wrapping in phase four increments the current record's cycle count; two
cycles move the global phase to five. This is not the completion aggregation
or clamping behavior of the accepted Spanish header-388 feedback template.

The packet is `POLY_GT4` at context `+0x2028`; origin is `SVECTOR` at
`+0x20F0`, direction is `VECTOR` at `+0x20F8`, step is at `+0x2130`,
and phase is at `+0x2170`. These accessed views establish a minimum extent,
not a whole-context allocation capacity. Unknown regions remain opaque.

## Source and evidence

The body and local declarations are
`src/overlays/french_model_variant/variant417_feedback.{c,h}`. The slot-one
wrapper changes only `func_8013E0E4` to `func_8017E0E4`. Declaration order
and reuse of the same counter for geometry and rendering preserve old-GCC
stack-slot and register allocation without register forcing or artificial
memory traffic.

The accepted `variant388_feedback.c` supplied the common framebuffer rendering
sequence. Geometry regeneration, field locations, signed size divisions,
the wrap/cycle transition, and both slot bodies were independently checked
against original French instructions. No reference-project types or flags
were used.

The distinct experiments remain under `tmp/fr-probe/f134/`; the table records
the measured results before integration:

| Experiment | Bytes | Different words |
|---|---:|---:|
| First complete recovery, pre-correction padding | 1652 | 391 |
| Height lifetime restricted to phase-five geometry | 1644 | 97 |
| Size-record pointer captured first | 1644 | 58 |
| Explicit separate radius locals | 1664 | 387 |
| Correct eight-byte direction-to-step padding | 1644 | 54 |
| Reuse one counter for geometry and quads | 1644 | 16 |
| Width/radius before active-buffer locals | 1644 | 12 |
| Scale local before active-buffer/angle locals | 1644 | 0 |

The first link also failed explicitly for five SDK names present only under
address-based aliases. Their original resident starts are added to the family
binding file: `GsGetActiveBuff` at `0x800852A8`, `GetTPage` at `0x80082CE8`,
`SetPolyGT4` at `0x80082EE8`, `SetSemiTrans` at `0x80082DA8`, and
`SetShadeTex` at `0x80082DD8`. Existing aliases and addresses are preserved.

All eight complete production images hash to their original targets. Each
linked helper has its original address and size `0x66C`, a section-defined C
object owner, and no helper `.NON_MATCHING` symbol. The
[terminal attempt ledger](french-model-variant417-feedback-attempts.csv)
records both source and header fingerprints after this full-image/owner gate.
Thirteen target-compiler field assertions plus record size cover the measured
layout. The dedicated regression checks registration, source/wrapper shape,
resident starts, fingerprints, complete hashes, and object/ELF ownership.

This adds eight matching-C instances and 13,152 bytes; no generated reports
are refreshed.
