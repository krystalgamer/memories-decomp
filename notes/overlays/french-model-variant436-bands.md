# French MODEL436 bands helper

`func_8013BD98` / `func_8017BD98` (image offset `0xD98`, 0x7A4 bytes) in the
MODEL707 stage 9 slot 0 and stage 10 slot 1 images is the MODEL449 band body
(`variant449_bands.c`) on the MODEL437 layout
(`MODEL_VARIANT437_BANDS`: bands at `+0x888`, every other field 0x600 lower).
The joints, the drift lift, the progress clamp and the two `POLY_GT4` halves
per segment are unchanged.

The only change is the sort test. Each half is submitted only when
`band->otz[j] >= 0` and `depth[i][j] >= 0`, where `depth` is a local
`PSXLONG [1][3]` declared after `coord` (at `sp + 0xC8`). Nothing writes the
table: `RotTransPers3` still uses the single `p`/`flag` pair. So retail tests
uninitialised stack, which the matching C reproduces. The table also accounts
for the frame being 16 bytes larger than in MODEL437.

The helper is registered in the French MODEL707 stage 9 slot 0 and stage 10
slot 1 images and in their byte-identical Spanish twins. `RotTransPers3` was
added to both MODEL436 binding files, and `rcos`/`rsin` to the Spanish one.
All four images rebuild exactly. The
[attempt ledger](french-model-variant436-bands-attempts.csv) records the
probes.
