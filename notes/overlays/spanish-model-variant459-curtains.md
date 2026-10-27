# Spanish MODEL459 curtains

Helper `+0x19AC..+0x1FB8` is matching C: `variant459_curtains.c`, with
`variant459_curtains_slot1.c` renaming it for the second load slot. It is
1,548 bytes in the twelve Spanish MODEL459 loads (MODEL47, MODEL238,
MODEL411 and MODEL620 at stages 7/8; MODEL231 and MODEL417 at stages 9/10)
and in the eleven French images that carry the identical body. French
MODEL238 has no stage-8 image. That makes 23 instances and 35,604
instruction bytes. The helper is entry-reachable.

## Behaviour

The body belongs to the same family as `model_variant/variant422_curtains.c`,
`variant398_curtains.c` and `spanish_model_variant/variant464_curtains.c`,
but it differs in every part that carries state, so it is an independent
source rather than a layout of an earlier one.

- **Curtains.** The `0x118`-byte `ModelVariantCurtain` records start at
  `work + 0x848`. The loop runs to the timing record's 16-bit count at
  `+0x24`, rather than a fixed count.
- **Rows.** Seventeen points at the rotating angle `work + 0x30C8` per row.
  The inner row is `rcos/rsin >> 6`, and the outer row is scaled by
  `radius = lift + 0x80` and set at depth `-(lift + 0x40)`.
- **Matrix.** The matrix turns by the mirrored `ratan2` yaw and the pitch.
  It is translated by the curtain's own position at `work + 0x2F8C + i * 16`
  and scaled by the curtain's scale.
- **Colours.** Curtain 3 and any curtain below `0x800` keep the colours at
  `work + 0x30C0/0x30C4`. The others are scaled by
  `(s16)(0x1000 - scale) / 2048`.
- **Sorting.** Each strip sorts when depth and flag are non-negative.
- **Update.** While the scale is below the timing record's limit and the
  phase at `work + 0x30DC` is below 3, the scale becomes
  `limit * clock / grow_end`, using the clock at `work + 0x3084`. On reaching
  the limit it clamps and counts a cycle. Otherwise, from `fade_start`, it
  shrinks linearly to zero at `fade_end`.

The shared `Timing459` record in `variant459_sheets.h` gains these
`fade_start`/`fade_end` words at `+0x40/+0x44`. The sheets body only uses the
record through a pointer, and it still matches in all 23 images.

## Matching notes

Old GCC's global allocator orders pseudos by
`floor_log2(refs) * refs / live_length`, with references weighted by loop
depth. Two source details are required for the exact register assignment:

- The unfaded colour loads are written as a `do { } while (0)` macro. The
  extra loop note keeps the work pointer in `s2` ahead of the strip counter
  and polygon pointer.
- In the fade branch, the clock goes through `otz` and the fade start goes
  through the colour scale `fade`, declared `s32` and assigned through an
  `(s16)` cast. This keeps the timing record in `a0` ahead of fade start
  (`a1`) and clock (`a2`). Fresh locals rank the short-lived fade start
  above the record.

All 23 complete images match using `gcc_2_8_1_g0_split`. The ledger
[`spanish-model-variant459-curtains-attempts.csv`](spanish-model-variant459-curtains-attempts.csv)
records the full-image results. The Psy-Q bindings the body needs were
already present in both 459 linker symbol files.
