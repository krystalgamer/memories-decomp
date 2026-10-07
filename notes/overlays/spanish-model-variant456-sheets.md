# Spanish MODEL456 sheets

Helper `+0x2ACC..+0x315C` is now matching C. The same 1,680-byte routine
appears in all six header-456/606 MODEL loads registered by the MODEL456 lines
change, giving 10,080 instruction bytes:

- MODEL163, stages 7/8
- MODEL460, stages 9/10
- MODEL536, stages 9/10

A separate second-slot compilation also matches. I screened 7,940 accepted
regional C entries, 50 of them the same size, and none has this routine's
normalised shape. No region has accepted C for it.

## Behaviour

The routine draws eight sheets. Each sheet is four `POLY_GT4` quads built from
corner arrays `v0..v3`. The quad packet lives at `+0x219C` in the context.

- **First sheet.** It sits at the origin (`+0x22DC`) and uses the upper half
  of the texture. On odd frames it grows by an eighth of its size. It grows to
  0x800 over the timing record's window at `+0x14..+0x18`, then moves to phase
  1. It shrinks to zero over the window at `+0x1C..+0x20`.
- **Other sheets.** Each sits at its own position (`+0x98`) and uses the lower
  half of the texture. Its scale is clamped at zero and grows by an eighth on
  even frames. From the spread start (`+0x24`) it widens by `step * 2048` up
  to 0x2000, then sets its faded flag (`+0xB8`).
- **Fading.** After that, its fade value (`+0xBC`) falls by `step * 32`. While
  the flag is set, the colours are scaled by that value over 1024.
- **Phase.** The last sheet sets phase 3 when it finishes spreading and phase
  5 when it finishes fading.

The sheet, timing and context records are declared in
`variant456_sheets.h`. No other routine shares their offsets.

The accepted MODEL411 sheets were used only as evidence for the overall
structure. Every offset, the sheet count, the scale handling, the UV halves
and the fade logic were taken from the retail instructions.

## Scale stores

The retail code stores the other sheets' scale after their last translation
store. When the three fields are plain assignments, the scheduler moves them
into the delay after the translation load, which leaves 3 instructions out of
place. Assigning the scale first moves the stores ahead of the translation
loads instead, with 10 words different.

A `do { } while (0)` statement macro used for both scale assignments matches
exactly. Its loop notes stop the scheduler from moving the stores. The ledger
keeps all three rejected layouts.

## Integration

This converts the `+0x2ACC` helper from ASM to C in all six modules, next to
the accepted MODEL456 lines (`+0x315C`) and streamers (`+0x34D4`). Every
callee, including `ReadRotMatrix` and `SetRotMatrix`, already had a binding.

## Acceptance evidence

All six complete 20 KiB images match using `gcc_2_8_1_g0_split`. The 11-row
ledger records:

- the three rejected experiments
- the exact source and its second-slot compilation
- all six terminal full-image results

Regression tests cover:

- the record offsets, including repeated header inclusion
- relocations and callees against every image
- C segment order, statuses and totals in the MODEL456 lines suite

The terminal dependency fingerprints hash the body, its record header and then
the shared model-variant header.
