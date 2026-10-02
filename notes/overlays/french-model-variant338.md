# French MODEL headers 338 and 488

Sixteen secondary images reuse the accepted header-321 rings, strand and
ribbon implementations through six wrappers. Rings and strand are unchanged;
ribbons use measured `VERSION_FRENCH` indexing differences. Shared headers
and the named `gcc_2_8_1_g0_split` profile are unchanged.
The profile uses GCC 2.8.1 and MASPSX 2.81.
The entry is also recovered in C, reusing accepted header-337 initialization
record views and canonical SDK/game declarations.

The original rings/strand canonical preflight and final production build reproduce all sixteen
complete images. All 172 preceding French module records are preserved,
and the full gate reproduces all 188 configured French images.

## Loader and function ownership

Models 164, 165, 210, 424 and 609 use stages 7/8; models 34, 443 and 459
use stages 9/10. Each stage selects ten 2,048-byte sectors from a compact
276-sector record. The [instance ledger](french-model-variant338-instances.csv)
records each independently verified record, command, slice and hash.
Loads are `0x8013B000` and `0x8017B000`, with entry at `+4`.
Actual header words are 338 and 488.

| Offset range | Bytes | Owner | Entry-call reachable |
|---|---:|---|---|
| `0x4..0xBA0` | 2,972 | entry C | yes |
| `0xBA0..0x16D8` | 2,872 | ribbons C | yes |
| `0x16D8..0x1B90` | 1,208 | rings C | yes |
| `0x1B90..0x1ED4` | 836 | strand C | no |
| `0x1ED4..0x270C` | 2,104 | generated assembly | yes |

Strict control-flow walks cover every word in all five spans of all sixteen
images, with one terminal return per function. Entry calls `0xBA0`,
`0x16D8` and `0x1ED4`. The strand is retained code with no direct entry-call
path; layout compatibility does not establish an execution path.
The four-byte header and 10,484-byte suffix each have one sized raw owner.
The suffix at `0x270C..0x5000` remains unclassified, not proven non-code.

The first experiment compiled only the newly accepted rings kernel with
French bindings; it did not repeat the previous broad reuse census.
Actual canonical C links then reproduced the 1,208-byte rings body in all
sixteen images. The adjacent retained strand independently compiled to
836 French bytes, rather than the 840-byte North American span. Both
results use unchanged accepted bodies, not patched machine instructions.
The [terminal attempt ledger](french-model-variant338-attempts.csv) records
the final wrapper fingerprints and profile for both slots.

## Independently observed layouts

Entry captures `a0 -> s3 -> s6`, derives the rings pointer at context
`+0x1234` and passes the same context to the rings helper. Initialization
advances by 152 bytes with a bound of two. Its four point groups at offsets
0, 32, 64 and 96, two colors at 128/132 and scale at 136 agree with the
accepted `ModelVariant337Ring` view. The two records end at `+0x1364`.

The retained strand captures `a0 -> s4`, derives records at `+0x1364`
and uses six records of stride 132, ending at `+0x167C`. Its inner loop
initializes thirteen eight-byte vectors. The line packet begins at
`+0x1EB4`; the visible-range halfwords are `+0x1F4A/+0x1F4C`.
The next-record address is independently present in entry initialization.

The rings use the quad at `+0x1DE0`, transform at `+0x1EC4`, target at
`+0x1EF4`, frame at `+0x1F28`, elapsed at `+0x1F2C`, step at `+0x1F34`,
configuration pointer at `+0x1F3C` and state at `+0x1F68`.
Thirty retail instruction anchors check capture, pointer formation,
descriptor addressing, increments and bounds in every image.
Forty-six freshly target-compiled constants check these local views and
SDK layouts. No reference-project types or flags were imported.

Entry computes a 52-byte descriptor stride and uses the table at module
`+0x2808`, indexed by the initial command modulo 1,000. Every selected
record lies inside its owning suffix. Start/end fields are at 32/36 and
fade start/end at 44/48; all selected intervals are positive. Legal archive
slices and commands were checked directly against the immutable input,
not only against extracted copies.

## Context provenance and limits

The matched resident initializer copies `D_80010024/28` into slot
`field_DEC`; retail values are `0x80136000/0x80176000`. The matched
controller dispatches the selected secondary callback with this context,
initially with request modulo 1,000 and subsequently with `-1`. The
matched loader establishes the selected model/primary/secondary load
ranges. All three caller/loader functions have sized matching C owners
whose bytes were checked against a fresh exact French resident.

Entry accesses a final observed halfword at `+0x1F7A`, establishing an
8,060-byte minimum view. The partial rings C view is 8,044 bytes and does
not claim to contain all entry fields. The selected 96-sector model,
two-sector primary and ten-sector secondary loads do not overlap the
minimum context view. This does not establish allocated reservation
capacity, every primary-context write or whole-game lifetime isolation.
No new backing allocation is introduced.

