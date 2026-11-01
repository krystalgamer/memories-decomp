# MODEL variant modules

`Model_LoadMonsterMerge` loads four more stages per model after the MODEL
primary. Each stage is 10 sectors at `record * 276 + 180`, `+ 190`, `+ 200` and
`+ 210`. Slot 0 loads at `0x8013B000` and slot 1 at `0x8017B000`. This note
calls these images the model variants. Sixty-two slot-0 images are registered;
see [Registered images](#registered-images).

The older USA headers 71/201 are registered separately using the shared
French header-88 C and GCC 2.8.1. See the
[USA header-71 integration](usa-model-variant71.md) for its fourteen images,
loader and callee evidence, and preserved unclassified suffixes.

USA images under headers 72/202, 110/240, 111/241, 118/248, 119/249, 164/294, 172/302, 176/306, 627/694, 634/701,
647/714, 654/721, 673/740, 752/849, 754/851, 756/853, 766/863, 779/876, 827/924 and 828/925 build from North American C; see [USA MODEL headers 72 and 202](usa-model-variant72.md),
[USA MODEL headers 110 and 240](usa-model-variant110.md),
[USA MODEL headers 111 and 241](usa-model-variant111.md),
[USA MODEL headers 118 and 248](usa-model-variant118.md),
[USA MODEL headers 119 and 249](usa-model-variant119.md),
[USA MODEL headers 164 and 294](usa-model-variant164.md),
[USA MODEL headers 172 and 302](usa-model-variant172.md),
[USA MODEL headers 176 and 306](usa-model-variant176.md),
[USA MODEL headers 627 and 694](usa-model-variant627.md),
[USA MODEL headers 634 and 701](usa-model-variant634.md),
[USA MODEL headers 647 and 714](usa-model-variant647.md),
[USA MODEL headers 654 and 721](usa-model-variant654.md),
[USA MODEL headers 673 and 740](usa-model-variant673.md),
[USA MODEL headers 752 and 849](usa-model-variant752.md),
[USA MODEL headers 754 and 851](usa-model-variant754.md),
[USA MODEL headers 756 and 853](usa-model-variant756.md),
[USA MODEL headers 766 and 863](usa-model-variant766.md),
[USA MODEL headers 779 and 876](usa-model-variant779.md),
[USA MODEL headers 827 and 924](usa-model-variant827.md) and
[USA MODEL headers 828 and 925](usa-model-variant828.md).

## Compiler

The helpers registered here were not built with the gcc 2.8.1 that builds the
rest of the game. (The Spanish variant helpers in
`src/overlays/spanish_model_variant/` do match under `gcc_2_8_1_g0_split`, so
this is a statement about these functions, not about every variant.) They end in `addiu $sp, $sp, N; jr $ra; nop`. gcc 2.7.2 emits
that epilogue as reorder-mode text, and the assembler fills the delay slot
with a `nop`. gcc 2.8.1 emits the epilogue as RTL, and reorg moves the stack
adjustment into the slot. For the four header-397 helpers, gcc 2.8.1 output is
one word shorter than the retail function, and 21 to 24 words differ in all.

The `gcc_2_7_2_cdk_g0` profile uses the decompals/old-gcc 0.17
`gcc-2.7.2-cdk` release (`cygnus-2.7.2-970404`), installed by
`make compiler-272-prebuilt` from the pinned
`tools/bootstrap/old_gcc_272_prebuilt.json`. Apart from the compiler path it
is identical to `gcc_2_8_1_g0`: same flags, same maspsx arguments.

Four helper bodies were measured against their retail words with each 2.7.2
build before registration (quad and rings in their header-405 copies, spokes
and sheets under header 397):

| Helper | Instructions | `gcc-2.7.2-cdk` | `gcc-2.7.2-psx` |
|---|---:|---|---|
| quad | 218 | match | 4 words differ |
| rings | 224 | match | 24 words differ |
| spokes | 195 | match | 27 words differ |
| sheets | 314 | match | match |

PsyQ 4.1's `CC1PSX.EXE` (the same `cygnus-2.7.2-970404`) also matches all four,
run under wine.

Negative control: with `gcc_2_8_1_g0` selected, `make match-overlays` fails
for `model_variant_1_pos0_slot0` with "rebuilt module does not match its
input", and each of the four header-397 helpers compiles one word short.
`make overlays` and `make verify-overlays` pass under either profile, because
neither of them compiles C.

The Japanese MODEL images that reuse these US sources select the same profile
as the US image at the same sector; see
[Japanese MODEL images built from the US sources](japanese-model-us-layouts.md).

Eight European images also select `gcc_2_7_2_cdk_g0`, for the header-447
rings helper at image offset `0x3520`. They are the French, Spanish, German
and Italian `model_image_model_109772_8013b000` and
`model_image_model_109782_8017b000` images; the German and Italian images are
byte-identical to the Spanish ones. No loader entry or resident reference to the helper has been found,
and it calls an older resident layout.
Under gcc 2.8.1 the same source is two words longer and 96 words differ,
including the filled epilogue slot. Under 2.7.2 it matches exactly. See
[French header-447 image rings](french-model-image447-rings.md).

The header-372 entry (`model_variant/variant372_entry.c`, 921 instructions;
`variant372_entry_slot1.c` builds the header-522 images) is a whole function
built by the same gcc 2.7.2. It ends in `jr $ra; nop`, and its struct copies
use `v1`, `a0` and `a1` as scratch registers, as 2.7.2 allocates them. The French header-389 and header-539 images build the
same source with `gcc_2_8_1_g0_split`: the only source difference there is the
CLUT column passed to both `GetClut` calls (640 instead of 512). The two
helpers after it stay generated assembly in the North American images.

The six North American header-460/610 images build their entry and both
helpers from `model_variant/variant460_entry.c` under `gcc_2_7_2_cdk_g0`; the
helpers are the Spanish header-477 panels and quads sources included with
renamed addresses. Under gcc 2.8.1 those helpers swap `addu` operands and
end one word longer, and the entry is one word longer.

The two North American header-413/563 images build their entry and the
swarm helper from `model_variant/variant413_entry.c` under
`gcc_2_7_2_cdk_g0`; the helper is the French `image132118_swarm.c` included
at the North American address. The French header-430/580 images compile the
same entry under `gcc_2_8_1_g0_split` (0x12E8 bytes against the North
American 0x12EC) and keep their 2.8.1 helper.

The two North American header-437/587 images build their entry and the
blooms helper from `model_variant/variant437_entry.c` under
`gcc_2_7_2_cdk_g0`; the helper is the French `image22822_blooms.c` included
at the North American address. The French header-454/604 images compile the
same entry under `gcc_2_8_1_g0_split` and keep their 2.8.1 helper.

The two North American header-451/601 images build their entry and the
swarm helper from `model_variant/variant451_entry.c` under
`gcc_2_7_2_cdk_g0`; the helper is the French `image134612_swarm.c` included
at the North American address. The French header-468/618 images compile the
same entry under `gcc_2_8_1_g0_split` and keep their 2.8.1 helper.

The two North American header-366/516 images build their entry from
`model_variant/variant366_entry.c` and four of their five helpers (strip,
ribbons, quads and rays) from the French header-383 helper sources, all under
`gcc_2_7_2_cdk_g0`; the helper at `+0x1EC8` stays generated assembly. The
French header-383/533 images compile the same entry under
`gcc_2_8_1_g0_split` with `VERSION_FRENCH`, which moves the palette to VRAM
x 640, beside their existing 2.8.1 helper C.

The two North American header-411/561 images (model 507, stage 9 slot 0 and
stage 10 slot 1) build their entry and both helpers from
`model_variant/variant411us_entry.c` under `gcc_2_7_2_cdk_g0`. The entry is
the header-412 entry with a 32-sprite swarm at `work + 0x2FC` in place of the
six flares, 384-unit trail sprites, a `0x38`-byte config and no height offset
on the root part. The helpers are the French header-428 swarm and quads
sources included at the North American addresses (the swarm moves from
`0x8013C308` to `0x8013C30C`); under 2.7.2 both match unchanged.
The French header-428/578 images compile the same entry under
`gcc_2_8_1_g0_split` (0x1304 bytes, CLUT column 640, data at `0x8013CD94`)
through `french_model_variant/variant428_entry.c`, and build the two helpers
as their own 2.8.1 segments from `variant428_swarm.c` and `variant428_quads.c`.

The two North American header-430/580 images build their entry from
`model_variant/variant430_entry.c` and their swarm helper from
`model_variant/variant430_swarm.c` (`_slot1.c` for slot 1) under
`gcc_2_7_2_cdk_g0`; the helper is the French `variant447_swarm.c` included at
the North American address. The French header-447/597 images compile the
same entry under `gcc_2_8_1_g0_split` with `VERSION_FRENCH`, which moves the
palette to VRAM x 640, and build `variant447_swarm.c` under 2.8.1.

The two North American header-386/536 images build their entry from
`model_variant/variant386_entry.c` under `gcc_2_7_2_cdk_g0`, beside the
existing sheets and chains helper C; the helper at `0x8013BF3C` stays generated
assembly. The French header-403/553 images compile the same entry under
`gcc_2_8_1_g0_split` with `VERSION_FRENCH`, which moves the palette to VRAM
x 640, beside their existing 2.8.1 helper C.

The two North American header-454/604 images build their entry from
`model_variant/variant454_entry.c` and both quad helpers from
`model_variant/variant454_quad_pairs.c` and `variant454_quads.c` (`_slot1.c`
for slot 1) under `gcc_2_7_2_cdk_g0`; the helpers are the French
`variant471_quad_pairs.c` and `variant471_quads.c` included at the North
American addresses. The French header-471/621 images compile the same entry
under `gcc_2_8_1_g0_split` with `VERSION_FRENCH`, which moves the palette to
VRAM x 640, and build the two quad helpers under 2.8.1.

The six North American header-435/585 images (models 1, 360 and 550) build
their entry from `model_variant/variant435_entry.c` (`_slot1.c` for slot 1)
under `gcc_2_7_2_cdk_g0`, beside the C sheets and lines helpers. The
streamers helper at `+ 0x1524` is the French `variant452_streamers.c` with
`MODEL_VARIANT452_STREAMERS_DIRECT_TIP`, and the rays helper at `+ 0x23AC` is
the North American `variant425_rays.c` with its `MODEL_VARIANT425_RAYS_*`
selectors, from `variant435_streamers.c` and `variant435_rays.c` (`_slot1.c`
for slot 1) under the same profile. The entry wrapper includes
the French `variant452_entry.c` with `MODEL_VARIANT452_CLUT_X` set to 512 and
`MODEL_VARIANT452_DATA` pointed at the North American data symbol. The French
source defaults to x 640 and to `D_8013D3E4 + 0xA30`, since its data symbol
starts at a helper that the French module still keeps in its unclassified
tail, and its header-452/602 images keep building it under 2.8.1.

The four North American header-318/468 images (models 44 and 558) build
their entry from `model_variant/variant318_entry.c` the same way, from the
French `variant335_entry.c` with `MODEL_VARIANT335_CLUT_X` 512 and
`MODEL_VARIANT335_DATA`, whose French default is `D_8013D8EC + 0x6D0`. The
streamers helper at `+ 0x20B4` is the French `variant335_streamers.c` with
`MODEL_VARIANT335_STREAMERS_DIRECT_PREVIOUS`, the header-456 lever: a zero
step on the word after the advanced rotation, and the closing segment's
previous point indexed directly instead of through a walking pointer. The
sheets and rings helpers are C. The ribbons helper at `+ 0xB9C` is the
French `variant335_ribbons.c` with its `MODEL_VARIANT335_RIBBONS_*`
selectors, from `variant318_ribbons.c` (`_slot1.c` for slot 1). The six-streamer
helper at `+ 0x28D0` has no French counterpart; it is new C in
`variant318_short_streamers.c` (`_slot1.c` for slot 1), whose commented dead
`twist` step reproduces the retail register allocation. The French header-335/485 images keep building the entry
and the streamers under 2.8.1.

The two North American header-455/605 images build their entry from
`model_variant/variant455_entry.c` under `gcc_2_7_2_cdk_g0`, beside the
existing blooms, mesh and sheets helper C; the helper at `0x8013CD3C` stays
generated assembly. The French header-472/622 images compile the same entry
under `gcc_2_8_1_g0_split` with `VERSION_FRENCH`, which moves the palette to
VRAM x 640, beside their existing 2.8.1 helper C.

The two North American header-409/559 images build their entry from
`model_variant/variant409_entry.c` under `gcc_2_7_2_cdk_g0`, beside the
existing mesh, sheets and funnel helper C; the helpers at `0x8013C38C`,
`0x8013CDAC` and `0x8013DB50` stay generated assembly. The French
header-426/576 images keep their separately recovered entry.

The two North American header-426/576 images (model 62) build their entry
from `model_variant/variant426_entry.c` and their streamers helper from
`model_variant/variant426_streamers.c` (`_slot1.c` for slot 1) under
`gcc_2_7_2_cdk_g0`, beside the rings, mesh, sheets and bands helper C. The
ribbons helper at `+ 0x1AC4` is the French `variant443_ribbons.c` with
`MODEL_VARIANT443_RIBBONS_PACKED_SCREEN`, and the webs helper at `+ 0x4594`
is the North American `variant405_webs.c` with the
`MODEL_VARIANT405_WEBS_*_SHIFT` work-area offsets for this header, both under
`gcc_2_7_2_cdk_g0` from `variant426_ribbons.c` and `variant426_webs.c` in
both slots. The two helpers are `FrenchModelVariant443_DrawRibbons` and
`ModelVariant405_DrawWebs` in every image (see
[Per-image symbol bindings](#per-image-symbol-bindings)). The entry and streamers wrappers
include the French header-443 `variant443_entry.c` and
`variant443_streamers.c`. The entry wrapper sets `MODEL_VARIANT443_CLUT_X` to
512, points `MODEL_VARIANT443_DATA` at the North American data symbol (the
French default is `D_8013F5A8 + 0x554`, past a helper still in the French
unclassified tail) and defines `MODEL_VARIANT426_ENTRY_PAIRED_SCREEN`, which
reads both projected coordinates of the three-point loop before storing
them. Under 2.8.1 that order is two words shorter, so the French default
keeps the interleaved reads. The streamers source gains
`work->rotation_z += 0;`, which 2.7.2 keeps as a load and 2.8.1 drops. The
French header-443/593 images keep building both under 2.8.1.

## Header families

The first word of a variant image is a header id. Every image with a given
header that has been checked has the same text: the same functions at the same
addresses, byte for byte. Only the data after the text differs. So the C files
are named by header, as the Spanish variant files are
(`src/overlays/spanish_model_variant/variant432_*.c`), and each registered image
of a header reuses them unchanged.

The same helper body also occurs under other headers at other addresses. Those
copies are not byte-identical: the work-area offsets differ. For example, rings
reads its ring array at `ctx + 0x15AC` under header 405 and at `ctx + 0x72C`
under header 397, and quad reads its divisor from `+ 0x20` of a pointed-to
record under header 405 and from `+ 0x24` under header 397. The header-397
rings and quad files are the header-405 sources with those constants replaced,
and the header-418, 428 and 404 files are the header-397 ones with theirs
replaced; nothing else changed. Under header 428 the two fields of the
pointed-to record that sheets and quad divide by move from `+ 0x20`/`+ 0x24`
to `+ 0x1C`/`+ 0x20`. Each new header therefore needs its own copy with its own
offsets.

| Header | Text end | C functions | Assembly functions | Images |
|---:|---|---|---:|---:|
| 405 | `0x3850` | entry `0x8013B004`, halo `0x8013C12C`, veils `0x8013C620`, bands `0x8013CD04`, sheets `0x8013D410`, webs `0x8013D8FC`, spokes* `0x8013DE54`, rings `0x8013E168`, quad `0x8013E4E8` | 3 | 2 |
| 397 | `0x2DE4` | bands `0x8013C22C`, sheets `0x8013C994`, webs `0x8013CE7C`, spokes `0x8013D3F0`, rings `0x8013D6FC`, quad `0x8013DA7C` | 1 | 12 |
| 418 | `0x396C` | spiral `0x8013C088`, ribbons `0x8013D238`, sheet `0x8013CAA4`, webs `0x8013CE50`, bands `0x8013D86C`, spokes `0x8013DF78`, rings `0x8013E284`, quad `0x8013E604` | 1 | 13 |
| 428 | `0x359C` | bands `0x8013C038`, webs `0x8013CC94`, sheets `0x8013C7AC`, spokes `0x8013D1FC`, rings `0x8013D508`, quad `0x8013D888`, spiral `0x8013DBF0` | 2 | 6 |
| 404 | `0x40FC` | entry `0x8013B004`, ribbons `0x8013C1D4`, spiral `0x8013C864`, bands `0x8013D4F8`, sheets `0x8013DCA4`, webs `0x8013E18C`, spokes* `0x8013E700`, rings `0x8013EA14`, quad `0x8013ED94` | 0 | 6 |
| 416 | `0x30D0` | bands `0x8013C054`, webs `0x8013CC68`, spokes* `0x8013D6D4`, rings `0x8013D9E8`, quad `0x8013DD68` | 4 | 2 |
| 425 | `0x43FC` | entry `0x8013B004`, spiral `0x8013C568`, rays `0x8013CF14`, ribbons `0x8013DCC0`, webs `0x8013D8F8`, bands `0x8013E2F4`, spokes* `0x8013EA00`, rings `0x8013ED14`, quad `0x8013F094` | 0 | 2 |
| 448 | `0x43E4` | entry `0x8013B004`, radial `0x8013C254`, fan `0x8013C678`, orbit `0x8013D638`, webs `0x8013DB58`, spokes `0x8013E0BC`, rings `0x8013E3C8`, quad `0x8013E748`, spiral `0x8013EAB0` | 0 | 2 |
| 423 | `0x2B38` | entry `0x8013B004`, bands `0x8013BF18`, sheets `0x8013C6E4`, webs `0x8013CBDC`, spokes* `0x8013D13C`, rings `0x8013D450`, quad `0x8013D7D0` | 0 | 2 |
| 414 | `0x2F58` | entry `0x8013B004`, webs `0x8013BF20`, fan `0x8013C348`, bands `0x8013C7DC`, sheets `0x8013D448`, spokes `0x8013D8CC`, rings `0x8013DBD8` | 0 | 1 |
| 401 | `0x3124` | entry `0x8013B004`, bands `0x8013BFE4`, sheet `0x8013C764`, webs `0x8013CCA4`, rays `0x8013D210`, spokes* `0x8013D728`, rings `0x8013DA3C`, quad `0x8013DDBC` | 0 | 1 |
| 443 | `0x2888` | entry `0x8013B004`, ribbons `0x8013BD00`, sheets+ `0x8013C808`, strand `0x8013CD84`, streamers `0x8013D0C8` | 3 | 12 |

`sheet` (header 418) is a one-sheet form of sheets: a single `ModelVariantSheet`
at the variant origin whose size follows the two phases of the timing record at
`work + 0x1B74` rather than the sheets path. It was written from header 418's copy.
Header 418's entry at `0x0004` (`variant418_entry`, 1057 instructions) has the
French header-439 entry's shape without screen rings or curtains: three webs
and twelve spiral arms open the work area, then one band, one sheet, six rings,
four spokes and a fan. Its 0x44-byte config adds the header-338 matrix setup
when its mode is 1. As in header 428, the spoke loop steps its angle as
`base_angle + outer * 512` (which leaves the target's `li v1,4096` in the loop
for the later `0x1B8C` store), and the fan's `angle` keeps its register only
with the per-frame `angle = ratan2(...)` stored to `angles[0]`.

`webs` (header 418, 250 instructions) draws three 0x260-byte `ModelVariantWeb`
records as 6x6 `GsGLINE` segments from each near grid point to the far one,
fading the colour once the web's scale passes `0x1000`. Each scale grows by
`step * 0xC0` to `0x2000` and wraps until phase 4; when all three have stopped
in phase 4 the phase moves to 5. It opens with four `ratan2` calls whose
results are unused, and its loop counters are `s16`, which is what puts the
web counter and the done flag on the stack as the target has them.
Header 425 has a 242-instruction form at `0x28F8` (`variant425_webs`): 5x6
grids in 0x200-byte `ModelVariantWebSmall` records, projected with
`RotTransPers3`, faded past `0x800` and grown by `step * 128` to `0x1000`.
Header 398 has a 332-instruction form at `0x2514` (`variant398_webs`): the
header-397 phased webs over records at `work`. Its phase 0 places web `i` at
`(i << 12) / 3` less the progress `(now * 3 << 12) / end` of the timing record
at `work + 0x1A00`, plus `0x1000`, wrapped by `0x1000` (two statements, as in
the other staggered forms). In phase 4 each web stores the `done` flag, which
is set once and never cleared, into its own record at `+ 0x198` before the
phase moves to 5.
Header 398's entry at `0x0004` (`variant398_entry`, 1197 instructions) is the
French header-439 entry (US header 422) with four changes: each 0x1E4-byte
framebuffer ring stores a colour per point, cycling red, orange, yellow, green,
cyan, azure, blue and magenta by `j % 8`, with rows at radii `>> 4`, `* 7 >> 6`
(depth `-192`) and `* 5 >> 5` (depth `-384`); the curtains use radii `>> 4` and
`* 5 >> 6` at depth `-64`; it uploads five images (`+ 0xC4`, `+ 0x8C`, `+ 0x1C`,
`+ 0x38`, `+ 0x54` of the data at `0x2FE0`); and once the start frame is reached
it calls both `0x202C` and `0x12B8`, while the tint fades before phase 5. The
ring loop matches only with `angle = base_angle + outer * 512` as its step,
which gives the ring pointer and the angle the target's registers.
Header 405's form at `0x28FC` (`variant405_webs`, 342 instructions) is the
header-397 phased webs over records at `work + 0xDD4`. Its phase 0 places web
`i` at `(i << 12) / 3` less the progress `((now - start) * 3 << 12) / (end -
start)` of the timing record at `work + 0x1DC8`, plus `0x1000`, wrapped by
`0x1000`. That assignment matches only as two statements, the progress less
`0x1000` stored to the scale first. A line is sorted whenever its depth is
positive.
Header 405's veils at `0x8013C620` (`variant405_veils`, 441 instructions)
draw five 0x1A0-byte veils at `work`, each three rows of seventeen points with
radii `rsin(scale)` times 224, 320 and 416 (over 4096). The outer two rows are
lifted by `(u32)(rsin(scale) * 3) >> 8`. Each strip is a semi-transparent
`POLY_GT4`, textured from the screen half it lands on as in the header-398
veils, and lit white at the inner row and `(0, 0x80, 0xFF)` at the outer. Like
the header-459 curtains, the frame reserves 24 bytes before the two colour
locals. The veil pointer is taken from `ctx` before the `work` copy, and both
deadline tests are written `record[n] <= now`.
Header 405's halo at `0x8013C12C` (`variant405_halo`, 317 instructions) draws
five rings of seventeen points from the block at `work + 0x820`. Each ring has
a top and a bottom row, hung `-(height * 768 / 1024)` below the ring. The two
heights grow by `step << 5` up to 0x400, and wrap (keeping their gap) until
the deadline at `+ 0x28` of the timing record. The strips are textured
`POLY_GT4`s with `u = 16 j`. Their colours are staged per ring in six stack
byte arrays: grey `0x80` until a height passes 0x300, then fading with
`(0x400 - height) / 2`. The clamp of the heights is an `if / else if / else`
chain, the bottom row's fade is held in its own local like the top row's, and
the deadline test is written `record[0x28] <= now`.
Header 423's form at `0x1BDC` (`variant423_webs`, 344 instructions) is the
header-397 phased webs over records at `work`, projected with `RotTransPers3`
and grown by `step * 0x180` from phase 2. Its phase 0 places web `i` at
`(i << 12) / 3` less the progress `((now - start) * 3 << 12) / (end - start)`
of the timing record at `work + 0xF00`, plus `0x1000`, wrapped by `0x1000`;
that assignment matches only as two statements, the progress less `0x1000`
stored to the scale first.
Header 442 (models 47, 238, 411 and 620 at `+ 180`, 231 and 417 at `+ 200`)
and its slot-1 header 592 are the Spanish header-459 image built under gcc
2.7.2: all eight functions (entry, warp, curtains, rays, spiral, bands, sheets
and lines) build from `src/overlays/spanish_model_variant/variant459_*.c`
through `variant442_*.c` wrappers that rename the image-local symbols. The
entry wrapper sets `MODEL_VARIANT459_CLUT_X` to 512 (PAL loads the palette
from 640). The curtains fade branch re-reads the timing record into a fresh
`start` local; that one form matches both compilers, where the earlier shared
`rec` load only matched gcc 2.8.1.
Model 712's header-459 image (and its header-609 slot-1 image) has a
243-instruction form at `0x2064` (`variant459_webs`): the header-418 grids
with header 425's fade and growth, sorted whenever `otz > 0`.
The same image's grid at `0x2800` (`variant459_grid`, 356 instructions) draws
a `Variant459Grid` at `work + 0xFF8`, nine rows of seventeen points, as
`POLY_GT4` strips shaded per row and sorted when `otz >= 0 && flag >= 0`.
Before phase 5 the rows fade from blue to red by the scale at `work + 0x27FC`,
which grows by `step << 9` to `0x2000`, swings on `rcos` around `0x1800` in
phase 3, grows to `0x4000` in phase 4 and fades out in phase 5.
Its spiral at `0x1714` (`variant459_spiral`, 596 instructions) is the French
header-476 spiral (`variant476_spiral`) built under gcc 2.7.2 at the North
American address; the code is identical.
The image's entry at `0x4` (`variant459_entry`, 1239 instructions, written for
the North American stream; `variant459_entry_slot1` builds the header-609
image) fills the state on the setup command: the three webs, sixteen glows,
the sheet, the two grids (nine rows of seventeen points at `work + 0xFF8`,
nine rows of nine at the screen grid), five curtains and the packets, reading
its 0x38-byte config and five image descriptors from the data at `0x3338`.
Otherwise it tracks the part with the header-337 camera code, projects it, and
runs the five draw helpers through the config's start, second and fade frames.
What the retail code needs: one `packed` word for the first four uploads with
the ray's tpage and clut taken before the fourth call, and a fresh `packed2`
for the veil upload of phase 5, whose clut goes through the last halfword of
the 64-byte stack pad (`sh`/`lhu` at `sp + 78`) and whose tpage is held in
`frame` across `SetPolyGT4`; the grid rows indexed from `row` inside the loops
(`rowp = grid + row * 17`, `rsin(row * 256)`) so that the row pointers and the
angle are the loop pass's own registers, spilled after the colour pointers;
one counter `n` for the sheet, webs, curtains and the zeroes loop and `f12`
for the glows; `ang` for the grid's inner angle and the webs' outer angle with
a separate `angle` for the webs' inner angle, and `f99` declared before `f71`;
the `4096` of `field_27D8` stored before the halfword `1024` so the constant
keeps `v1`; and the comparisons written `config->start <= frame`,
`config->second <= frame` and `fade_end <= frame`. Both images rebuild
byte-identical to retail; every function of both is now C.
Header 397 has a 349-instruction form at `0x1E7C` (`variant397_webs`): 4x6
grids in 0x1A0-byte `ModelVariantWebNarrow` records, driven by the phase at
`work + 0xF78`. In phase 0 each scale shrinks by `step * 0xC0` from `0x1000`
and the colour fades in on the far end; in phase 1 web `i` waits at
`-(i << 13) / 3`; from phase 2 the webs sit on the `s16` position at
`work + 0xF0C`, grow by `step << 8` to `0x2000` with the colour on the near
end, and the phase moves to 5 as in the header-418 form. The timing record at
`work + 0x5FC` stops the phase-0 wrap once its word at `+ 0x88` passes `0x800`.
Header 404 carries the same body at `0x318C` (`variant404_webs`) with its own
work-area offsets.
Header 448's form at `0x2B58` (`variant448_webs`) keeps the records at
`work + 0x6F8`, shrinks by `step * 0xE0` in phase 0, and sorts a line whenever
its depth is positive.
Header 448's entry at `0x0004` (`variant448_entry`, 1172 instructions) is the
header-404 entry with a 0x34-byte config (start at `+ 0x1C`; the array starts
at `0x43E4 + 0xFC` and the entry indexes it by command) and the records of the
helpers above: one 0x128-byte radial record at `work`, twelve 0x7C-byte spiral
arms, three webs at `+ 0x6F8`, three 0x208-byte fan units at `+ 0xBD8`, an
orbit at `+ 0x11F0`, six rings, four spoke rings and a quad, then the packets
at `+ 0x1948`. The five image uploads read their descriptors from the data at
`0x43E4`. It matches only with the zero stores of the radial record's centre
point inside the seventeen-step loop (after the loop's label in the retail
code), which gives the radial base pointer its register; with the fan's angle
written inline as `base - j * 256 - k * 512` (a local holding it takes the
wrong register); and with a named `one = 1` set in the fan loop's initialiser
so that the hoisted constant precedes the base copy. The call-free record
loops of the fan, orbit and arm records share one counter, the call-crossing
ones another.

Header 448's fans at `0x1678` (`variant448_fan`, 1008 instructions;
`variant448_fan_slot1` builds the header-598 images) draw the three 0x208-byte
`Variant448FanUnit`s at `work + 0xBD8`: per unit, two arms of three fans, each a
two-point spine and its copy moved `0x20` along the view angle; the arm colours
are faded by the growth amounts into a stack table after `rot`; the amounts
grow by `step << 5` up to `0x400` (stage 1) or shrink by `step << 6` to zero
(stage 2), and the stage sum moves `work + 0x1B3C` from 1 to 2 and from 5 to 6;
then each spine is projected (`RotTransPers4` into `status[i][t][k]`) with its
screen angle, width and offset, and one POLY_GT4 per fan is sorted when its
depth and status are not negative. In mode 3 the amounts are reset to a
staggered start and the mode becomes 4. What the retail code needs, each form
measured against the alternative:
- the flag test at the start is `if ((flag & 1) == 0) { i = 0; } else { s32 s =
  work - (u8 *)poly; work = (u8 *)poly + s; }` with `poly` set before it and
  `unit` after it. The dead `i = 0` disappears in flow; the other arm survives
  until the combiner turns it into `work = work`, so reload emits
  `lw t9,384(sp)` for it and deletes the store, and reorg then skips the
  redundant reload at the join. This leaves the arm's dead reload that the
  retail code has, and the spill-register parity of the rest of the function;
- the zone colours of phase 2 use one name per channel in the faded arm (`{
  s32 a, b, d; ... }`) and, in the full arm, a shared `u` with one `w` per
  channel; the full arm's first channel is `x = i2 + ip0 + j; c =
  unit->cb[j][0]; do { *(u8 *)x = c; } while (0);`, whose loop notes keep the
  next channel's address after the store; `jj = j` is set after the fade
  amount;
- the status test reads `status[i][t][k]` itself, which makes the
  `+ 168` from `&rot` a hoisted invariant;
- the j == 1 arm of the projection reads `(s16)unit->sb[t][k][j]`, the
  colour pointers of phase 4 are `i * 2 + w1 + j` with `w1`, `w2`, `w3` set
  once each, and phase 1 initialises `tangle` in the arm loop
  (`for (t = 0, tangle = 0; ...)`).
All four images rebuild byte-identical to retail; every function of the
header-448 and header-598 images of models 108 and 573 is now C.

Header 425's entry at `0x0004` (`variant425_entry`, 1117 instructions; models
259 and 630, both slots) sets up the sheet, the sixteen spiral arms, the
sixteen rays, the 5x6 webs and the ribbon, band, spoke-ring and quad records of
the helpers above, and reads its config and image descriptors from the data at
`0x43FC`. It matches only under these conditions:

- The web loop's `outer = 0;` is a statement of its own above
  `far_scale = 512;`. The longer live range lowers that counter's allocation
  priority, so it spills as in the retail code.
- `work->frame_count++` comes before `Model_GetSlotAnimationFrame`.
- The spoke loop ends with `n = 4096;`, and the post-loop store is
  `work->field_272C = n;`. The retail code sets the constant inside the loop,
  in the back branch's delay slot, and stores it after the loop.
- The entry header carries its own views of the spiral arm and ray records
  (`Variant425EntryArm`, `Variant425EntryRay`), with the colour rows it fills
  named. They have the helpers' layouts; the helpers' headers are unchanged.

Header 428's form at `0x1C94` (`variant428_webs`) keeps the records at
`work + 0x5D0`, projects with `RotTransPers3`, and shrinks by `step * 0xE0` in
phase 0.
Header 416's entry at `0x0004` (`variant416_entry`, 1044 instructions) opens
its work area with sixteen 0xA8-byte records (as header 407), then three webs,
two 0xA8-byte bands, three sheets, six rings, four spokes and a fan. Its
0x30-byte config has a second threshold at `+ 0x20` that starts the
`0x1814` and `0x1054` helpers. The spoke loop steps its angle as
`j + i * 512`, and `screen_delta` takes both `dx` and `dy` through locals.
Header 401's entry (`0x8013B004`, 1,016 instructions under `gcc_2_7_2_cdk_g0`)
is the header-416 entry with one 0x27C-byte band instead of two 0xA8-byte
bands: nine columns of colour pairs at `+ 0x1F8`/`+ 0x21C`, offsets
`-(j * 768) / 8` at `+ 0x1D4`, no zeroed second column and no `field_70`.
It initializes two sheets, loads a fifth texture (`D + 0x38`, `D + 0x54`
instead of `D + 0xE0`) and has no burst threshold: once the config start is
reached it calls sheet then webs, bands while the phase is positive and rays
from phase 2. The tint fades only below phase 5. The same
`variant401_entry.c` with palette x = 640 is the French header-418 entry
([French notes](french-model-variant418.md)).
Its bands at `0xFE4` (`variant401_bands`, 480 instructions;
`variant401_bands_slot1` builds the header-551 image) draw that band, whose
`Variant401EntryBand` now names the nine-by-three points at `+ 0x0`, their
projections at `+ 0xD8` and the depths at `+ 0x258`. The first pass puts three
points per column at radius `(work[0x1B0C] * 21 + bias) >> 12` (angles
`0x400`, flat, `0xC00`), turns them by the screen yaw, moves each column by
its growth offset along the `work + 0x1AB4` direction, projects it with `RotTransPers3`, and grows the
offsets towards `0x400` (the first from the timing record at
`work[0x1AF4] + 0x20`, the others by `step << 6`; the first reaching `0x400`
sets `work + 0x1B18` to 2). The second pass sorts two POLY_GT4 per column
pair, clamping negative depths to zero; in phase 2 `work + 0x1B0C` then fades
to zero over the record's second interval and the phase becomes 3. Several
statements steer the retail allocation rather than read naturally, each
measured against the plain form: `if (tc > 0)` keeps identical arms under
`if (rec)`; dead `if (x != 0) { lo = 0; }` statements after the radius, the
yaw and at the end of the first loop split blocks for the scheduler and give
the counter its reference count; the radius is a second reader of the
`srl 12`; the second half of the second loop reads through
`q2 = (u8 *)rec + (((u8 *)rec + j * 4) - (u8 *)rec);`, which cse does not
reduce, so the final depth reads keep the first half's register, and which
the combiner then turns into the copy the retail code has. Both images
rebuild byte-identical to retail; every function of both is now C.
Header 416's form at `0x1C68` (`variant416_webs`) keeps the records at
`work + 0xA80` and replaces the phase-0 shrink: each web with a positive scale
is set to `(i << 12) / 3` less the progress `(now - start) * 3 << 12 / (end -
start)` of the timing record at `work + 0x1A60`, plus `0x1000`, and wrapped by
`0x1000` when that is not positive. It matches only with the progress stored to
the scale first and subtracted from the third in a second statement.

`ribbons` (header 418, 397 instructions) builds eight 0x74-byte
`ModelVariantRibbon` records fanned `0x200` apart around the path, projects each
two-point spine and a copy of it moved 16 units sideways, and draws the first
point of each as a `POLY_G3` whose width is the projected offset. Three things
in its first loop nest decide the match, all through gcc 2.7.2's loop pass:

- the `16` is an `s16` local, which leaves `lui 0x10; sra 16` in the outer loop;
- the depth `(flags & 1) * 32 + 0xA0` is written inside the inner loop from a
  local copy of the word at `work + 0x1B58`. Its three instructions are hoisted
  out of both loops, and each move makes the pass less willing to hoist the
  next, which is what stops the `sra` above from following them. Computed
  before the loop, the constant folds to `li 16` and the function is five
  words short;
- the radius is an `s16`, which orders the two inner-loop registers.

Header 425 has the same body at `0x2CC0` (`variant425_ribbons`) over 0x6C-byte
`ModelVariantRibbonShort` records, drawn when the depth is positive rather than
non-negative, with the last frame read from `+ 0x18` of the timing record.
Header 404's form at `0x11D4` (`variant404_ribbons`, 420 instructions) is the
header-425 source with its own offsets, the far colour `0x40, 0x60, 0xFF`, and
the `RotTransPers4` flag of every point kept in a stack array `status[8][2]`;
a point is drawn only when both its depth and its flag are non-negative.
Header 404's spiral at `0x1864` (`variant404_spiral`, 0xC94 bytes) is the Spanish
header-421 spiral built under gcc 2.7.2 at the North American address: a wrapper
that renames `func_8013C860` to `func_8013C864` (`func_8017C864` in slot 1) and
includes `spanish_model_variant/variant421_spiral.c`. It differs from the
retail bytes by one jump word under the maspsx pinned before `e85ecb3` (the
`j` out of the signed `x / 8` join landed one word late), and builds exactly
under the current pin.
Header 404's entry at `0x0004` (`variant404_entry`, 1140 instructions) follows
the French header-422 entry with a 0x3C-byte config (mode at `+ 0x1C`, start at
`+ 0x20`, an `s16` sheet size at `+ 0x34`), twelve 0x7C-byte glow records at
`work + 0x4E0` and five image uploads from the data at `0x40FC`. It matches
only when the shift of the first upload's result is written before the second
upload into the same `packed` local, and the glow pointer is taken before the
primitive pointers.
Two more North American helpers are other regions' sources built under
gcc 2.7.2 at the North American address, with only the function renamed:
header 414's bands at `0x17DC` (`variant414_bands`, 795 instructions) is the
French header-431 bands, and header 422's screen rings at `0x1234`
(`variant422_screen_rings`, 310 instructions) is the Spanish header-439 screen
rings. Header 404's spiral at `0x1864` builds from the Spanish header-421
spiral with every word identical except one jump, at `+ 0x1948`. The
`if`/`else` join at `0x8013C968` follows a load that needs a delay `nop`, and
two labels mark it. Retail's `bgez` and `j` both target the `nop`; in the
rebuilt image the `j` targets `0x8013C96C`, one word later. The spiral is
still assembly.

`spiral` (header 418, 647 instructions) is the ribbons helper over twelve
0x84-byte `Variant418SpiralArm` records at `work + 0x720`: each arm's outer
point sits on a spiral at `(i << 12) / 12` and `(i << 13) / 12` past the turn
at `work + 0x1B6C`, scaled by `(rsin << 9) * k`, and each arm is drawn as two
`POLY_GT4` halves on either side of the spine, sorted at depth 0. As in the
other ribbon and streamer helpers, the point stores need `setVector` on
`&arm->a[k]`, and the screen deltas need `dx`/`dy` locals. With the deltas
written inline in `ratan2`, the loads are scheduled in another order.

`bands` (451 instructions) draws one band of nine segments as pairs of
`POLY_GT4` quads along the variant path. Its screen coordinates are the `long`
values `RotTransPers3` writes, so `x0` is read with `lhu` and `y0` as `sxy >> 16`
with `lh`. Every work-area access goes through a `work` copy of `ctx`, including
the pointer initialisations; with the copy only on the field reads, every
callee-saved register is rotated. The header-418 copy's record is 0x24 bytes
longer, which shows only in the band stride.

Header 397's entry at `0x0004` (`variant397_entry`, 1162 instructions) runs the
French header-422 entry's init loops (bands, quads, sheets, webs, rings,
spokes) in the header-336 frame, with the sheet extent from the `u16` at
`+ 0x40` of a 0x48-byte config. The config's linked flag picks between part 0
plus a signed offset (the sign follows the slot) and the average of two
parts, in both branches. `packed` has to take every image upload's result,
which keeps the first upload's shift above the second call.

Header 397's bands (`0x8013C22C`, 474 instructions) is the same helper over one
0x11C-byte `ModelVariantBandShort` of five points, with the `RotTransPers3` flag
kept per point in the record. Its radius is scaled by the word at `+ 0x44` of
the timing record at `work + 0xF54` (by 20/16 of it when the flag bit is set),
and every depth is clamped to zero before the sort, which also clears the
point's flag. The second quad of each pair reads its current column through a
separate pointer, `(s32 *)band + j`, that moves on to the next column after the
clamp and is still assigned after the loops. cse then keeps that pointer as a
copy (`move v1,s0`) up to the clamp and reloads the depth through the loop's
own address register at the join. With the fields indexed directly, the copy
folds away and 33 lines differ.

Header 428's bands (`0x8013C038`, 477 instructions) keeps nine points per band at
`work + 0xAB0` with a radius of `* 48 / 4096` or `* 56 / 4096`. Each depth is
clamped to zero before its sort, which also clears that point's
`RotTransPers3` flag in a stack array `flag[i][j]`. The second quad reads
column `j` through `(s32 *)band + j`, a pointer that moves on to the next column
after the clamp and is still assigned after the loops. cse then keeps it as the
target's `move v1,s0` copy up to the clamp.
Header 428's entry at `0x0004` (`variant428_entry`, 1037 instructions) is the
French header-439 entry without screen rings or curtains: twelve spiral arms
open the work area, and a 0x34-byte config whose word at `+ 0x30` starts the
spiral at `0x2BF0`. Both the ring and the spoke loops step their angle as
`base_angle + outer * 512`. The fan's `angle` keeps its register only with two
extra references in the per-frame branch (`angle = ratan2(...)` stored to
`angles[0]`), which combine removes again.

Header 404's bands (`0x8013D4F8`, 491 instructions) keeps nine points per band at
`work + 0xE10`. Its radius is scaled by the word at `+ 0x38` of the timing
record at `work + 0x1938` (by 18/16 of it when the flag bit is set), and that
record's two phases sit at `+ 0x24` and `+ 0x2C`. Each depth is clamped to zero
before its sort, which also clears that point's `RotTransPers3` flag in a stack
array `flag[i][j]`. The second quad reads column `j` through
`(s32 *)band + j`, a pointer that moves on to the next column after the clamp
and is still assigned after the loops. cse then keeps it as the target's
`move v1,s0` copy up to the clamp.

Header 423's bands (`0x8013BF18`, 499 instructions) keeps five points per band
in 0x108-byte `Variant423Band` records at `work + 0x4E0`. Its radius is
`h * 48 / 4096` (or `h * 54 / 4096` with flag bit 0) plus
`rsin(angle) * (h / 256) >> 12`, where `h` is the `s16` at `work + 0xF18` and
the angle at `work + 0xF1E` then advances by `step * 256`. Written `<< 8`, the
step is narrowed to an `lhu` load. The path progress is the `s16` at
`work + 0xF1C`. As in the header-398 port, each depth is clamped to zero before
its sort and the second quad reads its column through `(s32 *)band + j`; the
sort key is the next point's depth, `otz[j + 1]`.

Header 423's entry (`0x8013B004`, 965 instructions under `gcc_2_7_2_cdk_g0`)
is the header-428 entry without the twelve spiral arms: no arm
initialization and no spiral call. The band is a 0x108-byte record of five
colour pairs, the config is 0x30 bytes without the spiral start, and seven
halfword/word fields sit at `work + 0xF14..0xF23` before the phase at
`+ 0xF24`. Dropping the 1,488-byte arms and the 0xC0 band bytes moves the
later fields by 0x690, so the config pointer is at `work + 0xF00`. The same
`variant423_entry.c` with palette x = 640 is the French header-440 entry under
the French profile ([French notes](french-model-variant440.md)).

`strand` (header 443) fans six strands of thirteen points around the origin and
draws the visible span `[0x2EA6, 0x2EA8)` of each as `GsLINE` segments. It needs
the same `work` copy.

`sheets+` (header 443, 351 instructions) draws a variable number of 0x9C-byte
sheets. The count, and how many of them sit on the variant's own 0x20-byte slots
at `work + 0x2C50`, come from the timing record at `work + 0x2E7C`. The rest sit
on the `VECTOR` table at `work + 0x2D3C` and follow 0x2E8-byte objects that
start at `ctx`. The object cursor advances only for those.

`streamers` (header 443, 496 instructions) coils two seventeen-point streamers
around the variant's axis, projects each spine and a copy of it moved along the
view, and draws every segment as a `POLY_G4` as wide as the projected offset.
Once the fourth phase starts it shrinks them by `step * 16` until they vanish.
Five things in the source decide the allocation and schedule:

- The radius is an `s16` local set to `0xA0` right after the turn angle. The
  coil loop reads it through a spilled `s32` copy made after both flag tests,
  so the retail `li v0,0xA0; sw v0,0x110(sp)` sits in that block. In the
  header-321 copy the same local is `work[0x1354] / 32`.
- `0x400` is one local shared by the flag-1 `ry` and the per-streamer base
  step.
- The last point's width is `2` or `sb - sa` through two locals, and the
  other points store each arm straight into `width[k]`. This keeps the
  constant `2` dead at the compare, so it can share `v0` and reorg puts it in
  the branch delay slot.
- The `ox`/`oy` pair is written twice, under `if (work)` and its `else`. jump2
  merges the identical arms and deletes the test, but the extra blocks change
  allocation earlier on.
- The `work[0x2EB8] = work[0x2EB8]` no-op store is kept; without it the
  allocation changes.

`ribbons` (header 443, 706 instructions) builds one 0x2E8-byte
`Variant443Ribbon` per entry of the count at `+ 0x20` of the timing record at
`work + 0x2E7C`. Each ribbon is turned by `0x400`, then `0x400 +- i * 1800 /
count` by parity, offset by `rsin(k * 128) * 384`, and placed at the per-ribbon
step `work[0x2DBC + 16 i] * k / 16`. It is projected with its flags in a stack
`PSXLONG flags[8][17]`, and its projected spine is bent by two travelling waves
scaled by `width * 64 / 48` and `width * 32 / 48`. The first `count` segments
are drawn as flat `POLY_FT4` quads. The count grows by two frame steps to 16,
then the view offset shrinks by `step << 6` and the ribbon restarts at a
negative count, or retires once the phase reaches 3. Source details that
matter:

- the flag tests are `(u32)(flags & 1) == 1`, and the per-ribbon tables are
  read through `work + (i << 4)` and `work + (i << 5)`;
- `k * 128` is held in a `coil` local, and the unused template `bend` of the
  first loop is still written there;
- the `s16` view-offset length is set to `0x30` at the top of the outer loop
  body, so loop.c hoists that constant and leaves its sign extension in the
  loop, as the retail `lui t0,0x30; sra` shows.

`ribbon` (header 376, 574 instructions) builds one seventeen-point ribbon
(`Variant376Ribbon`, 0x3A4 bytes at `work`) that twists around the variant's
axis, plus a copy moved along the view by `work[0xDD2] * 40 / 1024`. It
projects both as the streamers do (angle `- 0x400`, no width floor) and draws
the first `work[0xDCC]` segments as flat `POLY_FT4` quads, alternating between
two packets at `work + 0xC6C`. In phase 1 the drawn length grows by the frame
step up to 16. In later phases the offset shrinks with the timing record's
progress. Two source details:

- the offset length is an `s16` local. loop.c hoists its sign extension, and
  that new pseudo takes the last spill slot (`0x108`), above the three call
  results that are saved across calls in the twist expressions;
- both point rows are written with `setVector`, so `&b[k]` is a pointer of its
  own, and the drawing loop's `k = 0` comes before the packet pointer.

`variant324_bands.c` (header 324, 713 instructions at `0x17D4`) draws one
0x1EC-byte `Variant324Band` at `work + 0x4E0`: nine three-point fans, each
rotated by `ratan2 + 0x800` about its own point on the path from
`work[0xED8..0xEE0]` along `work[0xEEC..0xEF4]` (stretched by the level at
`work + 0xF48`), projected with `RotTransPers3` after `ReadRotMatrix`,
`RotMatrix` and `ScaleMatrix` on the light matrix, and drawn as two `POLY_GT4`
quads per segment when the depth and the flag are not negative. From phase 3
the colours are scaled by `work[0xF46] / 1024`. On the timing record's last
entry the band stretches (phase 1), widens its radius at `work + 0xF44`
(phase 2) and fades out (phase 3).

`variant425_rays.c` (header 425, 633 instructions at `0x1F14`) draws sixteen
three-point rays in 0xB8-byte `Variant425Ray` records at `work + 0xDC0`,
fanned around the variant's axis. Even rays reach `0x100` and odd ones `0x80`,
turning against each other with the sweep at `work + 0x2708` and leaning along
the direction at `work + 0x26C8` (mirrored by the flag at `work + 0x275C`).
Each spine and a copy moved along the view are projected like the header-443
streamers, and each segment is drawn as two `POLY_GT4` halves as wide as the
projected offset, the outer one fading to black. The projection flags sit in a
stack `PSXLONG status[16][2]`, and the third point's flag is written to
`status[i][2]`, one past the row: `[17][2]`, `[16][3]` and `&status[i + 1][0]`
all change the retail code. The size at `work + 0x273C` grows by
`step * 256` up to `0x1000`, which moves phase 2 on to phase 3.

`variant416_bands.c` (header 416, 496 instructions) is a two-by-two form of
the header-422 bands. There are two 0xA8-byte `Variant416Band` records at
`work + 0xF60`, each with two three-point fans projected with `RotTransPers3`
(flags in a stack `PSXLONG flag[2][2]`), placed along `work[0x1A20..0x1A28]` by
their clamped level. They are drawn as two `POLY_GT4` quads per band. Levels
grow by `step << 6`. The first one to fill moves the phase to 2. When both of a
band's levels are full they reset, staggered by `j << 8`, until the timing
record's `+ 0x28` deadline. Once every band is done the phase moves to 3. The
radius keeps the header-422 flag-bit-0 `if/else`, whose two arms fold to the
same `/ 256`; that is what leaves the retail `srl/sll/sra` form. The reset is
written `level - 0x400 - (j << 8)`, which keeps the retail association. The
`done` product after the reset loop reads `done[2]` (the reset reuses `j`), as
retail does.

`spokes*` is a second form of the spokes helper, 197 instructions instead of
195: it draws four rings like spokes but only sorts lines whose depth is below
`0x800`, as rings does. `variant405_spokes.c` was written from header 405's
copy and the other five were derived from it by the same offset substitution.

## Registered images

Model ids follow the compact-record ledger in
[`spanish-model-return-two-instances.csv`](spanish-model-return-two-instances.csv).
The modules are named `model_variant_<model>_<stage>_slot0`. The Spanish
notes tie record sectors 200..209 to loader stage 9, so the `+ 200` images are named `stage9`. The stage that
reads sectors 180..189 has not been confirmed from the loader, so those images
keep the neutral `pos0`. The images built from Spanish sources take the Spanish
module names instead: `stage7` for `+ 180`, `stage8` for `+ 190` and `stage10` for
`+ 210`, with the `+ 190` and `+ 210` images in slot 1 at `0x8017B000`.

| Header | Sector offset | Models |
|---:|---|---|
| 405 | `+ 180` | 1, 550 |
| 397 | `+ 180` | 2, 20, 87, 108, 138, 193, 573 |
| 397 | `+ 200` | 152, 168, 170, 388, 427 |
| 418 | `+ 180` | 34, 71, 124, 182, 279, 361, 491, 580, 640 |
| 418 | `+ 200` | 166, 275, 469, 590 |
| 428 | `+ 180` | 187, 596 |
| 428 | `+ 200` | 239, 361, 368, 478 |
| 404 | `+ 180` | 84, 162 |
| 404 | `+ 200` | 88, 114, 184, 369 |
| 416 | `+ 180` | 180, 440 |
| 425 | `+ 180` | 259, 630 |
| 448 | `+ 200` | 108, 573 |
| 423 | `+ 200` | 262, 631 |
| 414 | `+ 180` | 401 |
| 401 | `+ 200` | 410 |
| 443 | `+ 180` | 70, 460, 469, 704 |
| 443 | `+ 200` | 44, 98, 161, 370, 400, 458, 462, 558 |
| 443* | `+ 180` | 125 |
| 391 | `+ 200` | 54 |
| 541 | `+ 210` | 54 (slot 1) |
| 320 | `+ 180` | 410 |
| 320 | `+ 200` | 110, 159 |
| 470 | `+ 190` | 410 (slot 1) |
| 470 | `+ 210` | 110, 159 (slot 1) |
| 415 | `+ 200` | 401 |
| 565 | `+ 210` | 401 (slot 1) |

`391` and `541` are model 54's `+ 200` and `+ 210` images (stages 9 and 10;
the second loads at `0x8017B000`, see below). They are the North American build
of the Spanish header-408 variant: the files in `src/overlays/model_variant/`
named `variant391_*` include the Spanish sources and rename their addresses.
The same C matches the Spanish images under `gcc_2_8_1_g0_split` and the North
American ones under `gcc_2_7_2_cdk_g0`, so the European build recompiled these
helpers with the newer compiler. Two other things differ: the entry uploads its
palettes to CLUT column 512 instead of 640, and the data sits 0x18 bytes later
because the text is six words longer. The images also showed that the Spanish
linker symbols named `0x80082E48` `SetPolyF4`. Its body is `SetPolyG3` (length
6, code `0x30`), and the 28-byte primitive the entry sets up is a `POLY_G3`.

Headers 320/470 and 415/565 are ported the same way, from the Spanish
header-337 rings helper (`variant320_*`) and the header-432 draw, layers and
bands helpers (`variant415_*`). Every North American header is 17 below the
Spanish one. Their other functions stay in assembly, except header 415's entry
and ribbon helper below. The header-415 layers and bands helpers are 20 and 18 words longer than
their Spanish builds, so the compiler change is more than the epilogue `nop`.

Header 415's entry at `0x0004` (`variant415_entry`, 875 instructions) is
written for this build rather than ported, since the Spanish header-432 entry is
still assembly. It sets up three ribbons, two sheets, two sets of three
seventeen-point strands, four rings, two lines and three sweeps. It reads its
config (at `0x1F7C + 0x38`, indexed by command) and its three image descriptors
from the data at `0x1F7C`. It matches only under these conditions:

- The two strand loops share one angle variable. Separate ones need one
  callee-saved register fewer, and sharing it with the ribbon counter gives the
  angle too low an allocation priority, so it loses `s0` to the strand cursor.
- The ribbon loop is counted by `i`.
- The second strand set's scale is written `-(n * 4096) / 3`. A running
  accumulator stepped by `-4096` is a source statement, and its copy lands in
  front of the hoisted `255`, which the retail code has after it.

The two flat-textured packets take their tpage and clut from locals that are
never assigned. The retail code also loads them from stack slots it never
writes.

Header 415's ribbon helper at `0x0DB0` (`variant415_ribbons`, 359 instructions)
is also new C. For each of the five anchors it turns the three ribbons by the
anchor's `ratan2` angle, places their ends along the anchor's direction by the
sweep amount, and projects them. On the last anchor it grows each amount by
`step * 64` up to `0x400`, advancing the phase and the start index. Once
`field_30DC` is set it restarts the amounts at `-(j << 8) - (k << 9)`, and it
draws one Gouraud quad per ribbon. Its parameter is the state, used directly
as the ribbon cursor. The retail registers need one no-op in the source,
`if (angle != 0) k = 0;` before the ribbon loop, whose `for` sets `k` again.
The branch lasts until after register allocation and does two things there. It
splits the block, so the sign-extended `i` is allocated globally (`s2`) instead
of locally (`s0`). It also changes the global allocation priorities
(`floor(log2 refs) * refs / live length`) of the cursor and the packet pointer
from `390/227` and `438/255` to `384/229` and `438/256`. That reverses their
order, so the packet pointer gets `s0`. The entry's prototype for it now takes
a `Variant415EntryRibbon *`.

`variant320_ribbon.c` (header 320, 554 instructions) is the one-ribbon form of
the five-ribbon helper at `0x8013BBA4` in the header-321 images: a
seventeen-point spine bent by `rsin(bend) * (rsin(wave) * 64 / 4096 + 0x40)`
and a copy moved along the view, projected and drawn like the header-376
ribbon, over a `Variant320Ribbon` whose depths come before the projection
flags. The template's per-ribbon turn (`0x400 +- i * 360`) and coil angle
(`k * 128`) are still computed but never used. That changes the retail code
even so. The empty turn branches survive until jump2, so reload still loads `i`
for their test, and that dead `lhu` is left behind when jump2 removes them. The
coil's `k` extension is shared with the later `k` uses, which places it before
the first call.

`variant320_streamers.c` (header 320, 524 instructions at `0x1714`) draws two
seventeen-point streamers in 0x378-byte `Variant320Streamer` records at
`work + 0x4D4`, the template of the header-443 streamers with a radius of
`0xC0`, a width of 1 past `0x200` and a length that follows the timing record
at `work + 0x43C` (its word at `+ 0x88` over eight) until phase 5. As in the
header-443 form, the radius is an `s16 reach` copied in the join block, and the
last point stores its width through a local. The no-op store
`work[0xD58] = work[0xD58]` and the identical arms of `if (poly)` are both
needed: without either one the body is a word short and the registers differ.

The header-443 entry (`variant443_entry.c`, 831 instructions) is the French
and Spanish header-460 entry built with gcc 2.7.2, which makes it one word
longer. As with header 391, the entry reads its CLUT from column 512 instead of 640
(`MODEL_VARIANT460_CLUT_X`), and the wrapper renames the helpers and the data
block, which starts at the end of the text (`D_8013D888`, read at `+ 0x1C` to
`+ 0xFC`). Model 125's longer images (`443*`, `593*`) use the same wrappers, and
their streamers at `0x20C8` are the same bytes as the other twelve images', so
they reuse `variant443_streamers`. Their data block at `0x2888` is 0x77C bytes
(`D_8013D888`/`D_8017D888`, now a data owner). The 0x1D08 bytes from
`0x3004` to `0x4D0C`, formerly registered as `func_8013E004`/`func_8017E004`,
are an unclassified tail: the two images disagree in 1835 of their 1858 words,
neither has a function boundary before `0x4D0C`, and their address pairs point
at `0x8058xxxx` and `0x8054xxxx`, outside this executable. They are kept as raw
data (`D_8013E004`/`D_8017E004`), like the suffixes of models 174 and 531.

The header-405 entry (`variant405_entry.c`, 1098 instructions) is the French
and Spanish header-422 entry built with gcc 2.7.2. Unlike the other entries,
it needs no CLUT change. The wrapper renames the halo helper
(`func_8013C128` to `func_8013C12C`) and the data block after the text
(`D_8013E834` to `D_8013E850`).

`443*` is model 125's header-443 image. Its five functions (entry, ribbons,
sheets+, strand and streamers) are byte-identical to the other twelve images'
at the same addresses, so it reuses those five files and its text ends at
`0x2888` like theirs; the entry's data block follows, and `0x3004..0x4D0C` is an
unclassified tail of raw bytes (see above). The disassembler had provisionally
registered that tail as a sixth function, `func_8013E004`; it is not one. Model
168's longer header-443 image is left out because it calls `0x80054A44`, inside
`Model_DrawSlot`.

`variant321_ribbon.c` (header 321, 705 instructions) draws five ribbons in the
style of the header-376 ribbon, over `Variant321Ribbon` records that keep a
per-ribbon segment count at `+ 0x23A`. Each ribbon is turned by its own angle,
`0x400` for the first and `0x400 +- i * 360` by parity for the others, and
offset by `rsin(k * 128) * 384`. The first segment is anchored on the spine and
the last ends on it. Packets alternate before or after the sort, depending on
flag bit 0. Three source forms matter:

- `k * 128` is held in a `coil` local, which keeps `work` in `fp`;
- the flag test is `(u32)(flags & 1) == 1`, kept as a compare with 1, and the
  count step `(u32)(step * 3) >> 1` gives the retail `srl`;
- the drawing loop is `k = 0; packet = ...; for (; k < count; k++)`, so reorg
  fills the count test's delay slot with the packet pointer.

Model 361 has two registered images: header 418 at `+ 180` and header 428 at
`+ 200`.

All images except model 54's stage-10 image load at `0x8013B000`. Their resident calls resolve to the North
American addresses of the same names and are listed once in
`model_variant_linker_symbols.txt`.

### Slot-1 images

The `+ 190` (stage 8) and `+ 210` (stage 10) images load at `0x8017B000`.
Each one's header is its slot-0 header plus 150, its text has the same function
boundaries, and every `lui` of a module address in it is `0x8018`. The
`*_slot1.c` files include the slot-0 C and rename the one function it defines to
`0x8017`. Families converted to
[per-image symbol bindings](#per-image-symbol-bindings) need no slot-1 file:
the slot-1 manifest names the slot-0 C directly. Sixty-two images are registered this way:

| Header | Slot-0 header | Sector offset | Models |
|---:|---:|---|---|
| 547 | 397 | `+ 190` | 2, 20, 87, 108, 138, 193, 573 |
| 547 | 397 | `+ 210` | 152, 168, 170, 388, 427 |
| 551 | 401 | `+ 210` | 410 |
| 554 | 404 | `+ 190` | 84, 162 |
| 554 | 404 | `+ 210` | 88, 114, 184, 369 |
| 555 | 405 | `+ 190` | 1, 550 |
| 564 | 414 | `+ 190` | 401 |
| 566 | 416 | `+ 190` | 180, 440 |
| 568 | 418 | `+ 190` | 34, 71, 124, 182, 279, 361, 491, 580, 640 |
| 568 | 418 | `+ 210` | 166, 275, 469, 590 |
| 573 | 423 | `+ 210` | 262, 631 |
| 575 | 425 | `+ 190` | 259, 630 |
| 578 | 428 | `+ 190` | 187, 596 |
| 578 | 428 | `+ 210` | 239, 361, 368, 478 |
| 593 | 443 | `+ 190` | 70, 460, 469, 704 |
| 593* | 443 | `+ 190` | 125 |
| 593 | 443 | `+ 210` | 44, 98, 161, 370, 400, 458, 462, 558 |
| 598 | 448 | `+ 210` | 108, 573 |

`593*` is model 125's longer image. Like its slot-0 image, it has words past
`0x3004` that decode as `jal`s to addresses no resident function starts at
(here `0x8054xxxx`; the slot-0 tail points at `0x8058xxxx`, and the two tails
share only 23 of their 1858 words). The provisional disassembler reading kept
them as assembly (`func_8017E004`); they are an unclassified tail and the module
matches with them kept as raw data (`D_8017E004`). Model 168's longer image is
left out, as its slot-0 image is, because it calls `0x80054A44` inside
`Model_DrawSlot`.

### Per-image symbol bindings

Shared C defines one name per function or object, the same in every region,
slot and image. It does not `#define` that name to an image address. Each
image's `*_symbols.txt` binds the name at its own address, and its
`*_functions.csv` uses the same name. Splat writes that name into the
generated assembly that calls it, the C object defines it, and the linker
joins them. The Spanish model_variant_62 stage-9 image, whose entry is still
generated assembly, links its `jal` to the C ribbons helper this way.

The name is an evidence-backed `<Subsystem>_<Verb><Object>` name, recorded in
[`semantic-symbol-map.csv`](../semantic-symbol-map.csv). The subsystem is the
directory and header number of the shared source: `ModelVariant405_` for
`model_variant/variant405_*.c`, `FrenchModelVariant443_` for
`french_model_variant/variant443_*.c` (French header numbers differ from the
North American ones). An address-based name such as `func_8013D8FC` is not
used for a binding: the one address cannot be correct in all images, and the
same address names other functions in other families.

Layout selectors such as `MODEL_VARIANT405_WEBS_*_SHIFT` and
`MODEL_VARIANT443_RIBBONS_PACKED_SCREEN` stay in their wrappers. A wrapper
left with nothing but the include is deleted and its manifests name the shared
C.

An image entry (`0x8013B004`) is `<Subsystem>_HandleRequest`:
`Model_UpdateSlotControl` calls it at base + 4 with the selected request
modulo 1000 and then with -1, and the entry sets its work area up for a
non-negative request and runs one frame otherwise. Image-local data with no
established role is `g<Subsystem>_D<offset>`, the offset taken from the
canonical image's load base (`gModelVariant445_D2848` is `0x8013D848` in the
North American image and `0x8013D860` in the European ones). Only the function
names have `semantic-symbol-map.csv` rows; its overlay rows accept functions
only.

