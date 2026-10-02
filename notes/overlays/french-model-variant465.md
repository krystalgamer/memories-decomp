# French MODEL headers 465 and 615

4 distinct secondary images reuse the accepted
`src/overlays/model_variant/variant448_*.c` webs, spokes, rings, quad
bodies through 8 three-line canonical renaming wrappers.
An independently reconstructed regional fan body and its thin slot-1
wrapper additionally cover the entry-called helper at `0x124C`.
The regional orbit body and its slot-1 wrapper cover `0x256C`, reusing
the unchanged accepted `Variant448Orbit` header after independent layout
verification.
Two additional wrappers select the measured `VERSION_FRENCH` branch of
the accepted `variant448_spiral.c` body for the retained helper at `0x39FC`.
Its existing arm header is unchanged.
The existing `gcc_2_8_1_g0_split` profile uses GCC 2.8.1/MASPSX 2.81.
For the original webs/spokes/rings/quad additions, shared bodies, headers,
G32/PSXLONG annotations and profiles are unchanged
from accepted master `0d8c1df39`. The original spokes/rings/quad addition
at `eed176532` rebuilt its calibration, layouts and canonical links after
the accepted PSXLONG migration invalidated earlier fingerprints.

## Loader and boundaries

Models 108, 573 use these images at stages 9/10.
The matched loader selects ten 2,048-byte sectors from 276-sector compact
records, at record offsets 200/210.
Loads are `0x8013B000`/`0x8017B000`, with entry at `+4`; the controller
supplies context and the initial nonnegative command or update `-1`.
The [instance ledger](french-model-variant465-instances.csv) records
independently checked indices, commands, slices and complete hashes.

The additional function at `0x39FC..0x4354` is **2,392 bytes of retained
spiral C**, not entry-reachable and not hidden in raw suffix storage.
Entry-call closure alone would have missed it. Inspection at the known
post-quad boundary and strict full-span walking establish its ownership.

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `0x4..0x124C` | 4680 | generated assembly | yes |
| `0x124C..0x166C` | 1056 | fan C | yes |
| `0x166C..0x256C` | 3840 | generated assembly | yes |
| `0x256C..0x2AB4` | 1352 | orbit C | yes |
| `0x2AB4..0x3014` | 1376 | webs C | no |
| `0x3014..0x331C` | 776 | spokes C | no |
| `0x331C..0x3698` | 892 | rings C | no |
| `0x3698..0x39FC` | 868 | quad C | no |
| `0x39FC..0x4354` | 2392 | spiral C | no |

Strict control-flow walks cover every word of each span with one terminal
return and no unresolved indirect transfer. The four previously matched
helpers remain **retained code, not direct-entry reachable**; the new fan
and orbit are entry-called. No additional execution path is claimed.
Each four-byte header and 3,244-byte suffix at
`0x4354..0x5000` has a real storage owner. Suffixes remain unclassified,
not proven non-code.

## Accessed layouts

Entry captures `a0 -> s2 -> s6`. **One 296-byte record**, not two
standard sheets, begins at context `+0x11F0`. Its sheet-compatible prefix
has size at `+0x88`, which entry clears and spokes consumes. Two 18-word
arrays at `+0x98` and `+0xE0` are initialized by nested 3-by-2-by-3 loops.
The outer counter starts zero, increments once and repeats only while
nonpositive; both record pointers advance by 296. The orbit addition
independently verifies the accepted `Variant448Orbit` view against these
initializers and the helper's accesses. No `ModelVariantSheet` array cast
or whole-context allocation claim is introduced.

Six 144-byte rings follow at `+0x1318`, four 144-byte spoke records at
`+0x1678`, and one 144-byte quad at `+0x18B8`. Adjacent extents agree exactly.
The generic local `sizeof(ModelVariantSheet) == 152` layout constant is
not claimed as this family's record stride.

