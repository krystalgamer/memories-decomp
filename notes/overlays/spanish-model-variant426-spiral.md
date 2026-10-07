# Spanish MODEL426 spiral

Helper `+0x13B0..+0x1DF0` is matching C. It is 2,624 bytes in the two
MODEL426 loads (MODEL0 stages 7/8), giving 5,248 instruction bytes from one
unique routine. A separate second-slot compilation also matches.

No region has accepted C for this routine. Excluding this change, a screen of
8,010 accepted regional C entries found no body of this size. The entry calls
the helper directly.

## Behaviour and method

This is the twelve-arm spiral of the accepted MODEL473 spiral (#7134), using
the shared `Variant425SpiralArm` records at `+0x84C` and the packet at
`+0x1F90`. While the phase at `+0x2180` is not negative:

- **Radius.** It is `0x300` from phase 1 on, otherwise three quarters of the
  size half at `+0x2150`.
- **Spread.** The size half itself moves the second spine along the view. The
  per-point factor is `k * 32 + 16` on even frames and `k * 16 + 8` on odd
  frames.
- **Sorting.** Each half quad clamps a negative depth to zero and clears its
  projection status before sorting. The first half sorts a non-negative depth,
  the second a positive one.

After drawing:

- **Size.** Before phase 1 it grows by `step * 64` up to `0x400`. Otherwise it
  shrinks by `step * 16` down to zero.
- **Sweep.** The sweep at `+0x214C` advances by 40.

The ledger keeps three rejected layouts of the size reads. The retail
allocation needs the spread assigned after the two dead size tests, which
MODEL473 also keeps.

## Acceptance evidence

Both complete 20 KiB images match using `gcc_2_8_1_g0_split`. The MODEL426
binding list gains the SDK alias `RotTransPers`; its address was already
bound. Regression tests cover:

- arm offsets
- every relocation against both images
- C segment order, statuses and bindings in the MODEL426 suite

The terminal dependency fingerprints hash the body, the spiral header, the
shared model-variant header and `gpu_packets.h`.
