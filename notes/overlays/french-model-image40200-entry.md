# French MODEL40200 travelling quads and echoes

This recovers the complete raw header-175 entry in two French MODEL.MRG
images. It is not the separately inventoried header-175 tube component:
header numbers alone do not establish equivalent code or state.

| Image sector | Entry | C bytes | Complete image SHA-256 |
| --- | --- | --- | --- |
| 40200 | `0x8013B004` | `0x1104` | `b23c1d717c9648114ea264bd3ca12e250ffed2f434ff02ff0a179df3a6bb2ce8` |
| 40210 | `0x8017B004` | `0x1104` | `1a45abc4403e2d0380e60e96e29064eab6fb832a2c47eeb1e57f7fa9083453c8` |

Both use the authoritative named `gcc_2_8_1_g0_split` profile. The shared
source and relocated wrapper retain the original 4356-byte function ownership.
The four-byte image header and unclassified suffix remain original raw bytes;
neither is claimed as newly recovered C.

## Measured ownership and layout

Slot-zero data consists of a 16-byte vector at `0x8013C108`, two `GsIMAGE`
records at `0x8013C118`, and 28-byte configuration records at `0x8013C150`.
Slot one moves these symbols by `0x40000`. Configuration selection is
`command % 100`.

| Config offset | Recovered field |
| --- | --- |
| `0x00`, `0x03` | Three travel and echo RGB bytes |
| `0x06`, `0x07` | Active-slot part and particle count |
| `0x08`, `0x0A` | Travel extent and random radial spread |
| `0x0C`, `0x0E` | Travel wobble height and echo extent |
| `0x10`, `0x12`, `0x14` | Travel, hold and echo durations |
| `0x16`, `0x18` | Particle spacing and initial delay |
| `0x1A` | Unused in this entry; no semantic claim |

The independently recovered 1560-byte state starts with the stored
configuration pointer, followed by 64 `SVECTOR` positions at `0x004`,
64 targets at `0x204`, and 64 echoes at `0x404`. Two packed texture values
occupy `0x604`; notification is at `0x60C`, elapsed time at `0x610`, and
update count at `0x614`. Pointer storage uses `G32`. All owning types are
declared in the component header.

## Native behavior

Initialization copies the opposing slot position, substitutes Y = -350,
and creates spherical targets from the native three random draws per
particle. It uploads both textures and clears the three lifecycle fields.
The otherwise unused initial active-slot query and early frame-step query
are retained.

The first render pass refreshes each not-yet-started particle from its
configured active-slot part. During travel it projects a textured quad,
applies the native cosine wobble, alternates the delayed RGB flash, and
advances position and the `pad` angle toward their targets. Integer division
precedes multiplication by the saved frame step. During the hold phase,
the quad stays at its target, stretches vertically, and fades.

The second pass copies targets into echo positions before activation,
renders only even update/particle parity, and expands uniformly. Its RGB
ramps up for the first half and down afterward. Division results remain
signed 32-bit values through doubling, then narrow into the common
halfword RGB locals before byte packet stores. Narrowing the quotient
first introduces masks absent from the native function.

Both passes preserve the native UV arithmetic, depth/flag guards, matrix
sequence, and packet submission. The footer advances elapsed time and
update count. It returns 0 before travel completion, 4 while the final
spacing/hold subtraction is **less than or equal to zero**, then 1 for the
first notification and 2 thereafter.

## Recovery evidence and limits

The native frame is 336 bytes. The local six-vector corner region models
the measured 48-byte allocation; only its first four vectors are accessed.
This does not establish an original six-element declaration or a purpose
for the unused region. No padding access or synthetic operation was added.
The initialization radius and later hold-age calculation share a scalar
lifetime, restoring the native saved-register allocation and target-Z spill.

The imported trigonometric entrypoints are bound to their actual native
calls at `0x800868A8` and `0x80086B38`. Similar SDK names in other regional
providers are not evidence of equivalent addresses or SDK versions.
No SDK-wide semantic rename is claimed.

Eight frozen source experiments, their source/header fingerprints, and
precise mismatch reasons are recorded in
`french-model-image40200-entry-attempts.csv`. The initial missing trig-map
linker setup was resumed from its unchanged compiled object, not counted
as a new compiler experiment. Final proof covers both relocated native
bodies and both complete 20480-byte production images, including their
original-sized C symbols. External reference types, flags, and sources
were not used.
