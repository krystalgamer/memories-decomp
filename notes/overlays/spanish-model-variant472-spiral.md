# Spanish MODEL472 spiral

Helper `+0x1D30..+0x28B0` is matching C. It is 2,944 bytes in the two MODEL472
loads (MODEL719 stages 7/8), giving 5,888 instruction bytes from one unique
routine. A separate second-slot compilation also matches.

No region has accepted C for this routine. Excluding this change, a screen of
8,032 accepted regional C entries found no body of this size. The entry calls
the helper directly.

## Behaviour and method

This is the twelve-arm spiral of the accepted MODEL418 spiral, built for
four-point spines. The records are new 0x104-byte `Variant472SpiralArm`
records at `+0x1EF4` with per-point outer and inner colours and projection
flags. The new header `variant472_spiral.h` declares them.

Before phase 3 the radius is half the size at `+0x2F1C` and the spread is
0x400. From phase 3 on, the radius is 0x200 and the spread is the size. Each
point:

- **Lean.** Leans along the direction by `size * dir / 1024 * k / 3`.
- **Length.** Moves along the view by `(k * 32 / 3 + 8) * spread / 1024`.
- **Projection.** The last point pairs with the previous point through a
  pointer that steps by one record per arm, as in the MODEL474 streamers.

Both halves are drawn as `POLY_GT4` where the depth and the flag are not
negative. The sweep at `+0x2F20` advances by 16. In phase 0 the size grows
over the timing record's window up to 0x400. In later phases it fades out over
the record's fade window.

The ledger keeps the two near candidates:

- **Shared tail.** Merging the angle, width and offset statements after the
  branches is 100 bytes short.
- **Packet pointer.** It must be assigned before the turn angle.

## Acceptance evidence

Both complete 20 KiB images match using `gcc_2_8_1_g0_split`. The MODEL472
binding list gains the SDK aliases `ratan2`, `rcos`, `rsin` and
`RotTransPers`; their addresses were already bound. Regression tests cover:

- record offsets
- every relocation against both images
- C segment order, statuses, owners and bindings in the MODEL472 suite

The terminal dependency fingerprints hash the body, the spiral header, the
shared model-variant header and `gpu_packets.h`.
