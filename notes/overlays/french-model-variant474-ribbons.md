# French MODEL474 ribbons helper

`func_8013BE38` / `func_8017BE38` (image offset `0xE38`, 0x900 bytes) is the
first helper of the MODEL474 family images: model_variant_121 stage 7 slot 0
and stage 8 slot 1, and model_variant_431 stage 9 slot 0 and stage 10 slot 1.
It draws one seventeen-point swaying ribbon from a 0x4AC-byte record at
`work + 0x230`.

- **Build.** For each point, the radius `(0x400 - amp[k]) / 4` swings the
  point around the angle `-phase[k] - W(0x1BB8)`. Half-word `0x1C34` adds a
  half turn (`0x800`). The point then moves along `W(0x1B64..0x1B6C) *
  amp[k] / 1024`. The far edge is offset by the usual yaw and side length.
- **Growth.** While `W(0x1C1C) > 0`, a negative phase counts up first. Then the
  amplitude grows to `0x800`. Stage 1 becomes 2 once `amp[0]` reaches
  `0x400`. Stage 2 becomes 6 once `amp[16]` reaches `0x800`.
- **Projection.** The usual seventeen-point `RotTransPers4` / `RotTransPers`
  edge projection, scaled by `W(0x1BBC)`. Each ribbon record stores its own
  projection flags at `+0x3E0`.
- **Drawing.** Per-point colours at `+0x2A8` fill one-sided `POLY_GT4` strips
  at `work + 0x19CC`. A strip is sorted when both depth and flag are
  non-negative. The packet pointer advances once, after the first strip;
  later strips reuse the second packet.
- **Tail.** The scale grows from the unsigned timing fields `+0x18`/`+0x1C`,
  capped at `0x1000`. Before `+0x20` the sweep `W(0x1BB8)` advances freely.
  After that, at stage 0, it advances until it completes a wrap, then sets
  stage 1.

The stored timing pointer is read through its G32 view. In the last segment
the vy delta uses the packed high half. The completed-wrap compare holds the
new wrap count in its own local before `prev + 1 == cur`. Both reproduce
retail scheduling and operand order.

All bindings were already present in the French and Spanish MODEL474
linker-symbol files. The helper is registered in the four French images and
their byte-identical Spanish twins. All eight rebuild exactly. The
[attempt ledger](french-model-variant474-ribbons-attempts.csv) records the
probes.
