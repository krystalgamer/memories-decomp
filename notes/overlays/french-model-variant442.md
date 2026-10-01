# French MODEL headers 442 and 592

Four distinct secondary images reuse the accepted
`src/overlays/model_variant/variant425_{webs,ribbons,bands,spokes,rings,quad}.c` bodies
through twelve French symbol-renaming wrappers. Only the two ribbon wrappers
define `VERSION_FRENCH` for the measured first-segment indexing form. The existing
`gcc_2_8_1_g0_split` GCC 2.8.1/MASPSX 2.81 pipeline is authoritative.
Other shared expressions, headers, G32 annotations and US profiles remain unchanged.
US header 425 is source provenance, not French byte-identity evidence.

## Loader and boundaries

Models 259 and 630 (compact records 259 and 580) use stages 7/8. Record
sectors 180/190 in 276-sector compact records select ten-sector images at
`0x8013B000`/`0x8017B000`. The resident controller calls image `+4` with
context and initial command or update `-1`. The
[instance ledger](french-model-variant442-instances.csv) records actual
nonnegative commands, slices and four distinct complete hashes. Other
stages and models are not covered.

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `0x4..0x1174` | 4464 | generated assembly | yes |
| `0x1174..0x1560` | 1004 | generated assembly | yes |
| `0x1560..0x1F2C` | 2508 | generated assembly | yes |
| `0x1F2C..0x2924` | 2552 | generated assembly | yes |
| `0x2924..0x2CE8` | 964 | webs C | yes |
| `0x2CE8..0x3334` | 1612 | ribbons C | no |
| `0x3334..0x3A40` | 1804 | bands C | no |
| `0x3A40..0x3D50` | 784 | spokes C | no |
| `0x3D50..0x40CC` | 892 | rings C | no |
| `0x40CC..0x4430` | 868 | quad C | no |

Strict walks cover all ten functions with one terminal return per span and
no unresolved indirect transfer. Entry reaches only the first five
functions. The webs C helper is entry-reachable; the other five C helpers
remain retained module-local code without a demonstrated entry execution
path. Real storage owners preserve each
four-byte header and 3,024-byte suffix at `0x4430..0x5000`; the suffix is
explicitly unclassified, not established non-code.

## Accessed layouts and ownership

Entry captures `a0 -> s3 -> s6`. A single 456-byte `ModelVariantBand` begins
at context `+0x1CA0`, followed by one 152-byte sheet at `+0x1E68`, six
144-byte rings at `+0x1F00`, four 144-byte spoke records at `+0x2260`,
and one 144-byte quad at `+0x24A0`. Adjacent extents agree exactly. This is
the 456-byte band, not header435's 492-byte padded form.

Forty-four entry anchors check saved pointers, initialization counters,
increments, bounds, strides and cleared rotation. The band counter starts
zero and repeats only while nonpositive after its increment, establishing
one record. Its color rows at 324/360 each contain nine four-byte entries.
The quad loop also initializes one record; rings/spokes use bounds six/four.
These are accessed views, not whole-context allocation or extra execution
path evidence.

Ninety-five local layout constants are independently target-compiled for
this family. They cover band/sheet/ring/spoke/quad records and the SDK views
listed in the [companion422 evidence](french-model-variant422.md), including
four-byte target `long` and stored-pointer-bearing `GsCOORDINATE2`. The band
has point rows at 0/72/144, screen rows at 216/252/288 and depths at 420..452.

## Exact matching and preservation

The [attempt ledger](french-model-variant442-attempts.csv) records eight
initial terminal canonical-wrapper matches and two subsequent webs matches
after fresh current-header compilation.
Rebased candidate searches are not the acceptance gate: all four real
canonical links reproduce unmasked complete images. Thirty-six resident
bindings are independently checked; the named `RotTransPers3` address comes
from the accepted French Exodia manifest.

