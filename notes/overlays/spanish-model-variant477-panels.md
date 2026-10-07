# Spanish MODEL477 panels

Helper `+0x1484..+0x1958` is matching C. It is 1,236 bytes in each of the six
MODEL477 images, for 7,416 instruction bytes from one unique routine:

- MODEL262 and MODEL631, stages 7/8
- MODEL117, stages 9/10

Every complete 20,480-byte image matches retail, and an independent
second-slot compilation also matches. Only the six entry instances remain ASM.

Excluding this change, the screen checked 7,834 configured regional C entries.
It compared nineteen same-size bodies and found no accepted normalized match,
so no region has accepted C for this routine. The source was recovered from
the retail instructions with the `gcc_2_8_1_g0_split` profile.

## Context

The routine shares the accepted [quads](spanish-model-variant477-quads.md)
state: the same two discarded arctangents of the direction and projected
point, the step, the per-group `enabled` flags and the phase. Its own records
are two 0x8C-byte panels at the start of the context, before the quad groups.
Each holds four points, a colour, a size, a completion flag, a base matrix
whose translation is used, and a growth direction. The packet is the
POLY_FT4 at `+0x1FC4`, after the quads' packet. The time is at `+0x2068` and
the descriptor pointer at `+0x2078`.

## Behaviour

The routine draws one panel once the time reaches descriptor `+0x1C`, and two
once it reaches `+0x20`. For each drawn panel with a non-negative size:

1. The colour is used as is up to size 512, then fades as
   `(1024 - size) / 512`.
2. The panel is translated from its base by `direction * size / 512`, scaled
   by 768 and projected as one POLY_FT4. It is sorted while not yet done.
3. The size grows by step × 48. From size 512 the phase moves from 0 to 2 and
   the panel's `enabled` flag is set. At 1024 the panel is done.

Once both panels are done while the phase is 2, it becomes 5.

The original keeps a single-iteration inner loop over one-element arrays, an
unused cosine call, and a half-size test on the first size element; the C
keeps them because the retail code does.

## Recovery notes

The ledger keeps every form. The first candidate differed only in where the
packet pointer is assigned, which swapped two saved registers. Assigning it
anywhere before the second time threshold matches; the source sets it between
the two thresholds.

## Acceptance evidence

All six complete 20KiB images match using `gcc_2_8_1_g0_split`. All required
SDK aliases were already bound.

Regressions cover:

- the panel record and state offsets, including repeated header inclusion
- relocations and callees against every image
- C segment order, statuses and totals in the MODEL477 suite

Terminal dependency fingerprints hash the body, the panel header, the MODEL477
quads header, the shared model-variant header and `gpu_packets.h`, in that
order.
