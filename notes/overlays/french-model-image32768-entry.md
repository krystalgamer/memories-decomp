# French MODEL32768 atlas beam and dual-origin fan

Part of #6460. The MODEL32768/32778 images, headers 804/901, now use pure C
for their complete 4,824-byte loader entries. Both retain the original
368-byte stack frame and use `gcc_2_8_1_g0_split`, the named GCC 2.8.1 and
MASPSX 2.81 pipeline.

## Measured storage

The slot-zero entry is at `0x8013B004`; the slot-one entry and its data
relocate by `0x40000`. The immutable suffix contains the initial VECTOR
at `0x8013C2DC`, two GsIMAGE records at `0x8013C2EC`, and the 44-byte
configuration at `0x8013C324`. None of that data is redefined or rewritten.

| Configuration offset | Observed use |
| --- | --- |
| `0x00..0x02` | Initial beam RGB |
| `0x03..0x05` | Initial flare and fan RGB |
| `0x06`, `0x08`, `0x0C` | Beam extent, height and roll |
| `0x10`, `0x12` | Flare size and fan-tip distance |
| `0x14`, `0x16`, `0x18` | Fan height, count and depth |
| `0x1C`, `0x1E` | Beam/flash and secondary-beam/flare fade amounts |
| `0x28`, `0x2A` | Drawing start and camera-change thresholds |

Other configuration bytes remain unclassified. The first row has extent
128, height 150, flare size 256, tip distance 384, twelve fan slices,
drawing start 90 and camera change 70.

The 68-byte work state holds its configuration pointer at `0x00`, two
SVECTOR fan tips at `0x04/0x0C`, beam phase at `0x14`, unsigned elapsed
time at `0x18`, ambient phase/value at `0x1C/0x20`, and three signed packed
texture words at `0x24`. Three beam-red, beam-green and beam-blue arrays
occupy `0x30..0x38`; flare RGB, flash, white fan intensity and colored fan
RGB occupy `0x39..0x40`.

These are local declarations measured from the native entry and retail
data, using the project's canonical SDK interfaces. No reference project
types, flags or compiler claims are involved.

## Preserved behavior

Construction chooses direction from the active slot, initializes both fan
tips, and packs the two images into three page/CLUT variants. The secondary
tip uses the native direction-doubling/addition before its distance product
and signed division by four.

Updates capture the frame step before setting its override. The camera
changes from the active to the opposing slot at the configured threshold,
while ambient light first dims and then restores. Drawing starts only at
the configured elapsed threshold.

The three beam phases preserve the native atlas coordinates, per-vertex
colors, blend modes and fading. A 320-by-256 flash fades independently.
The glow block draws full-size and half-size textured flares, followed by
twelve configured fan slices with colored and white triangles projected
from separate tips. Depth and GTE-flag rejection are retained where the
native entry performs them; the beam intentionally submits at depth zero.

Each fade occurs once per update, not once per captured frame step.
Elapsed time still advances by the captured step. Ambient transition
returns one exactly once; completion returns two only after ambient
restoration and disappearance of the flare RGB.

## Matching evidence

The [attempt ledger](french-model-image32768-entry-attempts.csv) records both
frozen compiler outputs and both complete production builds. The first
candidate already had the native frame and drawing structure, but its
constructor product reassociation added one instruction and its unsigned
texture view emitted four unsigned high-half reads. The second candidate
restored the measured direction expression and signed texture words.
All 1,206 instruction words then matched at both load addresses.

Both original assembly baselines and both integrated 20,480-byte images
were reproduced completely. The production ELF symbols retain their
original addresses, sections and 4,824-byte C ownership:

| Image | SHA-256 |
| --- | --- |
| MODEL32768 | `ddeab01516f7aaadca73034956f35fea5024143123b483efa3e04e752c2d694c` |
| MODEL32778 | `670489bbca58ea0cd33ce2ff9e736f08b703aaf0e3fd7d15c83fe2cec5f5ee08` |

Original headers, raw suffixes, image identities and unrelated registry
modules are unchanged. No assembly, source-local externs, fixed registers,
volatile controls, fabricated accesses or unused stack objects are used.