77 entry anchors check pointer capture/saves/reloads,
initialized counters, increments, bounds, strides and accessed fields.
Seventy-one freshly target-compiled local C/SDK constants check primitive
widths, ring/spoke/quad and sheet-prefix views, `SVECTOR`, `VECTOR`,
`MATRIX`, `GsCOORDINATE2`, `GsGLINE`, `POLY_G4` and `POLY_GT4`.
Unrelated generic type sizes do not establish runtime record layouts.
These are accessed-view observations, not whole-context allocation proof.

## Retained narrow webs

The unchanged accepted `variant448_webs.c` body also reproduces
`0x2AB4..0x3014` in all four complete images. Entry initializes three
416-byte records at context `0x6F8..0xBD8`: two 4-by-6 `SVECTOR` grids
at record offsets `0`/`0xC0`, color at `0x180`, and scale at `0x194`.
Pointer saves/reloads, 8-byte element and 48-byte row advances, four/six
loop bounds, 416-byte record advances and the three-record bound establish
the local layout independently of the accepted US declarations.

The helper reads timing at context `0x11F0 + 0x88 = 0x1278`, the accessed
prefix of the single 296-byte record described above, not an array of
152-byte sheets. It uses `GsGLINE` at `0x1A84`, shrinks phase-zero scale
by `step * 0xE0`, and sorts only when projected depth is positive.
No direct entry-call path to this retained helper is claimed.

Forty-three fresh retail anchors and 27 target-compiled C/SDK constants
verify these views. Real archive command `631000` selects descriptor zero
in the 52-byte-stride table at module `0x4450`; the entry's shift/add
sequence independently establishes that stride. The direct context access
minimum is `0x1B50`, separated from the measured model, auxiliary and
overlay loads. This is not whole-allocation capacity or global-isolation
proof. Eight helper callees and three resident caller owners were checked
against actual resident ELF bytes. The existing `0x80089928` binding is
renamed to the independently established `ratan2`, retaining all 37 addresses.

## Exactness and preservation

The [attempt ledger](french-model-variant465-attempts.csv) records
the original 8 terminal wrapper matches. Candidate rebasing was
only a locator; actual links reproduce all 4 complete unmasked images.
Production verification checks selected object owners and final ELF bytes:
16 C owners / 15,648 C bytes,
20 assembly owners / 53,280 assembly bytes,
8 raw owners, 37 independently verified fresh-resident
callees, and 27 freshly recompiled web-layout constants. The original
71-constant spokes/rings/quad evidence remains preserved.

Five inherited regressions check archive slices/commands/hashes, canonical
sources/fingerprints, all spans/local calls, entry anchors and raw extents.
Two additional regressions cover the preserved binding address and
family-specific descriptors/context separation.
The combined 431/440/465 batch preserves all 158 accepted French
registrations, reproduces 168 complete images and the clean
French resident, and adds 28 C instances / 23,656 instruction bytes.
Configured totals are 714/1231 C instances
and 641,460 C instruction bytes. The accepted #6668 images are preserved;
no unmerged PR is stacked. Untranslated functions, unclassified tails and unregistered
runtime areas remain; these totals do not establish exhaustive coverage.
General report snapshots remain separate.

The webs addition is independent of that earlier census batch and starts
from accepted master `0d8c1df39`. It adds four C instances / 5,504 bytes,
preserves all 198 French image registrations and every other owner, and
reproduces the complete French resident. Configured totals are now
842/1,377 C instances and 792,204 C instruction bytes. Shared bodies,
headers and compiler profiles remain unchanged; no unmerged PR is stacked.

## Entry-called fan

This addition starts independently from accepted master `355c0d797`,
without pending French431 work. No suitable Spanish or accepted local
fan body was found. The standalone
`src/overlays/french_model_variant/variant465_fan.c` reconstructs the
complete local assembly using existing project SDK declarations, with a
three-line `variant465_fan_slot1.c` renaming wrapper. The family-local
`src/overlays/french_model_variant/variant465_fan.h` owns the measured
view, following the repository prohibition on C-file type definitions.
Shared source, existing headers and compiler profiles remain unchanged.

