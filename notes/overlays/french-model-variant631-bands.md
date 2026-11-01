# French MODEL631 bands helper

`ModelVariant423_DrawBand` (image offset `0xF14`, 0x7CC bytes, at
`0x8013BF14` / `0x8017BF14`) in the
model_variant_262 and model_variant_631 stage 9 slot 0 and stage 10 slot 1
images draws one band of five columns. It uses the NA
`Variant423Band` layout from `model_variant/variant423_bands.h`.

- **Projection.** The heading is `ratan2(work[0xED2], work[0xED0]) + 0x800`.
  The radius comes from `work[0xF18]` (48 or 54 per 4096, chosen by bit 0 of
  `work[0xEEC]`) plus a sine wobble. Each column is placed along the variant
  path and projected with `RotTransPers3`.
- **Drawing.** Each of the four segments sorts two `POLY_GT4` quads (at
  `work + 0xD88`): first from the sa/sb columns, then from the sc/sb columns.
  Negative depths clamp to zero first.
- **Timing.** Phase 1 fades `work[0xF1C]` in to 0x400. Phase 2 fades
  `work[0xF18]` out over the timing record at `work + 0xF00`. That stored
  pointer is read through a 4-byte `G32` view (`BAND631_CONFIG`).

Matching details:

- The second quad reads its columns through `col` (`(s32 *)band + j`) and
  `next` (`(u8 *)band + (j + 1) * 4`). Spelling `next` as a pointer sum makes
  GCC distribute the `+ 1` into a `+ 4` offset. The byte form reuses the
  first quad's `j + 1` address instead.
- Storing NULL to `col` and `next` after the loops gives both views several
  sets. CSE then copies them from the first quad's registers instead of
  replacing them.
- The clamp stores through `next = col` and ends with `col = next`. That way
  the following `band->otz[j]` test reads the band address, not `col`.

`rcos` (`0x800866F8`) was added to the Spanish MODEL440 linker-symbol file,
which these Spanish images share; the French file already had it. The helper is
registered in the four French images and their byte-identical Spanish twins.
All eight images rebuild exactly. The
[attempt ledger](french-model-variant631-bands-attempts.csv) records the probes.
