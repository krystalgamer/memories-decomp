# Spanish reuse of gather and tile effects

The complete North American sources accepted in #6560 are reused through
the repository's existing `VERSION_EUROPE` wrapper pattern, without copying
their bodies or adding compiler flags/profiles:

| Spanish function | Bytes | Shared source |
| --- | ---: | --- |
| `801481A8..80148BA4` | 2,556 | `gather_effect.c` |
| `80149F90..8014A8E4` | 2,388 | `tile_effect.c` |

The wrappers live under `src/overlays/european/duel_effects/` and retain
`gcc_2_8_1_g0_split`. Only Spanish registration is claimed here; another
release must independently pass its own image and ownership gates.

## Measured regional differences

An unchanged-source Spanish full-bank link had just one differing word in
the gather routine and eight in the tile routine. These were real literal
differences, not relocation guesses:

| Constant | North American default | Verified PAL arm |
| --- | ---: | ---: |
| Gather curve height | 98 | 106 |
| Tile vertical row step | 28 | 30 |
| Tile vertical origin | 98 | 106 |
| Tile bottom-V addend | 126 | -120 |

Horizontal tile coordinates remain exactly `k * 28 - 70` / `k * 28 - 42`.
The first PAL trial incorrectly changed the horizontal spacing/origin too:
gather became exact, but tile retained five differing words. Restoring the
horizontal expressions and using the row step for the vertical edge removed
all five. The signed bottom-V addend is preserved as observed; its eventual
byte store wraps rather than introducing a new clamp.

The final independent Spanish bank reproduces all 90,112 bytes, SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
The unchanged default arm independently reproduces the complete North
American overlay images, including bank SHA-256
`baa203b937dc6bdf91b1826c5832f0f32e11ae5fe9d05193a4361bc08158b9e0`.
Diagnostic diffs and receipts remain in `tmp/shared-effects-full/`.

## Real storage and complete functions

The 16-byte initial vector `D_80146024`, 20-byte gather descriptor
`D_8015A5F8`, 28-byte SDK image `D_8015A62C` and two eight-byte tile descriptors
`D_8015A648` retain their real generated header/data owners. Explicit extents
do not create C storage or absolute aliases. The texture-pair table keeps
the accepted canonical union and its existing data owner.

Target-GCC probes verify the accepted descriptor, work and frame layouts.
Production Spanish matching, metadata and all previous C object/final-ELF
extents are checked separately from the image hash. Both complete functions
are promoted; the adjacent unmatched vortex and other routines remain
explicit assembly boundaries.

Before combining accepted effect 18, these two functions plus the independently
matched effect zero bring the local Spanish bank to **61/85 C functions /
21,940 bytes**, preserving all 58 prior entries. The batch adds **6,136 exact
bytes** across three complete routines. This is not exhaustive runtime
completion.

After integrating accepted effect 18 and regional work through `9b25c6617`,
all 59 prior Spanish entries remain intact. The combined batch reaches
**62/85 bank C functions / 23,480 bytes**, with **23 explicit assembly
boundaries**, and **186/209 configured-overlay instances / 79,332 bytes**.