The accessed local `Variant465Fan` is **one 296-byte record at context
zero**. It is not the separately established 296-byte sheet-compatible
record at `0x11F0`. The fan has 18 `SVECTOR` inner vertices at `0`,
17 outer vertices at `0x90`, three four-byte color groups at
`0x118/0x11C/0x120`, and signed scale at `0x124`.
Entry captures the context at `0xC/0x14`, saves its unadvanced record
pointer at `0x90`, clears the outer counter at `0x150`, and forms
the scale pointer at `0x15C`. The vertex loop at `0x404..0x490`
clears the center, initializes 17 inner-ring and outer-ring points,
advances by eight bytes, and uses a 17-point bound. The color writes
at `0x4A8..0x4C8` establish white center, `(0,128,255)` middle and
black outer colors. Scale is cleared at `0x4CC`; both record pointers
advance by `0x128`, with a one-record counter/bound at `0x494..0x4EC`.
These independently recovered bounds agree with the helper accesses;
equal strides elsewhere do not establish interchangeable types.

The helper builds its matrix once, before its one-record loop, using
zero rotation, scale at `0x124`, and word translations at
`0x1AB8/0x1ABC/0x1AC0`. Each of 16 iterations projects a triangle
`inner[0], inner[j+2], inner[j+1]` into the `POLY_G3` at `0x1948`,
then a quad `inner[j+1], inner[j+2], outer[j], outer[j+1]` into
the `POLY_G4` at `0x1964`. Projection output `p` and flag occupy
stack `0xD0/0xD4`. Both projected depth and flag must be nonnegative;
the custom packet emitter receives the **low 16 bits** of depth and
fourth argument one. Triangle colors are center/middle/middle;
quad colors are middle/middle/outer/outer.

Entry calls the fan at `0x1088`, with original context in its `0x108C`
delay slot, once unsigned frame `0x1B00` reaches descriptor word `0x1C`.
The actual command remains `631000`, selecting descriptor zero at
module `0x4450`, stride 52. All four selected descriptors have
timing words **40, 140, 220** at `0x1C/0x20/0x24`; the growth divisor
is therefore 100. Phase zero computes unsigned
`((frame - start) << 12) / (end - start)`, then clamps to `0x1000`
and advances phase to one. The original divide-zero trap is retained,
not replaced by a fallback. The `i + 1 == 1` expression preserves
the retail index addition in the phase-store delay slot.
At later phases, unsigned frame reaching 220 permits shrinkage by
`step << 7`, clamped to zero. An already-zero scale in phase two
advances to three on that call; newly clamping scale does not
immediately perform that phase transition.

Thirty-eight freshly target-compiled layout constants and **266 retail
anchors**, including the preserved 117 anchors, verify these observations.
The direct context minimum remains `0x1B50`, not allocation capacity.
The existing `0x80087898` binding becomes the independently established
`RotTransPers3` alias without changing any of the 37 resident addresses.
Eight helper callees, all 37 bindings and three resident caller owners
are verified against actual resident ELF bytes. The known model,
auxiliary and overlay load ranges remain separate from the accessed
context extent; no whole-allocation isolation claim is made.

The first reconstruction produced 1,044 rather than 1,056 bytes, with
142 different aligned common words in each slot, despite the correct
272-byte frame. It omitted both low-16 depth conversions, used a
constant instead of the phase delay-slot addition, and evaluated the
threshold in the opposite order, eliminating a retail load-delay nop.
It also passed typed packets without the established `u32 *` cast.
The measured refinement fixes those issues without changing profiles:
both functions and all four complete images match. The failed sources,
objects and word diffs remain under local `tmp/`; their two ledger
records precede the two new canonical terminal matches, preserving
the eight historical terminal rows.

Canonical and combined scratch links reproduce all four complete
unmasked images with **20 real C owners / 19,872 bytes**, including
all 16 prior C owners / 15,648 bytes. The fan contributes four C
instances / 4,224 bytes. Sixteen assembly owners / 49,056 bytes and
eight raw owners / 12,992 bytes remain. The unclassified suffixes
are unchanged.

