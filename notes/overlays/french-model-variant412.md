# French MODEL412 trails and retained quad renderer

The two complete `MODEL.MRG` images for model 10, stages 7 and 8, have
header words 412 and 562. Command 578000 initializes the secondary
handler with argument zero; subsequent controller updates pass `-1`.
[The instance table](french-model-variant412-instances.csv) records both
loader slices and their independent complete-image hashes.

The helpers at module offsets `0xB98..0x12C4` and `0x1EC8..0x2364`
are matching C: respectively 1,836 bytes/frame 1,808 and 1,180 bytes/frame
272 in each slot, using `gcc_2_8_1_g0_split` (GCC 2.8.1/MASPSX 2.81).
Their slot-1 wrappers only rename the functions. No assembly, forced registers, artificial stores,
fake dependencies, source-local external declarations, or compiler
changes are used. No other region's source changes.

## Ownership and remaining work

The entry at `0x4..0xB98` remains generated assembly (2,964 bytes).
All three inventoried functions have closed contiguous control-flow graphs. The entry
calls the helper at `0xA10` and passes its incoming context in the delay
slot. All 34 distinct external call targets are French resident
function starts; the helper uses the shared SDK declaration for
`GsGetLw` at `0x8008A428`.

The four-byte header, `0x12C4..0x1EC8` prefix, and `0x2364..0x5000`
suffix retain separate sized raw owners. The previous `0x3D3C`-byte
raw declaration no longer spans the newly owned C function.
The entry directly selects a 32-byte descriptor at `0x28A4`;
that bounded observation does not classify the remaining suffix as
data, prove it contains no code, or exclude it from further research.

Across both images there are six inventoried functions, four C instances,
and 6,032 C instruction bytes. The retained renderer adds two inventoried
functions and 2,360 C bytes without adding or resizing an image.
The two assembly owners cover 5,928 bytes; six raw owners cover 29,000.
It is not exhaustive French archive coverage.
The entry and uninventoried material remain work, not exclusions.

## Retained code discovery and quad renderer

Independent complete-image hashes and native CFG walks recover these
additional contiguous functions in both images:

| Module interval | Bytes | Frame | Current owner |
| --- | ---: | ---: | --- |
| `0x12C4..0x1874` | 1,456 | 256 | Raw; uninventoried |
| `0x1874..0x1EC8` | 1,620 | 296 | Raw; uninventoried |
| `0x1EC8..0x2364` | 1,180 | 272 | Matching C |
| `0x2364..0x27A8` | 1,092 | 304 | Raw; uninventoried |

Each CFG reaches every word of its measured span and closes with the
balanced stack return. The quad renderer has nine resident imports and
no local calls. No direct entry-call path or aligned image word pointing
to its start was found. This is a bounded image-local observation, not
proof of global unreachability or permission to exclude retained code.
The other three functions and material beyond `0x27A8` remain work.

Native instructions establish two `0x90`-byte records at context `0x12C0`,
with four rows of four `SVECTOR` points and a signed scale at record
offset `0x80`. These agree with the existing local `Family402Ring`
layout. The accepted French MODEL402 renderer supplies the same control
flow and expression structure; only independently measured context and
configuration offsets differ. No external reference types or compiler
claims are used.

The canonical source includes the unchanged MODEL402 body rather than
duplicating it. It loads shared declarations first, then binds the work-view
type to `Variant412RingView` and renames the function. This header-first
order keeps the original record declarations intact while selecting the
independently measured context layout. Both slot wrappers remain byte-exact.

The `POLY_GT4` packet is at `0x1978`, transform at `0x1AB0`, and velocity
at `0x1AD8`. Frame parity at `0x1B04` adds a scale/8 pulse. The second
record adds velocity times the signed halfword at `0x1B5E`, divided by
1024, to translation. Each record projects four quads, colours the first
three corners `(0,64,192)` and the fourth `(192,192,192)`, and sorts only
depths strictly between zero and 2048.

Updates are gated by selected-part word `0x1B48` plus one equalling the
configuration halfword at offset `0x10`, through the stored `G32`
pointer at `0x1B24`. The first record grows using elapsed word `0x1B0C`
and unsigned configuration duration `0x14`; subsequent phases use step
`0x1B14` and completion word `0x1B64`. The original unsigned division,
phase order and scale clamps are preserved. These are interpretations
of retained code, not evidence that the selected trail entry invokes it.
Twenty-three additional target-compiled constants verify the SDK types,
reused record, configuration fields and bounded `0x1B68` context view.

