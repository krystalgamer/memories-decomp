# Spanish MODEL425 streamer

Helper `+0x2BC8..+0x3360` is matching C. It is 1,944 bytes in the two MODEL425
loads (MODEL0 stages 9/10), giving 3,888 instruction bytes from one unique
routine. A separate second-slot compilation also matches.

No region has accepted C for this routine. Excluding this change, a screen of
8,014 accepted regional C entries found no body of this size. The loader entry
does not call the helper; it remains a closed retained helper.

## Behaviour and method

This is the routine of the accepted MODEL479 streamers, built for a single
streamer of two points. The records are new 0x64-byte `Variant425Streamer`
records at `+0xF4C`, which are the two-point form of the shared streamer
layout. The new header `variant425_streamers.h` declares them.

The routine:

- **Builds.** Coils the spine around the variant's axis by the sweep at
  `+0x17E8`. The twist steps by `k * 1500` per point.
- **Projects.** Projects the spine and a copy of it moved along the view.
- **Width.** Uses 2 once the length at `+0x17E0` exceeds 0x200, otherwise the
  projected width.
- **Draws.** Draws the segment as a `POLY_G4` from `+0x16D4` where the depth is
  not negative.

The ledger keeps the first candidate, which still divided the twist step by 16
as MODEL479 does.

## Acceptance evidence

Both complete 20 KiB images match using `gcc_2_8_1_g0_split`. Regression tests
cover:

- record offsets
- every relocation against both images
- C segment order, statuses and owners in the MODEL425 suite

The terminal dependency fingerprints hash the body, the streamer header, the
shared model-variant header and `gpu_packets.h`.
