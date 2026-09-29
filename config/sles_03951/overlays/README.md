# Spanish overlay build configuration

Run `make verify-spanish-inputs` and `make spanish-match-overlays` from the
repository root. The inputs are the unmodified Spanish `SLES_039.51`,
`DATA/SU.MRG`, `DATA/WA_MRG.MRG` and `DATA/MODEL.MRG`, placed under
`game/spain/`; their sizes and hashes are pinned in `../target.yaml` and
`../files.sha256`. The Spanish CI workflow uses the shared authenticated
`YGOFM_CI_FILES` bundle described in
[continuous integration](../../../notes/continuous-integration.md).

The original six independently linked runtime modules reuse the existing European C
sources and named GCC 2.8.1/MASPSX 2.81 profiles unchanged. Only the resident
symbol bindings and retail archive/module hashes differ. No Spanish C copies,
new inline assembly, or source-local external declarations are needed.

| Module | Archive | First sector | Sectors | Matching functions | C bytes |
|---|---|---:|---:|---:|---:|
| `free_duel` | `WA_MRG.MRG` | 9304 | 5 | 9 | 4252 |
| `main_menu` | `SU.MRG` | 98 | 16 | 31 | 18280 |
| `overworld_after_coup` | `WA_MRG.MRG` | 9920 | 6 | 15 | 6184 |
| `overworld_before_coup` | `WA_MRG.MRG` | 9762 | 6 | 15 | 6184 |
| `password_a` | `WA_MRG.MRG` | 9374 | 15 | 27 | 10476 |
| `password_b` | `WA_MRG.MRG` | 9460 | 15 | 27 | 10476 |
| `duel_effects` | `WA_MRG.MRG` | 7193 | 44 | 84 / 85 | 80968 |
| `model_intro` | `SU.MRG` | 1767 | 16 | 5 | 1484 |
| MODEL copy primaries (4 images) | `MODEL.MRG` | Per record | 8 | 4 | 1104 |
| MODEL effect primaries (10 images) | `MODEL.MRG` | Per record | 20 | 10 | 2160 |
| MODEL particle primaries (6 images) | `MODEL.MRG` | Per record | 12 | 6 | 13168 |
| MODEL variant 408/558 (2 images) | `MODEL.MRG` | 15104 / 15114 | 20 | 6 | 10400 |
| MODEL return-two primaries (2 representatives) | `MODEL.MRG` | 220 / 222 | 4 | 2 | 16 |
| MODEL variant 432/582 (2 images) | `MODEL.MRG` | 97076 / 97086 | 20 | 6 / 10 | 5928 |
| Total configured | | | 207 | 247 / 252 | 171080 |

Sector sizes are 2048 bytes. Counts are per module instance: the overworld and
password variants share code but have different archive slices and full-image
hashes. These are not 124 distinct new C implementations. Main-menu code loads
at `0x80180000`; the other five original modules load at `0x80168000`.

The additional `model_intro` SU module also loads at `0x80180000`. Its five
recovered routines occupy `0x80180004..0x801805D0`; the complete 32,768-byte
production image matches, with real generated-data owners rather than absolute
overlay aliases. The 31,116-byte tail at offset `0x674` remains **unclassified
preserved storage, not C coverage**. The configured counts are not an exhaustive
runtime-completion claim. See the
[SU intro evidence](../../../notes/overlays/spanish-model-intro.md).

The MODEL primaries load at `0x8013A000` and `0x8017A000`. Every image retains
its complete unclassified ranges and real descriptor storage, not absolute
data aliases. The copy/effect families have 48-byte descriptor-prefix owners.
The six particle images additionally retain real scale/image owners and
16-, 12- or 90-byte descriptors. These 20 entries contain 16,432 matching
instruction bytes across five shared bodies, not 20 distinct implementations.
See the
[MODEL primary evidence](../../../notes/overlays/spanish-model-primary.md).

Two model-zero representatives cover the eight-byte return-two primary body at
both slots. The full-image evidence ledger covers all 1,222 instances across
611 compact records, but configured progress counts only the two registered
images and 16 instruction bytes. Every 4,084-byte suffix remains unclassified;
these are not identical-image duplicates. See the
[return-two evidence](../../../notes/overlays/spanish-model-return-two.md).

Model 54's second variant loads at `0x8013B000` / `0x8017B000` during
stages 9/10. Each ten-sector image contains three matching functions and
148 bytes of individually identified image/configuration storage. The
remaining 15,128 suffix bytes per image stay unclassified and uncounted.
These two images are not a completion claim for all MODEL variants.
See the [variant 408/558 evidence](../../../notes/overlays/spanish-model-variant408.md).

