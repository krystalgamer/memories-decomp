# French MODEL headers 338 and 488

Sixteen secondary images reuse the accepted header-321 rings and strand
implementations through four three-line symbol-renaming wrappers. Shared
sources, headers and the named `gcc_2_8_1_g0_split` profile are unchanged.
The profile uses GCC 2.8.1 and MASPSX 2.81.

Canonical preflight and the final production build reproduce all sixteen
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
| `0x4..0xBA0` | 2,972 | generated assembly | yes |
| `0xBA0..0x16D8` | 2,872 | generated assembly | yes |
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

Final acceptance also includes a clean exact French resident build,
production checks of every new function/raw owner and all 35 resident
callees, and fresh compilation of the 46 layout constants. All 88 French
MODEL variant regressions and 47 progress/global-usage regressions pass,
as do metadata, basic-type and G32/PSXLONG policy checks.