Fan production acceptance passed: all 252 complete French overlays and
the clean French resident match, with all 20 actual C owners and the
assembly/raw owners above. Fresh compilation rechecks all 38 constants
through the actual family-local header, and fresh resident ELF intervals
preserve all 37 bindings plus three caller owners. All 211 French,
112 Spanish and 21 progress/toolchain regressions pass without skips;
metadata, attempt-ledger, basic-type, C-type placement and G32 policies pass.
Configured totals are 1,140/1,581 C instances / 1,274,596 instruction bytes.
Accepted Family421, 422, 431, 435, 439, 445 and 476 C owners are preserved.
This is an independent accepted-master checkpoint, not a claim that the
remaining assembly or unclassified tails have been decompiled.

### Accepted Family431 reconciliation

The independently verified fan checkpoint `eb003db2e` is ordinarily
reconciled with accepted French431 webs `3e711eceb`. Only the aggregate
progress expectations conflict; they now include 1,142/1,581 C instances
and 1,276,700 C instruction bytes across the same 252 images.
No pending PR is stacked. All 23 original non-note/non-progress paths
remain unchanged. Fresh combined production acceptance passed: all
252 complete French overlays and the clean French resident match.
The 20 Family465 C owners / 19,872 bytes remain exact alongside all six
accepted Family431 C owners / 5,440 bytes and the previously checked
Family421, 422, 435, 439, 445 and 476 owners. All 213 French, 112 Spanish
and 21 progress/toolchain regressions and policy checks pass without skips.

## Entry-called orbit

This independent addition starts from accepted master `06d5a34bd`, including
the accepted Family341 fan but not pending Family431 fan work. The regional
body reuses the unchanged accepted `variant448_orbit.h` declarations.
The shared US implementation and its compiler contract remain untouched;
French uses the existing `gcc_2_8_1_g0_split` profile exclusively.

Fresh assembly evidence establishes the single 296-byte record at
`0x11F0..0x1318`: four rows of four `SVECTOR` points at `0/0x20/0x40/0x60`,
outer/inner colors at `0x80/0x84`, and two 3-by-2-by-3 signed-word arrays
at `0x98/0xE0`. The interval `0x88..0x98` remains opaque in this orbit
view; earlier independently proven users of its prefix remain unchanged.
Entry initializes the four planar quads with coordinates zero or
plus/minus 256, clears both 18-word arrays and advances by `0x128`.
The following six ring records still begin exactly at `0x1318`.

Three opaque companion records start at `0xBD8`, stride `0x208`,
ending exactly at the orbit record. Entry clears each companion's two
signed selector words at `0x200/0x204`. The orbit helper accesses these
words without inventing a larger companion struct. Only positive
selectors activate their three orbit lanes. The companion helper at
`0x166C` writes these selectors and can advance phase from five to six.
Entry calls orbit at `0x10A4`, with the original context in the delay
slot, when signed phase `0x1B3C` is at least five; the companion call
follows at `0x10BC`. This is **not a descriptor-frame gate**.

Each active lane draws four GT4 quads using rows zero, one, two and
three at the same column index. One 52-byte packet at `0x19BC` is reused.
The first three corners use inner RGB `(140,128,16)`; the fourth uses
outer RGB `(224,180,160)`, recovered from actual command `631000` and
descriptor zero at `0x4450`, stride 52. Matrix rotation is zero.
Angles combine `i*1024/3`, `j*2048` and `k*4096/3`; horizontal
translations add cosine/sine displacements of radius `192*(j+1)` to
context `0x1AB8/0x1AC0`, with vertical translation at `0x1ABC`.
Odd parity `0x1AFC` adds signed `size/8` to each scale component.

Both projected depth and projection flag must be nonnegative.
The three-argument `GsSortPoly` receives the low 16 bits of depth.
Each lane grows by `step << 10` to `0x2000`, setting done to one,
then shrinks by `step << 8` to zero and sets done to two.
Done values are summed across active lanes; a sum at least 36 changes
phase six to seven. The helper does not reset done values on every call.

Calibration retained five distinct two-slot experiments. The unchanged
US body had 14 differing words despite the correct 1,352 bytes and
304-byte frame. Establishing the sheet pointer before the ordering-table
call fixed 13 prologue words. Ordinary byte indexing and typed-word
indexing both retained the one addition-operand mismatch at helper
`+0x78` / image `0x25E4`. The existing French negative-index addressing
form, with bounded `j` in `0..1`, reproduces `addu v0,t3,v0` exactly.
There are no new flags, forced registers, assembly substitutions or
masked comparisons. The ledger preserves its original 12 rows and adds
ten experiment rows plus two canonical terminal rows.

