# French MODEL headers 476 and 626

Two alternate-1 images for model 712 reuse the accepted header-459 webs
body through two three-line canonical symbol wrappers. Shared C, headers
and the named `gcc_2_8_1_g0_split` profile are unchanged. The authoritative
pipeline is GCC 2.8.1 and MASPSX 2.81.

Both complete canonical and production images match. All 192 preceding
module records are preserved; the full gate reproduces all 194 configured
French images.

## Loader and complete ownership

Model 712 uses compact record 612. Stages 9/10 select ten 2,048-byte
sectors starting at 169,112 and 169,122, loading at
`0x8013B000/0x8017B000`. Entry is at `+4`. The actual command is 642,000,
so initialization receives zero. The
[instance ledger](french-model-variant476-instances.csv) records the
independently read legal slices, actual header words and complete hashes.
Alternate-0 stages 7/8 are different images and are not registered here.

| Offset range | Bytes | Owner | Entry-call reachable |
|---|---:|---|---|
| `0x4..0x135C` | 4,952 | generated assembly | yes |
| `0x135C..0x170C` | 944 | sheet C | yes |
| `0x170C..0x20BC` | 2,480 | spiral C | yes |
| `0x20BC..0x247C` | 960 | webs C | no |
| `0x247C..0x2848` | 972 | curtains C | yes |
| `0x2848..0x2DD4` | 1,420 | globe C | yes |
| `0x2DD4..0x3374` | 1,440 | screen-grid C | yes |

Strict walks cover all fourteen spans with one terminal return per span.
Entry calls every other function except webs. **Webs is retained code,
not an established entry-call execution path.** Its measured context
compatibility does not prove runtime execution.

Each four-byte header and 7,308-byte suffix has one sized raw owner.
The suffix at `0x3374..0x5000` remains unclassified, not proven non-code.
The remaining unmatched entry per image stays generated assembly rather
than an opaque raw prefix or a C coverage claim.

## Entry-called spiral follow-up

The independently recovered `0x170C..0x20BC` helper matches all 2,480 bytes
with a 304-byte frame in each slot. It reuses the unchanged local
`Variant418SpiralArm` declaration, not the other family's geometry or
timing behavior. Sixteen 132-byte records occupy `context + 0x720..0xF60`.
Each has two spine points at `0x10`, projections at `0x20`, angles at
`0x28`, displaced points at `0x30`, their projections at `0x40`, widths
at `0x48`, flags at `0x64`, depths at `0x6C`, and signed halfword screen
offsets at `0x74/0x78`. Existing color storage is not read by this helper.

The spine alternates its azimuth sign by record parity; both branches
compute the second angle from `i * 512 + word27F0`. The displaced copy
uses signed word `0x27EC` divided by 64 at its first point and by 16 at
its second. The original branch-local arithmetic and indexed projection
path are retained. Matrix translation comes from words
`0x274C/0x2750/0x2754`; the low mode bit at `0x27AC` selects scale
`word27E4` or that value plus its signed eighth.

Both sides of each arm use one GT4 at `0x22B4`. Four signed halfword
color scalars share red/green values: inner `(64,64,0)` and outer
`(160,160,128)`; endpoint colors are zero. Initializing the draw counter
before these scalars is required for exact scheduling. Both depth and
flag must be nonnegative, and sorting consumes the low sixteen depth bits.

The direct call at `0x10D8` requires unsigned frame `0x27B0` to be at
least descriptor `0x20` and below `0x2C`: 144 through 269 for command
642000. Scale ramps to 4096 between descriptor fields `0x20/0x24`
(144/152), clamping at 4096. Positive width shrinks from 1024 between
`0x24/0x28` (152/260), clamping at zero. Angle word `0x27F0` advances by
48 per helper call, not by the frame-step field.

Five two-slot trials are preserved in the attempt ledger. Independent
checks compile 58 layout constants, verify 77 literal retail anchors,
the actual descriptor and direct caller, eleven helper callees, all
35 resident bindings and three resident caller owners. `RotTransPers`
keeps its established address `0x80087868`. Both complete canonical
links contain twelve real C owners totaling 16,432 bytes, preserving
the ten accepted owners and adding 4,960 bytes. The entry and both raw
suffixes remain separately owned; suffix classification is unchanged.
Configured French totals become 1,298/1,581 matching C instances and
1,671,340 instruction bytes across 252 images, not exhaustive coverage.