| Name | Shared C | Bound at |
|---|---|---|
| `ModelVariant405_DrawWebs` | `model_variant/variant405_webs.c` | USA models 1 and 550, Japanese images 456/466 and 138180/138190: `0x8013D8FC`/`0x8017D8FC`. USA model 62, Japanese images 17312/17322: `0x8013F594`/`0x8017F594`. European, French, German, Italian and Spanish models 1 and 550: `0x8013D8F0`/`0x8017D8F0` |
| `FrenchModelVariant443_DrawRibbons` | `french_model_variant/variant443_ribbons.c` | USA model 62, Japanese images 17312/17322: `0x8013CAC4`/`0x8017CAC4`. European, French, German, Italian and Spanish model 62: `0x8013CADC`/`0x8017CADC` |
| `ModelVariant159_HandleRequest` | `model_variant/variant159_entry.c` | Model 135 slot 0 and slot 1 in every region, Japanese images 37440/37450: `0x8013B004`/`0x8017B004` |
| `gModelVariant159_D1D94` | data subsegment (`variant159_entry.h`) | Model 135 slot 0 and slot 1 in every region, Japanese images 37440/37450: USA and Japanese `0x8013CD94`/`0x8017CD94`; European, French, German, Italian and Spanish `0x8013CD70`/`0x8017CD70` |
| `gModelVariant159_D1DB0` | data subsegment (`variant159_entry.h`) | Model 135 slot 0 and slot 1 in every region, Japanese images 37440/37450: USA and Japanese `0x8013CDB0`/`0x8017CDB0`; European, French, German, Italian and Spanish `0x8013CD8C`/`0x8017CD8C` |
| `ModelVariant137_HandleRequest` | `model_variant/variant137_entry.c` | Model 378 slot 0 and slot 1 in every region, Japanese images 90728/90738: `0x8013B004`/`0x8017B004` |
| `gModelVariant137_D1C34` | data subsegment (`variant137_entry.h`) | Model 378 slot 0 and slot 1 in every region, Japanese images 90728/90738: USA and Japanese `0x8013CC34`/`0x8017CC34`; European, French, German, Italian and Spanish `0x8013CC18`/`0x8017CC18` |
| `gModelVariant137_D1C88` | data subsegment (`variant137_entry.h`) | Model 378 slot 0 and slot 1 in every region, Japanese images 90728/90738: USA and Japanese `0x8013CC88`/`0x8017CC88`; European, French, German, Italian and Spanish `0x8013CC6C`/`0x8017CC6C` |
| `ModelVariant445_HandleRequest` | `model_variant/variant445us_entry.c` | Model 457 slot 0 and slot 1 in every region, Japanese images 112512/112522: `0x8013B004`/`0x8017B004` |
| `ModelVariant445_DrawRibbons` | `model_variant/variant445us_ribbons.c` | Model 457 slot 0 and slot 1 in every region, Japanese images 112512/112522: `0x8013C4C4`/`0x8017C4C4` |
| `FrenchModelVariant462_DrawSheets` | `french_model_variant/variant462_sheets.c` | USA model 457, Japanese images 112512/112522: `0x8013CEEC`/`0x8017CEEC`. European, French, German, Italian and Spanish model 457: `0x8013CEF8`/`0x8017CEF8` |
| `FrenchModelVariant462_DrawRings` | `french_model_variant/variant462_rings.c` | USA model 457, Japanese images 112512/112522: `0x8013D41C`/`0x8017D41C`. European, French, German, Italian and Spanish model 457: `0x8013D42C`/`0x8017D42C` |
| `gModelVariant445_D2848` | data subsegment (`variant445us_entry.h`) | USA model 457, Japanese images 112512/112522: `0x8013D848`/`0x8017D848`. European, French, German, Italian and Spanish model 457: `0x8013D860`/`0x8017D860` |
| `ModelVariant105_HandleRequest` | `model_variant/variant105_entry.c` | Model 33 slot 0 and slot 1 in every region, Japanese images 9308/9318: `0x8013B004`/`0x8017B004` |
| `gModelVariant105_D1424` | data subsegment (`variant105_entry.h`) | Model 33 slot 0 and slot 1 in every region, Japanese images 9308/9318: `0x8013C424`/`0x8017C424` |
| `gModelVariant105_D1440` | data subsegment (`variant105_entry.h`) | Model 33 slot 0 and slot 1 in every region, Japanese images 9308/9318: `0x8013C440`/`0x8017C440` |
| `ModelVariant106_HandleRequest` | `model_variant/variant106_entry.c` | Model 429 slot 0 and slot 1 in every region, Japanese images 104784/104794: `0x8013B004`/`0x8017B004` |
| `gModelVariant106_D2174` | data subsegment (`variant106_entry.h`) | Model 429 slot 0 and slot 1 in every region, Japanese images 104784/104794: `0x8013D174`/`0x8017D174` |
| `gModelVariant106_D21C8` | data subsegment (`variant106_entry.h`) | Model 429 slot 0 and slot 1 in every region, Japanese images 104784/104794: `0x8013D1C8`/`0x8017D1C8` |
| `ModelVariant132_HandleRequest` | `model_variant/variant132_entry.c` | Model 472 slot 0 and slot 1 in every region, Japanese images 116672/116682: `0x8013B004`/`0x8017B004` |
| `gModelVariant132_D1388` | data subsegment (`variant132_entry.h`) | Model 472 slot 0 and slot 1 in every region, Japanese images 116672/116682: `0x8013C388`/`0x8017C388` |
| `gModelVariant132_D13C0` | data subsegment (`variant132_entry.h`) | Model 472 slot 0 and slot 1 in every region, Japanese images 116672/116682: `0x8013C3C0`/`0x8017C3C0` |
| `ModelVariant133_HandleRequest` | `model_variant/variant133_entry.c` | Model 701 slot 0 and slot 1 in every region, Japanese images 166056/166066: `0x8013B004`/`0x8017B004` |
| `gModelVariant133_D1864` | data subsegment (`variant133_entry.h`) | Model 701 slot 0 and slot 1 in every region, Japanese images 166056/166066: `0x8013C864`/`0x8017C864` |
| `gModelVariant133_D18B8` | data subsegment (`variant133_entry.h`) | Model 701 slot 0 and slot 1 in every region, Japanese images 166056/166066: `0x8013C8B8`/`0x8017C8B8` |
| `ModelVariant135_HandleRequest` | `model_variant/variant135_entry.c` | Model 36 slot 0 and slot 1 in every region, Japanese images 10116/10126: `0x8013B004`/`0x8017B004` |
| `gModelVariant135_D16A0` | data subsegment (`variant135_entry.h`) | Model 36 slot 0 and slot 1 in every region, Japanese images 10116/10126: USA and Japanese `0x8013C6A0`/`0x8017C6A0`; European, French, German, Italian and Spanish `0x8013C67C`/`0x8017C67C` |
| `gModelVariant135_D17B8` | data subsegment (`variant135_entry.h`) | Model 36 slot 0 and slot 1 in every region, Japanese images 10116/10126: USA and Japanese `0x8013C7B8`/`0x8017C7B8`; European, French, German, Italian and Spanish `0x8013C794`/`0x8017C794` |
| `ModelVariant149_HandleRequest` | `model_variant/variant149_entry.c` | Model 253 slot 0 and slot 1 in every region, Japanese images 70028/70038: `0x8013B004`/`0x8017B004` |
| `gModelVariant149_D14D4` | data subsegment (`variant149_entry.h`) | Model 253 slot 0 and slot 1 in every region, Japanese images 70028/70038: USA and Japanese `0x8013C4D4`/`0x8017C4D4`; European, French, German, Italian and Spanish `0x8013C4B0`/`0x8017C4B0` |
| `gModelVariant149_D15EC` | data subsegment (`variant149_entry.h`) | Model 253 slot 0 and slot 1 in every region, Japanese images 70028/70038: USA and Japanese `0x8013C5EC`/`0x8017C5EC`; European, French, German, Italian and Spanish `0x8013C5C8`/`0x8017C5C8` |
| `ModelVariant154_HandleRequest` | `model_variant/variant154_entry.c` | Model 26 slot 0 and slot 1 in every region, Japanese images 7376/7386: `0x8013B004`/`0x8017B004` |
| `gModelVariant154_D23B4` | data subsegment (`variant154_entry.h`) | Model 26 slot 0 and slot 1 in every region, Japanese images 7376/7386: USA and Japanese `0x8013D3B4`/`0x8017D3B4`; European, French, German, Italian and Spanish `0x8013D390`/`0x8017D390` |
| `gModelVariant154_D2408` | data subsegment (`variant154_entry.h`) | Model 26 slot 0 and slot 1 in every region, Japanese images 7376/7386: USA and Japanese `0x8013D408`/`0x8017D408`; European, French, German, Italian and Spanish `0x8013D3E4`/`0x8017D3E4` |
| `FrenchModelVariant410_HandleRequest` | `french_model_variant/variant410_entry.c` | Model 54 slot 0 and slot 1 in every region, Japanese images 15084/15094: `0x8013B004`/`0x8017B004` |
| `FrenchModelVariant410_DrawQuads` | `french_model_variant/variant410_quads.c` | USA model 54, Japanese images 15084/15094: `0x8013BAB0`/`0x8017BAB0`. European, French, German, Italian and Spanish model 54: `0x8013BAAC`/`0x8017BAAC` |
| `FrenchModelVariant410_DrawRibbons` | `french_model_variant/variant410_ribbons.c` | USA model 54, Japanese images 15084/15094: `0x8013BF60`/`0x8017BF60`. European, French, German, Italian and Spanish model 54: `0x8013BF58`/`0x8017BF58` |
| `FrenchModelVariant410_DrawRings` | `french_model_variant/variant410_rings.c` | USA model 54, Japanese images 15084/15094: `0x8013C898`/`0x8017C898`. European, French, German, Italian and Spanish model 54: `0x8013C8B8`/`0x8017C8B8` |
| `gFrenchModelVariant410_D1D04` | data subsegment (`variant410_entry.h`) | USA model 54, Japanese images 15084/15094: `0x8013CCE8`/`0x8017CCE8`. European, French, German, Italian and Spanish model 54: `0x8013CD04`/`0x8017CD04` |
| `FrenchModelVariant458_HandleRequest` | `french_model_variant/variant458_entry.c` | Model 202 slot 0 and slot 1 in every region, Japanese images 55932/55942: `0x8013B004`/`0x8017B004` |
| `FrenchModelVariant458_DrawSheets` | `french_model_variant/variant458_sheets.c` | USA model 202, Japanese images 55932/55942: `0x8013BC14`/`0x8017BC14`. European, French, German, Italian and Spanish model 202: `0x8013BC10`/`0x8017BC10` |
| `FrenchModelVariant458_DrawStrands` | `french_model_variant/variant458_strands.c` | USA model 202, Japanese images 55932/55942: `0x8013C158`/`0x8017C158`. European, French, German, Italian and Spanish model 202: `0x8013C150`/`0x8017C150` |
| `FrenchModelVariant458_DrawStreamers` | `french_model_variant/variant458_streamers.c` | USA model 202, Japanese images 55932/55942: `0x8013C9CC`/`0x8017C9CC`. European, French, German, Italian and Spanish model 202: `0x8013C9EC`/`0x8017C9EC` |
| `gFrenchModelVariant458_D230C` | data subsegment (`variant458_entry.h`) | USA model 202, Japanese images 55932/55942: `0x8013D288`/`0x8017D288`. European, French, German, Italian and Spanish model 202: `0x8013D30C`/`0x8017D30C` |
| `ModelVariant152_HandleRequest` | `model_variant/variant152_entry.c` | Model 371 slot 0 and slot 1 in every region, Japanese images 88796/88806: `0x8013B004`/`0x8017B004` |
| `gModelVariant152_D1158` | data subsegment (`variant152_entry.h`) | Model 371 slot 0 and slot 1 in every region, Japanese images 88796/88806: USA and Japanese `0x8013C158`/`0x8017C158`; European, French, German, Italian and Spanish `0x8013C134`/`0x8017C134` |
| `gModelVariant152_D1190` | data subsegment (`variant152_entry.h`) | Model 371 slot 0 and slot 1 in every region, Japanese images 88796/88806: USA and Japanese `0x8013C190`/`0x8017C190`; European, French, German, Italian and Spanish `0x8013C16C`/`0x8017C16C` |
| `ModelVariant155_HandleRequest` | `model_variant/variant155_entry.c` | Model 362 slot 0 and slot 1 in every region, Japanese images 86312/86322: `0x8013B004`/`0x8017B004` |
| `gModelVariant155_D1770` | data subsegment (`variant155_entry.h`) | Model 362 slot 0 and slot 1 in every region, Japanese images 86312/86322: USA and Japanese `0x8013C770`/`0x8017C770`; European, French, German, Italian and Spanish `0x8013C74C`/`0x8017C74C` |
| `gModelVariant155_D17FC` | data subsegment (`variant155_entry.h`) | Model 362 slot 0 and slot 1 in every region, Japanese images 86312/86322: USA and Japanese `0x8013C7FC`/`0x8017C7FC`; European, French, German, Italian and Spanish `0x8013C7D8`/`0x8017C7D8` |
| `ModelVariant809_HandleRequest` | `model_variant/variant809_entry.c` | USA model 428 slot 0 and slot 1, Japanese images 104528/104538: `0x8013B004`/`0x8017B004` |
| `gModelVariant809_D10D8` | data subsegment (`variant809_entry.h`) | USA model 428 slot 0 and slot 1, Japanese images 104528/104538: `0x8013C0D8`/`0x8017C0D8` |
| `gModelVariant809_D1110` | data subsegment (`variant809_entry.h`) | USA model 428 slot 0 and slot 1, Japanese images 104528/104538: `0x8013C110`/`0x8017C110` |

