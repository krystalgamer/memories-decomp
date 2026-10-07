# Spanish MODEL438 rings

Helper `+0x1B5C..+0x23E8` is matching C. It is 2,188 bytes in each of the
twelve MODEL438 loads (MODEL147, MODEL211, MODEL263, MODEL525, MODEL610 and
MODEL632, two stages each), giving 26,256 instruction bytes from one unique
routine. A separate second-slot compilation also matches.

No region has accepted C for this routine. A screen of 8,008 accepted regional
C entries found no body of this size. The entry calls the helper directly.

## Behaviour and method

This is the ring routine of the MODEL446 and MODEL385 rings, built for a single
ring. It draws one seventeen-point `POLY_GT4` ring around the variant's
direction. The ring grows with the timing record and shrinks over the record's
fade window.

The source is written with `RING_COUNT` set to 1, so the timed ring is ring 0
and the cycling-ring branches stay in the body as they do in the four-ring
families. The source differs from MODEL446 in four ways:

- **Offsets.** The ring sits at `+0x848`, the packet at `+0x2140`, the pulse
  group at `+0x1938`, the direction at `+0x226C`, the timing record pointer at
  `+0x22B4` and the phase at `+0x22E8`.
- **Angle.** The ring's start angle is a word at `+0x22D4`.
- **Radii.** Every radius is scaled from timing field `0x44`: the inner radius
  of the timed ring is a quarter of it, and the outer and depth terms are
  `field * (pulse + n) / 256`.
- **Sort test.** Quads are sorted when depth and flag are non-negative, as in
  MODEL385.

The records are the shared `ModelVariantCurtain`. The first candidate was
exact.

## Acceptance evidence

All twelve complete 20 KiB images match using `gcc_2_8_1_g0_split`. Regression
tests cover:

- record offsets
- every relocation against all twelve images
- C segment order and statuses in the MODEL438 suite

The terminal dependency fingerprints hash the body, the shared model-variant
header and `gpu_packets.h`.