The first preparation failed during layout compilation because its
scratch header had the wrong include depth; neither slot was compiled.
That failure is preserved with no invented byte or difference counts.
After correcting the paths, the first measured candidate was exact in
both slots. A second paired calibration verified the shared-body wrapper
without changing the original MODEL402 source or header. Independent whole-image links retained the actual entry
assembly and accepted trail C, and checked all 34 resident callee input
objects before canonical integration.

## Independently recovered local views

The entry initializes six rows of original and moved `SVECTOR` points
and two `CVECTOR` colour planes. Its inner loop initializes 30 columns;
the physical row strides hold 31. The bounded trail prefix is:

| Field | Offset | Bytes |
| --- | ---: | ---: |
| Original points `[6][31]` | `0x0` | 1,488 |
| Moved points `[6][31]` | `0x5D0` | 1,488 |
| Angles `[31]` | `0xBA0` | 124 |
| Progress `[31]` | `0xC1C` | 124 |
| Inner colours `[6][31]` | `0xC98` | 744 |
| Outer colours `[6][31]` | `0xF80` | 744 |

The prefix ends at `0x1268`. Vector and colour row strides are 248 and
124 bytes. Twenty-one target-compiled constants check pointer and SDK
sizes, every field offset, and the timing and coordinate-link views.
The timing prefix ends at byte 28 and describes only the fields this
helper needs: capture start/stop words at offsets 20/24. Its link and
the six coordinate links use stored `G32` pointers.

The `POLY_G4` packet is at context `+0x1920`; translation words are at
`+0x1AC4/+0x1AC8/+0x1ACC`. The ignored initial `ratan2` reads halfwords
at `+0x1AEA/+0x1AE8`. Clock, count, timing link, coordinate links, and
completion are at `+0x1B0C`, `+0x1B18`, `+0x1B24`, `+0x1B30`,
and `+0x1B64`. The bounded static helper view ends at `+0x1B68`.
This is not an unconditional bound on variable-index accesses or a
proof of global allocation size or lifetime.

Resident pointers place the contexts at `0x80136000` and `0x80176000`.
Those bounded spans are below 2^32 and do not overlap the loaded model,
primary-handler, or secondary-handler images. The three offset-first
`u32` address calculations operate on these guest work addresses, never
native-stack objects. `src/port_ptr.h` explicitly retains guest storage
at retail addresses on native ports; `model_control.c` also uses integer
guest record-address cursors. This does not justify truncating arbitrary
native pointers or promise that this retail routine is portable as-is.

## Preserved retail timing and uninitialized-value behaviour

The selected descriptor's final three words are **100, 140, 118**.
Capture runs in the half-open clock interval `[100, 140)`. The count
advances only when current and previous animation-frame stamps differ;
the entry uses that same condition to advance the clock by
`Model_GetFrameStep`. The helper does not clamp the capture count.

Under a minimum step of two, that 40-unit window allows at most 20
captures, within the 30 initialized and 31 physical columns. Battle and
library startup use wait threshold one, normally publishing step two.
However, `Model_GetFrameStep` has only an upper clamp of six, and an
unconditional lower-bound proof has **not** been established. Step one
could permit 40 captures. The value 118 is not the capture stop, and
controller result four does not establish unconditional termination.
No unconditional array-safety claim is made and no new clamp is inserted.

Retail also computes `dx/dz` only on row five while calling `ratan2`
with those locals on every nonzero row. Rows one through four therefore
consume unwritten differences. The C preserves that instruction-level
quirk, including the calls and stores; it does not initialize them or
claim defined behaviour under arbitrary modern compilers.

## Reconstruction evidence and acceptance

[The attempt ledger](french-model-variant412-attempts.csv) preserves
18 paired trail source experiments, the retained renderer's preparation
failure, two paired exact candidates, and production terminals. A single merged original/destination
cursor changed allocation throughout the helper. Recovering a separate
destination cursor and computing the actual x displacement before address
formation recovered the retail frame and then all but 13 instructions.
Selecting the point before loading original coordinates and advancing
to the moved plane after the x store reduced this to three commuted
`addu` instructions.

Native pointer, byte-stride, and array-lvalue forms reproduced those
three operand-order differences. Offset-first guest-address expressions
left one. Keeping the row and selected point as separate meaningful
variables eliminated GCC's self-update commutation and matched both
slots. Adding the typed stored timing link preserved the exact result.
The final rows record canonical production acceptance, not merely
standalone helper equality.

Acceptance requires complete resident and overlay image identity plus
actual sized linked C ownership and retained assembly/raw owners.
Regression coverage checks the loader, commands, image hashes, CFGs,
caller and descriptor anchors, initialization strides and bounds,
target-compiled views, external callee starts, and slot wrapper.