The combined422/442 production gate preserves all 144 previously accepted
registrations, reproduces all 152 French images and matches the clean
French resident. This family adds 16 C owners and 17,392 bytes, alongside
24 assembly owners, eight raw owners, 36 fresh-resident callee owners and
95 recompiled layouts. The 52,416 assembly bytes and 12,096 unclassified
suffix bytes remain untranslated.

Five inherited regressions check actual archive slices, commands, complete
hashes, canonical sources/fingerprints, raw extents, all ten control-flow
spans and 44 entry anchors. Together the two families add eight images,
32 C instances and 34,784 C bytes. Configured totals become 152 images,
668/1105 matching C instances and 602,540 C bytes, without claiming
exhaustive runtime coverage or seven-release completion. General progress
snapshots are separate.

## Entry-reachable smaller-web follow-up

The accepted `variant425_webs.c` compiles unchanged to 964 French bytes.
Two wrappers rename `func_8013D8F8` to `func_8013D924/func_8017D924`.
All four complete canonical images match after replacing only the assembly
owner at `0x2924..0x2CE8`, adding four C instances / 3,856 instruction
bytes. No module records, hashes, function boundaries, resident bindings,
symbol addresses or raw extents change. Final production validation
reproduces all 192 configured French images and the clean French resident.
The family has 20 section-defined C owners, 20 assembly owners and eight
raw owners; the former webs assembly object is absent from selected link
inputs. All 36 family callees and three caller/loader owners were checked
again, and all 25 layout constants were freshly recompiled. All 96 French
MODEL variant and 47 progress/global-usage regressions pass, together with
metadata, basic-type and G32/PSXLONG policy checks.

French entry captures `a0 -> s3 -> s6`, saves the web pointer at `sp + 0x94`,
and initializes three 512-byte records at context `0..0x600`. The next
record area begins at `+0x600`. Each record has two five-by-six `SVECTOR`
grids at 0 and `0xF0`, with 48-byte rows, color at `0x1E0` and scale at
`0x1F4`. These are not the 608-byte six-by-six webs of header 435.
Entry loop bounds and strides establish the smaller shape independently
of the reused header. Entry calls the helper at module `0xFF0`, passing
the same context through `a0 = s3`.

The helper uses `RotTransPers3`, the line packet at context `+0x266C`,
translation at `+0x26A8/+0x26AC/+0x26B0`, step at `+0x2700`, and state at
`+0x2748`. Twenty-five freshly target-compiled constants verify the local
web and SDK views, including packed `GsGLINE` color offsets 12/15.
Twenty-two retail anchors check entry/helper capture, record pointers,
grid and record bounds, strides and scale access in every image.

The selected descriptor is a 56-byte record at module
`+0x452C + (command % 1000) * 56`, within the single preserved suffix.
Every legal archive slice and command is independently checked. All eight
helper callees, including `RotTransPers3`, and the three matching resident
initializer/controller/loader owners agree with the fresh exact French
resident. The existing 36 family bindings already supply every name.

The fixed slot contexts `0x80136000/0x80176000`, passed through `field_DEC`,
have a direct-entry access minimum of `0x2760` (10,080 bytes). This is not
an allocated-capacity declaration. The selected model/primary/secondary
loads do not overlap that view; whole-game lifetime isolation and all
primary-context writes are not inferred.

The family becomes 20 matching C owners / 21,248 C bytes, 20 assembly
owners / 48,560 bytes, and eight raw owners / 12,112 bytes (including
the four headers). Configured French totals become 192 images,
792/1,343 matching C instances and 724,668 C instruction bytes.
Suffixes remain unclassified and counts are not exhaustive runtime coverage.

## Retained short ribbons

The [Family435 first-segment indexing recovery](french-model-variant435.md)
also produces this family's complete 1,612-byte helper and 296-byte frame.
Both slots and all four complete canonical images match. The shared
`variant425_ribbons.c` uses `VERSION_FRENCH` only for the two indexed screen
coordinate expressions inside `k == 0`; the original US path is unchanged.
Two guarded wrappers add four C instances / 6,448 bytes.