Production acceptance reproduces all 252 French overlay images and the
clean French resident. Final ELF/input-object checks establish twelve
C owners, two generated assembly entry owners and four raw owners for
this family; all 58 layout constants recompile against the canonical
header. All 410 French/Spanish family and progress regressions pass.
Spanish still selects its existing five helpers: inheriting the French
fixture does not promote the Spanish spiral or its SDK alias.

## Independently observed layout

French entry captures `a0 -> s3 -> s8`, saves the initial web base at
`sp + 0x8C`, and initializes three 608-byte records at context `0..0x720`.
The next record area's independently formed address is `context + 0x720`.
Each record has two six-by-six `SVECTOR` grids at 0 and `0x120`, color at
`0x240` and scale at `0x254`. Entry's six/six/three bounds, pointer strides,
saved pointer and grid offset are checked by fourteen retail anchors.
Twenty-five freshly target-compiled constants verify this local web
view and the SDK argument types.

The reused helper reads the line packet at `+0x237C`, translation at
`+0x274C/+0x2750/+0x2754`, step at `+0x27B8`, and state at `+0x284C`.
It fades after scale `0x800`, grows toward `0x1000`, and sorts positive
depths without the other families' flag condition. The accepted source
is reused verbatim, not normalized to a sibling's behavior.

The selected 56-byte descriptor is at module `+0x3470`, indexed by
`command % 1000`. Both observed commands select descriptor zero, wholly
inside the single preserved suffix. The legal archive hash, both slices,
command words and target address-forming instructions were checked
independently of the compiler output.

## Context provenance and limitations

The matching resident initializer supplies fixed `0x80136000/0x80176000`
context pointers through slot `field_DEC`; the matching controller and
transfer phase establish secondary dispatch and selected load ranges.
Their three sized C owners agree with a fresh exact resident image.
Direct entry accesses establish a minimum extent of `0x2864` (10,340
bytes). This is not an allocation-capacity declaration or a complete
whole-game lifetime audit.

The selected 96-sector model, two-sector primary and ten-sector secondary
loads do not overlap that minimum context view. Other primary-context
writes and all potential dispatch paths are not inferred. No new backing
allocation is introduced, and the three initialized web records do not
declare the size of the entire context.

## Exactness and preservation

Discovery compiled only the newly accepted header-459 kernel with French
bindings. Header-number similarity was not acceptance evidence. Subsequent
actual canonical links reproduce both 20,480-byte images without masks,
instruction patches or post-link rebasing. The
[attempt ledger](french-model-variant476-attempts.csv) records the two
terminal wrapper fingerprints.

Preflight establishes two sized C owners, twelve explicit fallback
function owners and four raw owners across 40,960 compared bytes.
The C contribution is 1,920 instruction bytes. All 35 distinct resident
callees across the seven functions match a fresh resident with SHA-256
`57ecdfb9a9e1faf8b342fe7c7304c23723810861f3ab2fa3bef9eb27b5146b44`.

The new fixture checks loader records, complete hashes, wrappers, all
function spans/calls, raw extents, initialization anchors and selected
descriptor/context separation. Aggregate progress assertions change with
registration; reports remain separate snapshots.
Configured totals become 194 images, 794/1,357 C instances and 726,588
C instruction bytes. These are not exhaustive runtime-coverage totals.

Final acceptance includes a clean exact French resident build, production
ELF checks of both C owners, twelve generated assembly owners and four
raw owners, all 35 callees and three caller owners, and recompilation of
all 25 layout constants. All 102 French MODEL variant and 47
progress/global-usage regressions pass, together with metadata,
basic-type and G32/PSXLONG policy checks.

## Entry-called sheet and curtains follow-up

The unchanged accepted US459 `variant459_sheet.c` and
`variant459_curtains.c` bodies match both French slots using the existing
French profile. The sheet is 944 bytes with a 256-byte frame; curtains
are 972 bytes with a 304-byte frame. Four three-line wrappers only rename
the symbols. The shared bodies, `model_variant.h`, private
`variant459_curtains.h` and original US registrations remain unchanged.
No regional conditional, new declaration or compiler option is introduced.