Model 401's second variant has matching 848-byte, 1,064-byte and 1,052-byte
drawing helpers at each slot. Two other functions per image remain explicitly game-owned unmatched
assembly; all 12,576 bytes after offset `0x1EE0` remain unclassified storage.
The lower configured percentage reflects the newly inventoried unmatched
functions, not a regression in previously accepted C. See the
[variant 432/582 evidence](../../../notes/overlays/spanish-model-variant432.md).

The added duel-effect bank instead loads at `0x80146000`. Its representative
image is 90,112 bytes; all seven terrain copies at `7193 + terrain * 240`
have SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
`duplicate_sector_offsets` checks every complete copy before building the
representative, without multiplying identical inventories. The table's sector
total counts representative images only.

This bank reuses the accepted shared `duel_effects/utility_helpers.c` and
`duel_effects/texture_words.c` and `duel_effects/vector_init.c` units with
`gcc_2_8_1_g0_split` unchanged. Two additional complete groups,
`number_helpers.c` and `primitive_draw.c`, add four functions / 964 bytes
using that same profile. The `textured_quads.c` pair adds another 536 bytes.
The complete `layered_drawing.c` group adds three functions / 1,696 bytes.
The contiguous `packet_helpers.c` group adds four
functions / 828 bytes: packet translation/submission and matrix initialization.
Three accepted French groups, `color_test.c`,
`quad_helpers.c` and `matrix_helpers.c`, add six more functions / 588 bytes
without changing shared source. `color_transition.c` and `screen_draw.c`
add six functions / 860 bytes for color transitions and fullscreen polygons.
The complete `drawing_tail.c` group adds three functions / 1,204 bytes for
fixed and randomly selected texture tiles and two-sided projected strips.
The complete `gradient_lines.c` routine adds 760 bytes for projected gradient
segments and one-time endpoint capture.
The `gradient_strip.c` routine adds 652 bytes; unchanged accepted French
`projected_wrappers.c` and `matrix_setup.c` add three functions / 676 bytes.
The complete `display_quads.c` group adds three functions / 1,004 bytes for
four-part texture displays, translated tiles and paired gradient bands.
The complete `cross_lines.c`, `circle_vertices.c` and `random_vectors.c`
helpers add three functions / 532 bytes for crossed screen lines, a 32-point
circle and signed random-vector generation.
The complete `dispatch.c` entry adds 1,288 bytes for texture setup and 25
effect cases; unchanged accepted French `rect_vertices.c` adds 148 bytes.
The dispatcher establishes 21 texture pairs / 84 bytes at `D_8015B748`.
Its canonical union retains every accepted named-prefix offset while exposing
the full pair array; this and the image/mode tables remain generated data.
The unchanged shared effect-4 and effect-5 lifecycles add 4,352 bytes after
independent Spanish whole-bank verification. Their real configuration and
initial-vector owners, layouts and variant bounds are documented in the
[Spanish reuse evidence](../../../notes/overlays/spanish-duel-effects-4-5.md).
The unchanged shared effect-10 lifecycle adds another 2,684 bytes, including
all six variants and the phase-triggered dissolve. Its separate image table
retains real generated storage; see the
[Spanish effect-10 evidence](../../../notes/overlays/spanish-duel-effect-10.md).
The complete shared effect-11 breakup lifecycle adds 5,048 bytes after
independent Spanish verification. Its canonical 180-byte mode/image union
preserves the overlap at mode 20 without inventing a separate data alias.
See the [Spanish effect-11 evidence](../../../notes/overlays/spanish-duel-effect-11.md).
The complete shared effect-17 vortex adds 5,100 bytes after independent
Spanish whole-bank and real-owner verification. Its halfword brightness
contract preserves low-byte packing and the complete shared helper object.
See the [Spanish effect-17 evidence](../../../notes/overlays/spanish-duel-effect-17.md).
Complete shared effects 7 and 13 add 5,592 bytes after independent Spanish
whole-bank, target-layout and real-data-owner verification. No existing shared
implementation or caller contract changes.
See the [Spanish effects 7/13 evidence](../../../notes/overlays/spanish-duel-effects-7-13.md).
The complete effect-16/20 lifecycle adds 2,800 bytes, retaining overlapping
particle windows, five-column activation and both completion paths; see the
[effect-16/20 evidence](../../../notes/overlays/spanish-duel-effects-16-20.md).
The complete `number_renderer.c` adds 1,024 bytes for projected signed glyphs.
It uses the canonical texture-pair table and a separate, real 96-byte glyph
owner; see the [renderer evidence](../../../notes/overlays/duel-effect-number-renderer.md).
The unchanged shared effect-23 card sweep adds 3,708 bytes after independent
Spanish verification, preserving the same-byte card rotations and both
completion paths. See the
[Spanish effect-23 evidence](../../../notes/overlays/spanish-duel-effect-23.md).
The complete effect-22 ritual lifecycle adds 8,216 instruction bytes and its
228-byte compiler-generated switch table, linked separately in the original
header interval. See the [effect-22 evidence](../../../notes/overlays/spanish-duel-effect-22.md).
The unchanged accepted French effect-24 Exodia burst adds 3,956 bytes after
independent Spanish whole-bank and seven-copy verification. Its initial scale
has real generated storage, and its five-slot copy uses the existing canonical
resident API. See the
[Spanish effect-24 evidence](../../../notes/overlays/spanish-duel-effect-24.md).
Including the accepted effect-8 and effect-12 lifecycles, the 84 exact C
functions cover 80,968 bytes.
The complete text interval
`0x80146258..0x8015A1E4` contains 85 provisional boundaries / 81,804 bytes;
the remaining curve stays generated assembly and visible in progress. External C
bindings for `rand`, `ScaleMatrix`, `GsSetLsMatrix`, `MulMatrix2`, `RotTrans`,
`RotMatrix`, `RotAverage3`,
`RotAverage4`, `GsSortPoly`, `GsSortGLine`, `GsSortLine`, `ccos`, `csin`,
`RotTransPers`, `SetGeomOffset` and the
resident packet submitter agree with the Spanish resident inventory.
The ordering-table pointer remains defined by preserved module data; no raw
data is claimed as C-owned storage. The effect-22 table is actual compiler
`.rodata`, not preserved raw data or an absolute jump-table alias.
The packet helpers likewise only read the preserved vector and priority data;
their [recovery notes](../../../notes/overlays/duel-effect-packet-helpers.md)
record the differing FT4/GT4 priority paths and untouched matrix padding.
The [color helpers](../../../notes/overlays/duel-effect-color-helpers.md)
reuse the resident `D_8009B300` declaration and its measured Spanish binding
at `0x8009C688`, rather than allocating overlay storage for that word.

