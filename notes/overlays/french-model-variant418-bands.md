# French MODEL418 bands

`ModelVariant401_DrawBand` (`0x8013BFE0` / `0x8017BFE0`, image offset
`0xFE0..0x1760`, 1,920 bytes) is the helper after the entry in both French MODEL418 images (model
410 at stages 9/10). The entry calls it directly. It is the same routine as
the accepted North American header-401 `variant401_bands.c`
(`0x8013BFE4`): nine points grown along three rows, drawn as eight
two-strip `POLY_GT4` bands. French header 418 is North American header
401 + 17, so the differences are a release difference behind
`VERSION_FRENCH`.

The French `variant418_bands.c`, which both slots compile, defines
`VERSION_FRENCH` and includes the shared source; each image binds
`ModelVariant401_DrawBand` at its own address.

## Source differences

The shared source now spells two things in a way that is neutral for the
North American `gcc_2_7_2_cdk_g0` build and needed by the French
`gcc_2_8_1_g0_split` build. Both North American images still rebuild
exactly:

- The `POLY_GT4` cursor is assigned right after the context copy. Assigning
  it later swaps `s1`/`s2` between the cursor and the growth amount.
- The first strip reads `rec->screen[row][j]` and `rec->screen[row][j + 1]`
  directly, so the French build keeps separate `rec + 4j` and
  `rec + 4(j + 1)` bases.

Two selector macros cover what differs between the releases:

- `VARIANT401_BANDS_COLUMN` adds the record first in the first growth read
  (`rec + off4` rather than `off4 + rec`).
- `VARIANT401_BANDS_NEXT_X` / `_Y` read the second strip's next column from
  `rec->screen[row][j + 1]` instead of the column-shifted `q2` view.

The second strip's depth clamp keeps the original `q2` spelling. Moving it
to the record changes the register the clamp uses.

## Resident binding

The C body calls `RotTransPers3`. The French family-418 linker bindings
and both image symbol files previously spelled resident `0x80087898` as
`func_french_80087898`. They now use `RotTransPers3`, as other French MODEL
families do. The address is unchanged.

## Result

The named `gcc_2_8_1_g0_split` profile matches both slots. Both complete
French images rebuild exactly, adding two C instances / 3,840 bytes. See
the [attempt ledger](french-model-variant418-bands-attempts.csv).