Each final wrapper reproduces both complete images independently.
Subsequent combined links select the sheet, curtains **and retained webs
as real compiled C objects** in each image, preserving all six sized C
owners and comparing every byte of both images. The two historical
terminal ledger rows remain byte-identical, followed by four new matches.

Forty-two target-compiled layout constants and 133 instruction anchors
verify the accessed views. One 152-byte `ModelVariantSheet` occupies
`context + 0xF60..0xFF8`, with four four-point rows at `0/0x20/0x40/0x60`,
outer colors at `0x80`, inner colors at `0x84`, and size at `0x88`.
Its `POLY_GT4` packet is at `0x22E8`; projection `p/flag` are separate
stack words at `0xD0/0xD4`. Mode word `0x27AC` low bit adds size divided
by eight to the scale. Word translations are at `0x274C/0x2750/0x2754`.
The four quads retain signed depth multiplication by eight and division
by ten, both nonnegative depth/flag checks, and low-sixteen-bit sorting.

Five 428-byte `Variant459Curtain` records occupy
`context + 0x1A18..0x2274`. The helper consumes two 17-point rows at
`0/0x88`, rotation at `0x198` and scale at `0x1A0`. Entry independently
initializes those rows, rotation and scale with a `0x1AC` stride and
five-record bound. It also writes a 17-point row-like area at
`0x110..0x198`; those bytes and the trailing eight bytes remain opaque
in the unchanged private helper view. They are not newly claimed fields.

Curtains use signed halfword translations at `0x2758/0x275A/0x275C`, not
the sheet's three words. Each positive-scale curtain draws sixteen GT4
strips through the packet at `0x2704`. Dark and light `CVECTOR` locals
are at frame `0xE8/0xF0`, with `p/flag` at `0xF8/0xFC`; the existing
24-byte reserved local remains. The active-buffer getter, two initial
`ratan2` calls and per-curtain `rsin` calls remain even where their
results are unused. Colors stay constant below scale `0x1400` and fade
to zero by `0x1800`. Scale advances by step word `0x27B8` shifted seven;
wrapping adds `0x180/0x320` to the rotation halves.

Actual command `642000` selects the existing 56-byte descriptor at
`0x3470`. The seven words at `+0x1C..+0x34` are independently measured as
`80, 144, 152, 260, 270, 284, 420` in both images. The sheet uses
unsigned division over the positive-denominator intervals `80..144` and
`152..260`, grows toward `0x1000`, expands toward `0x2000`, then shrinks
to zero. Its entry guard uses unsigned word `0x27B0` in `[80, 270)`.
Curtains' entry guard starts at 284, and a wrap stops at full scale only
when that word is **greater than** deadline 420. Calls at
`0x10A0/0x1178` pass the original context at `0x10A4/0x117C`.
Webs remains retained code without an entry-call path.

All twelve distinct helper callees, 35 existing resident binding addresses
and three resident caller owners agree with the resident ELF and retail
bytes. Five existing addresses receive SDK names:
`GsSortPoly/GsGetActiveBuff/rsin/ReadRotMatrix/SetRotMatrix` at
`0x800842A8/0x800852A8/0x80086628/0x800872A8/0x80087738`.
The 16-byte active-buffer getter alone has an ambiguous SDK signature;
its name is additionally supported by the established US LIBGS block
and corresponding French draw-offset/swap routines, which read or write
the same halfword at `0x800FF454` (US `0x800FE0CC`). All three French
SDK intervals have independently checked resident section owners.
No binding address, global storage declaration or resident inventory name
changes. The context minimum stays `0x2864`, not an allocation capacity,
and the complete 7,308-byte suffix per image remains unclassified.

