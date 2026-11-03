# French MODEL562 shards helper

`FrenchModelVariant562_DrawShards` at `0x8013C2CC` / `0x8017C2CC` (image offset
`0x12CC`, 0x4A8 bytes) in the model_variant_149 and model_variant_419 stage 7 slot 0 and stage 8 slot 1
images, and the model_variant_57 and model_variant_562 stage 9 slot 0 and
stage 10 slot 1 images, is a two-pass shard body. Each `Variant562Shard` is
0x74 bytes at the start of the work area. The count is at +0xC of the record
that `work + 0x3230` points to. The layout lives in
`french_model_variant/variant562_shards.h`.

- **Placement.** The heading is `ratan2(work[0x31FE], work[0x31FC]) + 0x800`.
  Each shard half sits at the s16 origin (`0x31E4`) plus the s32 direction
  (`0x31EC`) times its scale over 1024. Negative scales clamp to zero. The
  two triangles are projected with `RotTransPers3`.
- **Drawing.** A shard whose second scale is below 0x800 sorts one
  white-to-grey `POLY_GT4` (at `work + 0x2E08`) when its depth and flag are
  non-negative.
- **Growth.** While the shard state is 1, both scales grow by
  `work[0x3224] * 128` up to 0x800. The phase at `0x3254` moves from 1 to 2
  once the first scale reaches 0x400.

Scheduling details:

- The ratan2 result goes into the reused temp `v`. The poly pointer is set
  before an empty `do {} while (0)` and the `+ 0x800` after it. With the add
  folded into the call, sched1 moves the poly add or the count load across
  the call.
- `do { s = v; } while (0);` in the clamp gives the retail v1/a0 assignment.

`RotTransPers3` (`0x80087898`) was added to the French and Spanish MODEL405
linker-symbol files, which these images share. The helper is registered in
the eight French images and their byte-identical Spanish twins. All sixteen
images rebuild exactly. The [attempt ledger](french-model-variant562-shards-attempts.csv)
records the probes.
