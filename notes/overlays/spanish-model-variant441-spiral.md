# Spanish MODEL441 palette spiral

Helper `+0x3630..+0x414C` is matching C. It is 2,844 bytes in all ten MODEL441
loads, for 28,440 instruction bytes from one unique routine:

- MODEL166, MODEL360, MODEL487 and MODEL590, stages 7/8
- MODEL709, stages 9/10

An independent second-slot compilation also matches. Excluding this change, the
screen checked 7,870 configured regional C entries and found no body of the
same size, so no region has accepted C for this routine. As the existing
inventory records, no entry call-graph path to it was recovered.

## Behaviour

While the phase at `+0x271C` is not negative, the routine:

1. Builds twelve two-point arms from `+0x1F5C`, on a spiral of half the size at
   `+0x2700`.
2. Moves a copy of each arm along the view by the spread: from phase 3 an
   eighth of the timing record's `+0x88` word (record at `+0x10DC`), before
   that `0x400` minus the half-word at `+0x2702`.
3. Projects each arm with the MODEL474 form: per-half depth, angle and width.
4. Draws each arm as two POLY_GT4 halves from `+0x256C`, where the depth is
   positive. The inner and outer colours come from an eight-entry palette
   selected by `(flags + arm) % 8`, with the flags at `+0x26D0`.

The translation is the half-word triple at `+0x269C`. The sweep at `+0x26FC`
advances by 16 a frame. Once the frame counter at `+0x26D4` reaches the timing
record's `+0x20` bound (signed), while the secondary phase at `+0x2720` is
zero, the size grows by step × 64 up to 0x400 and the secondary phase becomes 2.

## Relationship to the other spirals

The routine combines two accepted Spanish spirals, used as structural evidence:

- the phase-guarded MODEL474 spiral (body, projection and gates)
- the MODEL421 palette spiral (palette values and tail form)

The `Variant425SpiralArm` record of the accepted shared header is reused. The
offsets, the half-word translation and the signed tail were recovered from the
retail body. The ledger keeps every form. Two details were required:

- **Palette index.** The index is `(flags + arm) % 8`, a signed remainder, and
  the eighth entry is the final `else`.
- **Spill slots.** The arm-half index is declared before the radius, which
  gives the retail stack slots.

## Integration

This change converts the `+0x3630` helper from ASM to C, beside the accepted
MODEL441 helpers. All required SDK aliases were already bound.

## Acceptance evidence

All ten complete 20KiB images match using `gcc_2_8_1_g0_split`.

Regressions cover:

- the arm record and packet layouts, including repeated header inclusion
- relocations and callees against every image
- C segment order, statuses and totals in the MODEL441 suite

Terminal dependency fingerprints hash the body, the shared spiral header, the
shared model-variant header and `gpu_packets.h`, in that order.