This batch starts from accepted
`acddee179d4f0fa38bfb26cb27aea2c645c12f5f`, excluding pending French421
ribbons. Four C instances / 3,832 bytes give independent configured
totals of 252 images, 1,124/1,581 C instances and 1,250,212 bytes.
Verified family ownership is six C owners / 5,752 bytes, eight generated
assembly owners / 20,584 bytes and four real raw owners / 14,624 bytes.
Helper production acceptance passed: all 252 complete French overlay images
and the clean French resident match, with all six family C owners selected
from their compiled objects and sized in the final ELFs. The two previously
accepted webs owners / 1,920 bytes remain unchanged. Fresh linked-object
checks also preserve all 72 accepted Family421, 24 Family422, 208 Family435,
56 Family439 and 72 Family445 C owners. The 42 layout constants were freshly
compiled; all 38 resident binding/caller owners agree with the clean resident.
The 208 French, 112 Spanish and 21 progress/toolchain regressions pass, along
with metadata, attempt-ledger, basic-type and G32/PSXLONG policy checks.

### Accepted French421 reconciliation

After independent helper acceptance, the maintainer-accepted French421
ribbons squash `69e8f56f8e4727e91f54f554942f3428053f0eba` was ordinarily
merged into this branch. Its 55 authored paths were checked byte-for-byte
against the reviewed head, and all twelve exact-head checks succeeded.
Only the aggregate progress assertions conflicted; the combined configured
totals are 252 images, 1,136/1,581 C instances and 1,270,372 bytes.
No pending branch is included. Fresh combined production acceptance passed:
all 252 complete French images and the clean resident match. The six
Family476 C owners / 5,752 bytes remain exact, alongside all 84 accepted
Family421 C owners / 105,984 bytes and the previously checked Family422,
435, 439 and 445 owners. All 209 French, 112 Spanish and 21 progress/toolchain
regressions and policy checks pass without skips. Fifteen of the original
seventeen helper paths remain byte-identical; only this note and the aggregate
progress fixture change during reconciliation.

## Entry-called globe follow-up

The locally recovered globe at `0x2848..0x2DD4` matches both model 712
stage 9/10 images: 1,420 instruction bytes and a 280-byte frame each.
The private `Variant476Globe` view uses the existing local SDK declarations;
no accepted Spanish or US globe body was available to reuse. Slot 1 only
renames the function through a three-line wrapper. Shared sources, SDK
headers and the `gcc_2_8_1_g0_split` profile remain unchanged.

Entry forms the globe at `context + 0xFF8` and the following record at
`0x14E4`. Its spherical initializer constructs nine latitude rows with
seventeen longitude samples each, using eight-byte `SVECTOR` points and
`0x88`-byte row strides. The nine four-byte color entries start at relative
`0x4C8`; only their RGB bytes are interpreted. Point padding and the fourth
color byte acquire no new meaning. The complete private view is `0x4EC`
bytes, ending exactly at the independently formed next-record boundary.
This is not a declaration of the whole context's allocation capacity.

Entry requires unsigned frame word `0x27B0` to reach descriptor field
`0x30` (284 in the selected descriptor). It raises signed phase `0x284C`
to at least two, calls the accepted curtains, then calls the globe at
`0x1194` only while phase is below six. The original context is passed
in the delay slot at `0x1198`. Rendering is not unconditional.

The helper retains both unused `ratan2` calls. Signed halfword `0x2860`
selects the sign of X rotation from `0x2804`; Y/Z rotation are zero.
Translations are signed halves at `0x2758/0x275A/0x275C`, distinct from
the sheet's word translations. `RotMatrix` precedes `ScaleMatrix`, which
uses word `0x27FC`, before copying the matrix into the local coordinate.
`GsGetLs` and `GsSetLsMatrix` retain their original order.

Below phase five, intensity is `1024 - (scale - 8192) / 8`, with signed
truncation. Row red/blue use complementary `i * 255 / 8` ramps scaled by
intensity/1024; green is intensity/16. Later phases use intensity/8 for
all three channels. Eight bands of sixteen quads use adjacent grid rows,
with the first pair of corners colored from the current row and the
second pair from the next. One 52-byte GT4 at `0x26D0` ends at the
curtain packet `0x2704`; this helper does not rewrite UVs. Depth and flag
must both be nonnegative, and sorting takes the low sixteen depth bits.

