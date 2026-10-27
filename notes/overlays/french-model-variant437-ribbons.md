# French MODEL437 ribbons helper

`func_8013CB08` / `func_8017CB08` (image offset `0x1B08`, 0xACC bytes) in the
model_variant_103 stage 9 slot 0 and stage 10 slot 1 images draws four
nine-point ribbons. It is a close relative of the
MODEL436 ribbons helper at offset `0x1AEC` of the header-707 images and uses the
same 0x200-byte ribbon record. It differs in these ways:

- **Side length.** `W(0x1610) / 32` once mode `W(0x163C)` reaches 4, and 32
  before that.
- **Build modes.** Modes below 3 use the straight-line spine, with no `- 24`
  lift. Higher modes use the curled spine.
- **Projection.** A single projection flag is shared by `RotTransPers4` and
  `RotTransPers`. The 4 x 9 flag array is gone.
- **Drawing.** Strips use the `POLY_FT4` template at `work + 0x1458`. They
  are submitted with `Graphics_SubmitDrawModePacket(poly, ot, otz, 1)` when
  the depth is positive.
- **State machine.** It is keyed on `W(0x15E0)` against the unsigned timing
  fields at `+0x24` (half-step growth) and `+0x14` (reset with end
  `-(i * 4)`). Otherwise, below mode 4, the range grows by full steps and a
  finished ribbon restarts unconditionally. Half steps use an unsigned
  `W(0x15E8) / 2`.

In the full-step restart, `count++` must come before `state = 0`. That
order keeps the strided ribbon pointer biased at the state field, as in
retail.

All required bindings were already present in the French and Spanish
MODEL437 linker-symbol files. The helper is registered in the two French
images and their byte-identical Spanish twins. All four rebuild exactly. The
[attempt ledger](french-model-variant437-ribbons-attempts.csv) records the
probes.
