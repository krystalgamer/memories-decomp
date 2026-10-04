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
| `0x4..0xF1C` | 3864 | entry C | yes |
| `0xF1C..0x1338` | 1052 | webs C | yes |
| `0x1338..0x17C8` | 1168 | fan C | yes |
| `0x17C8..0x2444` | 3196 | bands C | yes |
| `0x2444..0x28C4` | 1152 | sheets C | yes |
| `0x28C4..0x2BCC` | 776 | spokes C | no |
| `0x2BCC..0x2F48` | 892 | rings C | no |

Strict control-flow walks cover every word of each span with one terminal
return and no unresolved indirect transfer. The original spokes and rings
C helpers remain **retained code, not direct-entry reachable**. The
entry-called webs, fan and sheets follow-ups are described below. Each four-byte header and 8,376-byte suffix at
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

## Entry-called five-fan renderer

Both 1,168-byte fan functions at `+0x1338` match with 264-byte frames using
the existing `gcc_2_8_1_g0_split` profile. The accepted US414 body first
produced only two differing address-register instructions at helper
`+0x37C/+0x380`: the target retains the descriptor base in `v1` rather than
`v0`. The no-CSE profile and ordinary typed-pointer addition still differed
at those two words. A typed descriptor base minus a negative index, following
the existing French `PETAL_SCALE` expression pattern, resolves both words.
All four two-slot experiments are retained before the two canonical terminal
records; all 26 earlier ledger rows remain byte-identical.

The regional body reuses the unchanged `Variant414Fan` and SDK header.
Keeping the measured expression local preserves the accepted US source and
its build contract; no shared source, header, binding or profile changes.
No alternate compiler was used. The independent source draft was not
compiled once the accepted US body became available for calibration.

Forty-two target-compiled constants and 393 retail instruction anchors check
the recovered layouts, entry gates and updates. Five 116-byte records occupy
`0x13B0..0x15F4`, immediately preceding the next packet. Each contains eleven
`SVECTOR` points, inner color at `0x58`, outer color at `0x5C`, size at `0x60`
and done at `0x70`. Entry constructs a center and two five-point ellipses,
initializes inner RGB `(255, 192, 192)` and outer RGB `(64, 64, 0)`, and clears
size and done. The helper reuses one 36-byte `POLY_G4` at `0x1610..0x1634`.
These independent offsets and strides justify the existing header; no
whole-context or unrelated-record ownership is inferred.

Each record emits four quads, pairing center point zero with edge triples
`1/2/3`, `3/4/5`, `6/7/8` and `8/9/10`. The center corner receives inner
color; the other three receive outer color. Positions are words at
`0x1794/0x1798/0x179C + i * 0x20`, rotation uses the low half of `0x18DC`,
and odd `0x189C` parity doubles that record's size. Projection `p/flag`
occupy separate frame words at `0xD0/0xD4`; the ordering-table pointer is
spilled at `0xD8`. Both depth and flag must be nonnegative, and sorting uses
the unmodified low sixteen depth bits.

Entry calls at `0xD34` with the original context loaded at `0xD30`, after
unsigned frame `0x18A0` reaches descriptor `+0x14`. This gate exists even
though the selected descriptor's threshold is zero in both images. It
precedes the separate sheets gate and companion-advancement call.
Command `597000` selects the 104-byte descriptor at `0x3044`.

The fan changes size only when unsigned frame reaches the selected word at
`descriptor + 0x2C + index * 4`, minus eight. Entry initializes index
`0x18D8` to zero. The companion helper at `+0x17C8` increments it only while
below ten, on completion of the last progress lane of the fifth companion.
After advancing the index, that helper rearms all five fan done flags at
`0x1C90..0x1CB4`, without resetting their sizes in that loop.
All eleven selected timing words fit within the descriptor; both retail
images contain `10, 40, 70, ..., 310`. This is independent of the caller's
zero threshold and the sheets' fixed first-word threshold of ten.

Size below `0x1000` with done zero grows by `step[0x18A8] << 10`, clamps to
`0x1000`, and sets done to one. Positive size with done one instead shrinks
by `step << 8` to zero. Outside the size gate, angle advances by
`192 * step` on every call. This helper does not update phase `0x18E0`.
The direct context minimum remains `0x18F4`, not an allocation-capacity claim.

Canonical scratch links reproduce both complete images using all ten
freshly compiled C owners /10,080 bytes, preserving all eight prior owners
/7,744 bytes, including both accepted sheets and their no-CSE profile.
Four assembly owners /14,120 bytes and four raw owners /16,760 bytes cover
the rest. Nine fan callees, all 36 resident bindings and three resident
caller owners agree with the resident ELF and retail bytes.
The independent accepted base is `2e4a727ade1fcd92b617aca99c30cf7daf9a3dc1`,
without pending French341 fans. This adds two C instances /2,336 bytes;
configured French totals become 1,164/1,581 C instances and 1,306,900 bytes
across 252 images. Untranslated code and unclassified suffixes remain;
these totals do not establish exhaustive French completion.

