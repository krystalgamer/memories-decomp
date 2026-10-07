# French MODEL582 arcs helper

`func_8013C560` / `func_8017C560` (image offset `0x1560`, 0x638 bytes)
animates and draws four groups of paired five-point arcs as gouraud lines.
**No release had matching C for this body before.**

The local view in `variant582_arcs.h` holds:
- four 0xD0-byte groups at `+0`, each with near and far point grids
  `[2][5]`, the near/far progress words at `+0xB4`/`+0xBC` and done flags at
  `+0xC4`;
- the `GsGLINE` at `+0x34AC` and the origin at `+0x34E0`;
- the direction at `+0x3658`, the unsigned time at `+0x3988`, the step at
  `+0x3990`, the timing pointer at `+0x3998` (limit at `+0x28`) and the
  phase at `+0x39D8`.

Measured shape:
- **Points:** each arc `k` clamps its near/far progress to 0..1024 and places
  five points every `4096 / 5` from a base angle that advances 300 per
  group. The radius is `(k + 1) * 192`; points are offset along the direction
  by `progress / 1024`.
- **Progress:** progress grows by `step * 48`. When the far value reaches
  1024, the arc either finishes (time at or past the limit: both values set
  to 1024, done = 1) or wraps (near = far - 1024, far -= 1408). The wrap also
  moves phase 1 to phase 2.
- **Completion:** eight finished arcs move phase 2 to phase 3.
- **Drawing:** unfinished arcs are drawn as `GsGLINE`s from each near point
  to its far point through `RotTransPers3`, with colours 160/0/160 and 8/0/8.

Matching notes:
- The group pointer reset is written after `ScaleMatrix`. sched1 lifts it
  above the origin loads, which local-alloc then addresses through the group
  register.
- The timing test reads the limit first.

The helper is registered in all four French images that carry this body
(models 582 and 129, stage 9 slot 0 and stage 10 slot 1). `GsSortGLine` and
`RotTransPers3` were added to the French MODEL469 binding file at their
French resident addresses. All French overlay images rebuild exactly. The
[attempt ledger](french-model-variant582-arcs-attempts.csv) records the
probes.
