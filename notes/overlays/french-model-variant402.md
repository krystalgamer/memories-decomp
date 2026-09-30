# French MODEL headers 402 and 552

Four stage-7/8 images for models 6 and 551 contain an independently
recovered 1,180-byte rings helper, 1,320-byte band helper and 1,516-byte strip
helper. Their bodies,
local accessed-view headers and slot-1 wrappers use the named `gcc_2_8_1_g0_split` profile,
GCC 2.8.1 and MASPSX 2.81. No reference-project types or compiler claims
were imported.

Canonical symbols reproduce all four complete images. The original rings
and band integrations preserved all 194 preceding module records and
reproduced all 198 configured French images. The later strip addition
preserves the 222-image accepted cutoff described below.

## Loader and image boundaries

The compact records are 6 and 501. Stages 7/8 select ten 2,048-byte
sectors at `record * 276 + 180/190`, loading at
`0x8013B000/0x8017B000`. Entry is at `+4`. All selected commands are
568,000, passing initialization argument zero. The
[instance ledger](french-model-variant402-instances.csv) records each
independently verified slice, actual header word and complete hash.

| Offset range | Bytes | Owner | Entry-call reachable |
|---|---:|---|---|
| `0x4..0xA5C` | 2,648 | generated assembly | yes |
| `0xA5C..0x1048` | 1,516 | strip C | no |
| `0x1048..0x169C` | 1,620 | generated assembly | no |
| `0x169C..0x1B38` | 1,180 | rings C | no |
| `0x1B38..0x2060` | 1,320 | bands C | yes |

Strict control-flow walks cover all twenty spans, with one terminal return
per span. Entry calls only the last helper at `+0x1B38`, not `+0xB38`.
The three intervening functions are retained code, including strip and rings.
**No entry-call execution path is claimed for strip or rings.**
Each four-byte header and 12,192-byte suffix has a real sized raw owner.
The suffix at `0x2060..0x5000` remains unclassified, not established
wholly data or non-code.

## Independent rings recovery

The entry captures `a0 -> s2 -> s8`, derives `context + 0x58`, and
initializes two records with stride 144. Four point rows at offsets
0/32/64/96 contain four `SVECTOR` values each. Entry clears the scale
word at 128 and following measured words; both records end at `+0x178`.
The helper independently captures the context and repeats twice with the
same 144-byte stride and four quads per record.

This is **not** the accepted 152-byte header-337 ring with scale at 136.
The new view keeps the measured 144-byte record, scale at 128 and unknown
trailing bytes, rather than importing a similar but incompatible type.
Thirty-one entry/helper instruction anchors and 25 freshly target-compiled
constants establish these layouts and SDK argument offsets.

The helper's partial context view includes the quad at `+0x6DC`,
transform at `+0x814` with translation at `+0x828`, target at `+0x834`,
velocity at `+0x83C`, frame at `+0x868`, unsigned elapsed at `+0x86C`,
step at `+0x874`, configuration at `+0x87C`, selected part at `+0x890`,
signed interpolation halfword at `+0x8A6` and phase at `+0x8AC`.
The view ends at `+0x8B0`; entry accesses further fields through
`+0x8C4`. Neither extent is an allocation-capacity assertion.

The selected descriptor is a 20-byte record at module `+0x215C`, indexed
by initialization argument. All four archive commands select record zero,
whose part count is one and unsigned duration is 92. The descriptor
remains inside the single preserved suffix, without duplicate storage.

The first ring uses the transform translation. The second adds velocity
scaled by the signed interpolation divided by 1,024. Three quad vertices
use RGB `(0,64,192)` and the fourth uses `(192,192,192)`. Drawing requires
`0 < depth < 2048`; the projection flag is not tested. Phase-dependent
scale changes run only for the final selected part. Unsigned duration
arithmetic and the distinct first/second-ring phase transitions are
preserved exactly.

## Experiments and ownership

The [attempt ledger](french-model-variant402-attempts.csv) records the first
candidate, its refinement and two terminal canonical-source records.
The first candidate already had the target's 1,180-byte code size and
272-byte stack frame, but eleven aligned instructions differed around
the depth check. GCC folded a combined range expression into subtraction
and an unsigned comparison. Nested `if` statements retain the target's
two branches and produce byte-identical text. Do not simplify that nesting
without repeating exact acceptance.

Actual source/header and slot-wrapper links reproduce all 81,920 image
bytes. Each image has one sized C owner, four assembly owners and two
raw owners. All 35 resident callees across the module and all ten C
call relocations were checked against a fresh exact French resident.
Three resident initializer/controller/loader owners have sized matching C
symbols and retail-identical bytes.