### Header 407: petals

`variant407_petals.c` (`func_8013C2DC`, 379 instructions) draws 48 petals, one
`POLY_G4` each. Each petal grows from the variant centre along its own velocity
(`scale * velocity / 512`), swings around a circle whose radius shrinks with
`rcos(scale)`, fades past scale `0x200`, and cycles its scale through the timing
record's phases. It is in 15 header-407 images (models 68, 96, 186, 297, 376
and 595 at `+ 180`; 165, 242, 294, 352, 358, 399, 465, 520 and 621 at
`+ 200`) and their header-557 slot-1 images. What reproduces the target:

- two copies of the parameter, one for the work fields and one for the petal
  scales and colours;
- the counters, the phase, the sweep angle, the radius and the lift are `s16`;
- 16 unused stack bytes between `rot` and `scale`, and the stack locals
  declared in the target's slot order;
- the timing record's `u32` comparisons written with the record's field on the
  left, which fixes the load order;
- the velocity read as `base - -(i * 16) + K`, which keeps the base as the
  first `addu` operand;
- the angle written as `phase + (sweep - fade)` after `fade = 0x400`: with a
  literal, fold reassociates the constant out of the sum, while a named value
  is only propagated by cse, after fold.

`variant407_entry.c` (`func_8013B004`, 1206 instructions) is the entry of the
same images: the 48 petals open the work area, followed by sixteen 0xA8-byte
records, bands, sheets, rings, spokes, quads and webs, and the petals drawer is
its only per-frame helper. The petals pointer is set again before the
per-frame petal loop and the ring loop steps `angle += 512`. As in header 428,
the spoke loop steps `angle = j + i * 512`, which leaves the target's
`li 4096` in the loop for the later `0x29F0` store, and `angle` keeps its
register with the per-frame `angle = ratan2(...)` stored to `angles[0]`.

