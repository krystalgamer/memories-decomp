# Spanish MODEL443 sheets

Helper `+0x2F2C..+0x3668` is now matching C. It is 1,852 bytes in both
MODEL443 loads, MODEL62 stages 9 and 10, giving 3,704 instruction bytes from
one unique routine. A separate second-slot compilation also matches.

No region has accepted C for this routine. A screen of 7,994 accepted
regional C entries found no body of the same size. The loader entry calls this
routine.

## Behaviour

The routine draws five `ModelVariantSheet` records at `+0x2A68`, each as four
`POLY_GT4` quads.

- **First three sheets.** Each sits at its own position (`+0x3570`) and uses
  its own inner colour. It grows to 0x1000 over the timing record's growth
  window while the phase is zero. Otherwise it shrinks after the fade bound,
  and the fourth phase step (5 to 6) is set when it finishes shrinking.
- **Fourth sheet.** It sits at the origin (`+0x35A0`). In phase 3 it spreads
  to 0x1800 over the record's spread window and then moves to phase 4.
  Otherwise it shrinks after the fade bound.
- **Fifth sheet.** It moves along the direction by the progress until the
  travel word reaches 0x400, then rests at the halfword position (`+0x35B0`).
  Below phase 4 it is hidden, in phase 4 it is 0x1800, and afterwards it
  shrinks from 0x2000.
- **Hues.** The last two sheets cycle through the same eight hues as the
  accepted MODEL441 sheets, selected by `frame % 8`.
- **Flicker.** On odd frames, every sheet grows by an eighth of its size.

## Method

The routine was reconstructed from the retail instructions. The accepted
MODEL441 sheets were used only as structural evidence. Two source details fix
the retail schedule:

- **Shared variable.** One variable holds the flicker and then the projected
  depth. This puts both in `a2`, as in the retail code.
- **Outer colour.** Both colour branches store the outer colour. Cross-jumping
  merges the identical tails after scheduling, so the depth scale stays in its
  own block. A single shared store let the scheduler interleave the two. An
  empty statement block also separated them, but it left the registers
  different.

## Integration

Both modules convert the `+0x2F2C` helper from assembly to C. The record
declarations live in `variant443_sheets.h`. The MODEL443 binding list gains
the SDK aliases `ReadRotMatrix` and `SetRotMatrix`; their addresses were
already bound.

## Acceptance evidence

Both complete 20 KiB images match using `gcc_2_8_1_g0_split`. The ledger keeps:

- three rejected layouts
- the exact source and its second-slot compilation
- both terminal full-image results

Regressions cover:

- the record offsets, including repeated header inclusion
- every relocation against both images
- C segment order, statuses, totals and bindings in the MODEL443 suite

The terminal dependency fingerprints hash the body, the sheets header and the
shared model-variant header, in that order.
