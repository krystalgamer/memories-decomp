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
| `0x2444..0x28C4` | 1152 | generated assembly | yes |
| `0x28C4..0x2BCC` | 776 | spokes C | no |
| `0x2BCC..0x2F48` | 892 | rings C | no |

Strict control-flow walks cover every word of each span with one terminal
return and no unresolved indirect transfer. The original spokes and rings
C helpers remain **retained code, not direct-entry reachable**. The
entry-called webs follow-up is described below. Each four-byte header and 8,376-byte suffix at
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
Fresh linked owners also preserve all 84 accepted Family421, 24 Family422,
208 Family435, 56 Family439, 72 Family445 and two prior Family476 C owners.
All 209 French, 112 Spanish and 21 progress/toolchain regressions pass without
skips, alongside metadata, attempt-ledger, basic-type and G32/PSXLONG checks.
The 24 constants were freshly compiled and all 39 binding/caller intervals
agree with the clean resident.
