# Spanish MODEL438 advancing band

Helper `+0x3638..+0x3DE8` is independently recovered matching C. It is 1,968
bytes in all twelve configured header-438/588 MODEL loads, for 23,616
instruction bytes from one unique routine:

- MODEL147, 211 and 610, stages 7/8
- MODEL263, 525 and 632, stages 9/10

An independent second-slot compilation also matches. Screening 6,217 accepted
regional C entries found no same-sized body. No region has accepted C for this
routine.

The accepted MODEL415 bands were used as structural evidence only. Compiled in
that form, the result is 48 bytes short and differs in 452 words. The typed
state, the radius rule, the second-quad columns and the submission gate were
all recovered from the retail body.

The modules already carry the accepted MODEL438 sheets at `+0x3DE8`. This
change converts the entry-reachable `+0x3638` helper from ASM to C. The MODEL438
binding list gains the SDK alias `RotTransPers3` for the address already bound
as `func_spanish_80087898`.

## Behavior

One shared `ModelVariantBand` at context `+0x1770` holds nine segments.

**Radius.** The radius is the halfword size at `+0x22D8` times the radius word
at `+0x40` of the timing record, divided by 4096. On odd frames, the radius
word is first scaled by 18/16.

**Projection.** Each segment's three points sit at angles 1024 and 3072 and at
the centre. The segment is rolled by `ratan2` of the projected vector at
`+0x227C`, plus 2048.

**Position.** Segment `j` is placed at the origin advanced by
`progress / 1024 * j / 8` along the direction. It is then projected with
`RotTransPers3` into the band's screen coordinates, and its flag is recorded.

**Drawing.** Eight spans are drawn, each as two `POLY_GT4` quads through
`+0x20D8`:

- the first spans columns a to b
- the second spans columns c to b

Both quads use the band's two colour rows. Each is submitted only when the
segment depth and flag are non-negative.

**Phase 1.** The word progress at `+0x22DC` grows over the descriptor's
unsigned grow window to 1024, then advances the phase to 2.

**Phases below 3.** While the size is positive and the fade start has been
reached, the size shrinks from 4096 over the unsigned fade window. At zero, the
phase becomes 3.

## Source detail that reproduces the target

The quad pointer is taken before the angle, as in the accepted MODEL415 bands.
Taking it afterwards leaves four prologue scheduling words different. There
are no extra locals, forced registers or inline assembly.

## Acceptance evidence

All twelve complete 20KiB images match using `gcc_2_8_1_g0_split`, with the
bands and sheets in C. The sixteen-row ledger keeps:

- the two rejected experiments
- the exact source compilation and the second-slot compilation
- all twelve terminal full-image results

Regressions cover:

- the band, timing and state layouts, including repeated header inclusion
- relocations and callees against every image
- C segment order, statuses and bindings in the existing MODEL438 suite

Terminal dependency fingerprints hash the body, the private header, the shared
model-variant header and `gpu_packets.h`, in that order.
