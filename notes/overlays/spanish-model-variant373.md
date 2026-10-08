# Spanish MODEL373 grid, points, strip, ribbons, quads and rays

Model707 compacts to record607. Stages7/8 select distinct 20KiB images
at sectors167712/167722, headers373/523, and load addresses
`0x8013B000`/`0x8017B000`. The instance ledger records both retail hashes.
Identical French archive/image hashes were discovery leads, not the
basis for assuming portability.

## Independently verified ownership

Twelve unchanged canonical sources and slot wrappers in
`src/overlays/french_model_variant/variant373_*.c` are selected under
`gcc_2_8_1_g0_split` (GCC2.8.1/MASPSX2.81).

| Image offset | Bytes per image | Selected owner |
| --- | ---: | --- |
| `0..4` | 4 | Raw header |
| `4..10B0` | 4268 | Entry assembly |
| `10B0..176C` | 1724 | Assembly |
| `176C..1EE4` | 1912 | Grid C |
| `1EE4..223C` | 856 | Point-group C |
| `223C..2784` | 1352 | Strip C |
| `2784..2E68` | 1764 | Ribbon C |
| `2E68..33EC` | 1412 | Quad C |
| `33EC..3EF0` | 2820 | Ray C |
| `3EF0..5000` | 4368 | Unclassified raw suffix |

The original helper proofs on accepted `e70cb8b24` reproduced both
complete retail images. Twenty selected inputs cover all40,960 bytes:
ten C owners/14,592 bytes, six assembly owners/17,624 bytes and four
raw owners/8,744 bytes after adding the grid. The ray integration adds
two C owners/5,640 bytes, giving twelve C owners/20,232 bytes and four
assembly owners/11,984 bytes with the same four raw owners/8,744 bytes.
All retained assembly/raw objects in the original proof equal independent
all-assembly baselines; selected C objects equal freshly calibrated objects.
Audit relinks reproduce the production ELFs. The original eight C instances
were accepted in #6950. The grid addition starts independently at accepted
`e44afae24`, with its canonical body/header/wrapper, SDK declarations,
profiles and relevant Spanish configuration unchanged from proof base
`87352723e`. Both new1912-byte compiler objects were freshly compiled and
fully relocated, not inferred from regional archive equality; all eight
previously accepted C objects remain byte-identical.

Thirty-four actual Spanish resident callees were verified through sized
input definitions, linker-map contributions and complete retail bodies.
Real context-pointer data select `0x80136000`/`0x80176000`.
The resident output `.main` mixes code and data: data ownership is established
from selected non-executable input sections, not output section flags.
All137 existing locally measured point/strip/ribbon/quad layout constants
were independently target-compiled and read without optional pyelftools.
The largest partial view is`0x24A0`, not a proven allocation bound.

The entry directly calls offsets`10B0`, `176C` and `1EE4`.
No direct local caller of strip, ribbons, quads or rays is demonstrated among
the sixteen closed function spans. Assembly and suffix material remain
game-owned/in scope; they are not padding or exclusions.

## Conditional request and descriptor path

Actual Spanish request instructions compact707 to607 and request276 sectors.
Transfer callback stages7/8 select ten sectors at record offsets180/190
for the corresponding slot when alternate is zero. Stage15 selects the
metadata sector275 into`0x801DD000`. The actual final phase-step callback
invokes stage16, whose word-copy instructions map metadata`0x110` to
slot`0xD08`. The observed command is539001. A nonzero disable flag replaces
all three command words with`-1`.

For controller state7 and command index0, actual remainder instructions
pass argument1 to the selected image's entry. Its table is at`0x3F60`,
and its measured stride is **40 bytes**, not52. Descriptor1 at`0x3F88`
belongs to the verified raw suffix and contains unsigned count1 at`+0xC`
and duration60 at`+0x14`.

Eight conditional transfer cases cover both slots, alternates and disable
states, with branch/load delays; eight valid-instruction mutations are rejected.
GPU uploads are explicit ABI-only hooks that clobber caller-saved registers.
Retail sector bytes, the async request's returned descriptor, controller
state/context and active-slot value are supplied to the isolated CPU paths.
This does not establish complete live CD/controller or animation reachability.

## Preserved helper behavior

The canonical declarations and expressions retain the independently
measured behavior described in [the shared family research](french-model-variant373.md).
Spanish proofs additionally exercise actual retail state instructions,
not merely equivalent scalar source:

- Points:101,376 sampled byte-step cases and8,640 word/halfword wrapping
  edges across all three records and both slots; ten mutations rejected.
  Scale subtracts6144 only once after crossing. For example, scale5952,
  step255 and phase0 produce48768, which then stops advancing above6144.
  Crossing at phase>=7 instead clamps to6144. This is retained behavior,
  not a closed/reachable-domain or stable frame-step claim.
- Strip:64,032 cases preserve the positive-phase and count/index gates,
  inclusive factor<=1024 growth, signed-halfword narrowing, phase1->2,
  and phase-five positive-width shrink/clamp.
- Ribbons:1,400 cases preserve the count/index-gated word-angle update
  by`step*50`, including when the positive-phase drawing gate is inactive.
- Quads:42,468 cases preserve all phase thresholds, halfword factor
  narrowing, unsigned elapsed-shift/division and subsequent signed scale
  comparison. The180 zero-divisor cases reach retail`break7` with an
  unchanged state window. Strip/ribbon/quad proofs reject twenty mutations.

The isolated state comparisons include the entire surrounding state window.
Arbitrary edge cases describe retail MIPS wrapping, not portable defined
C overflow. They do not prove whole-animation termination or reachability.

