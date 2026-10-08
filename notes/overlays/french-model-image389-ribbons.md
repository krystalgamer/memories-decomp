# French header-389 image ribbons helper

`func_8013BE68` / `func_8017BE68` (image offset `0xE68`, 0x9FC bytes)
updates, projects and draws the eight header-389 ribbons.
**No release had matching C for this body before.**

The body appears in the same eight French raw images as the
[header-389 sheets helper](french-model-image389-sheets.md): models 184, 269,
409 and 32, in slot 0 (header `0x185`) and slot 1 (header `0x21B`). Each
layout gains a C segment at `0xE68`.

The state starts with eight 0x3E4-byte ribbons of 17 points each:
- **Update:** each point grows its `progress` by `step * 32` up to 1024 and
  sweeps out from `directions[i]` around a per-ribbon angle. The angle is 1024
  for ribbon 0, then fans out by 225 per ribbon on alternating sides.
  `mode` picks the sign of the vertical wave. The paired `b` points are offset
  by 64 along the view yaw.
- **Phases:**
  - When any point of ribbon 0 reaches 1024 in phase 1, the phase moves to 2.
  - When point 15 reaches 1024 below phase 3, the ribbon restarts with
    staggered negative progress.
  - In phase 3, a finished ribbon is marked done. When all eight are done,
    phase 3 moves to 4.
  - The sheet in the second set (`sheets[8 + i]`) is shown or hidden to follow
    its ribbon.
- **Projection:** each ribbon uses its `transforms[i].t` translation. Points
  are projected with `RotTransPers4`/`RotTransPers`, and the quad's angle and
  edge offset come from the screen-space deltas. The last point reuses the
  segment before it.
- **Drawing:** 16 textured quads per ribbon, with separate UVs for the first
  and last segments, drawn only when depth and flag are non-negative.

`length` (64) and `amplitude` (900) are function-scope variables. As literals
they would become shifts. Initialising them once also controls GCC's loop
invariant motion: their hoisted sign-extensions are already-moved candidates in
the ribbon loop. That keeps `&interpolation` recomputed per ribbon, as in the
target.

The [attempt ledger](french-model-image389-ribbons-attempts.csv) records 19
measured probes.
