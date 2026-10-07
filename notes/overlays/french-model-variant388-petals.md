# French MODEL388 petals helper

`func_8013D410` / `func_8017D410` (MODEL388 family, image offset `0x2410`,
0xA6C bytes) builds twelve petal arms, projects them and draws them, then
advances the bloom. **No release had matching C for this body before.**

It follows the accepted MODEL_EXODIA petals helper (`model_exodia/petals.c`)
over a local 0x84-byte arm, `Petal388Arm` in `variant388_petals.h`. The arms
start at `+0x1050` and the `POLY_GT4` cursor at `+0x16C0`.

Measured shape:
- the turn is `ratan2(work[0x1848], work[0x1840]) + 0xC00`. The cursor must
  be assigned before it, or the turn's stack store moves ahead of the guard
  load and adds a nop;
- the build, projection and draw passes run only when the word at `+0x1898`
  is not negative. The radius and spacing come from the half at `+0x1888`
  and the spread is `1024 - half[0x188A]`;
- the tail advances the phase at `+0x1884` by 16, grows `+0x1888` by
  `step * 64` up to 1024 once `timing[4] <= time`, then opens the spread
  over the `timing[7]..timing[8]` window.

The helper is registered in all eight French MODEL388-family images that
carry this body (models 8, 43, 235 and 706, both slots). `RotTransPers` and
`rcos` were added to the French MODEL388 binding file at their French
resident addresses. All French overlay images rebuild exactly. The
[attempt ledger](french-model-variant388-petals-attempts.csv) records all
four probes.
