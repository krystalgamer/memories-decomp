# French MODEL450 shared PAL helpers, bands, and primary ribbons

French model 174, stages 9 and 10, contains two complete 20,480-byte images
with headers 450/600 and command 616000. Independent reads and hashes of
both legally obtained archives establish that these complete French images
are byte-identical to their accepted Spanish counterparts.
[The instance table](french-model-variant450-instances.csv) records the
French loader slices and hashes; no Spanish input substitutes for a French
target check.

The quad and line matching entries select the existing accepted Spanish sources
directly, including their existing slot-1 wrappers. No tracked source/header
duplication, regional macro, new compiler profile, or other region's metadata
change is needed. All four French helpers were independently compiled with
`gcc_2_8_1_g0_split` (GCC 2.8.1/MASPSX 2.81):

| Offset | Bytes | Frame | Selected implementation |
| --- | ---: | ---: | --- |
| `0x27C0..0x2D88` | 1,480 | 264 | `spanish_model_variant/variant450_quads.c` |
| `0x2D88..0x310C` | 900 | 288 | `spanish_model_variant/variant450_lines.c` |

The slot-1 files only rename each function. This adds four matching C
instances and 4,760 instruction bytes, not a newly invented algorithm.
[The attempt ledger](french-model-variant450-attempts.csv) records the
independent French compilations and terminal production ownership checks.
The historical Spanish experiments remain in their original ledgers.

## Inventory and unresolved scope

All seven function spans have independently checked, closed contiguous
control-flow graphs: `4..DD8`, `DD8..1794`, `1794..1ECC`, `1ECC..27C0`,
`27C0..2D88`, `2D88..310C`, and `310C..3940`. The primary ribbon, band, quad,
and line helpers are C. The three other functions in each image remain
generated assembly.
Closed retained functions are not automatically claimed to be entry-reachable.

The four-byte header and complete `0x3940..0x5000` suffix keep raw owners.
The suffix is 5,824 bytes per image, remains unclassified, and is not
excluded from further research. Registering the two images adds 14
inventoried functions; it does not establish exhaustive French coverage.

The entry calls lines at `0xC34`, quads at `0xC3C`, and primary ribbons at
`0xC70`, with `s3` passed in all three delay slots. All 35 distinct external
targets resolve to actual French resident function starts. Canonical SDK names replace duplicate
aliases for `GsSortPoly`, `RotTransPers4`, `RotTransPers`, `rcos`, and `rsin`;
other unresolved regional
placeholders remain address-based.

## Shared views and explicit limits

The accepted [line recovery](spanish-model-variant450.md) and
[quad recovery](spanish-model-variant450-quads.md) document the source
structure, packet arithmetic, caller assumptions, and isolated execution
evidence. Their source files are shared, not rewritten. Sixty-five
target-compiled constants recheck the two bounded views and SDK layouts.
The line groups, primary records, and fade views have strides 104, 536,
and 160; the seven quad groups have stride 160.

French resident pointers place secondary contexts at `0x80136000` and
`0x80176000`. The line view ends at `+0x42F8`; the quad view ends at
`+0x42FC`. Their prefixes overlap the separate primary-handler loader
windows at `0x8013A000` and `0x8017A000` by **760 and 764 bytes**,
respectively. This measured overlap is not an allocation-size, lifetime,
complete-context, or noninterference proof. No such claim is introduced
by reusing the exact sources.

The quad's depth scaling precedes the nonnegative-depth test, preserving
the retail negative-one rounding case. Its unsigned configuration
arithmetic and zero-duration behavior are unchanged. The lines retain
their four initial angle calls, aliased projection output, conditional
threshold submission, and per-fading-group phase increments.

Complete-image identity and real sized linked C ownership remain the
acceptance gate, rather than donor identity alone. The production check
must cover all 18 C/assembly/raw owners, all other configured French
images, and the French resident. The saved exact French resident ELF also
supports 38 independently verified loader/controller/callee owners.

## Independently recovered three-band helper

`func_8013C794` / `func_8017C794` at `0x1794..0x1ECC` are each exactly
1,848 bytes with a naturally generated 408-byte frame under the same named
profile. The new French source and symbol-only slot wrapper leave every
accepted Spanish source, header, and regional manifest unchanged. This adds
two C instances and 3,696 instruction bytes; at that checkpoint the two French
images had six C owners totaling 8,456 bytes. No direct local caller to this retained
helper was found; global reachability remains unresolved.

Fifteen entry instructions independently establish the `+0xF00` record
base, three 520-byte records, RGB at record `+0x120`, and the two 40-byte
FT4 packets at `+0x41E0`. Each record contains nine points and edge points.
The helper builds circular bands translated by `delta * i / 3`, retaining
both angle calls, signed16 width selection, and zero-rotation matrix setup.
Its `flags[3][9]` projection results are distinct from the scalar edge flag.
Eight segments per band alternate the two packets; nonnegative depth and
the corresponding projection flag gate low16-depth submission. There is
no phase update.

Thirty-one target-compiled constants verify SDK layouts, the 520-byte
record, and the bounded `0x42D0` context view. Screen words have component
and packed SDK views of the same four bytes: signed component differences
feed angle recovery, while signed high-half extraction preserves native
draw-Y loads. Endpoint8 keeps fixed7/8 projection inputs and fixed short
offset destinations, but word results and projected reads use the actual
loop index. This preserves separate native address bases without padding,
forced registers, or artificial uses.