Fan production acceptance passed: all 252 complete French overlays and the
clean French resident reproduce retail bytes. Production ELF and defining
object checks select all ten family C owners, with the new assembly fallback
absent, retaining both accepted sheet owners. All 42 layout constants were
freshly recompiled and all 39 resident binding/caller owners rechecked.
An additional 526 C owners across ten unchanged French families preserve
their sources, metadata and exact linked bytes. All 228 French, 133 Spanish
and 21 progress/toolchain regressions pass without skips, including the
companion rearm-loop anchors, together with repository policy checks.

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

### Accepted Family341 fan reconciliation

The independently published fan checkpoint `7fc800a99` is ordinarily
reconciled with accepted master `06d5a34bd`, including the four accepted
Family341/491 fan owners. The sole textual conflict is the aggregate
French progress fixture; the shared attempt-history fixture merges both
families' independent records. No pending PR is stacked, and the separate
French465 orbit checkpoint is not included.

Only the original two Family431/581 fan instances /2,336 bytes are new
relative to accepted master. Both accepted Family431 sheet sources and
their no-CSE profile remain unchanged. Combined configured totals are
252 images, 1,168/1,581 C instances and 1,311,428 instruction bytes.
The original source, metadata, layout and attempt fingerprints remain
unchanged; reconciliation affects only this note and the two shared
fixtures among the thirteen original authored paths.

Reconciled production acceptance passed: all 252 complete French overlays
and the clean French resident match. Production ELF and defining-object
checks retain all ten Family431 C owners /10,080 bytes and all twenty
Family341 C owners /19,360 bytes, including all four accepted fans.
Both accepted Family431 sheets retain their source and no-CSE profile.
Fresh compilation checks all 42 fan layout constants; all 36 resident
bindings and three caller owners retain exact resident bytes.
All 231 French, 133 Spanish and 21 progress/toolchain regressions and
repository policy checks pass without skips. Ten original authored paths
remain byte-identical. Fresh coordinated CI is still required for the
new merge head; previous-head checks are not substituted.

## Complete entry initialization and update

Both 3,864-byte entries now match with 240-byte frames under the unchanged
`gcc_2_8_1_g0_split` profile. The standalone French body reuses the canonical
web, sheet, ring, spoke and `Variant414Fan` declarations, plus the accepted
entry projection view and SDK headers. Its slot-one wrapper relocates all
four local helper calls and the suffix resource base. The five previously
accepted helpers per image, including the sheets' no-CSE profile, remain
unchanged.

The initial candidate was four bytes short: GCC factored the two progress
terms into one final shift. Expressing the outer band term as `i << 8`
preserves the independently observed outer-band and inner-point induction,
recovering the complete instruction sequence. The remaining 37 differences
were stack slots only. Declaring the flat-textured pointer after the other
packet pointers, and the web-angle scalar before the row counter, recovers
every byte. All three two-slot experiments precede the canonical terminal
records in the attempt ledger. No fixed registers, inline assembly,
one-off flags or masked acceptance are used.

The entry initializes five 288-byte companion records at
`0x4E0..0xA80`, three existing narrow webs, five existing 116-byte fans,
six sheets, six rings and four spokes. Each companion exposes five colors
per row at `0xB4/0xC8`, scale at `0xDC`, five signed progress words at
`0xF4`, and the initialized word at `0x108`. Its other fields remain
uninterpreted. Existing renderer declarations are not replaced by these
entry-only views. The two unused matrix locals retain the observed
64-byte stack interval; they do not assert recovered runtime matrix state.

Fresh target compilation checks 123 size/offset constants, and 206 literal
retail anchors cover the entry and retained helper evidence. Both full
archive slices and their actual 104-byte descriptors are independently
hashed. Command `597000` selects descriptor zero at `0x3044`, with parts
`22, 17, 9, 2, 14`, the first threshold zero and eleven timing words
`10, 40, ..., 310`. The descriptor's remaining four timing-view words are
zero, not additional observed animation stages. The retained companion
helper bounds its index at ten. The entry's `0x18F4` view is a minimum
accessed extent, not whole-context allocation proof.

Initialization and updates both obtain five part matrices and target
deltas. Updates preserve the unsigned projected target halves, the signed
second projection's y half, and the original ordering of fan, sheet,
companion and web calls. Distinct descriptor gates are retained even where
the selected threshold is zero. Frame-step accounting, phase-dependent
tint and return-state transitions remain byte-identical.

Sixteen entry SDK identities are freshly checked against the pinned Psy-Q
4.6 catalogue; this identity lookup does not change the GCC 2.8.1/MASPSX
2.81 pipeline or mask image comparisons. Thirteen established bindings
receive their confirmed SDK names without changing addresses. All 36
resident bindings and three caller owners agree with a freshly matched
resident ELF, selected C objects and retail bytes; each entry has 69 direct
call sites, covering 25 distinct resident callees.

