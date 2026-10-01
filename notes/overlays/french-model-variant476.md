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
| `0x170C..0x20BC` | 2,480 | generated assembly | yes |
| `0x20BC..0x247C` | 960 | webs C | no |
| `0x247C..0x2848` | 972 | curtains C | yes |
| `0x2848..0x2DD4` | 1,420 | generated assembly | yes |
| `0x2DD4..0x3374` | 1,440 | generated assembly | yes |

Strict walks cover all fourteen spans with one terminal return per span.
Entry calls every other function except webs. **Webs is retained code,
not an established entry-call execution path.** Its measured context
compatibility does not prove runtime execution.

Each four-byte header and 7,308-byte suffix has one sized raw owner.
The suffix at `0x3374..0x5000` remains unclassified, not proven non-code.
All six unmatched functions per image remain generated assembly rather
than an opaque raw prefix or a C coverage claim.

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
