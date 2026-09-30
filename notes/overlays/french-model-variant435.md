# French MODEL headers 435 and 585

These 26 secondary-handler archive instances reuse the unchanged accepted
`src/overlays/model_variant/variant418_{sheet,spokes,rings,quad}.c` bodies. Eight
three-line wrappers rename the functions to their measured French addresses.
All use `gcc_2_8_1_g0_split`, GCC 2.8.1 and MASPSX 2.81; the original US
registration's GCC 2.7.2 profile is not used or modified. Header numbers are
not cross-region identities: the matching French family is 435/585, not
418/568.

## Loader-backed images

The matched `Model_LoadMonsterMerge` and texture-transfer phase callback read
276 sectors per compact model record. Stages 7/8 occupy record sectors
180/190; stages 9/10 occupy 200/210. Each image occupies ten 2,048-byte sectors.
Slots load at `0x8013B000` and `0x8017B000`. The accepted resident
`model_control.c` and `model_intro_controller.c` dispatch secondary handlers
through `D_80010014/18 + 4`, using the stored context and initial command or
the update argument `-1`.

| Stages | Models |
|---|---|
| 7/8 | 34, 71, 124, 182, 279, 361, 491, 580, 640 |
| 9/10 | 166, 275, 469, 590 |

The [instance ledger](french-model-variant435-instances.csv) records compact
indices, stages, sectors, actual positive metadata command words and complete
image hashes. All 26 slices are independently verified against the French
archive; they contain 23 distinct complete images. Shared suffixes or code
prefixes alone were not used to declare whole-image identity.

## Function and storage ownership

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `0x4..0x1084` | 4224 | generated assembly | yes |
| `0x1084..0x1AD4` | 2640 | generated assembly | yes |
| `0x1AD4..0x1E7C` | 936 | sheet C | yes |
| `0x1E7C..0x2258` | 988 | generated assembly | yes |
| `0x2258..0x28A4` | 1612 | generated assembly | no |
| `0x28A4..0x2FB0` | 1804 | generated assembly | no |
| `0x2FB0..0x32B8` | 776 | spokes C | no |
| `0x32B8..0x3634` | 892 | rings C | no |
| `0x3634..0x3998` | 868 | quad C | no |

All nine spans have complete direct control flow, one terminal return, and no
unresolved indirect transfer. The last five are not reachable from the
entry's direct call graph. No direct J/JAL, absolute pointer word or matching
low-half address immediate for the three retained C helpers was found anywhere in
these images. This does not exclude computed or resident-mediated dispatch.

There is positive module-local ownership evidence beyond recognizing bytes.
The active entry captures its context through `a0 -> s2 -> s6`; at offsets
`0x28`, `0x30` and `0x38` it forms the exact ring, spoke and quad record bases
`context + 0x1374`, `+ 0x16D4` and `+ 0x1914`. Its initialization loops advance
these records by 144 bytes; the ring loop initializes six records. It also
clears the quad rotation/state word at `context + 0x1B94`. Ten entry anchors
were checked in every distinct image. Combined with the contiguous complete
functions, shared record layout and exact game-specific bodies, this supports
retained module-local game code rather than unrelated residual payload.

**Retained code is not proof of execution.** The three retained helpers count matching C
owners in the configured inventory, not additional demonstrated runtime call
paths. No exhaustive overlay coverage or unreachable-code exclusion is claimed.

Each image retains real generated storage for its four-byte header and its
5,736-byte suffix at `0x3998..0x5000`. The suffix remains unclassified;
representing its preserved bytes with a data segment does not establish that
it is entirely non-code. No linker-only alias substitutes for either owner.
Record layout checks likewise do not establish a context allocation bound.

## Matching evidence and experiments

The [attempt ledger](french-model-variant435-attempts.csv) records the initial six
terminal canonical-wrapper matches and two subsequent sheet matches.
No function-body or compiler-flag
variation was needed. Initial calibration compiled the three accepted local
US bodies with the authoritative French profile, scanning 2,362 distinct
secondary payloads. Rebased internal J26 values were used only to locate
candidates; actual subsequent links and raw complete-image comparisons, not
masked comparisons, established byte identity.

Two probe assumptions were corrected before integration: internal `.text`
J26 relocations are legitimate, and entry reachability does not cover all nine
functions. A layout probe also initially expected `move s6,a0`; the measured
two-step context capture above replaced that assertion. These were probe
setup/ownership failures, not invented nonmatching C experiments. Their
original artifacts remain under ignored `tmp/coverage-probes/`.

Fifty-two target-compiled constants verify the local quad/ring/spoke records
and canonical `SVECTOR`, `VECTOR`, `MATRIX`, `GsCOORDINATE2`, `GsGLINE`,
`POLY_G4`, `long` and `s32` layouts. The shared header's original compiler
comment describes the US registration; French profiles are explicit in every
matching manifest.

The initial production gate reproduced all 62 then-configured French images and
the full French resident executable. For these 26 images it checked 78 selected C
owners, 156 assembly owners, 52 raw header/suffix owners and all 37 actual
resident callee owners. Those initial helpers added 65,936 matching instruction
bytes; at that point 317,304 bytes remained assembly and 149,136 suffix bytes
remained unclassified in this family. Existing shared sources, compiler profiles, other-region
registrations and all previously accepted French modules are preserved.

## Entry-reachable one-sheet follow-up

The accepted `variant418_sheet.c` body compiles unchanged to 936 bytes under
the French `gcc_2_8_1_g0_split` profile. Two canonical wrappers rename
`func_8013CAA4` to `func_8013CAD4` and `func_8017CAD4`, respectively.
All 26 complete images match after actual linking with those C objects in
place of the original assembly owner at offset `0x1AD4`. The US function's
different instruction count and compiler profile are not adopted.

This helper is the third function in the existing entry call graph, not
another retained-only helper. Its one `ModelVariantSheet` record begins at
`context + 0x12DC`. The entry stores that base at `sp + 0x84`, then reloads,
advances and stores the same pointer by 152 bytes at offsets
`0x78C/0x794/0x798`. The loop counter starts at zero, increments once, and
repeats only while nonpositive, independently confirming a single initialized
sheet. Ten context/base/pointer/counter instruction anchors were checked in
every registered image. This is initialized-record ownership, not proof of
the entire context's allocation bound.

Seventy-one target-compiled constants extend the earlier layout checks to
include `ModelVariantSheet` and `POLY_GT4`. Fresh production validation
reproduces all 98 configured French images and the complete French resident,
checking this family's 104 C owners, 130 assembly owners, 52 raw header/suffix
owners and 37 fresh-resident callees. The sheet adds 26 C instances and
24,336 instruction bytes without changing any boundary, module hash, archive
selection, existing helper body or resident binding.

The family now has 90,272 matching C bytes, 292,968 assembly bytes and
149,136 unclassified suffix bytes. Configured French totals become
98 images, 500/769 C instances and 405,156 C instruction bytes. The three
retained C helpers remain distinct from proven entry paths, and every suffix
remains unclassified. General report snapshots are separate.