### Sibling bodies

Some helpers exist in other images as a sibling body: the same instruction
sequence with other immediates. They are ported by mapping each differing
immediate against the reference image. The registered ports, and what each
changed beyond the map:

| Header | Offset | From | Edit |
|---:|---|---|---|
| 422 | `0x1E84` | header-397 sheets | the size step is `<< 7`, not `<< 6` |
| 428 | `0x2BF0` | header-418 spiral | 0x7C-byte arms at `work` with a 0x10-byte head; radius `work[0x15A8] / 2`; drawn when the depth and the per-point flag (a stack `flags[12][2]`) are not negative; the size grows by `step * 64` (written `* 64`; `<< 6` was rejected because it narrows the word load to `lhu`) up to 0x400. The retail head computes `half * work[0x15A6]` and `(work[0x15A8] / 8) * work[0x15A6]` without using them: template tests with dead arms (`if (half * h < 0) length = 0;`), whose multiplies survive flow while jump2 deletes the empty branches; the eighth needs its own local |
| 448 | `0x3AB0` | the header-428 port | arms at `work + 0x128`, the pointer set before the two dead template tests; radius `work[0x1B28] / 2`; a quad is sorted when `otz > 0`, with no flag array |
| 422 | `0x236C` | the header-398 webs | a line is sorted whenever its depth is positive |
| 398 | `0x202C` | the header-422 port | none |
| 398 | `0x2514` | header-397 webs | records at `work`, a staggered phase 0, a per-web `done` field |
| 398 | `0x18AC` | header-418 bands | each depth is clamped to zero before its sort, which also clears that point's `RotTransPers3` flag in a stack array `flag[i][j]`; the second quad reads column `j` through `(s32 *)band + j`, a pointer that moves on after the clamp and is still assigned after the loops, so cse keeps it as the target's `move v1,s0` copy |
| 422 | `0x170C` | header-418 bands | the radius is `/ 256` or `* 24 / 4096`; each depth is clamped to zero before its sort, which also clears that point's `RotTransPers3` flag in a stack array `flag[i][j]`; the second quad reads column `j` through `(s32 *)band + j`, a pointer that moves on after the clamp and is still assigned after the loops, so cse keeps it as the target's `move v1,s0` copy |
| 423 | `0x16E4` | the header-398 sheets | sheets at `work + 0x5E8`; the bias is `size * 768 / 4096`; the path progress is the `s16` at `work + 0xF1C`; sorted at `otz - 8`; the shrink step is `<< 6` |
| 405 | `0x2410` | header-397 sheets | sheets at `work + 0x147C`; the corners are passed as `&sheet->vN[k]`, so all four offsets become loop inductions; a quad is sorted at its unscaled depth when that is positive, with no flag test; the timing record's grow phase is at `+ 0x1C` |
| 425 | `0x1568` | header-428 spiral | sixteen arms at `work + 0x600` at `(i << 8)` and `(i << 9)` past the sweep at `work + 0x2708`; radius `0x200 * k` from a constant local that reload rematerialises; length from `0x400 - work[0x2710]`; fixed colours; scale from `+ 0x88` of the timing record at `work + 0x1E68`, shrunk by `work[0x2738]`. Both dead template tests stay, with `r = 0x200;` set before the eighth (the first test folds away but shares the `0x270A` load), and the arm pointer is taken from `ctx` before the `work` copy |
| 458 | `0x0E94` | header-443 ribbons | thirteen-point 0x2E4-byte ribbons (`Variant458Ribbon`) at `work + 0x1E4`, the pointer set before the first `ratan2`; the count is at `+ 0x14` of the record at `work + 0x2FCC`; the coil is `k * 2048 / 12` and the bend step `k * 512`; the projection flags live in the ribbon; the length grows to 12 and the view offset shrinks by `step * 128` |
| 458 | `0x21D0` | header-443 strand | 0x84-byte strands (`ModelVariantStrandWide`), `otz > 0` |
| 458 | `0x2514` | header-443 streamers | 0x334-byte streamers (the gap before `otz` is 4 bytes), `otz > 0` |
| 321 | `0x1B64` | the header-458 port | the depth test is `otz >= 0 && flag >= 0` |
| 376 | `0x16D8` | Spanish header-337 rings | rewritten over `ModelVariant376State`: one ring or two, no rotation angle, other scale limits and phases |
| 376 | `0x1B64` | header-443 streamers | a per-point `RotTransPers4` flag at `+ 0x2AC` in the record; a segment is sorted when `otz >= 0` and its flag is not negative; the radius is `0x180`, the width floor 3 and the shrink step `* 12` |
| 321 | `0x16A8` | the header-376 rewrite | two rings always, over `ModelVariant321State`; the second ring grows to `0x3000` and fades out with the first |
| 321 | `0x1EAC` | the header-376 streamers port | the radius is `work[0x1354] / 32` (an `s16` set after the turn angle) and the shrink step is `<< 4` |
| 324 | `0x29E0` | header-397 rings | `flag > 0` in place of `otz < 0x800` |
| 324 | `0x26C8` | header-405 spokes | `flag > 0` in place of `otz < 0x800` |
| 324 | `0x22F8` | Spanish header-432 draw | one ring over `ModelVariant324State`, placed at base + velocity * time / 1024, with its own phases |

