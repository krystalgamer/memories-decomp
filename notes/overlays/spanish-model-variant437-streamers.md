# Spanish MODEL437 streamers

Helper `+0x25D4..+0x2F30` is matching C. It is 2,396 bytes in the two MODEL437
loads (MODEL103 stages 9/10), giving 4,792 instruction bytes from one unique
routine. A separate second-slot compilation also matches.

No region has accepted C for this routine. Excluding this change, a screen of
8,032 accepted regional C entries found no body of this size. The entry calls
the helper directly.

## Behaviour and method

This is the coiling-streamer routine of the accepted MODEL474 streamers, built
for three streamers of thirteen points. The records are new 0x274-byte
`Variant437Streamer` records at `+0x93C`, which are the thirteen-point form of
the shared streamer layout. The new header `variant437_streamers.h` declares
them.

The routine:

- **Builds.** Coils each spine with reach 0x60, steps the twist by
  `k * 1500 / 12`, and moves a copy along the view.
- **Places.** Sets the matrix origin at the base position plus the direction
  scaled by the travel at `+0x1590`. The scale is the word at `+0x1598` before
  phase 6, and `0x4000` minus it afterwards.
- **Projects.** Keeps each segment's projection flag in a `status[3][13]`
  table. The width is 3 once the length exceeds 0x200.
- **Draws.** Scales each depth by 9/10 and draws a segment as a `POLY_G4` from
  `+0x14A8` when the depth and its flag are not negative.
- **Fades.** From phase 6 on, the length falls as the scale shrinks, down to
  zero.

The matrix, projection, draw and fade steps sit in a single-pass outer loop. In
the ledger, v01 differs only in spill slots and v02 in one schedule. The
retail layout needs the pass counter declared after `turn` and the packet
pointer assigned before the turn angle.

## Acceptance evidence

Both complete 20 KiB images match using `gcc_2_8_1_g0_split`. The MODEL437
binding list gains the SDK aliases `rcos`, `rsin` and `RotTransPers`; their
addresses were already bound. Regression tests cover:

- record offsets
- every relocation against both images
- C segment order, statuses, owners and bindings in the MODEL437 suite

The terminal dependency fingerprints hash the body, the streamer header, the
shared model-variant header and `gpu_packets.h`.