Canonical complete-image links contain **24 real C owners / 25,280 bytes**,
preserving all 20 prior owners / 19,872 bytes and adding four orbit
instances / 5,408 bytes. Twelve assembly owners / 43,648 bytes and
eight raw owners / 12,992 bytes remain. Independent evidence comprises
46 target-compiled constants, 122 retail anchors, 11 helper callees,
all 37 resident bindings and three resident caller owners. The
`0x800866F8` alias becomes the established `rcos` without changing its
address. Direct context extent `0x1B50` is still a minimum, not a claim
about allocation capacity. Unmatched code and unclassified tails remain.

Orbit production acceptance passed: all 252 complete French overlays and
the clean French resident match. Actual linked sections and defining
objects verify the 24 C, 12 assembly and eight raw owners above; 518
additional C owners across ten unchanged French families are preserved.
Fresh layout compilation and resident ELF interval checks retain all
46 constants, 37 bindings and three caller owners. All 231 French,
133 Spanish and 21 progress/toolchain regressions pass without skips,
together with metadata, attempt-ledger, basic-type, C-type placement,
G32 and notes policies. Configured totals are 1,170/1,581 C instances
and 1,314,500 C instruction bytes. This excludes pending Family431 fan
work and does not establish exhaustive French coverage.

### Orbit accepted-master reconciliation

The verified orbit checkpoint `d01170d37` is ordinarily reconciled with
accepted master `72a73fb9d`, including both accepted Family431 fans and
sheets, the four Family341 fans and the accepted US414 sheets. Only the
aggregate progress expectations conflicted. The combined totals are now
1,172/1,581 C instances and 1,316,836 C instruction bytes across 252 images.
No pending PR is stacked. All 21 original paths outside this note and the
two shared test fixtures remain byte-identical.

Orbit accepted-master reconciliation passed: all 252 complete French
overlays and the clean French resident match again. Fresh production
ELF sections and defining objects retain all 24 Family465 C owners /
25,280 bytes, including the four new orbit instances / 5,408 bytes.
All 520 C owners across the ten other checked families are preserved,
including ten Family431 owners / 10,080 bytes and twenty Family341
owners / 19,360 bytes. Fresh layout compilation and resident intervals
retain all 46 constants, 37 bindings and three caller owners.
All 234 French, 133 Spanish and 21 progress/toolchain regressions pass
without skips, together with the metadata, attempt-ledger, basic-type,
C-type placement, G32 and notes policies. Shared US sources, compiler
profiles, unmatched assembly and unclassified tails remain unchanged.

## Retained spiral

This independent addition starts from accepted master `50f2ee860`,
not pending options-suffix work. The accepted North American header-448
spiral provides source structure; French uses only the existing
`gcc_2_8_1_g0_split` profile, GCC 2.8.1/MASPSX 2.81. Two four-line
wrappers select `VERSION_FRENCH` and rename the entry for each slot.
No new types, compiler profiles, source-local externs, forced registers,
artificial stores or assembly substitutions are introduced.

The target independently establishes **twelve 124-byte arms at context
`0x128..0x6F8`**, ending exactly where the previously measured webs begin.
Each arm has two `SVECTOR` points at `0x10`, projected words at `0x20`,
angles at `0x28`, displaced points at `0x30`, projected displaced words at
`0x40`, widths at `0x48`, two RGB rows with four-byte element strides
at `0x50/0x58`,
depths at `0x64`, and signed-halfword screen offsets at `0x6C/0x70`.
Eight-byte point strides, four-byte word strides, two-byte offset strides,
the twelve-arm bound and `0x7C` record advances are observed in the helper.
The unchanged `Variant448SpiralArm` declarations agree with these accesses;
unaccessed bytes remain opaque. This is not a whole-context allocation or
initialization claim.