All eight helper frames have in-frame direct stack stores disjoint from
their projection outputs: points272 bytes, strip256, ribbons296, quads288.
Point stack`B8..C8` and quad stack`30..40` retain sixteen-byte opaque gaps
without invented stores or semantic objects. This direct-store audit is
not a complete indirect/callee writer audit or a new native/GTE/GPU oracle.

## Grid: independent Spanish evidence

The additional43 local grid layout constants were target-compiled and read
without pyelftools. Both288-byte frames keep direct stores disjoint from
the interpolation and flag outputs at`sp+D0` and`sp+D4`.

Actual isolated retail state/color instructions pass27,496 state cases
and864 color cases, with32 rejected instruction mutations. Unsigned timeline
differences and shifted numerators wrap before division; comparisons of the
stored scale/translation are signed. Preserve sequential timeline updates,
phase3 growth/clamp, phase4 cosine scaling, phase5 expansion and phase6 fade,
texture advancement only at signed offset<=128, nine color rows, truncation
toward zero and byte narrowing. The1,100 zero-divisor cases reach the exact
scale/translation`break7` and preserve all preceding writes: unlike the
quad proof, this need not equal the entry state. `rcos` is an explicit
supplied-value ABI hook, including caller-saved clobbers, not executed
trigonometry or a reachable-input-domain claim.

A fresh exact Spanish resident proves ten selected sized callees, including
the grid's nine external callees and the152-byte packet-queue helper at
`80084018`. Selected non-executable raw data owns the screen offsets at
`800FF444`/`800FF446` and packet cursor at`800FF5C4`; these are region owners,
not newly recovered sized global definitions or C ownership.
Actual`GsSortPoly` and its actual queue helper execute2,048 GT4 copies across
16 sequences. Each source quad is reused16 times. Full52-byte packets and
the ordering table match independent expectations, including signed-offset
halfword narrowing and24-bit links. Destroying source quads after return
does not change queued copies. Eight instruction mutations are rejected.

The actual grid loop executes32 traversals/4,096 projection calls and1,536
accepted packet insertions. Each traversal passes all153 distinct vertex
addresses in a9x17 grid, using136-byte row strides, four distinct screen
outputs, exact stack output pointers, eight52-byte quads and16 columns.
Negative depth or flag rejects sorting; accepted depths narrow to16 bits.
Projection results are explicitly supplied by an ABI hook that clobbers
caller-saved registers; packet sorting/queuing executes actual SDK code.
All context, packet-buffer and ordering-table bytes match the independent
oracle. Twenty-two instruction mutations are rejected.

Fifty-four actual entry initialization/gate cases retain an important
overlap: FT4 view`2224..2364` and GT4 view`21A0..2340` are **not disjoint**.
The original ordered writes leave all eight grid command bytes at`3E`.
Entry selects the grid when unsigned elapsed>=descriptor`+14` and signed
phase<8. Phases6/7 refresh all eight quads through actual`SetPolyGT4`,
`SetSemiTrans` and`SetShadeTex`, with the texture lookup's return explicitly
supplied. Four initializer callees and that texture callee have independently
verified selected resident owners. Initial texture words, context, phase,
colors, OT offset0/capacity and packet allocation are supplied. These proofs
do not establish the negative-relative-depth diagnostic path, matrix setup,
GTE/GPU results, allocation bounds or whole-animation reachability.

## Regression and scope

The current-master takeover of #7030 retains its reviewed feature delta while
reconciling both aggregate fixtures with all subsequently accepted Spanish
coverage. Before tracked promotion, both private shadow images and all twelve
sized input/final C owners were independently reproduced. The ray selections
already match in French in both slots; no source, header, compiler profile or
French registration changes are needed. Both existing physical registrations
and their retained assembly/raw ownership remain intact.

Both additional 2,820-byte ray bodies are freshly compiled and linked from
the accepted French C using the Spanish bindings. Regression checks require
the selected compiler object, exact sized executable ELF definition, every
external call relocation and complete retail image equality. Raw headers
and the 4,368-byte suffixes retain section-defined, byte-exact non-code owners.
Canonical ray layout, geometry, projection, strict depth bounds and phase
assertions are reused without changing the source, headers or compiler profile.
The two remaining assembly functions per image stay unmatched; ray C ownership
does not establish a direct entry-call path or whole-animation reachability.

Spanish regression coverage reuses canonical source/layout assertions,
while independently checking Spanish registration, complete inventories,
selected source fingerprints, bindings, closed spans, packet initializers,
descriptor stride and direct stack-output separation. Optional layout skips
are not counted as executed; the separate137+43-constant proofs did execute.

The original change added two images and eight C instances; the grid adds
two C instances/3,824 bytes without new images or changed module records.
Configured Spanish totals become257 images,1,382/1,628 C instances and1,870,848 C
instruction bytes. These totals are not exhaustive runtime completion.
No canonical C, shared SDK header, compiler profile or retail input changes.

The original helper acceptance matched all257 Spanish overlays and the final
clean North American executable. A separate production audit rechecks all
twenty selected inputs and exact audit relinks; all255 earlier module
records remain unchanged. The focused61-test run passes with nine explicit
optional-layout skips; full discovery passes1,687 tests with25 explicit
environment skips. Its policy gates passed and #6950 was externally accepted.
The grid addition separately matches all257 production overlays and the
final clean North American executable. Its twenty production inputs equal
the independently verified private objects; audit relinks reproduce both
production ELFs and all257 module records remain unchanged. The focused
50-test suite passes with five optional-layout skips; full discovery passes
1,693 tests with27 explicit environment skips. The eight policy gates pass.
Separate external acceptance is still required for the grid addition.
