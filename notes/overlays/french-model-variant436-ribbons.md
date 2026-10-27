# French MODEL436 ribbons helper

`func_8013CAEC` / `func_8017CAEC` (image offset `0x1AEC`, 0xBDC bytes) in the
model_variant_707 stage 9 slot 0 and stage 10 slot 1 images draws four
nine-point ribbons. Each ribbon is stored in a 0x200-byte
`Ribbons436Ribbon` record at the start of the work area.

## Build pass

- **Side offset.** The yaw is `ratan2(W(0x15CC), W(0x15C4)) + 0xC00`. A
  second, unused `ratan2` call is also made. The side length is
  `W(0x1610) * 10 / 1024` while mode `W(0x163C)` is positive, and 10
  otherwise.
- **Spine phase.** Ribbon `i` uses phase `i * 1024`. The wave starts at
  `W(0x1634)`. It is negated for even ribbons while `W(0x15E0)` is still
  below the `wave_end` field of the timing record at `work + 0x15F4`. The
  wave advances by 1300 per point.
- **Mode 0.** Points lie on the line to `W(0x1580..0x1588)`. A bend
  `phase - k * 768 ± W(0x1638)` scales the sine offset around the phase
  angle. The sign is `+` for even ribbons. The end points stay straight.
- **Other modes.** A curl of `k * 128` (256 at the ends) is added to the
  bend `k * 768 - W(0x1638)`. The point drops by
  `W(0x1594) / 4 * (0x400 - W(0x1590)) / 1024`.
- **Second edge.** Each point is mirrored to a second edge offset by the
  yaw and the side length.

## Draw pass

Both edges are projected with `RotTransPers`, using a fixed angle at the
last point. A `POLY_FT4` template at `work + 0x14A8` is then drawn between
the clamped `start` and `end` indices. Each record's state machine grows
the drawn range by half or full frame steps. It restarts or holds at the
end depending on the timing record's `loop_end`. Mode 1 seeds alternating
signed ends and switches to mode 2 after the last ribbon. Finally, the wave
and bend words advance by the frame step and the phase word at `0x161C`
advances by 0x40.

## Matching notes

- **Pointer bias.** In the restart path, `count++` must come before
  `state = 0`. That order makes the strided ribbon pointer biased at
  `+0x1F8`, as in retail.
- **Register allocation.** The second-mode interior points also test the
  odd-ribbon flag, and both arms of that test are identical. The extra
  reference gives the flag `fp`, so the side length is spilled to the stack
  as in retail. Cross-jumping after reload then removes the redundant test.

## Wiring

`RotTransPers`, `rcos` and `rsin` were added to the Spanish MODEL436
linker-symbol file. The helper is registered in the two French images and
their byte-identical Spanish twins. All four rebuild exactly. The
[attempt ledger](french-model-variant436-ribbons-attempts.csv) records the
probes.
