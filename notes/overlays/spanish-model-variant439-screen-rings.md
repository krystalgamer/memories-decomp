# Spanish MODEL439 screen rings

Helper `+0x1230..+0x1704` is matching C. It is 1,236 bytes in each of the
fourteen MODEL439/589 images, for 17,304 instruction bytes from one unique
routine. Every complete 20,480-byte image matches retail, and an independent
second-slot compilation also matches. It was the last generated-assembly
routine in these images.

Excluding this change, the screen checked 7,764 configured regional C entries.
It compared five same-size bodies and found no accepted normalized match, so
no region has accepted C for this routine. The source was recovered from the
retail instructions with the `gcc_2_8_1_g0_split` profile.

## Context

The entry does not call this helper directly, as the existing inventory
records. Three 0x1A8-byte records start at `+0xE08`. Each holds three rows of
seventeen points, inner and outer colours, a scale and a wrap count. This is
the layout of the French entry header's screen-ring record; the Spanish source
declares it in `variant439_screen_rings.h`. The packet is at `+0x1808`, the
translation at `+0x18F8`, the step at `+0x1944` and the phase at `+0x197C`.

## Behaviour

For each ring with a positive scale, the routine:

1. Builds a matrix from a zero rotation, the translation and the ring's scale.
2. Rebuilds the inner row on a radius of `scale * 192 / 4096 + 128`, in angle
   steps of 256. The other two rows keep their stored points.
3. Draws sixteen POLY_GT4 strips. Each strip is first projected between the
   inner and outer rows to choose its texture: a 16-bit page from the left or
   right half of the frame buffer, depending on the active buffer and on
   whether the first vertex is left of x = 160. The texture coordinates are
   the projected positions, shifted by 128 on the right half.
4. Projects the strip again between the inner and middle rows, sets the
   ring's inner and outer colours, and sorts it when the depth is positive.

Each scale grows by step × 96. Until phase 3 it wraps at full size and counts
a cycle; from phase 3 it stays at full size.

## Recovery notes

The ledger keeps every materially distinct form. Two details were required:

- **Scale and page locals.** The ring scale is held in a local, and the
  texture page is an `int`, which gives the retail zero-extension.
- **Radius offset.** The radius is computed as `scale * 192 / 4096`, and the
  128 offset is added inside the point loop, where loop motion hoists it. This
  gives the retail register assignment for the scale and the division.

## Integration

The MODEL439 binding list gains the SDK alias `GsGetActiveBuff`, whose address
was already bound as `func_800852A8`.

## Acceptance evidence

All fourteen complete 20KiB images match using `gcc_2_8_1_g0_split`.

Regressions cover:

- the ring record and packet layouts, including repeated header inclusion
- relocations and callees against every image
- C segment order, statuses and bindings in the MODEL439 suite

Terminal dependency fingerprints hash the body, the ring header, the shared
model-variant header and `gpu_packets.h`, in that order.