Clock `0x2814` advances by eight while at most 128 and resets on reaching
128. Before phase three, scale grows by step `0x27B8` times 512, clamps
at 8192 and sets phase three. Phase three oscillates around 6144 using
`rcos(angle) * 2048 >> 12`; angle `0x2808` advances by step times 64.
The strict unsigned descriptor-`0x34` deadline (420) advances phase to
four. Phases four and five grow scale by step times 256; crossing 16384
sets phase five and intensity 1024 **without clamping scale**. Positive
intensity fades by step times 32, clamps at zero and sets phase six.
Angle `0x2804` advances by step times 64 on every call.

The initial reconstruction and packet-order refinement were eight bytes
short: scheduling the flag load across globe-pointer setup removed two
retail no-ops. Disabling pre-allocation scheduling through an existing
named profile instead produced 1,444 bytes. Establishing the globe view
before both retained angle calls reproduces the target exactly without
volatile accesses, barriers, forced registers or assembly. The six prior
ledger rows remain byte-identical, followed by eight measured experiment
rows and two canonical terminal records.

Independent checks compile 37 layout constants and verify 119 literal
retail anchors per image, nine helper callees, all 35 resident bindings
and three resident caller owners. The `rcos` alias keeps address
`0x800866F8`. Both complete canonical links select eight real C owners
totaling 8,592 bytes, preserving all six prior owners / 5,752 bytes.
Six generated assembly owners / 17,744 bytes and four raw owners /
14,624 bytes preserve the remainder, including the unclassified suffixes.
The batch starts independently from accepted
`86c3a544b77e29152de211e3269f5211f4a2885d`, excluding pending report work.
Configured French totals become 252 images, 1,174/1,581 matching-C
instances and 1,319,676 bytes; these are not exhaustive runtime coverage.

Globe production acceptance passed: all 252 complete French overlay images
and the clean French resident match. Final ELF and input-object checks
confirm all eight family C owners, six assembly owners and four raw owners,
and preserve the accepted C objects in ten other French families. The
private layout constants were freshly recompiled against the canonical
header, and the clean resident confirms all binding and caller bytes.
All 236 French, 136 Spanish and 21 progress/toolchain regressions pass
without skips, together with metadata, attempt-ledger, basic-type,
G32/PSXLONG and notes policy checks.

## Entry-called screen-grid follow-up

The independent local reconstruction of `0x2DD4..0x3374` matches both
model 712 stage 9/10 images on its first calibration: 1,440 bytes with a
304-byte frame each. The existing `gcc_2_8_1_g0_split` profile and local
SDK declarations remain unchanged. No accepted Spanish or US counterpart
was available; the private view and source were recovered from French
entry and helper instructions. Slot 1 only renames the function.

Entry independently establishes `context + 0x14E4..0x1A18`. Two nine-by-nine
`SVECTOR` grids occupy relative `0..0x288` and `0x288..0x510`; nine
four-byte color entries follow, giving an exact `0x534`-byte view.
Only RGB channels acquire meaning; point padding and the fourth color
byte remain uninterpreted. Initializer `0x59C..0x714` uses nine latitude
rows with angle increments of 256 and nine longitude samples `j << 9`.
The first grid uses radius/height factor 480, the second 512; the X/Z
radius additionally takes signed `*640/1024` truncation. The row stride
is `0x48`, point stride eight, and both grids share the nine-color ramp.
The following independently formed curtain view begins at `0x1A18`.

Entry calls at `0x1138` with the original context at `0x113C` while
unsigned frame `0x27B0` lies between descriptor `0x2C` and descriptor
`0x30 + 4`, inclusive: 270 through 288 for the selected descriptor.
At frames at most descriptor `0x30 - 10` (274), it refreshes word
`0x2834` from `ratan2(word2774, word2778) + 2048`. This is a measured
entry-call path, not an unconditional draw or a retained-only helper.

The helper keeps the ordering-table and active-buffer calls. X rotation
is `-lowhalf2834 - 1024`, Y/Z are zero, and translations use words
`0x274C/0x2750/0x2754`, not the globe/curtain halfwords. Unit scale 4096
still goes through `ScaleMatrix` after `RotMatrix`, before coordinate
copying, `GsGetLs` and `GsSetLsMatrix`.