The header-422 images are models 185, 391, 436, 504 and 594 at `+ 180` and 367
and 395 at `+ 200`; header 398 is models 102, 282, 288, 642 and 645 at `+ 200`;
header 458 is models 116 and 576 at `+ 180`; header 321 is models 164, 165,
210, 424 and 609 at `+ 180` and 34, 443 and 459 at `+ 200`; header 376 is models
427, 458 and 459 at `+ 180` and 190, 217, 221, 296, 457, 598, 612 and 647 at
`+ 200`; header 324 is models 7 and 552 at `+ 180`; header 459 is model 712 at
`+ 200`. Their slot-1 images (headers 572, 548, 608, 471, 526, 474 and 609) are
registered with `_slot1` wrappers, as above. The other
functions of these images stay in assembly.

Header 422 also has its own `curtains` helper at `0x2888` (264 instructions,
`variant422_curtains`): three 0x118-byte `ModelVariantCurtain` records, each two
rows of seventeen points drawn as sixteen `POLY_GT4` strips, turned by two
`ratan2` angles and faded from `0x800`. Each scale grows by `step << 7` to
`0x1000` and wraps with a count until the fourth phase. It needs the `work`
copy of `ctx`, and the packet pointer is set before the second angle.
Header 398 has the same helper at `0x2A44` (359 instructions,
`variant398_curtains`), which rebuilds the two rows of each shown curtain every
frame around the angle at `work + 0x1A2C`. The inner row is `rcos >> 4`, read as
`(u32)` so that the shift is `srl`; the outer row is at a radius of `lift +
0x180`, lifted by `lift + 0x80`, where `lift` is `((flags & 1) << 5)` times the
size of the first sheet at `work + 0x6A8`, over 4096. The strip colours are
stored inside the strip loop. The retail register and stack-slot layout needs
the sheet pointer set before the packet pointer and both before the second
angle, the declaration order `curtain, ot, i, lift, yaw, pitch, radius`, and
`i = 0` set between `lift` and `radius`.
Header 398's rings at `0x12B8` (381 instructions, `variant398_rings`) are the
three framebuffer rings of `variant398_entry` at `work + 0xE08`, drawn like the
French header-464 rings: turned by the two `ratan2` angles of the direction at
`work + 0x19C0`, scaled by the ring scale and moved to the translation at
`work + 0x19AC`. Past `0x800` the inner row is rebuilt at radius `r + 256` and
depth `-r`, where `r = (scale - 0x800) * 192 / 2048`; otherwise it is rebuilt
flat at radius 256. Each strip is textured from the screen half it lands on,
white at the inner row and tinted at the outer by `frame_count % 8` through the
entry's eight colours. Each scale grows by `step * 96` and wraps with a count
until phase 3. The flat-row loop must be written with `goto` and its own point
pointer. As a `for` loop it adds loop weight to the shared `angle`, so global
allocation puts the angle in `s3` and the first row's pointer in `s4`, the
reverse of retail.

