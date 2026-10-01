# French MODEL headers 431 and 581

2 distinct secondary images reuse the accepted
`src/overlays/model_variant/variant414_*.c` spokes, rings
bodies through 4 three-line canonical renaming wrappers.
The existing `gcc_2_8_1_g0_split` profile uses GCC 2.8.1/MASPSX 2.81.
Shared bodies, headers, G32/PSXLONG annotations and profiles are unchanged
from accepted master `eed176532`. The accepted PSXLONG migration
invalidated earlier source fingerprints, so calibration, layouts and actual
canonical links were freshly rebuilt against the migrated sources.

## Loader and boundaries

Models 401 use these images at stages 7/8.
The matched loader selects ten 2,048-byte sectors from 276-sector compact
records, at record offsets 180/190.
Loads are `0x8013B000`/`0x8017B000`, with entry at `+4`; the controller
supplies context and the initial nonnegative command or update `-1`.
The [instance ledger](french-model-variant431-instances.csv) records
independently checked indices, commands, slices and complete hashes.

Model 401 here means stages 7/8 and headers 431/581, not the separately
registered model-401 stages 9/10/header-432 renderer.

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `0x4..0xF1C` | 3864 | generated assembly | yes |
| `0xF1C..0x1338` | 1052 | webs C | yes |
| `0x1338..0x17C8` | 1168 | generated assembly | yes |
| `0x17C8..0x2444` | 3196 | generated assembly | yes |
| `0x2444..0x28C4` | 1152 | sheets C | yes |
| `0x28C4..0x2BCC` | 776 | spokes C | no |
| `0x2BCC..0x2F48` | 892 | rings C | no |

Strict control-flow walks cover every word of each span with one terminal
return and no unresolved indirect transfer. The original spokes and rings
C helpers remain **retained code, not direct-entry reachable**. The
entry-called webs and sheets follow-ups are described below. Each four-byte header and 8,376-byte suffix at
`0x2F48..0x5000` has a real storage owner. Suffixes remain unclassified,
not proven non-code.

## Accessed layouts

Entry captures `a0 -> s6`. Six 152-byte sheet views begin at context
`+0xA80`, six 144-byte rings at `+0xE10`, and four 144-byte spoke records
at `+0x1170`. Adjacent extents agree exactly. Entry clears the first sheet's
size at `+0x88`, consumed by the spokes helper. The separate five 116-byte
records at `+0x13B0` are **not** declared as `ModelVariantQuad`; this family
has no promoted quad helper.

57 entry anchors check pointer capture/saves/reloads,
initialized counters, increments, bounds, strides and accessed fields.
Seventy-one freshly target-compiled local C/SDK constants check primitive
widths, ring/spoke/quad and sheet-prefix views, `SVECTOR`, `VECTOR`,
`MATRIX`, `GsCOORDINATE2`, `GsGLINE`, `POLY_G4` and `POLY_GT4`.
Unrelated generic type sizes do not establish runtime record layouts.
These are accessed-view observations, not whole-context allocation proof.

## Exactness and preservation

The [attempt ledger](french-model-variant431-attempts.csv) records
4 terminal wrapper matches. Candidate rebasing was
only a locator; actual links reproduce all 2 complete unmasked images.
Initial production verification checked selected object owners and final ELF bytes:
4 C owners / 3,336 C bytes,
10 assembly owners / 20,864 assembly bytes,
4 raw owners, 36 independently verified fresh-resident
callees, and all 71 recompiled layout constants.

Five inherited regressions check archive slices/commands/hashes, canonical
sources/fingerprints, all spans/local calls, entry anchors and raw extents.
The combined 431/440/465 batch preserves all 158 accepted French
registrations, reproduces 168 complete images and the clean
French resident, and adds 28 C instances / 23,656 instruction bytes.
Configured totals are 714/1231 C instances
and 641,460 C instruction bytes. The accepted #6668 images are preserved;
no unmerged PR is stacked. Untranslated functions, unclassified tails and unregistered
runtime areas remain; these totals do not establish exhaustive coverage.
General report snapshots remain separate.

## Entry-called narrow webs follow-up

The accepted US324 webs body has the same control-flow and declaration-order
structure as this family's `0xF1C..0x1338` helper. Sixteen independently
measured context offsets differ, and French431 sorts at **nonnegative** depth
and projection flag rather than requiring both to be positive. Applying only
those differences gives exact 1,052-byte functions with 296-byte frames in
both slots, then exact complete images.

