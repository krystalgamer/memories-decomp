# Spanish MODEL411 funnels

Helper `+0x24A4..+0x29B4` is now matching C. It is 1,296 bytes in each of the
four MODEL411 loads, for a total of 5,184 instruction bytes from one routine:

- MODEL364, stages 9/10
- MODEL452, stages 9/10

A separate compilation into the second slot also matches. A screen of 7,966
accepted regional C entries found 5 bodies of this size, and none has this
routine's normalized shape. No region has accepted C for this routine. The
loader entry calls it.

## Behaviour

The routine draws two funnels at `+0x67C`. Each one is a `Funnel411` record of
0x118 bytes holding two seventeen-point rings:

- an inner ring of radius 64
- an outer ring of radius 192 plus the flicker, set back by 128 plus the
  flicker

The flicker is a sixty-fourth of the size of the first sheet at `+0x54C`, on
odd frames.

The routine then:

- Turns the funnels to face the view direction at `+0x160C`, using two
  `ratan2` angles.
- Draws each funnel as sixteen `POLY_GT4` quads, using the inner and outer
  colours at `+0x1660`/`+0x1664`.
- Below phase 2, handles only the second funnel.
- Below phase 3, grows each funnel to 0x1000 over the growth window of the
  timing record at `+0x164C`.
- From phase 3, shrinks each funnel to zero once the time passes the record's
  `+0x18` bound. The fade start of this ramp is the word at `+0x114` in the
  overlay data that follows the code (`D_8013D9B4 + 0x114`), not a timing
  field. The source keeps that read as it is.
- Finally, advances the ring spin at `+0x1668` by `step * 80`.

## Method

The routine was reconstructed from the retail instructions. The sheet, timing
and state records are declared in `variant411_funnels.h`, along with the
overlay data label. The accepted MODEL411 sheets header was left untouched, so
its fingerprints stay valid.

The ledger keeps three rejected layouts. Three source details decide the exact
schedule:

- the ring points are set through `setVector`
- the frame bit is shifted by 6
- the pitch offset is added after the quad pointer

The MODEL444 helper at `+0x240C` has the same structure with smaller
constants. It is left for a separate change.

## Integration

All four modules convert the `+0x24A4` helper from ASM to C. The slot-1
wrapper renames both the function and the data label. Every callee already had
a binding.

## Acceptance evidence

All four complete 20 KiB images match using `gcc_2_8_1_g0_split`.

Regressions cover:

- the record offsets, including repeated header inclusion
- every relocation against all images, including the HI16/LO16 data pair
- C segment order, statuses and totals in the MODEL411 suite

The terminal dependency fingerprints hash, in order:

1. the body
2. the funnel header
3. the shared model-variant header
