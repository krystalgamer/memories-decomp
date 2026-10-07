# Spanish MODEL446 rings

Helper `+0x24BC..+0x2CAC` is matching C. It is 2,032 bytes in the two
MODEL446 loads (MODEL146 stages 9/10), giving 4,064 instruction bytes from one
unique routine. A separate second-slot compilation also matches.

No region has accepted C for this routine. Excluding this change, a screen of
8,000 accepted regional C entries found no body of this size. The entry
calls the helper directly.

## Behaviour and method

This is the routine of the accepted MODEL385 rings (#7149), using this
family's work block. It draws up to four seventeen-point `POLY_GT4` rings
around the variant's direction:

- **Ring 3.** It grows with the timing record and shrinks over the record's
  fade window.
- **Other rings.** They grow by `step * 128` and cycle until phase 5.

The source differs from MODEL385 in two ways:

- **Offsets.** Every work-block offset moves. The rings sit at `+0x1510`, the
  packet at `+0x1A4C`, the pulse group at `+0x1130`, the direction at
  `+0x1B64` and the phase at `+0x1BDC`.
- **Sort test.** Quads are sorted whenever the depth is positive. MODEL385
  uses a non-negative depth and flag test.

The records are the shared `ModelVariantCurtain`. The first ledger row keeps
an experiment where the `0x1000` size constants were mistakenly remapped
along with the offsets.

## Acceptance evidence

Both complete 20 KiB images match using `gcc_2_8_1_g0_split`. Regression
tests cover:

- record offsets
- every relocation against both images
- C segment order, statuses and totals in the MODEL446 suite

The terminal dependency fingerprints hash the body, the shared model-variant
header and `gpu_packets.h`.