The standalone French431 body and its three-line slot-one wrapper preserve
the existing local `ModelVariantWebNarrow` and SDK declarations. No Spanish
webs body was present. The accepted US324 source also serves French341 at
its original offsets and positive-only threshold; it remains unchanged,
along with all shared headers and compiler profiles. A blanket French
conditional would incorrectly conflate these two French families.

Twenty-four target-compiled layout constants and 154 instruction anchors
verify the views. Entry and helper independently agree on three 416-byte
records at `context + 0..0x4E0`: two 4-by-6 `SVECTOR` grids at `0/0xC0`,
48-byte rows, color at `0x180`, scale at `0x194`, and done word at `0x198`.
Entry uses six-element/four-row/three-record bounds, initializes the color
bytes to 255, initializes done to zero and staggers the scales. The next
record array begins at `0x4E0`; these are not inferred whole-context types.

The reusable `GsGLINE` packet is at `0x1730`; projection `p/flag` occupy
separate frame words at `0xD0/0xD4`. Four ignored `ratan2` calls remain,
including their signed-halfword arguments. Phase word `0x18E0` selects
word translations at `0x1758/0x175C/0x1760` below two, otherwise signed
halves at `0x1764/0x1766/0x1768`; it also selects which endpoint is black.
All 24 lines per record retain depth/flag checks and low-sixteen-bit sorting.
Colors fade above scale `0x1800` toward zero at `0x2000`.

Scale advances by step word `0x18A8` shifted eight, wrapping at `0x2000`
except in phase five, which clamps scale and sets that record's done word.
The third record's completion sets phase six. The separate local `done`
stays one: the helper does not scan all three stored done words. Its
constant comparison and corresponding retail branch are preserved.

Entry calls at `0xDB4` only when signed phase is at least two, loading the
original caller context at `0xDB0`. The helper's below-two path remains
despite that entry guard. Command `597000`, independently read from compact
record 351's stage command table, selects descriptor zero at `0x3044` with
a 104-byte stride. Entry's shift/add sequence establishes that stride;
this helper does not read timing denominators. The direct context minimum
is `0x18F4`, disjoint from the measured model, primary and secondary loads,
not an allocation-capacity claim. The suffix remains unclassified.

All eight helper callees, 36 existing resident binding addresses and three
resident caller owners agree with the resident ELF and retail bytes.
Only the existing `0x80089928` SDK binding receives the established
`ratan2` name; no binding address or global declaration is added.

Both final canonical sources independently reproduce complete images.
Combined scratch links additionally use freshly compiled retained spokes
and rings, proving six actual C owners / 5,440 bytes across both images.
The four historical terminal ledger rows remain byte-identical, followed
by two webs matches. This independent batch starts from accepted
`69e8f56f8e4727e91f54f554942f3428053f0eba`, excluding pending French476
helpers, and adds two C instances / 2,104 bytes. Configured totals become
252 images, 1,134/1,581 C instances and 1,268,644 bytes.
Web production acceptance passed: all 252 complete French overlays and the
clean French resident match. Normal links select all six family C objects
and size their exact functions in the final ELFs, preserving the four prior
spokes/rings owners / 3,336 bytes. Eight assembly owners / 18,760 bytes and
four raw owners / 16,760 bytes cover the remaining complete-image bytes.

## Entry-called six-sheet renderer

The two 1,152-byte sheets functions at `+0x2444` match with 272-byte
frames using the existing `gcc_2_8_1_g0_split_no_cse_follow_jumps` profile.
The default split profile produced the same instruction shape but exchanged
the context and vertex registers in 20 words. Eight rejected source/profile
experiments and the exact ninth calibration remain recorded alongside the
two canonical matches; the six prior terminal records are unchanged.
No compiler flags, shared source, header, SDK binding or profile is changed.

The standalone French body reuses the accepted US423 projection/color
structure and existing `ModelVariantSheet` and SDK declarations. Its
translation, visibility and terminal-sheet updates are independently
recovered from these French images, rather than assumed regional equality.
Only the second-slot wrapper renames the function.

Thirty-nine target-compiled constants and 250 retail instruction anchors
check the layouts and control flow. Entry initializes six 152-byte sheets
at `0xA80..0xE10`, with four `SVECTOR` rows at `0/0x20/0x40/0x60`,
outer color at `0x80`, inner color at `0x84`, and size at `0x88`.
The outer color is white, the inner color is `(255, 128, 0)`, and size
starts at zero. The helper reuses the `POLY_GT4` packet at `0x1668`;
projection `p/flag` occupy frame words `0xD0/0xD4`.

