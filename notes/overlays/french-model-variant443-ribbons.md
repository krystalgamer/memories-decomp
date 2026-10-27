# French MODEL443 ribbons helper

`func_8013CADC` / `func_8017CADC` (image offset `0x1ADC`, 0x8D0 bytes) in the
model_variant_62 stage 9 slot 0 and stage 10 slot 1 images draws three
seventeen-point ribbons. It reuses the 0x370-byte `Model400FirstRibbon`
layout from `french_model_variant/variant400_ribbons.h` and keeps the
header-400 build, projection and strip loops. It differs in these ways:

- **Ribbon count.** Three ribbons, with phase `i * 4096 / 3`.
- **Spine.** Each ribbon spans from the shared origin at `work + 0x35A0` to
  its own origin in the `VECTOR origins[3]` array at `work + 0x3570`. The
  wave amplitude is 32, and both phase words at `0x3670` and `0x3674` advance
  by the frame step at `0x362C`.
- **Width.** The ribbon width is `sheet->size * 48 / 4096`, read from the
  sheet at `work + 0x2A68`.
- **Draw range.** Strips are drawn from `work + 0x3668` up to 16 with the
  general two-sided quad only. In phase 2 that start is recomputed from the
  spread window of the timing record at `work + 0x3634`. When it reaches 0,
  the phase moves to 3.

The sheet pointer must be assigned before the `func_80058F10` call. Retail
forms the sheet base with an `addiu` and then loads the size field at offset
`0x88`. The origins must be read as a `VECTOR` array member of
`Ribbons443State`. A computed `MODEL_VARIANT_WORD` offset swaps the operands
of the address add.

`RotTransPers` was added to the Spanish MODEL443 linker-symbol file; the
French file already had it. The helper is registered in the two French
images and their byte-identical Spanish twins. All four rebuild exactly. The
[attempt ledger](french-model-variant443-ribbons-attempts.csv) records the
probes.