The textured quads use the existing SDK `addVector` comma-expression macro.
Separate field statements are semantically similar but do not reproduce the
original induction/register allocation. Their private header describes a
72-byte stack record and only the observed texture-word prefix at
`D_8015B748`; it does not define or claim the underlying module storage.
See the [textured-quad matching record](../../../notes/overlays/duel-effect-textured-quads.md)
for the rejected forms and complete-bank acceptance.
The [layered-drawing record](../../../notes/overlays/duel-effect-layered-drawing.md)
adds the observed texture-word pairs at offsets zero and four while preserving
the earlier pair's offsets. It also records the projection branch ordering,
two-pass color/geometry changes and the GT4 stack record's unaccessed gap.

The loader sequence and region-independent bank layout are documented in
[duel-effect bank research](../../../notes/overlays/duel-effect-bank.md).
All seven Spanish copies and the boot module were independently compared,
not inferred from French alone. The boot image at WA sector 9509 (three
sectors, load `0x80168000`) is still unregistered pending ownership analysis.
MODEL/SU loads and the preserved overworld fragment also require further
coverage work. These seven configured modules are not an exhaustive runtime
completion claim.

Spanish instruction words at 2205 relocation sites in the verified European
objects recover 332 symbol/address bindings. Consistency across all uses,
unchanged opcode bits, and final full-module SHA-256 matches validate those
bindings. The issue's `eur_esp.csv` is supplementary evidence, not an authority:
it incorrectly maps European `GsSetProjection` at `0x80085544` to `0x8004AB68`,
whereas the Spanish calls require `0x80085748`; European `DisplayObject_Reset`
at `0x80040514` similarly requires `0x80040714`, not `0x8008B768`.

Initial linkage exposed an implicit European auto-symbol, `D_8009C02B`, absent
from the password symbol files. The Spanish linker files explicitly bind that
shared C name to the recovered `0x8009C44B`; all six full modules then match.

The original layouts are preserved, including raw data, jump-table ownership,
and the overworld assembly range at offsets `0x1618-0x17D0`. That range is not
claimed as matching C. Retail archives, extracted modules, generated assembly,
object files, and relocation probes remain ignored under `game/` and `tmp/`.

The progress generator includes these inventories alongside the Spanish
resident metrics. `make spanish-match` verifies the independent resident
executable; `make spanish-match-overlays` verifies these runtime modules.
Progress snapshots are refreshed separately under #443.
