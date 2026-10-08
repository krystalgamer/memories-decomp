# French MODEL443 three short bands

MODEL62 stages 9/10 contain the independently recovered 1,720-byte helper
at image `+0x3EF0`, `func_8013EEF0` / `func_8017EEF0`. The body and local
views are `src/overlays/french_model_variant/variant443_bands.{c,h}`; the
second slot changes only the function symbol. Both use the existing
`gcc_2_8_1_g0_split` profile (GCC 2.8.1 / MASPSX 2.81).

## Behavior and measured view

Three 0x78-byte bands begin at context `+0x2900`. Each has three rows of
two points at record `+0/+0x10/+0x20`, projections at `+0x30/+0x38/+0x40`,
inner/outer colour rows at `+0x48/+0x50`, and depths at `+0x70`.
Each band emits two GT4 halves between its endpoints, reusing one packet
at context `+0x33D0`. Nonnegative depth and projection-flag checks are
retained for both halves.

Radius comes from the first accepted `ModelVariantSheet` at `+0x2A68`,
combined with the halfword size at `+0x3678`. On even frame parity it is
`size * (sheet_size /256) /1024`; on odd parity it is
`size * (sheet_size *24 /4096) /1024`. These signed divisions preserve the
original rounding and narrowed radius.

Each band has its own screen angle, origin and direction. Three origins are
VECTOR views at `+0x3570`, three directions at `+0x35C8`, screen-x/y arrays
at `+0x35FC/+0x3602`. The first endpoint uses origin; the second adds the
corresponding direction directly. Rotation and the two-stage matrix pipeline
follow the original helper; there is no nine-point path interpolation.

The unsigned time at `+0x3624` and four-byte timing pointer at `+0x3634`
grow size in phase one using descriptor words `+0x3C/+0x40`. At 1024 it
clamps and moves phase at `+0x3698` to two. There is no fade tail in this
function. These are minimum accessed views, not allocation-capacity claims.

## Investigation and acceptance

The accepted Spanish `variant438_bands.c` supplies the common two-quad
projection sequence. Original French instructions establish the different
record size, three-band/two-point loops, angle arrays, radius formula,
direct origin/direction translation and descriptor offsets.

The optimized advisory driver records measurements and dependency-aware
cache hits under `tmp/fr-probe/b62/` and its attempt journal:

| Experiment | Bytes | Different words |
|---|---:|---:|
| Complete measured candidate | 1720 | 13 |
| Sheet pointer initialized before OT query | 1720 | 10 |
| Angle declared after the short indices | 1720 | 6 |
| Both sheet and packet pointers before OT query | 1720 | 0 |
| Equivalent explicit parity comparison | 1720 | 6 |
| Explicit parity temporary | 1720 | 6 |

The first link failed explicitly for `RotTransPers3`, whose original
destination `0x80087898` was already present under an address-based alias.
Adding its canonical name preserves that resident start and all old aliases.
The retry reused only the valid compiled object, not the failed link.

The final source retains natural pointer lifetimes and declaration order;
no register forcing, inline assembly, volatile values or fabricated memory
operations are used. Advisory text equality did not establish acceptance:
all 3,594 complete French production images were rebuilt and matched first.
Both new objects then define section-owned 0x6B8-byte C functions at their
original ELF addresses, without helper `.NON_MATCHING` symbols.

The [terminal ledger](french-model-variant443-bands-attempts.csv) records
source and header fingerprints after that gate. Dedicated regressions cover
twenty-two target-compiled field offsets and record size, exact registrations,
resident starts, wrapper/source shape, fingerprints, complete hashes and
sized object/ELF ownership. Existing regional helpers and physical archive
registrations remain unchanged. This adds two C instances and 3,440 bytes;
no generated reports are refreshed.