Radius is signed halfword `0x1B28 / 2`; phase is at `0x1B24`, width at
`0x1B30`, and orientation uses words `0x1AEC/0x1AE4`. Two otherwise unused
initial products with signed halfword `0x1B26` are actual target operations.
Parity at `0x1AFC` chooses scale `0x1000` or `0x1200`; translations use
`0x1AAC/0x1AB0/0x1AB4`. Projection outputs `p` and flag are stack locals
at `0xD0/0xD4`. One GT4 at context `0x1988` is reused for both sides of
each arm. Only **strictly positive** depth sorts, with its low 16 bits
passed to the existing three-argument `GsSortPoly`. Signed-halfword radius
grows by word `0x1B08 * 64` and is clamped to `0x400`, preserving the
target's intervening halfword truncation.

The French last-point branch indexes projected-word reads and word caches
with the current `k`, while keeping fixed projection endpoints and
halfword offset indices. This reproduces the separate `arm + 4` word
induction base and `arm + 0x72` halfword base, including the spilled
projection pointer and 304-byte frame. The non-French branch is
byte-for-byte the previously accepted source after conditional selection;
North American declarations, profiles, registrations and behavior are
unchanged. Its four affected complete images also match with 28 actual
C owners / 34,624 bytes.

The ledger preserves all 24 historical rows and appends 19 records:
eight earlier retained-helper experiments from tick 365, the initial
missing-`RotTransPers` link failure, eight new two-slot experiments,
and two canonical terminal matches. Historical mismatch counts compare
aligned common words; the new comparisons also count unequal-length tails.
The unchanged accepted body produced 2,372 bytes / frame 296 / 399
different words. Current-point cache indexing recovered the target size
and frame but left 80 words; limiting it to word caches produced identical
bytes. Indexing the projected-word reads as well yielded **2,392 bytes,
frame 304, zero differences** in both slots. The initial link failure was
not a compiler failure: its unchanged object was resumed after independently
verifying the existing SDK entry at `0x80087868`.

Fresh evidence checks 54 target-compiled layout constants, 85 retail
instruction anchors, all 37 resident binding owners and 11 spiral callees
against the complete exact French resident ELF. The canonical
`RotTransPers` alias replaces the numeric SDK alias without changing any
address. Four complete French production images contain **28 actual C
owners / 34,848 bytes**, preserving all 24 previous C objects byte-for-byte.
Eight assembly owners / 34,080 bytes and eight raw owners / 12,992 bytes
remain. The four 3,244-byte suffixes remain unclassified, not excluded as
padding or proven non-code. No new runtime reachability is claimed.

The spiral contributes four existing inventoried functions / 9,568 bytes
to matching C. At this fixed base, configured French totals are
1,355/1,595 C instances and 1,821,584 instruction bytes across 253 images.
Pending options-suffix changes are intentionally excluded. General progress
snapshots remain separate, and these totals do not establish exhaustive
French coverage.


Spiral final acceptance passed: the clean French resident and all 253
configured French overlays match. Mapped proof ELFs equal the actual
production ELFs in all four French and four affected North American images;
the 24 prior French C objects remain byte-identical. All 62 targeted
French-family, Spanish-family, progress and toolchain regressions pass
without skips, together with metadata, attempt-ledger, basic-type,
translation-unit header, matching-source, data-symbol and notes policies.


### Spiral accepted-master reconciliation

The verified spiral checkpoint `fcdefc226` is ordinarily reconciled with
fixed accepted master `bd7ad10c7`, including the now-accepted French options
suffix leaves. Only the aggregate progress fixture conflicted. The combined
totals are **1,357/1,597 C instances / 1,822,092 instruction bytes** across
253 French images. All 24 authored paths outside this note and the progress
fixture remain byte-identical to the verified checkpoint; no pending PR is
stacked and no retired options branch is pushed.

Fresh combined acceptance reproduces the clean French resident and all 253
French overlays. The four French and four affected North American mapped
proof ELFs again equal their production ELFs, with all prior French C objects
preserved. All 78 targeted family, options, progress and toolchain regressions
pass without skips, together with the same repository policy gates.