The [attempt ledger](french-model-variant442-attempts.csv) preserves the
rejected unchanged-body trial (1,600 bytes/frame 288/218 unequal words),
the rejected typed-screen trial (1,604/frame 288/215 unequal words), both
exact indexed-coordinate trials and the two canonical terminal records.
Semantic equivalence alone was never accepted.

Eight 108-byte ribbons occupy context `0x1940..0x1CA0`, immediately before
the existing band. Point, screen, angle, offset-point, offset-screen and
width fields agree with the 116-byte Family435 shape through offset 64.
The opaque middle extent is eight bytes shorter: depths are at 92 and
halfword offsets at 100/104. The record is not interchangeable with the
116-byte form. The helper draws a `POLY_G3` at `0x2530` for strictly
positive depth, unlike Family435's nonnegative-depth condition.

Scale is the sheet field at `0x1E68 + 0x88`. The selected 56-byte descriptor
is still `module + 0x452C + (command % 1000) * 56`; every command is
`608000`. Its halfword at `+0x18` is one and is compared with the signed
halfword at context `0x2728`, plus one. Equality advances the angle at
`0x2744` by the step at `0x2700` shifted left five. This comparison value
is not evidence of a whole-animation duration.

The combined independent proof compiles 45 local/SDK constants, checks
59 focused anchors per family and verifies every slice, command and
descriptor, all 11 ribbon callees and three resident caller/loader owners.
The 36 binding addresses do not change; `0x80087868` receives the accepted
`RotTransPers` name. The direct context minimum remains `0x2760`, not
allocation capacity or global lifetime isolation. Ribbons is retained-only.

This family becomes 24 C owners / 27,696 bytes, 16 assembly owners /
42,112 bytes and eight raw owners / 12,112 bytes. The independent combined
435/442 batch adds 48,360 C bytes across 30 existing registrations, with
27 distinct complete images. No registration, boundary, profile or suffix
changes. All 222 French and 263 US complete images and both clean residents
passed exact matching. Fresh production checks establish the combined
206 French C / 68 assembly / 60 raw owners, preserve all 30 US ribbon C
owners and recompile all 45 layout constants. All 130 French, 57 Spanish,
16 progress and five US toolchain regressions pass without skips, alongside
the repository policy gates. Inventory totals are not exhaustive runtime coverage.

### Accepted ownership reconciliation

The combined ribbon branch normally merges fixed accepted `e8c82d3d5`,
preserving all 252 registrations, 936 accepted C instances and the accepted
petal and Family445/433 webs owners. Only the original 30 ribbon instances
/ 48,360 bytes are added: totals are 966/1,581 C instances and 965,620 C
bytes. All 135 original paths remain; 131 files are byte-identical, with
aggregate progress, the two family notes and a regional ledger guard
updated. The guard keeps French experiments out of Spanish ledger checks.
One additional Spanish435 fixture preserves its original six-helper selection rather
than inheriting the French ribbon promotion. No Spanish source, inventory
or binding changes; the total scope is 136 paths. No pending French branch
is stacked.

The accepted additive header declarations are freshly calibrated in all
30 canonical images and 45 compiled layouts, alongside 59 anchors and
eleven callees per family. The baseline assembly's old projection alias
is retained only for the scratch relink at its unchanged verified address.
All 263 US and 252 French complete images and both clean residents pass
again. Production verification preserves thirty petals, twelve Family445
webs, four Family433 webs and all thirty US ribbon C owners, with the
combined ribbon families' 206 C, 68 assembly and 60 raw owners unchanged.
The 141 French, 72 Spanish, 16 progress and five US toolchain regressions
pass without skips, alongside metadata/basic-types/external-attempts/G32 checks.
