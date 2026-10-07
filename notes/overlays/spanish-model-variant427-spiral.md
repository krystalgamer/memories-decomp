# Spanish MODEL427 spiral

Helper `+0x157C..+0x1FC4` is matching C. It is 2,632 bytes in the two
MODEL427 loads (MODEL379 stages 9/10), giving 5,264 instruction bytes from one
unique routine. A separate second-slot compilation also matches.

No region has accepted C for this routine. Excluding this change, a screen of
8,010 accepted regional C entries found no body of this size. The entry calls
the helper directly.

## Behaviour and method

This is the twelve-arm spiral of the accepted MODEL473 spiral (#7134), drawn in
three passes. The arms are 0x84-byte `Variant418SpiralArm` records at
`+0x19C8`, which carry their own projection flags. The packet is at `+0x3560`.
While the phase at `+0x37C4` is not negative, each pass:

- **Angle.** Turns the arms a further 1300.
- **Timing.** Steps the timing pointer from `+0x1FF8` by 0x98 when the phase is
  at least 3, and takes the spread from it. Otherwise the spread is `0x400`
  minus the half at `+0x3796`.
- **Arms.** Builds them on a spiral of the size at `+0x3794`, moves them along
  the view by `(k * 48) * spread / 1024`, and draws them as two `POLY_GT4`
  halves at the pass's 32-byte position record (`+0x363C + n * 32`). A half is
  drawn where the depth and the arm's flag are not negative.

Reaching size `0x400` moves phase 1 to 2. From phase 2 on, the sweep advances
by `step * 24`.

## How it was matched

The ledger keeps the materially distinct experiments that set the retail
schedule:

- **Length.** Written as `(k * 48) * spread`, so the loop hoists `spread * 48`.
- **Indexing.** The `k == 1` branch indexes with `[k]`.
- **Angle order.** The angle offset is added first.
- **Colour order.** The `0xC0` colour is assigned before `0x80`.
- **Position pointer.** The position is read through a pass record pointer
  `work + n * 32`, which gives the retail operand order.

## Acceptance evidence

Both complete 20 KiB images match using `gcc_2_8_1_g0_split`. Regression tests
cover:

- arm offsets
- every relocation against both images
- C segment order, statuses and owners in the MODEL427 suite

The terminal dependency fingerprints hash the body, the spiral header, the
shared model-variant header and `gpu_packets.h`.
