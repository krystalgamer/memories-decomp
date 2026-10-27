# French MODEL426 streamers helper

`func_8013DBC0` / `func_8017DBC0` (image offset `0x2BC0`, 0x880 bytes) in the
model_variant_0 stage 7 slot 0 and stage 8 slot 1 images is the coiled
streamer body (`spanish_model_variant/variant474_streamers.c`) on the
header-458 streamer layout (`Variant458Streamer`, 0x334 bytes). It differs in
these ways:

- four streamers at `work + 0xF4C`, coil radius `0x240`;
- the spine bulge is scaled by 24 and the spin wobble by 16;
- the clamped width is 3;
- the view fields sit 0x564 bytes higher, and the matrix origin and
  `POLY_G4` 0x56C bytes higher.

`RotTransPers4` writes each point's flag into a local
`PSXLONG depth[4][17]` (at `sp + 0xD0`). `RotTransPers` keeps the scalar
`flag`. A segment is sorted only when `otz[k] >= 0`, `depth[i][k] >= 0` and
the word at `work + 0x2128` is at least `0x1000`.

Two spellings were needed for an exact match:

- `wave = 0` is assigned before `twist` in the outer loop, not in the inner
  `for` initialiser.
- The last segment uses `&streamer->sa[15]` directly, not a separate
  `previous` pointer. Loop strength reduction then creates that induction
  register after the hoisted `&p` and `&depth` addresses. This places the
  spilled `&depth` store where retail has it.

The helper is registered in the French images and their byte-identical
Spanish twins; the existing MODEL426 binding files already cover every call.
All four images rebuild exactly. The
[attempt ledger](french-model-variant426-streamers-attempts.csv) records the
probes.