Other images with a portable sibling are left out because they call into the
middle of a resident function, as model 168 does. Headers 372 (models 184 and
269), 435 (models 1, 360 and 550 at `+ 200`) and 172 (models 188 and 597) call
`0x800534BC`, `0x8002A5BC`, `0x80051288` and `0x8002B35C`. The spokes port
(header-405 spokes, reading three words at `0x1FF0` where the reference reads
three halfwords at `0x1D80`) and a sheets port with `otz > 0 && flag > 0`
matched those images' functions, but the images cannot be registered yet. The
header-172 slot-1 image has header 302, not 322, so it does not follow the
plus-150 rule either. Models 149 (header 388) and 264 (header 766) carry the header-324
rings and spokes with `otz < 0x800` and `otz >> 2` as the sort depth, and both
match, but model 149 calls `0x8004D5E8` and model 264's text has words that
link as `jal`s outside RAM, so neither image is registered.

## Images left unregistered

Eight more images contain these helpers, but their text calls addresses that
are not function starts in the North American executable. Examples:

- `0x80060A00` inside `Model_GetSharedPrimitiveHandler`;
- `0x8005F6D0` inside `func_8005F5C8`;
- a `jal` to `0x8053E210` in the `+ 200` image of model 1.

They are headers 435 (models 1, 360, 550 at `+ 200`), 172 (188, 597 and 149 at
`+ 200`) and 372 (184 and 269 at `+ 180`). They are left as they are until it is
known how those images are loaded. No symbols are declared inside resident
functions to make them link. The header-372 images now build their entry from
`variant372_entry.c`, but these helpers stay generated assembly there.

## Header 412 entry

`variant412_entry.c` (profile `gcc_2_7_2_cdk_g0`, `0x133C` bytes) builds the
North American image `model_variant_517_pos0_slot0` (model 517, record 467,
sector 129072). It is the header-407 entry with the 48 petals replaced by a
ten-sprite trail at `work + 0` and six flares at `work + 0x2FC`, a `0x3C`-byte
config whose word at `+0x38` raises the root part's matrix height, no
`+0x38` image upload, a sweep of `6144` over the config window `+0x30..+0x34`
that offsets the flare positions by `rsin(sweep) * 512 >> 12`, and two helpers
(`0x1340` from frame `+0x1C`, `0x18B8` from phase 2) kept as generated
assembly. The opposite-slot copy must be written as two calls: one call with a
computed argument compiles to `sltiu`, while retail branches to set `$a0`.
Declaring `packed` before the web radii places it at `sp + 0xB8`.