The six paired experiments preserve sizes/word differences of
`1808/377`, `1848/12`, `1848/8`, `1808/373`, `1848/2`, and `1848/0`.
Making every screen access a packed-word access was rejected because it
collapsed the endpoint bases. Measured component/packed views, displacement
operand order, and draw-index-before-packet initialization recover the
complete bodies. The original eight ledger rows remain byte-preserved.

Both complete 20,480-byte scratch images were linked with frozen inputs:
six C owners, eight unchanged fallback functions, and four raw header/suffix
owners. All 14 closed CFGs and 35 actual resident callee owners were checked.
The new view overlaps each primary-handler loader window by 720 bytes.
This is not an allocation-capacity, lifetime, noninterference, or exhaustive
runtime-coverage claim. The complete `0x3940..0x5000` suffix remains
unclassified and retains its raw owner.

## Band production acceptance

The sequential clean French resident and all 323 configured overlays match
their retail targets. Both complete MODEL450 production images independently
relink to identical ELFs with maps. Their 18 actual input owners cover all
40,960 bytes: six C owners / 8,456 bytes, eight assembly owners / 20,848 bytes,
and four raw owners / 11,656 bytes. Every C object text equals its frozen
exact input; all 35 actual resident callee owners and complete bodies are
reverified.

The initial production link exposed three stale SDK names in the per-image
Splat declarations after their linker bindings had been canonicalized.
Both tracked symbol files now agree with those bindings, and an
input-independent regression checks that agreement. No generated assembly,
candidate body, duplicate alias, or shared Spanish source was changed to
resolve the failure. Both affected complete images passed before the full
clean acceptance sequence was repeated.

All 28 focused checks pass without skips; the complete suite passes 1,779
tests with four skips. At band acceptance, the French inventory contained 1,679 matching C
instances out of 1,975 functions and 2,300,684 C instruction bytes across
323 configured images. These counts do not establish exhaustive runtime
coverage or complete French decompilation.

## Independently recovered primary ribbons

`func_8013BDD8` / `func_8017BDD8` at `0xDD8..0x1794` each contain 623
instructions, exactly 2,492 bytes with a 536-byte frame under
`gcc_2_8_1_g0_split`. The entry call at `0xC70` is independently verified in
both complete images. The existing French band and shared Spanish quad/line
sources remain unchanged. The new slot-1 wrapper only renames the function.

Twenty-one entry anchors establish six 536-byte records at `+0x270..+0xF00`,
nine phases initialized as `(-j * 512) / 8 - i * 512`, RGB 128/128/192,
completion zero, and target/delta vectors. Each record has nine point and
edge vectors, component/packed projection words, angles, widths, depths,
short offsets, and a measured 36-byte tail. Thirty-seven target-compiled
constants verify this record, its tail, the SDK types, and the bounded
`0x42FC` view.

The first phase at context `+0x440` coincides with the accepted primary
view's threshold; the target vector at `+0x468` coincides with its
translation. These are overlapping views of the same bytes, not separate
allocations. The bounded ribbon view overlaps the primary-handler loader
window by 764 bytes. Neither observation establishes allocation capacity,
lifetime, or noninterference.

Geometry uses guarded progress capped at 1024, twice-progress bend angles,
record-dependent turns, and a wave advancing by 1300 per point. Phases at
or below 1024 advance by `frame_step << 6`; clamping the first record can
change state 1 to 2, and clamping endpoint 8 sets completion. Projection
retains fixed 7/8 endpoint inputs, dynamic word-result indexing, and fixed
endpoint short-offset stores. Signed packet parity selects one of two FT4
packets. Strictly positive depth, nonnegative projection flags, and incomplete
records gate low16-depth submission. The two ripple updates remain
`frame_step * 850` and `frame_step << 7`.

Five paired experiments preserve sizes/differing words of `2536/586`,
`2496/580`, `2544/588`, `2492/13`, and `2492/0`. Direct embedded-tail
accesses eliminate redundant pointer induction and recover the native
instruction shape. Naming the genuinely reused bend-angle scalar resolves
the final `s0`/`s2` exchange without forced registers, artificial stores, or
fake dependencies. All earlier 22 ledger rows remain byte-preserved.

Both complete 20,480-byte scratch images match with frozen inputs: eight C
owners totaling 13,440 bytes, six fallback functions totaling 15,864 bytes,
and four raw header/suffix owners totaling 11,656 bytes. All 14 closed CFGs,
18 image owners, and 35 resident callee owners are verified. Report
regeneration had removed resident input objects referenced by an old map;
a fresh exact resident build restored those inputs before the ownership
proof was repeated successfully. The incomplete proof was preserved rather
than treated as acceptance. The suffix remains unclassified, and no exhaustive
runtime-coverage claim is made.

## Primary ribbon production acceptance

The sequential clean French resident and all 323 configured overlays match
their complete retail targets. Both MODEL450 production images independently
relink to identical ELFs with maps. All 18 actual input owners cover all
40,960 bytes: eight C owners / 13,440 bytes, six assembly owners / 15,864 bytes,
and four raw owners / 11,656 bytes. All eight C object texts equal their frozen
exact inputs, and all 35 actual resident callee owners are reverified.
The accepted band, quad, and line sources and headers remain byte-preserved.

All 31 focused metadata, layout, and progress regressions pass without skips.
The initial focused run identified the inventory's required
`direct-entry reachable` wording; both target rows now use that established
convention while retaining the verified `0xC70` caller evidence. Metadata,
matching-source contracts, attempt records, primitive types, and G32 policies
pass. The 13-path change adds two matching C instances and 4,984 instruction
bytes, bringing the French inventory to 1,681 / 1,975 C instances and
2,305,668 C instruction bytes across 323 configured images. No progress
snapshot is bundled, and these counts do not establish exhaustive coverage.
