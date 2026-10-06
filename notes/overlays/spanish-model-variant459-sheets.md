# Spanish MODEL459 paired sheets

The independently reconstructed helper at `+0x3A24..+0x3FE0` is 1,468 bytes.
It contributes twelve matching C instances and 17,616 instruction bytes,
representing one unique routine, not twelve independently recovered routines.
Screening 6,169 accepted regional C entries found no same-sized accepted body.
No accepted French or other regional implementation supplied this source.

## Physical inventory

The exhaustive MODEL loader scan finds twelve physical loads but eleven
distinct complete-image hashes. Both physical instances of the repeated image
are retained rather than disappearing through hash deduplication.

| Model | Record | Stages | Sectors | Command | Descriptor count |
|-------|--------|--------|---------|---------|------------------|
| 47 | 47 | 7/8 | 13152/13162 | 625000 | 3 |
| 231 | 231 | 9/10 | 63956/63966 | 625003 | 3 |
| 238 | 238 | 7/8 | 65868/65878 | 625004 | 6 |
| 411 | 361 | 7/8 | 99816/99826 | 625001 | 3 |
| 417 | 367 | 9/10 | 101492/101502 | 625002 | 2 |
| 620 | 570 | 7/8 | 157500/157510 | 625004 | 6 |

The actual descriptors have 72-byte stride at `+0x4644`, selected by the
command's low three decimal digits. Headers are 459/609 in the two load slots.

Each image contains eight closed contiguous functions beginning at `+4`,
`+0x13E0`, `+0x19AC`, `+0x1FB8`, `+0x2604`, `+0x3224`, `+0x3A24`,
and `+0x3FE0`; raw data begins at `+0x4548`. The functions at `+0x13E0`
and `+0x1FB8` are not entry-reachable. All seven unselected functions per
image remain generated ASM: 84 retained instances, including 24 orphans.

## State, geometry, and lifetime

Entry `+0x121C` passes the original context captured in `s4` at `+0xC`,
after the unsigned start-time gate. Thirteen initialized `0x98`-byte sheet
records begin at `+0x2568`; constructor stride, four corner arrays, colors,
and size independently agree with the shared local `ModelVariantSheet`.
Observed descriptor counts are 2, 3, and 6, safely selecting `2*count+1`
records from this allocation.

The first group of sheets interpolates along the six-element VECTOR path
array from the corresponding origins. The second group stays at those
origins. The last sheet uses the signed-halfword center position instead.
Odd frames add one eighth of the signed sheet size to its scale.

Every sheet draws four quads through the second initialized `POLY_GT4`,
at `+0x2D94..+0x2DC8`. Three vertices use the inner color and the fourth
uses the outer color. Original UV, CLUT, texture-page, and packet setup
remain owned by the ASM constructor. Depth is multiplied by eight and
divided by ten before the nonnegative depth/flag gates and 16-bit narrowing.

Noncentral sheets grow using the unsigned descriptor time ratio in phase
zero, then follow the signed-halfword global scale. Clamping one sheet
changes the shared phase immediately, affecting later sheets in that same
invocation. The central sheet stays at size zero in phase zero, grows to
8192 in phase two once progress reaches 1024, and shrinks in phase three
until it clears size and advances the shared phase to four.

The private state is only the observed prefix through `+0x30E0`, not a
claim about the complete caller allocation. Stored pointers use `*G32`.

## Exact reconstruction

The initial independent body was 1,464 bytes. Initializing the packet pointer
after the ordering-table call restored the original value lifetimes and the
missing instruction. The resulting 1,468-byte body and an independent
second-slot compilation are exact with the authoritative
`gcc_2_8_1_g0_split` GCC 2.8.1/MASPSX 2.81 profile.

The natural frame is 264 bytes. There are no forced registers, inline
assembly, synthetic padding, or unused allocation controls. All twelve
complete 20KiB images match. The fifteen-row ledger preserves three
source/slot probes and twelve terminal image records.

Regressions cover 38 layouts, every physical load and descriptor, bounded
sheet and VECTOR indexing, original-context reaching definitions, packet
initialization and lifetime, all input/final function and data owners,
every selected relocation, and 38 resident bindings with loader/context
ownership. Terminal dependency fingerprints hash the body, private header,
shared model-variant header, and `gpu_packets.h`, in that order.