The initializer passes fixed `0x80136000/0x80176000` contexts through
slot `field_DEC`; the controller supplies the selected command or `-1`
to secondary entry. The selected model/primary/secondary load ranges do
not overlap the measured minimum view. Complete allocation capacity,
all primary-context writes and whole-game lifetime isolation are not
inferred. Layout compatibility alone does not prove execution of retained
code.

The initial rings addition was four configured images and four matching C
instances / 4,720 instruction bytes. Configured French totals became 198
images, 798/1,377 C instances and 731,308 C instruction bytes. Four assembly
functions per image and the unclassified suffixes remained; these counts
are not exhaustive runtime coverage. General report snapshots remain
separate from this matching change.

Initial rings acceptance included the clean exact French resident gate, checks of
all four new C owners, sixteen assembly owners and eight raw owners, all
35 resident callees and three caller owners, and fresh compilation of the
25 layout constants. All 108 French MODEL variant and 47 progress/global
usage regressions pass, together with metadata, basic-type and
G32/PSXLONG policy checks.

## Entry-called band follow-up

The independently recovered `func_8013CB38` compiles to all 1,320 target
bytes with its 304-byte stack frame. The slot wrapper renames it to
`func_8017CB38`. Actual canonical links reproduce all four complete images
while preserving the accepted rings C and replacing only the final
assembly function. Bands is the sole local helper called by entry, at
module `+0x8E0` with the captured context in `a0`; unlike rings, its
entry-call path is proven. Final production validation reproduces all 198
configured French images and the clean French resident. The family has eight
section-defined C owners, twelve assembly owners and eight raw owners;
the old band assembly object is absent from selected link inputs. All
35 module callees and three caller/loader owners were checked again,
along with ten band and twenty-five ring layout constants. All 109 French
MODEL variant and 47 progress/global-usage regressions pass, together with
metadata, basic-type and G32/PSXLONG policy checks.

Entry initializes two 280-byte records at context `+0x438..+0x668`.
Each has seventeen inner `SVECTOR` points at 0, seventeen outer points at
`+0x88`, scale at `+0x110`, and a cycle counter at `+0x114`.
The seventeen-point bound, two-record bound, 280-byte stride, initialization
stores and call pointer are verified in all images by twenty additional
anchors. Ten new target-compiled constants check this record and its SDK
types. This is not the similar 288-byte header-432 band with inline colors.

The helper derives a bearing from the velocity, preserves a second unused
`ratan2` call, and orients both bands using the measured slot direction.
Frame parity changes the outer radius between 512/576 and depth between
192/256. It generates seventeen points per edge and draws sixteen quads
per band, fading six context color bytes above scale 2,048. The signed
halfword fade calculation, scale limit 4,096, cycle increment and final
phase transition are preserved without normalizing them to another effect.

Six source/compiler experiments are retained in the attempt ledger. The
first missed loop induction and depth-product caching. Reusing the draw
depth for the product added spills; a separate scalar restored allocation.
Grouping outer-vector stores with the existing `setVector` macro restored
the target address formation. Separating the initial bearing from the
loop angle then fixed the final four differing register words. All
experiments use the same named compiler profile; no padding or register
binding was introduced.

Fresh checks cover all 35 module callees, the three resident caller/loader
owners and thirteen C call relocations per slot. `rcos`, `rsin` and
`ratan2` replace address-based binding names using identities already
established in accepted French family-435 bindings. All 35 destination
addresses remain unchanged; no duplicate binding addresses or new SDK
declarations are introduced. Existing legal commands, descriptors and
context/load separation were checked again. The `+0x8C4` minimum view
remains a lower bound, not an allocation or global lifetime assertion.

The follow-up adds four C instances and 5,280 instruction bytes, without
adding image registrations or function inventory rows. The family now
has eight C owners, twelve remaining assembly owners and eight raw owners.
Configured French totals become 198 images, 802/1,377 C instances and
736,588 C instruction bytes. Rings remains retained-only, and every
suffix remains unclassified; these counts are not exhaustive coverage.

## Retained strip follow-up

The strip at `0xA5C..0x1048` matches all 1,516 bytes with a 264-byte frame.
The canonical body, local accessed-view header and slot-1 wrapper reproduce
all four complete images while compiling the existing rings and bands C.
It is retained module-local code: the entry still calls only bands, and
this match establishes no new execution path.

Entry captures `a0 -> s2 -> s8`, saves the context at stack `+0x80`,
and initializes one 88-byte record at context zero. The helper uses three
rows of two `SVECTOR` values at `0/0x10/0x20`, three rows of two packed
`PSXLONG` projection results at `0x30/0x38/0x40`, and two signed depth
words at `0x50`. The `0x48..0x50` bytes remain opaque. The entry and both
helper loops independently establish the 88-byte stride and single-record
bound; projection iterates over two endpoints.

