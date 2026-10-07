# French MODEL388 sheets helper

`func_8013C9A4` / `func_8017C9A4` (MODEL388 family, image offset `0x19A4`,
0x4F8 bytes) draws two four-quad sheets. **No release had matching C for this
body before.** It is reachable directly from the entry.

The helper draws two sheets:
- The first sheet sits at the origin. The second is advanced along the
  direction by a halfword `progress / 1024`.
- The scale is the sheet size plus a size/8 pulse on odd frames.
- Three corners take the sheet's inner colour and the fourth its outer colour.
  The depth is scaled by 8/10 before `GsSortPoly`.

Sizes are updated only on the last index of each timing cycle:
- **First sheet:** grows over the timing window `+0x10..+0x14` and shrinks
  over `+0x1C..+0x20`. The windows are unsigned.
- **Second sheet:** follows the phase. Phase 0 sets size 0, phase 1 sets
  4096, phase 2 grows to 8192 and phase 4 shrinks to 0, then moves to 5.

The drawing half is the MODEL426 sheets code. The update half is the shape
of the MODEL441 rainbow sheets helper (`variant441_sheets.c`), here with
unsigned timing words and the timing count at `+0x0C`. As in MODEL441, the
target reserves 16 unused stack bytes between the rotation and the scale.

The helper is registered in all eight French MODEL388 images (models 8, 43,
235 and 706, both slots). `ReadRotMatrix` and `SetRotMatrix` were added to the
French MODEL388 binding file at their French resident addresses. All French
overlay images rebuild exactly. The
[attempt ledger](french-model-variant388-sheets-attempts.csv) records the
experiment.
