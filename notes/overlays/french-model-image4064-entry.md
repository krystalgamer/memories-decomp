# French MODEL4064 shared particle-and-cone entry

French `MODEL.MRG` images 4064 and 4074 use headers 185 and 315 and load at
`0x8013B000` and `0x8017B000`. Their entry at offset `0x4` is 4,476 bytes.
Both original 20,480-byte assembly baselines were verified before recovery.

The complete native body independently establishes the same 38-byte command
row, 1,272-byte state and particle/burst/cone lifecycle as the already accepted
local `model_variant/variant168_entry.c`. This is local recovered C, not a
third-party reference declaration or toolchain claim. The French wrappers
reuse that body and its owning header with the authoritative
`gcc_2_8_1_g0_split` profile.

## Measured differences and default preservation

French data identities are the scale vector at `0x8013C180`, nine `GsIMAGE`
records at `0x8013C190`, and command rows at `0x8013C28C`. The slot-one wrapper
relocates only local function/data identities to `0x8017...`. The native French
initialization does not print the work-area size.

`MODEL168_EXTERNAL_SCALE` selects the canonical owning-header declaration
instead of the default source-defined constant. This keeps the French retail
vector in its existing raw-tail owner. `MODEL168_NO_SIZE_PRINT` suppresses only
the absent initialization print. Both switches are opt-in: existing USA
consumers still define their original local constant and print the work-area
size, with unchanged function and data bytes.

All four affected complete production images are reproduced: both new French
consumers and both existing USA consumers. French entries retain their original
4,476-byte owners; USA entries retain their original 4,488-byte owners. Retail
module headers, raw suffixes, image identities and unrelated metadata are not
replaced.

## Native layout and behavior

Command rows select by `command % 100`. Offsets `0x00..0x08` are the three
particle, burst and cone RGB triplets; `0x09` is the part byte. Signed
halfwords from `0x0A` are particle half-size, endpoint spread, burst half-size,
rise, burst-direction spread, cone radius, particle count, spacing, travel,
fade, burst duration, cone duration, particle delay and cone delay.

| State offset | Native accessed span |
|---|---|
| `0x000` | Command-row pointer |
| `0x004` | 64 particle vectors |
| `0x204` | 64 endpoint vectors |
| `0x404` | Four burst-direction vectors |
| `0x424` | Cone apex |
| `0x42C` | Eight rim vectors |
| `0x46C` | 24 stored vector pointers, grouped into eight triangles |
| `0x4CC` | Nine packed texture words |
| `0x4F0` | Completion byte and three unclassified bytes |
| `0x4F4` | Signed elapsed time |

Initialization clears particles, copies the opposing slot position for each
endpoint and adds native asymmetric random offsets. Four burst directions
start at the configured negative rise. Four large rim points use quarter-turn
angles; four smaller points use one-sixteenth radius and a half-quarter-turn
offset. The eight explicit pointer triples retain the measured alternating
rim connectivity. Texture indices 1 through 8 upload with mode 1; index zero
uploads with mode 2.

Before each particle's delay, its position tracks the selected active-slot
part. During travel/fade it scales from 512 using `time * 3584 / travel`,
animates the native eight-frame atlas, and interpolates each signed-halfword
position toward its endpoint while travel remains. Fade channels preserve
full-width multiply/divide before packet-byte storage.

Four burst quads per endpoint use a real signed-halfword intermediate:
direction times age is stored in the `SVECTOR` before division by burst
duration. Widening that expression across the division changes native
behavior and is not done. Their atlas uses 32-pixel U frames and V 174..205.
The eight-triangle cone tracks the same active part, rotates by `age << 6`,
scales by `(age << 12) / duration`, and fades the apex color while the other
two colors remain black. Cone submission requires nonnegative depth and
projection flags; the original unconditional textured-quad submissions are
preserved.

Elapsed time advances after drawing. The footer returns zero before travel
finishes, then 2 at/after `spacing * count + burst_duration`, 4 before
`spacing * count`, and a single 1 between those thresholds. The cone's delay
and duration do not replace these original completion conditions.

The measured native addresses and canonical SDK interfaces remain regional:
the matrix acquisition binding is `GsGetLwUnit = 0x8008A428`, trig bindings
are `ccos = 0x800868A8` and `csin = 0x80086B38`, and draw-mode submission is
`0x8004D5B8`. No source-local external declarations, fixed registers, volatile
tricks, fabricated arithmetic/dependencies or synthetic behavior harnesses
are introduced.

## Evidence

The original two frozen experiments are recorded in the adjacent attempt ledger. First,
the accepted local body calibrated exactly after substituting the measured
French identities, external retail scale vector and absent debug print.
Second, the opt-in shared-source wrapper reproduced the same 4,476 bytes,
all 1,119 instruction words and the 336-byte frame. Frozen inputs include
the shared implementation and owning header, not just wrapper text.

Both relocated native bodies and complete French production images match
their originals. Both existing USA default images also match their original
bytes and hashes with unchanged ownership. These proofs cover the shared
source's default behavior rather than inferring safety from preprocessing
alone. Retained repository policy checks are used locally; cloud validation
remains on the pull request.

Review of #8294 identified that the original shared `triangles[8][3]`
declaration did not permit a flat traversal from its first three-element row.
The member is now a genuine `triangles[24]` array, used directly by both
initialization and rendering. All 24 pointer values and their order remain
unchanged in `0x46C..0x4CB`; the renderer advances by three through the array
and finishes at its permitted one-past pointer. Texture storage remains at
`0x4CC`, and the complete state remains `0x4F8` bytes. Four additional
attempt records cover fresh complete-image matches for both French and both
USA consumers after this bounded-view correction, including the updated
shared implementation/header fingerprints. The opt-in switches and original
default scale/print behavior are unchanged.

| Complete image | SHA-256 |
|---|---|
| French 4064 | `3b83d5660677067dcb1096070a7aa051d3d2256c54ec6080a13b88e5bacea336` |
| French 4074 | `464676a6240b76bef90c2388243d27ac7b39872bac8bcf6230bb95cbd88b96be` |
| USA `model_variant_14_stage9_slot0` | `2d09a179ee89f43c9493e5042fe86c17fa31fecfec6fcb3d7e3a1a5755edf9fa` |
| USA `model_variant_14_stage10_slot1` | `8d2aafb720653e0e47cf932e158ea81c3cc47c9535f2abd3c2aa15fa316b53cd` |
