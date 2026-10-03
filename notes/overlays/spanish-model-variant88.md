# Spanish MODEL headers 88 and 218

Fourteen distinct images for models 31, 290, 295, 408, 501, 518 and 531
have an independently matched 2,384-byte entry. The unchanged accepted
French canonical body, header and slot wrapper are selected under
`gcc_2_8_1_g0_split`, using GCC 2.8.1 and MASPSX 2.81. No compiler,
shared SDK declaration or other regional mapping changes. The shared
model/graphics header now includes the existing canonical
`Model_GetFrameStep` declaration from `func_80058E1C.h`, avoiding an
implicit declaration without duplicating or changing its signed return type.

The [instance ledger](spanish-model-variant88-instances.csv) records
the physical slices, compact records, commands and complete hashes.
Models 290/295/501/518 use stages 7/8; models 31/408/531 use stages 9/10.
These select ten sectors at record offsets 180/190 or 200/210 and load
at `0x8013B000`/`0x8017B000`. Equal French archive/image hashes were a
discovery lead, not a substitute for independent Spanish compilation
and ownership checks.

## Exact images and measured ownership

| Image offsets | Bytes per image | Selected owner |
| --- | ---: | --- |
| `0..4` | 4 | Raw header |
| `4..0x954` | 2,384 | Canonical entry C |
| `0x954..0x5000` | 18,092 | One unclassified raw suffix |

Both canonical objects were freshly compiled and selected by the actual
overlay builder in all fourteen private complete-image builds. Independent
map-producing relinks reproduce the builder's ELFs. Sized input/final
symbols, executable flags and exact bytes verify fourteen C owners and
twenty-eight raw owners; no assembly fallback supplies the claimed C.

Twenty-five target-compiled constants verify the 24-byte descriptor,
canonical SDK layouts, context point span `4..0x1004`, timer span
`0x1004..0x1404`, frame at `0x1404` and texture words at `0x1408/0x140C`.
The completion byte intentionally aliases the second texture word.
The historical two-part declaration is not promoted: the canonical
five-element part view covers every selected group.

The two 28-byte image views begin at `0x954` and `0x970`; the latter
overlaps descriptor storage. Only the opaque suffix is declared as an
external object. Image and descriptor pointers are views of that one
owner, not separate overlapping resources.

## Conditional loader and controller evidence

A freshly reproduced complete Spanish resident supplies nineteen verified
callee bodies, seven loader/controller-related function owners and actual
module/context pointer data. Input symbol sizes, linker-map contributions,
selected sections and complete retail bodies were checked. The transfer
jump table's `R_MIPS_32` relocations were independently resolved; resident
data ownership comes from non-executable input sections, not the mixed
output `.main` section.

Actual retail instructions execute all fourteen model compactions and
transfer/initialization paths, plus fourteen update dispatches, with
branch and load delays. The selected ten-sector stage and metadata
sector 275 are checked. Metadata words at `0x110` reach the controller's
command array. For state 7, the corresponding request is reduced modulo
1,000 and passed with the verified secondary context; update dispatch
passes `-1` with the same context.

Executing the real entry prefix, including its resident `memset`,
active-slot getter and frame-step getter, selects descriptors at
`0x970 + index * 24`. Requests are
`18000/18002/18003/18004/18005/18006/18007`; selected point/timer counts
are 20, 30, 32, 40, 48 or 60, with positive periods and at most five
part indices. All accesses fit the measured spans. Fourteen deliberate
descriptor-base mutations are rejected.

These are conditional CPU proofs, not complete live CD or animation
execution. The async descriptor and verified sector contents are supplied;
GPU upload calls use explicit ABI-only hooks. The secondary controller is
entered at its state-selection slice with context and active-slot state
supplied. The minimum context extent `0x1410` is separate from the selected
model, primary module and overlay loads, but does not prove allocation
capacity, arbitrary-pointer safety or whole-game noninterference.

## Recovery history and remaining scope

The [attempt ledger](spanish-model-variant88-attempts.csv) preserves all
twenty-five earlier material mismatches, two isolated exact slot results
from the twenty-sixth source experiment, and two terminal canonical
matches. Historical fingerprints include each local header; terminal
fingerprints are the selected source-file hashes. Mismatch counts are
recomputed over complete stored linked spans, including length differences.

The five-word completion blocker is resolved by changing only the second
elapsed threshold to positive and nesting completion handling inside it,
while retaining the first negative period test. The accepted
[French recovery](french-model-variant88.md) supplied that structural lead.
The old partial declarations were used only for isolated calibration.

All 253,288 suffix bytes remain unclassified and in scope. The extra
return signatures in those bytes are not promoted functions, padding or
SDK exclusions. This change does not claim whole-animation recovery.

This independent branch starts at accepted
`0c3f929ddb7c87719231cf2da3a6b79371686d82` and preserves all 257 previous
Spanish modules. It adds fourteen C instances and 33,376 C instruction
bytes. Expected configured totals are 271 images, 1,396/1,642 C instances
and 1,904,224 C instruction bytes; these are configured counts, not
exhaustive runtime completion. Progress snapshots remain separate.

Production verification reproduces all 271 configured Spanish overlays
and the complete Spanish resident. Fresh production ownership checks
confirm the fourteen C objects and twenty-eight raw objects equal the
private proofs, with independent relinks and unchanged resident artifacts.
All 257 previous module records are preserved. The declaration correction
also preserves the complete North American resident match. Forty-seven
affected regressions pass; two optional pyelftools-dependent layout tests
skip locally, while all 25 constants are independently compiled and
extracted with the target toolchain. Metadata, basic types, external
attempts, G32 and matching-source contracts also pass.
