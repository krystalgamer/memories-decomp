# Spanish MODEL473 funnels

Helper `+0x386C..+0x3CE4` is now matching C. It is 1,144 bytes in the two
MODEL473 loads, MODEL385 stages 9/10, for 2,288 instruction bytes from one
unique routine. A separate second-slot compilation also matches.

No region has accepted C for this routine. A screen of 7,970 accepted
regional C entries compared the 2 bodies of the same size, and neither has
this routine's normalised shape. The entry calls the routine directly.

## Behaviour

The routine draws five funnels at `+0x337C`, one per object. Each funnel is
a pair of seventeen-point rings:

- **Inner ring.** Its radius is `rsin(1300) * 256 >> 12`.
- **Outer ring.** It is 160 plus the flicker wider than the inner ring and is
  set back by 256 plus the flicker. The flicker is a one-hundred-and-
  twenty-eighth of the second sheet's size on odd frames.

Each funnel is drawn as sixteen `POLY_GT4` quads:

- **Placement.** The funnels sit at the positions at `+0x3B10`, lowered by
  `rcos(1300) * 256 >> 12`.
- **Scale.** Each funnel is scaled by its object's size word at `+0x4EC`.
- **Colour fade.** Above 0x1000, the colours fade by `(0x2000 - size) / 0x1000`.

Drawing stops while the word at `+0x3C08` is negative. Each frame the spin
advances by `step * 80`.

The routine is closely related to the MODEL411 funnels (#7180), but it uses
different records, counts, radii and fades.

## Method

The routine was reconstructed from the retail instructions. Three source
details explain the retail allocation:

- **Dead view angle.** The retail code keeps `ratan2(...) + 0x800` in a saved
  register that nothing reads. The source computes the yaw and its quadrant
  fix-up, but does not use the result. Flow deletes the assignments, and the
  branch only disappears after register allocation, which leaves that value
  alive in the register.
- **Outer radius.** It is built as `radius + 160`, and the flicker is added
  afterwards.
- **Statement macro.** The first-object pointer is set through a
  `do { } while (0)` statement macro. The macro's loop notes keep the second
  sheet pointer at the top of the function, while the OT spill can still move
  past them. The same macro around the OT fetch delays the OT store instead,
  and the ledger keeps that layout.

## Integration

Both modules convert the `+0x386C` helper from ASM to C. Every callee
already had a binding.

## Acceptance evidence

All complete 20 KiB images match using `gcc_2_8_1_g0_split`. The 8-row
ledger keeps four rejected layouts, the exact source, its second-slot
compilation and both terminal full-image results.

Regressions cover:

- the record offsets, including repeated header inclusion
- every relocation against both images
- C segment order, statuses and totals in the MODEL473 suite

The terminal dependency fingerprints hash, in order, the body, the funnel
header and the shared model-variant header.
