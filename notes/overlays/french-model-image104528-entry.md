# French MODEL104528 flash and ray swarms

Part of #6460. French MODEL.MRG header 826 provides the entry in raw images
104528/104538, loaded at `0x8013B004`/`0x8017B004`. Each original C owner is
`0x10C4` bytes (1,073 instruction words), compiled with the authoritative
`gcc_2_8_1_g0_split` GCC 2.8.1/MASPSX 2.81 profile.

Both complete 20,480-byte images reproduce their retail bytes and hashes.
The image headers and unclassified suffixes remain unchanged.

## Recovered layout and behavior

The independently measured configuration is 26 bytes. It contains three
halfword color channels, flash/burst extents, random spread, fan extent,
burst interval, fade, an unused halfword, startup delay, spawn end and camera
transition time. The 9,564-byte state holds 64 flash positions, 64 burst
positions, 64 sets of sixteen random offsets, flash frames, elapsed/update
counters, ambient level, counts/phase, two texture handles and separate
64-byte red/green/blue arrays.

Initialization samples each offset axis from the difference of two random
values. During the active spawn interval, the entry places flashes and
periodic bursts around the opposing slot using its canonical dimension
adjustment. The camera switches sides at its configured time. Ambient light
dims before the first flash and recovers after the hit phase.

Flashes draw textured quads. Each colored burst draws a glow and sixteen
Gouraud triangles twice, at unit and doubled scale with distinct draw modes.
Its channels fade independently, using the native threshold and byte
subtraction rather than a new saturation rule.

## Exact source structure

`offsets[i][j]` preserves the native outer/inner induction variables and the
retained `1028 + 128*i` byte displacement. An explicit inner pointer changes
those genuine lifetimes and loses native instructions.

Active and opposing slots are distinct source values: the active slot dies
after camera selection, while the opposing slot spans the random placement
calls. Combining them with `slot ^= 1` changes fourteen register operands.

The owning header supplies a local union between canonical `SVECTOR` and
`ModelEffectAdjustment` views of the same eight-byte stack object. This
preserves camera/rotation and dimension-query accesses without inventing a
helper signature or adding source-local externs.

Native details retained include the repeated flash-frame `< 8` test after
projection, unscaled matrix copies before each fan scale, depth-only packet
submission gates and unused translation resets. Before the hit, phase zero
returns 4 after the delay; the hit transitions to phase one and returns 1;
ambient recovery eventually reaches phase two and returns 2.

## Evidence

`french-model-image104528-entry-attempts.csv` records all four compiled
experiments from checkpoint 287 and both production-image proofs. The first
object was retained through linker-map setup failures; resuming its link
did not create another source experiment. The third experiment first matched
the native body; the fourth added the direct canonical slot-position header
and confirmed the identical binary.

Both slot bodies match independently. Production ELF symbols retain one
original-sized C owner per image. No external reference types, headers,
compiler claims or flags were used; no assembly, fixed registers, volatile
tricks or fabricated operations were introduced.