The `POLY_GT4` is at context `0x6A8`. Each half of the strip uses two
outer and two center-line points, RGB `(0,64,192)` at the outer edge and
`(128,128,128)` at the center. Packed X reads use the unsigned low half;
Y uses the signed high half, matching retail `lhu`/`lh` instructions.
Sorting accepts nonnegative depth and flags and uses the low 16 depth
bits. The already established 20-byte descriptor, final-part condition,
step at `0x874`, interpolation at `0x8A6` and phase at `0x8AC` are reused,
not inferred from a different regional header.

Nineteen material source experiments and two terminal canonical records
are appended to the existing ledger. Early candidates differed in bearing
scheduling, packed projection extraction and point-address operand order.
The closest remained two words away: counter zero and quotient shift were
swapped at module `0xAC8/0xACC`. Explicit negative-width bias followed by
counter initialization fixed that ordering but coalesced width with the
call-live scale, changing 27 register words. A **separate short-lived
width temporary** restores the target `v0` input and `s7` quotient roles
without register bindings, padding or compiler changes. Bias-and-shift
division agrees with truncating division by 32 for all 65,536 signed
halfword inputs; the regression checks the complete domain.

Fresh independent evidence checks 37 target-compiled constants, 58 retail
anchors, all four physical slices and commands, 35 resident binding
addresses, 12 distinct helper callees, 16 call relocations and one bounded
local-jump relocation per slot, and three resident caller/loader owners.
The existing `0x80087898` binding is named `RotTransPers3`; its address and
the binding count are unchanged. Context minimum `0x8C4` remains a lower
bound, not allocation capacity or whole-game lifetime isolation.

This independent batch starts from accepted `af5859e680`, excluding the
then-pending petal and Family445 webs batches. It adds four C instances /
6,064 bytes, without changing any of the 222 accepted module records.
Family ownership becomes 12 C / 16,064 C bytes, eight assembly owners and
eight raw owners. Production acceptance reproduced all 222 complete
French images and the clean resident, with actual C objects and 37 fresh
canonical layouts. All 130 French variant and 16 progress regressions
passed without skips, along with repository policy gates.
Configured totals are 894/1,521 C instances and 856,228 C bytes. Eight family
regressions retain every prior boundary, helper selection and descriptor
check. Shared bodies, existing headers, profiles and other regions are
unchanged; the two remaining assembly functions and every suffix stay
untranslated.

### Accepted Family445 reconciliation

The fixed requested cutoff `2fb35390d` adds twelve accepted Family445
webs C owners to the earlier baseline. This branch preserves all 222
accepted registrations and those owners while retaining exactly the
original four strip additions. Combined totals are 906/1,521 C instances
and 872,788 C bytes. Source, wrappers, local headers, experiments and
terminal fingerprints are unchanged. Only aggregate progress expectations
and this note change within the original 24 authored paths; no pending
French branch is stacked. Fresh acceptance reproduces all 222 complete
French images and the clean resident. Actual production objects verify
all twelve accepted Family445 webs C owners and all twelve Family402 C
owners, preserving rings/bands and every assembly/raw extent. The 37
compiled layouts, 35 resident bindings and three caller/loader owners
pass again, along with 132 French, 71 Spanish and 16 progress regressions
and the repository policy gates.

### Accepted petal reconciliation

The subsequently requested accepted cutoff `c95089732` is merged normally,
preserving all 252 accepted registrations and 932 accepted C instances,
including thirty petal and twelve Family445 webs owners. The original
four strip additions / 6,064 bytes are unchanged: combined totals become
936/1,581 C instances and 917,788 C bytes. The original 24-path scope and
22 unchanged authored files are retained; only aggregate progress and
this note change. No pending French branch is stacked.

Fresh acceptance reproduces all 252 complete French images and the clean
resident. Production verification preserves all thirty accepted petal and
twelve Family445 webs C owners, plus this family's twelve C owners and
every assembly/raw extent. All 37 compiled layouts, 35 resident bindings,
three caller owners, 140 French, 71 Spanish and 16 progress regressions
and the repository policy gates pass.

### Accepted Family433 reconciliation

Before publication, the maintainer accepted Family433 webs as `290133e23`.
This accepted commit is also merged normally, preserving the verified
`c95089732` reconciliation rather than publishing another progress
conflict. All 252 accepted registrations and 936 accepted C instances
remain intact, including the four newly accepted Family433 webs owners.
The same four strip additions give 940/1,581 C instances and 923,324 C
bytes. No pending branch is stacked, and the original 24-path scope
with 22 unchanged authored files remains. Fresh acceptance reproduces
all 252 complete French images and the clean resident. Actual production
objects preserve the thirty petal, twelve Family445 webs and four
Family433 webs C owners, alongside this family's twelve C owners and
all assembly/raw extents. The 37 compiled layouts, 35 resident bindings,
three callers, 141 French, 72 Spanish and 16 progress regressions and
repository policy gates pass.