## Exactness and coverage

Preflight links reproduce all 327,680 unmasked image bytes with 32 sized
C owners, 48 sized assembly owners and 32 raw owners. The C contribution
is 32,704 instruction bytes; retained assembly is 127,168 bytes.
All 35 distinct resident callees across the five functions were checked
against the fresh resident image with SHA-256
`57ecdfb9a9e1faf8b342fe7c7304c23723810861f3ab2fa3bef9eb27b5146b44`.

The new regression fixture checks archive records, source fingerprints,
all function spans and calls, raw extents, instruction anchors, selected
timing descriptors and measured context/load separation. The aggregate
progress assertions change with the registrations; report regeneration
remains a separate snapshot operation.

This adds 16 configured images and 32 matching C instances. The resulting
configured totals are 188 images, 750/1,315 matching C instances and
684,500 C instruction bytes. These totals do not prove exhaustive runtime
coverage or classify the raw suffixes.

## Recovered entry

The 2,972-byte entry has a 208-byte frame and initializes five `0x3A4`-byte
ribbon records, two `0x98`-byte rings, two `0x378`-byte streamers and the
existing packet views. The accepted `variant337_entry.h` supplies those
three initialization-record types unchanged. A private 52-byte descriptor
and `0x1F7C` minimum context view preserve stored `G32` pointers; no backing
allocation is declared.

Descriptor byte `0x0C` selects the model part. When signed word `0x1C` is
one, the canonical resident `func_800593D0` transforms unsigned-halfword
vertex index `0x12` into the vector at context `0x1EE4`; its three components
replace matrix translation. Other modes use `GsGetLwUnit`. This branch is
present both during initialization and updates. The entry always dispatches
the ribbon, gates rings on unsigned elapsed time reaching descriptor `0x20`,
and dispatches streamers from phase two. The retained strand remains outside
the entry's direct-call graph.

Five experiments are preserved in the attempt ledger:

| Experiment | Bytes / frame | Differing aligned common words per slot |
|---|---|---:|
| Independently recovered initial body | 2956 / 208 | 573 |
| Ordered deltas and negation, separate upload result | 2960 / 208 | 589 |
| Directly reused packed upload result | 2972 / 208 | 13 |
| Packed/page local declaration order | 2972 / 208 | 0 |
| Accepted shared initialization-record views | 2972 / 208 | 0 |

The first texture result is split before directly reusing its variable for
the sixth upload. Declaring that packed variable before the two page locals
recovers stack offsets `0x94/0x98/0x9C`. Narrow-X/full-width-Y delta locals
and `-(i * 16)` preserve the observed operation order. No forced registers,
volatile barriers, inline assembly or compiler-profile changes are used.

Ninety-eight target-compiled constants and 46 literal entry anchors are
checked across all sixteen independently hashed archive slices and actual
52-byte descriptor selections. Fresh pinned Psy-Q 4.6 signatures identify
all sixteen entry SDK callees. The exact resident, selected objects and final
ELF establish all 35 resident bindings and three caller owners, including
the alternate vertex-transform callee; each entry has 57 call sites.
Eleven established SDK aliases receive canonical names at unchanged addresses.

All sixteen complete 20,480-byte scratch images match without masks or
retail-code `incbin` substitution. They contain 64 genuine C owners /
126,208 instruction bytes, sixteen assembled streamer fallbacks / 33,664
bytes, and 32 real header/tail storage owners / 167,808 bytes. The entries
add 47,552 C bytes while preserving all 48 earlier C owners / 78,656 bytes.
No unclassified suffix is reclassified.

Configured French totals become 1,258 / 1,581 matching C instances and
1,539,532 C instruction bytes across 252 images. These are inventory totals,
not exhaustive runtime coverage or campaign completion.

Production acceptance also passes the complete French resident and all 252
configured overlay images. Selected-object and final-ELF checks reproduce
the sixteen-image ownership totals above, preserve all earlier C owners and
the accepted header-337 sources, and recompile all 98 layout constants from
the promoted header. Forty-nine focused French/Spanish family and progress
regressions pass, together with basic-type, attempt, metadata and G32 gates.
Other regions and the fixed-cutoff report remain unchanged.

## Entry-called ribbons follow-up

The accepted US321 ribbon body and existing private header provide the
starting source for French `+0xBA0..+0x16D8`, independently measured at
2,872 instruction bytes / 320-byte frame in both slots. Three materially
distinct experiments are retained in the
[attempt ledger](french-model-variant338-attempts.csv):

| Experiment | Bytes / frame | Differing aligned common words per slot |
|---|---|---:|
| Unchanged accepted body | 2840 / 312 | 434 |
| Terminal `[k]` under `k == 16` | 2864 / 320 | 270 |
| Also next-point `[k + 1]` under `k == 0` | 2872 / 320 | 0 |

