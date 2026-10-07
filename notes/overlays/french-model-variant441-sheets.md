# French MODEL441 and MODEL417 rainbow sheets helper

The functions `func_8013DA48` / `func_8017DA48` (MODEL441, image offset
`0x2A48`) and `func_8013DACC` / `func_8017DACC` (MODEL417, offset `0x2ACC`)
contain the same 0x618-byte body. **No release had matching C for this body
before.** In both families the helper is reachable from the entry.

The helper draws two `ModelVariantSheet` groups of four `POLY_GT4` quads:
- **Colour:** three corners take a colour that cycles through eight hues with
  `frame % 8`; this is the same chain as in MODEL443. The fourth corner takes
  the sheet's outer colour.
- **Position:** the first sheet sits at the origin. The second is advanced
  along the direction by `progress / 1024`.
- **Size:** the scale is the sheet size plus a size/8 pulse on odd frames.
- **Sorting:** the depth is scaled by 8/10 before `GsSortPoly`.

Size is updated only on the last index of each timing cycle:
- **First sheet:** grows over the timing window `+0x1C..+0x20` and shrinks over
  the window `+0x28..+0x2C`, using signed division.
- **Second sheet:** follows the phase. Phase 0 sets size 0, phase 1 sets 4096,
  phase 2 grows to 8192 and phase 4 shrinks to 0.

Three source details control the exact allocation (see the
[attempt ledger](french-model-variant441-sheets-attempts.csv)):
- `i`, `red` and `green` are declared ahead of the remaining locals so that
  they spill to `0xF0`, `0xF4` and `0xF5`.
- `i = 0` is the `for` initialiser.
- `&light` is passed directly instead of through a pointer local. Loop
  invariant motion then hoists the address into the `0xF8` spill slot. That
  move raises the cost threshold, so the `4096` stores stay as local
  `li v0,4096` instead of becoming one hoisted constant that is
  rematerialised into `t0`/`t1`.

MODEL417 keeps every work field `0x5AC` bytes lower: sheets at `+0xA98`
instead of `+0x1044`, and the `POLY_GT4` at `+0x1FF4`. Accordingly,
`variant441_sheets.c` addresses work fields through
`SHEETS_WORD`/`SHEETS_OFFSET`, and `variant417_sheets.c` only sets
`SHEETS_SHIFT` to `0x5AC` and renames the function.

The helper is registered in all ten French MODEL441 images (models 166, 360,
487, 590 and 709, both slots) and all eight MODEL417 images (models 134, 232,
354 and 535, both slots). `GsSortPoly`, `ReadRotMatrix` and `SetRotMatrix` were
added to the French MODEL417 binding file at their French resident addresses.
All French overlay images rebuild exactly.
