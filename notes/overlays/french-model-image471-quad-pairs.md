# French header-471 image quad-pairs helper

`func_8013C530` / `func_8017C530` (image offset `0x1530`, 0x504 bytes)
grows, fades and drifts a group of sixteen quads.
**No release had matching C for this body before.**

The body appears in the two French raw images of model 82 that also hold
the header-471 quads helper, directly before it (`0x1530`–`0x1A34`).

The code follows the French header-470 quad-pairs helper
(`variant470_quad_pairs.c`) with one 0x794-byte group of sixteen quads:
- **Drawing:** quads are drawn up to size 1024 and drift by the size clamped
  at zero. Their scale ramps from 0 at -1024 to 1024 at zero.
- **Wrap and timeout:** growing quads wrap by 2048, and a quad that falls
  to -1024 or below finishes once the time limit passes.
- **Completion:** moves phase 2 to phase 3.

The local view lives in `variant471_quad_pairs.h`; the resident calls are
covered by `model_variant471_linker_symbols.txt`. The
[attempt ledger](french-model-image471-quad-pairs-attempts.csv) records the
probe, which matched on the first build.
