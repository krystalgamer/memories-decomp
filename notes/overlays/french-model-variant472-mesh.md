# French MODEL472 mesh helper

`func_8013C734` / `func_8017C734` (image offset `0x1734`, 0x5FC bytes)
draws a 6x16 grid of semi-transparent `POLY_GT4` quads and updates a single
mesh's progress from the effect timing. **No release had matching C for this
body before.**

The local view in `variant472_mesh.h` holds:
- one 0x954-byte mesh at `+0x15A0` with a front and a back point grid
  (`SVECTOR [7][17]` each, at `+0` and `+0x3B8`) and progress at `+0x94C`;
- the `POLY_F4` mask at `+0x2BFC` and the `POLY_GT4` quad at `+0x2DB4`;
- the position at `+0x2EA4`, the direction at `+0x2EBC`, the unsigned time at
  `+0x2EEC`, the step at `+0x2EF4`, the timing pointer at `+0x2EFC` (unsigned
  start/end at `+0x20`/`+0x24`, fade start/end at `+0x2C`/`+0x30`), the yaw
  angle at `+0x2F2C` and the phase at `+0x2F40`.

Measured shape:
- **Drawing:** a mesh with positive progress is turned by the stored angle,
  scaled by progress and placed at the position. Each cell is first
  projected from the back grid (corners written mirrored, x1/x0/x3/x2) to
  derive UVs and the texture page from screen half and active buffer. It is
  then projected from the front grid, coloured 128/160/160 and sorted. The
  mask copies the quad's screen points in grey 128 but is never submitted.
- **Progress:** in phase 1 below 4096, progress is the elapsed fraction of
  the start..end window; reaching 4096 clamps it and moves to phase 2.
  Otherwise, once the time passes the fade start, progress is 4096 minus the
  elapsed fraction of the fade window.
- **Angle:** after the mesh, the angle advances by `step * 8`.

Matching notes:
- The dead `ratan2` yaw clamp after `GsGetActiveBuff` keeps the yaw add and
  holds the quad/mask setup after the calls.
- `red`/`green` colour variables, initialised in the row-loop header and
  shared by the mask, keep 128/160 in fp/s6. Literal colours leave the
  back-row pointer in fp instead of the 0x108 spill slot.
- Reading the timing fields through `state->timing` (no local) loads the
  phase-1 timing pointer into v1.

The helper is registered in the French MODEL719 stage 7 slot 0 and stage 8
slot 1 images and in their byte-identical Spanish twins. `GetTPage`,
`SetPolyGT4`, `SetSemiTrans`, `SetShadeTex` and `GsGetActiveBuff` were added
to the French and Spanish MODEL472 binding files at their resident addresses.
All four images rebuild exactly. The
[attempt ledger](french-model-variant472-mesh-attempts.csv) records the
probes.
