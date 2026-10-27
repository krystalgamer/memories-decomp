# French MODEL446 rays helper

`func_8013DCAC` / `func_8017DCAC` (image offset `0x2CAC`, 0xA28 bytes)
builds sixteen three-point rays and draws each segment as two `POLY_GT4`
halves. It reuses the matched MODEL425 rays body
(`model_variant/variant425_rays.c`, also French MODEL442) and its
`Variant425Ray` layout. The rays now start at work `+0` instead of `+0xDC0`.

MODEL446 differences:
- **State:** the turn is `ratan2(+0x1B80, +0x1B78) + 0xC00`, the yaw
  `ratan2(+0x1B6C, +0x1B64)` and the pitch `ratan2(+0x1B68, +0x1B6C)`.
  The polygons are at `+0x19B0`, the mirror flag is the halfword `+0x1BE4`
  and the spin is the halfword `+0x1BC4`.
- **Points:** segment `k` has length `(k * 12 + 8) * W(+0x1BD8) / 1024`.
  The lean terms are `W(+0x1B64/68/6C) * sign / 2 * k / 2`. The copy is
  pushed along `turn * sign`.
- **Matrix:** the size is `W(+0x1BD4)`, plus a sixteenth when bit 0 of
  `+0x1B90` is set. The rotation is `(-pitch - sign * 512, yaw, 0)`, at
  `(+0x1B50, +0x1B54 - 0x80, +0x1B58)`.
- **Draw:** `RotTransPers4`/`RotTransPers` write separate `p` and `flag`.
  A half is sorted only when its depth is positive.
- **Tail:** the size grows by `W(+0x1B9C) * 128` up to 0x1000. Once the
  unsigned clock `+0x1B94` passes the timing table's `+0x44`, the length
  falls from 0x400 to zero over the span to `+0x48`. The spin then advances
  by 0x20.

Matching notes:
- The `k == 2` projection must use `[k - 1]`/`[k]`, not `[1]`/`[2]`. Then CSE
  knows `k == 2` and hoists the `ray + 4` and `ray + 8` bases.
- `r` is an `s16` declared after `spin`, and `length` is declared right after
  `i`. Spill slots and the a0/a1 choice follow declaration order.
- `r * k / 2` emits the same code as `r * k >> 1`. However, its expansion adds
  a spilled temporary that reserves retail's unused stack slot between `r`
  and the hoisted `turn * sign`. With `>> 1` every later spill slot is 4 bytes
  low.

The helper is registered in the French MODEL146 stage 9 slot 0 and stage 10
slot 1 images and in their byte-identical Spanish twins. The Spanish
MODEL446 binding file gained `RotTransPers`. All four images rebuild exactly.
The [attempt ledger](french-model-variant446-rays-attempts.csv) records the
probes.
