# French MODEL headers 336 and 486

Model 472 stages 7/8 select two independent ten-sector French `MODEL.MRG`
images at load slots `8013B000` and `8017B000`. The
[instance inventory](french-model-variant336-instances.csv) pins their actual
headers, sectors and complete hashes.

| Image offset | Bytes per image | Owner |
| --- | ---: | --- |
| `0` | 4 | Raw header |
| `4` | 2,712 | Entry assembly |
| `A9C` | 2,360 | First helper assembly |
| `13D4` | 1,164 | Second helper assembly |
| `1860` | 780 | Strand C |
| `1B6C` | 2,124 | Last helper assembly |
| `23B8` | 11,336 | Unclassified raw suffix |

Both complete scratch links reproduce all 40,960 bytes. Actual input
definitions account for two C owners / 1,560 bytes, eight preserved function
spans / 16,720 bytes, and four header/suffix spans / 22,680 bytes. The C owners
have real sized executable input sections, not absolute function aliases.

The clean production rebuild matches the complete French resident executable
and all 289 configured French overlays. Independent mapped relinks of both
new images equal the production ELFs and reverify all fourteen input owners:
two C, eight assembly and four raw. All 287 prior registrations are unchanged.
The 33 resident input owners were reverified after rebuilding. Staged metadata
and source-contract checks pass; the focused suite has 39 passes and one
optional native-clang guest-pointer check skipped. All six dedicated MODEL336
tests run without skips.

## Independent evidence and retained behavior

All ten measured function spans have closed, fully reached control-flow
graphs, one final return and resolved direct calls. Thirty-three resident
callees were verified against complete retail bodies, final ELF definitions
and their actual selected input objects. Existing French header-475 SDK
bindings cover these addresses and are reused unchanged.

The entry calls the helpers at `A9C`, `13D4` and `1B6C`. Neither it nor the
other measured functions directly calls the strand at `1860`. This does not
prove the absence of indirect or external entry points, and no runtime
reachability claim or dead-code exclusion is made.

The strand helper is gated by the signed state word at context `2000`.
It builds six sets of thirteen points, using the measured 132-byte stride,
radial division by twelve, six angular sectors and a phase increment of
`0x708`. The sine-to-height conversion deliberately uses a logical right
shift. Its matrix translation comes from the three signed halfwords at
`1F90`; scale remains `4096` on every axis.

It projects the visible halfword span `[1FF0, 1FF2)` into a `GsLINE` at
`1F10`, setting attribute `50000000` and RGB `0/64/128`. **The retail helper
does not submit these line packets.** No sort call, depth gate or projection
flag gate has been added. Span advancement retains the unsigned frame-step
shift at `1FD0`, signed halfword comparisons, clamp to twelve and reset of
both endpoints.

The accepted NA321 strand implementation provides structural evidence only.
Its offsets and line-submission behavior are not copied. The existing
`ModelVariantStrandWide`, SDK declarations and context macros are reused;
twenty target-compiled layout constants independently verify the relevant
record, packet and matrix layouts. No type, header, regional conditional or
shared implementation is added or modified.

Compilation uses the authoritative `gcc_2_8_1_g0_split` profile,
GCC 2.8.1 / MASPSX 2.81. Historical compiler claims in comparison sources do
not determine this pipeline.

## Experiments and scope

The [six-row ledger](french-model-variant336-attempts.csv) preserves the
initial paired code-only calibration, a paired calibration with distinct
slot symbols and layout checks, and the terminal promoted-source fingerprints.
Both calibrations reproduce all 780 instruction bytes with the 264-byte
frame. No rejected approximation is promoted.

The dedicated regression fixture pins both physical identities, sole C
ownership, preserved assembly/raw extents, source fingerprints, canonical
layouts, retail control flow and the absence of observed local calls or line
submission. French aggregate expectations include 289 images, 1,577 C
instances out of 1,859 inventoried functions, and 2,105,668 C bytes.

Four functions per image remain assembly. The suffix remains unclassified
and byte-preserved, not padding or excluded code. The observed views do not
establish the work allocation size, every possible bank entry point, or
complete family-wide C coverage.
