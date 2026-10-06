# French MODEL header97/227: staggered paths, rings and particles

Four complete 20480-byte runtime images are covered: model100 stages7/8,
argument0, and model150 stages9/10, argument1. Their source slices, command
words28000/28001, slot headers97/227 and independent hashes are recorded in
`french-model-variant97-instances.csv`.

Each image has one closed 4436-byte entry at offset4, a 296-byte stack frame,
63 ordered resident calls and24 distinct resident destinations. The canonical
GCC2.8.1/MASPSX2.81 profile is `gcc_2_8_1_g0_split`. Slot1 changes only the
measured entry and local-symbol addresses.

| Range | Ownership |
| --- | --- |
| `0x0000..0x0004` | Raw header |
| `0x0004..0x1158` | Sized linked C entry |
| `0x1158..0x1168` | Exclusive16-byte C unit-vector literal |
| `0x1168..0x11BC` | Raw84-byte three-image metadata view |
| `0x11BC..0x5000` | Unclassified raw suffix |

This adds17744 matching-C instruction bytes and64 source-literal bytes.
The63760 suffix bytes remain unclassified; descriptor interpretation does not
establish ownership or exhaustive code coverage for that suffix.

## Independently recovered layout and behavior

The36-byte descriptor selects primary and particle RGB, an attachment,
height/radius/particle size, random spread/lift, path/ring/particle counts,
three durations, stagger, spin and startup delay. Both selected descriptors
have positive observed divisors. Their counts fit32 path records,16 ring
records and8 particle offsets. Unused256-byte and64-byte state gaps remain
opaque; they are not inflated into additional meaningful vector arrays.

The constructor builds the ring and random spherical offsets, copies all
four opposite-slot halfwords, clears the target Y coordinate, and uploads
three images with modes1,1,2. Rendering uses textures0 and2. Initial setup
builds two16-point path rows, including the copied-and-overwritten second-row
Y stores. The first row emits staggered billboards; a separately timed ring
expands and rotates; particles rise from freshly copied opposite-slot
coordinates. Particle RGB increases with phase time rather than fading.

All three projection sites use `RotAverage4`, not `RotTransPers4`, and preserve
the depth/flag clipping. After frame advancement, negative phase time returns0.
Strictly after particle duration, the opposite slot's tint resets to128 and
the entry returns2. Through ring duration inclusive it returns4; subsequently
the completion byte yields1 once, then0.

## Source-shape recovery and acceptance

Sixteen frozen paired experiments are preserved in the attempt ledger.
Staging `growth_time = time * 3` prevents GCC from reassociating the height
product and introducing a load-delay NOP. Byte-valued RGB intermediates
recover the native color allocation. A shared scalar covers constructor
azimuth, ring height, radial scale and the particle frame. The beam frame
must remain separate: it overlaps a live flag pointer, and merging it grows
the frame to304 bytes. Constructor magnitude also remains independent of
phase time. No forced registers, artificial stores or fake dependencies are
used.

Acceptance compares complete unmasked images linked from the actual C objects
and disjoint raw owners, checks the sized entry and exclusive literal section,
and verifies all24 imported resident bodies against the exact resident ELF.
The family regression also guards loader slices, native instruction anchors,
all37 target-compiled layout values, descriptors, wrapper substitutions,
raw extents and the full attempt history. Complete-image identity does not
classify the preserved suffix.
