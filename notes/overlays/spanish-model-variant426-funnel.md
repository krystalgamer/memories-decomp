# Spanish MODEL426 funnel

Helper `+0x3440..+0x3874` is now matching C. It is 1,076 bytes in the two
MODEL426 loads (MODEL0, stages 7/8), for 2,152 instruction bytes from one
unique routine. A separate second-slot compilation also matches. Screening
7,982 accepted regional C entries found no body of this size, so no region has
accepted C for this routine. It is a closed retained helper: the entry does
not call it.

## Behaviour

The routine draws one funnel at `+0x1C1C` as sixteen `POLY_GT4` quads. The
funnel is a pair of seventeen-point rings:

- **Inner ring.** Its radius is `rsin(0x800) * 512 >> 12`.
- **Outer ring.** It is 160 plus the flicker wider than the inner ring, and
  set back by 256 plus the flicker. The flicker is the second sheet's size
  divided by 128, applied on odd frames.

The quads are placed at the halfword position at `+0x20C8`, lowered by
`rcos(0x800) * 512 >> 12`, and scaled by the size word at `+0x2128`. They are
only drawn while that word is positive. From phase 5 the colours are scaled by
the fade word at `+0x2138` divided by 1024. The rings spin by `step * 80`.

This routine and the MODEL473 funnels (#7183) share a structure, but they
differ in their records, counts, radii and fade.

## Method

The routine was reconstructed from the retail instructions. Three source
details explain the retail schedule and allocation:

- **Dead view angle.** As in the MODEL473 funnels, the view angle and its
  quadrant fix-up are computed and never used. This leaves
  `ratan2(...) + 0x800` in a saved register.
- **Empty statement block.** An empty `do { } while (0)` block sits before the
  OT fetch. It may be a compiled-out trace. Its loop notes keep the sheet
  pointer at the top of the function while still letting the OT store move.
  Putting the block around the OT fetch instead delays the OT store, and the
  ledger keeps that layout.
- **Ring counter.** The ring counter is cleared before the outer radius is
  built.

## Integration

Both modules convert the `+0x3440` helper from ASM to C. Every callee already
had a binding.

## Acceptance evidence

All complete 20 KiB images match using `gcc_2_8_1_g0_split`. The 8-row
ledger holds:

- four rejected layouts
- the exact source and its second-slot compilation
- both terminal full-image results

Regressions cover:

- the record offsets, including repeated header inclusion
- every relocation against both images
- C segment order and statuses in the MODEL426 suite

The terminal dependency fingerprints hash, in order:

1. the body
2. the funnel header
3. the shared model-variant header