Complete scratch links reproduce both 20,480-byte images with twelve
genuine C owners /17,808 bytes, retaining all ten prior owners /10,080
bytes. The addition is two entry instances /7,728 bytes. Two 3,196-byte
companion helpers remain generated assembly, and four real storage owners
/16,760 bytes preserve the loader headers and unclassified suffixes.
The independent accepted base is `3d58a395fd2359925adb88f90f133e0ddac67efe`.
Configured French totals become 1,288/1,581 C instances and 1,640,220 C
instruction bytes across 252 images. These inventory totals do not prove
exhaustive French runtime coverage or campaign completion.

Entry production acceptance passed: all 252 complete French overlays and
the final clean French resident reproduce retail bytes. Canonical selected
objects and final ELF sections confirm all twelve C owners above, with
the entry assembly fallbacks absent and the five earlier helper sources
and profiles per image unchanged. All 123 layout constants were freshly
recompiled from the promoted header, and the 39 resident binding/caller
owners were rechecked. All 73 focused French/Spanish family and progress
regressions pass without skips, alongside attempt-ledger, basic-type,
metadata, G32 and notes checks. Shared sources, other regional production
metadata and the fixed-cutoff README remain unchanged.

## Entry-called five-band renderer

Both `0x17C8..0x2444` functions match at 3,196 bytes with 400-byte frames
under the unchanged `gcc_2_8_1_g0_split` profile. All 799 instructions were
independently recovered from the French images. The first complete C
candidate differed in three words: reset staggering negated before shifting,
and the nonreset path redundantly restored the band cursor. Shifting before
negation and restoring the cursor only inside the reset path that advanced
it recover every byte. Both paired experiments precede the two canonical
terminal records in the attempt ledger. No artificial controls or compiler
changes are used.

The renderer expands the already accepted `Variant431EntryBand`, rather
than adding a second type for the same five 288-byte records. The three
five-point `SVECTOR` rows occupy `0/0x28/0x50`, their packed screen words
`0x78/0x8C/0xA0`, colors `0xB4/0xC8`, scale `0xDC`, progress `0xF4`,
width word `0x108`, and five depths `0x10C`. Existing field names, extent,
entry source and initialization remain unchanged. The renderer and entry
jointly establish `0x4E0..0xA80`; this is not an allocation-capacity claim.
Thirty-six target-compiled constants check this view, its context offsets,
the existing fan record, SDK primitives and packet.

For each band, projected target halves determine `ratan2(y, x) + 2048`.
Width divided by 64 sets the transverse radius. Nonpositive progress maps
to distance zero, without an upper geometry clamp. Three projected rows
interpolate each part's matrix translation using its direction and distance
divided by 1,024. Signed divisions preserve truncation toward zero. The
renderer uses the first textured packet at `0x1634` and a 5-by-5 stack
flag array; `RotTransPers3` keeps its independently verified resident
address `0x80087898`.

Progress **at or below** 1,024 advances by `step << 7`, clamps on reaching
1,024, and advances phases below two to two. Only the fifth point of the
fifth band advances the timing index, while it is below ten; otherwise
phase two becomes three. A reset staggers all 25 progress words as
`-(point << 8) - (band << 8)` and clears all five fan done flags, without
resetting fan sizes. The entry calls this renderer after the sheets when
unsigned frame reaches the indexed descriptor deadline. Its exact gate,
constructor anchors and reset-only cursor restoration are covered by
regressions.

Four segments per band produce two quads connecting the outer rows to the
center row. Outer colors come from `ca`, center colors from `cb`; phase
three and above scale each channel by signed-halfword `0x18D2 / 1024`.
Both depth and projection flag must be nonnegative, including zero, and
sorting uses the low sixteen depth bits. Only phase one grows word
`0x18D4`, inclusively at 1,024, by `step << 6`, then clamps it. This
renderer does not decrement the fade factor.

Complete scratch links reproduce both 20,480-byte images with fourteen
C owners /24,200 bytes, preserving all twelve earlier C owners /17,808
bytes and the sheets' distinct no-CSE profile. Four raw owners /16,760
bytes preserve the headers and suffixes. All fourteen inventoried CFGs
close, and 36 resident callee owners have exact defining objects and bodies.
The two new instances add 6,392 C bytes. All four entry-called helpers are
now C; the spokes and rings remain retained code without a direct-entry
path. The suffix at `0x2F48..0x5000` is still unclassified.

The independent integration base is accepted master
`011cfc2e0a063db291cd2cfb71f957a4d7354c1e`; the matching change does not
alter the separate fixed-cutoff report. Configured French totals become
323 images, 1,755/1,975 C instances and 2,462,076 C instruction bytes.
These totals do not establish exhaustive runtime coverage or complete #6460.

Band production acceptance passed: the clean French resident and all 323
configured overlay images reproduce retail bytes. The final family ELFs
and their actual selected input objects contain all fourteen C owners
above, with no assembly owner or fallback. Every C object's instruction
bytes match the frozen whole-image proof, including both entries after
expanding the shared band declaration and both sheets under their original
profile. All 36 canonical layout constants, all 36 resident callee owners,
42 focused family/progress regressions and repository policy checks pass.
The other regional sources, shared SDK declarations, compiler profiles,
overlay registry and fixed-cutoff reports remain unchanged.