The second experiment recovers the endpoint's record `+0x40/+0x20`
cursors and spills. Its remaining differences arise from two missing
first-segment record `+4/+2` cursor setups, eight corresponding loads and
resulting branch/jump offsets. The third recovers those cursors.
`VERSION_FRENCH` gates only these two indexing changes; previous endpoint
15 and first-point zero remain literals. The two consecutive draw
conditions remain separate, including their original overwriting stores;
they must not be rewritten as an `else if`.

All sixteen complete canonical scratch images match without masking.
Combined links contain 48 genuine C owners / 78,656 bytes, preserving all
32 accepted rings/strand owners / 32,704 bytes and adding sixteen ribbons /
45,952 bytes. The four historical ledger rows remain byte-identical, followed
by six experiment records and two canonical terminals. No streamers are
promoted by this change.

Forty-nine freshly target-compiled constants and 128 literal retail
anchors per image independently establish five `0x3A4` records at
context `+0..+0x1234`, ending at the accepted rings. Seventeen-point
arrays begin at `a=0`, `sa=0x88`, `angle=0xCC`, `b=0x110`, `sb=0x198`,
`width=0x1DC`, `otz=0x2D8`, `flag=0x31C`, `ox=0x360`, and `oy=0x382`.
RGB is at `0x220..0x222`; signed sixteen-bit count is at `0x23A`.
Entry reads RGB from descriptor bytes `0..2`, initializes count to
`-index * 16`, and advances five times by `0x3A4`. Other initialization
writes within the opaque intervals do not assign them semantic ownership.

Entry `+0x9F4` calls with the original context, prepared at `+0x9DC`.
The helper itself draws only for positive phase `+0x1F68`. It regenerates
five seventeen-point ribbons and their displaced copies, projects both,
and uses signed nonnegative depth/flag gates with low-sixteen-bit sorting.
Two forty-byte FT4 packets at `+0x1E14..+0x1E64` alternate with frame parity
and segment index. The adjacent streamer's separate packet begins at
`+0x1E64`.

Count advances by unsigned `(step * 3) >> 1` and clamps to sixteen;
the first record can advance phase one to two. In phase three,
positive displacement `+0x1F4E` shrinks according to unsigned clock
`+0x1F2C` and descriptor timing `+0x2C/+0x30`, then clamps at zero.
The two wave clocks advance by `step * 850` and `step << 7` even when
the draw guard is false. Every legal 52-byte descriptor has a positive
fade denominator; command, archive slice and full hash are independently
checked for all sixteen images.

All 35 resident bindings, eleven helper callees and three caller owners
are verified against the exact resident. Established `ratan2` and
`RotTransPers` aliases replace address names without changing addresses.
The minimum accessed context stays `0x1F7C`, not allocation capacity;
entry and streamers remain assembly and the suffix remains unclassified.
The original thirty shared entry anchors used by the Spanish fixture
remain unchanged; the new ribbon-specific anchors are a separate
French-only regression.

The independent accepted base is
`a37865fb1344f4bf03c66531af80c0f461d9338f`, excluding pending French422
veils. Fixed-cutoff configured totals become 252 images, 1,198/1,581
matching C instances and 1,382,068 C instruction bytes. General report
snapshots remain separate; these totals do not establish campaign completion.

Final acceptance reproduces all 252 complete French images and the clean
resident. Selected input objects and linked ELFs verify 48 Family338 C
owners / 78,656 bytes, 32 assembly owners / 81,216 bytes and 32 raw
owners / 167,808 bytes. Fresh normal-pipeline builds also reproduce all
sixteen North American consumers with their actual ribbon C owners.
The default source is byte-identical to the accepted body when the two
French alternatives are removed. All 240 French, 143 Spanish and 22
focused progress/toolchain regressions pass without skips, alongside
metadata, external-attempts, basic-types, G32, declaration and note-policy
checks.

An ordinary reconciliation with accepted
`79df11901a7426fdc915f534feddfbf3a46552f4` preserves the maintainer-merged
French422 veils; no pending branch is included. All 252 French images
and the clean resident match again. Fresh defining-object/ELF checks
preserve all 28 accepted Family422 C owners / 34,896 bytes as well as
the 48 Family338 owners. All 241 French, 143 Spanish and 22 focused
regressions and policy gates pass without skips. The shared ribbon
source/header and sixteen-consumer US preservation evidence are unchanged.
Combined fixed-cutoff totals are 1,202/1,581 matching C instances and
1,389,092 C instruction bytes. Only the same 73 ribbon-specific paths
differ from the accepted cutoff.

Final acceptance also includes a clean exact French resident build,
production checks of every new function/raw owner and all 35 resident
callees, and fresh compilation of the 46 layout constants. All 88 French
MODEL variant regressions and 47 progress/global-usage regressions pass,
as do metadata, basic-type and G32/PSXLONG policy checks.