Below phase five, intensity `0x2828` is recomputed as
`1024 - (word2818 - 8192) / 8`, with signed truncation. The nine-row
red/blue ramps use complementary `i * 255 / 8` factors and intensity/1024;
green is intensity/16. Later phases set RGB to intensity/8. This helper
does not advance the phase, rotation or scale state.

Eight-by-eight quads use the single GT4 at `0x2390`. The first projection
uses the second grid to derive screen-based UVs. Signed `poly->x0 < 160`
selects `GetTPage(2, 1, 320, 0)`; otherwise X is 448 and UV X coordinates
subtract 128 before byte truncation. Both branches call `SetPolyGT4`.
The signed word `tpage` local preserves the observed zero extension of
the SDK's unsigned-halfword return.

Explicit identical active-buffer branches naturally fold to the observed
unused stack reload before each page call. They are retained to reproduce
the actual GCC output, without volatile accesses, forced registers,
barriers or assembly. This does not claim to recover the original source
spelling. The second projection uses the first grid for final geometry;
`SetSemiTrans(poly, 0)` and `SetShadeTex(poly, 0)` remain before the
nonnegative depth/flag checks and low-sixteen-bit depth sort.

Independent evidence compiles 49 layout constants and checks 122 literal
retail anchors per image, twelve helper callees, all 35 resident bindings
and three resident caller owners. Established French408 SDK aliases
`GetTPage/SetPolyGT4/SetSemiTrans/SetShadeTex` preserve addresses
`0x80082CE8/0x80082EE8/0x80082DA8/0x80082DD8`. No resident inventory or
storage ownership changes. Both complete canonical links retain six prior
C owners / 5,752 bytes and add two / 2,880 bytes: eight owners / 8,632 bytes.
Six fallback assembly spans / 17,704 bytes and four raw owners / 14,624
bytes preserve the rest, including every unclassified suffix byte.

This branch starts from accepted
`1a22a113f3a7b149ebfea76e7f310ac7e1e12be8`, excluding pending globe
and report branches. The original six ledger rows remain byte-identical,
followed by two exact scratch records and two canonical terminals.
Independent configured totals are 252 images, 1,174/1,581 C instances
and 1,319,716 bytes, not exhaustive runtime coverage or campaign completion.

Screen-grid production acceptance passed: all 252 complete French overlay
images and the clean French resident match. Final ELF and selected-object
checks establish all eight family C owners / 8,632 bytes, including the six
unchanged accepted owners / 5,752 bytes. The six remaining assembly owners
and four raw owners retain their complete bytes and extents. Fresh checks
also preserve every accepted C owner in ten other French families.
All 49 layout constants were recompiled, and all 35 resident bindings and
three caller owners agree with the clean resident. The 236 French, 139
Spanish and 21 progress/toolchain regressions pass without skips, together
with repository metadata, attempt-ledger, basic-type and G32/PSXLONG checks.


## Accepted-master reconciliation

The publication guard stopped before pushing when accepted master gained
globe #6815. Its accepted tree is the exact clean merge of the reviewed head
with its accepted parent, and all twelve exact-head checks passed. Fifteen
authored paths are byte-identical; the two shared fixtures also retain the
previously accepted Spanish418 rays update. Ordinary merging of accepted
`24814f2460f814d7abc0171f90f2fd8155ca5fd3` preserves both helpers and all
twenty attempt records. No pending branch is included.

Combined configured totals are 252 images, 1,176/1,581 C instances and
1,322,556 instruction bytes. The family now selects ten C owners /
11,472 bytes, four assembly owners / 14,864 bytes and four raw owners /
14,624 bytes. The independent acceptance above remains historical.

Combined screen-grid production acceptance passed: all 252 complete
French overlay images and the clean resident match, with all ten family
C owners selected from their compiled objects in the final ELFs. The
eight accepted owners / 8,592 bytes, including globe, remain unchanged;
four assembly owners and four raw owners preserve all remaining bytes.
All accepted C owners in the ten previously checked French families
retain their registrations, sources and linked bytes. The 49 private
layout constants were freshly recompiled, and all 35 resident bindings
and three caller owners agree with the clean resident. All 238 French,
139 Spanish and 21 progress/toolchain regressions and policy checks
pass without skips.
