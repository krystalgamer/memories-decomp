# French MODEL412 trails

The two complete `MODEL.MRG` images for model 10, stages 7 and 8, have
header words 412 and 562. Command 578000 initializes the secondary
handler with argument zero; subsequent controller updates pass `-1`.
[The instance table](french-model-variant412-instances.csv) records both
loader slices and their independent complete-image hashes.

Only the helper at module offset `0xB98..0x12C4` is matching C:
1,836 instruction bytes and a 1,808-byte frame in each slot, using
`gcc_2_8_1_g0_split` (GCC 2.8.1/MASPSX 2.81). The slot-1 wrapper only
renames the function. No assembly, forced registers, artificial stores,
fake dependencies, source-local external declarations, or compiler
changes are used. No other region's source changes.

## Ownership and remaining work

The entry at `0x4..0xB98` remains generated assembly (2,964 bytes).
Both functions have closed contiguous control-flow graphs. The entry
calls the helper at `0xA10` and passes its incoming context in the delay
slot. All 30 distinct external call targets are French resident
function starts; the helper uses the shared SDK declaration for
`GsGetLw` at `0x8008A428`.

The four-byte header and the entire `0x12C4..0x5000` suffix retain raw
owners. The entry directly selects a 32-byte descriptor at `0x28A4`;
that bounded observation does not classify the remaining suffix as
data, prove it contains no code, or exclude it from further research.

This adds four inventoried functions, two C instances, and 3,672 C
instruction bytes. It is not exhaustive French archive coverage.
The entry and uninventoried material remain work, not exclusions.

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
18 paired source experiments. A single merged original/destination
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