Five separate 288-byte companion records occupy `0x4E0..0xA80`. Entry
initializes five progress words per record at `0xF4..0x104` to
`-256 * (record + point)`. The `+0x17C8` helper advances them by
`step << 7` and clamps at `0x400`; its projected depth is instead stored
at `0x10C`, so the sheets do not mistake a depth for progress. These
accesses justify an opaque byte cursor, not a new whole-record declaration.
The sixth sheet iteration still reads the cursor's `+0xF4` before ignoring
the amount. That address is the second sheet's `+0x5C`, **not evidence of
a sixth companion record**. The retail read is preserved.

The first five sheets use `max(first_progress, 0)` to interpolate word
positions at `0x1794 + i * 0x20` with deltas at `0x1820 + i * 0x10`,
dividing by `0x400`; their three scales are `0x400`. The terminal sheet
uses signed-halfword coordinates at `0x1764/0x1766/0x1768` and its own
size, with an additional `size / 8` on odd `0x189C` parity.
Each sheet projects four quads. Depth is multiplied by eight and divided
by ten, then sorted without the US423 helper's minus-eight adjustment.
Depth and flag must both be nonnegative. The first five sheets additionally
require their last progress word to be below `0x400`; the terminal sheet
bypasses that progress gate.

Only the terminal sheet updates size: phase `0x18E0 == 2` grows by
`step << 9` to `0x1000`; phase three shrinks by `step << 5` to zero and
then sets phase five. Entry calls at `0xD60`, loading the original context
at `0xD5C`, when unsigned frame `0x18A0` reaches descriptor `+0x2C`.
Command `597000` selects the existing 104-byte descriptor at `0x3044`,
whose sheet threshold is ten. This is a descriptor-frame gate, not a phase
gate. The sheet call precedes the companion-advancement call at `0xD94`.

Canonical scratch links reproduce both complete images with all eight
freshly compiled C owners / 7,744 bytes, preserving the six prior owners /
5,440 bytes. Nine helper callees, all 36 resident bindings and three resident
caller owners agree with the resident ELF and retail bytes. Six assembly
owners / 16,456 bytes and four raw owners / 16,760 bytes cover the rest.
The independent accepted-master base is
`7ec87f5733e957ca8b43055437c158acbbff0f21`; no pending report is stacked.
This adds two C instances / 2,304 bytes, for configured French totals of
1,162/1,581 instances and 1,304,564 bytes across 252 images. Untranslated
functions and unclassified suffixes remain; this is not exhaustive coverage.

Sheet production acceptance passed: all 252 complete French overlays and
the clean French resident reproduce the retail bytes. The production ELFs
select all eight C owners above, with the new assembly fallbacks absent.
All 39 layout constants are freshly recompiled and the 39 resident
binding/caller owners rechecked against the clean resident ELF. An additional
510 C owners across nine unchanged French families retain their original
sources, metadata and exact linked bytes. All 225 French, 133 Spanish and
21 progress/toolchain regressions pass without skips, together with the
attempt-ledger, basic-type, metadata, G32 and notes checks.
Fresh linked owners also preserve all 84 accepted Family421, 24 Family422,
208 Family435, 56 Family439, 72 Family445 and two prior Family476 C owners.
All 209 French, 112 Spanish and 21 progress/toolchain regressions pass without
skips, alongside metadata, attempt-ledger, basic-type and G32/PSXLONG checks.
The 24 constants were freshly compiled and all 39 binding/caller intervals
agree with the clean resident.

### Accepted French476 reconciliation

After independent acceptance, the maintainer-accepted French476 helpers
squash `355c0d797675432b8285e5741b965a00793973b4` was ordinarily merged.
All 17 authored paths were checked byte-for-byte against its reviewed head,
and all twelve exact-head checks succeeded. Only the aggregate progress
assertions conflicted. The combined configured totals are 252 images,
1,138/1,581 C instances and 1,272,476 bytes; no pending PR is included.
Fresh combined production acceptance passed: all 252 complete French images
and the clean resident match. The six Family431 C owners / 5,440 bytes remain
exact alongside all six accepted Family476 C owners / 5,752 bytes and the
previously checked Family421, 422, 435, 439 and 445 owners. All 211 French,
112 Spanish and 21 progress/toolchain regressions and policy checks pass
without skips. Thirteen of the original fifteen helper paths remain
byte-identical; only this note and the aggregate progress fixture change
during reconciliation.
