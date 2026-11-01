# French MODEL image 22822 blooms helper

`func_8017C230` (image offset `0x1230`, 0x658 bytes) in the French and
Spanish `model_image_model_22822_8017b000` images draws eight groups of
twelve growing `POLY_FT4` blooms. The loader entry reaches it. The same
bytes sit at `func_8013C230` in the `model_image_model_22812_8013b000`
images. Both slots compile `image22822_blooms.c` itself. The source defines
`FrenchModelVariant454_DrawBlooms`, and each image binds that name at its own
address. The North American and Japanese header-437 entry includes the same
body at `0x8013C230`/`0x8017C230` (see [Per-image symbol
bindings](model-variants.md#per-image-symbol-bindings)).

- **Relative.** It is a variant of the
  [header-472 blooms](french-model-variant472-blooms.md) body. The group is
  0x2A4 bytes because the pad before `direction` is gone. Sizes run to 3072
  instead of 2048. The colour fades above 2560, and growth is
  `step * 16`. The first five groups swirl their depth with
  `rcos(i * 4096 / 5)`, while the rest spread by half the overshoot, and a
  work flag picks the sign. The view lives in
  `french_model_variant/image22822_blooms.h`.
- **Swirl angle.** The angle must be an `s16` local assigned inside the
  swirl branch. If the expression is written inline, loop optimisation
  hoists the group index and the halves of the division out of the inner
  loop.
- **Bindings.** Calls use the current resident layout, so both regions reuse
  their `model_variant472_linker_symbols.txt`.

Both images rebuild exactly. The
[attempt ledger](french-model-image22822-blooms-attempts.csv) records the
probes.
