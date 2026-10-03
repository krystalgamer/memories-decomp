# French MODEL headers 336 and 486

Model 472 stages 7/8 select two independent ten-sector French `MODEL.MRG`
images at load slots `8013B000` and `8017B000`. The
[instance inventory](french-model-variant336-instances.csv) pins their actual
headers, sectors and complete hashes.

| Image offset | Bytes per image | Owner |
| --- | ---: | --- |
| `0` | 4 | Raw header |
| `4` | 2,712 | Entry assembly |
| `A9C` | 2,360 | Ribbon C |
| `13D4` | 1,164 | Paired-origin sheet C |
| `1860` | 780 | Strand C |
| `1B6C` | 2,124 | Streamer C |
| `23B8` | 11,336 | Unclassified raw suffix |

The accepted strand-only registration was followed by independent scratch
links replacing all three other helpers with actual C input objects. Both
complete images reproduce all 40,960 bytes; the six new C definitions own
11,296 bytes. Their thirteen resident imports and 82 target-compiled layout
constants were independently verified. The already accepted strand was
retained as raw bytes in this independent scratch proof.

The clean production rebuild matches the complete French resident and all
289 configured overlays. Independent mapped relinks of both images equal
the production ELFs. All fourteen selected input owners are verified:
eight C owners / 12,856 bytes, two entry assembly owners / 5,424 bytes, and
four header/suffix spans / 22,680 bytes. Every production C object's text
matches its frozen candidate, including the accepted strand. The 33 resident
owners were reverified after rebuilding. All 63 focused tests pass without
skips, including nine dedicated MODEL336 tests; metadata and source-contract
checks pass. No physical registrations, resident bindings, shared types or
accepted strand sources change.

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
record, packet and matrix layouts. No shared type, header, regional conditional or implementation is modified.

### Ribbon nodes at `A9C`

Four independently recovered 960-byte records precede the sheet array.
The record remains in a dedicated ribbon header, as required by the
repository's C-type-definition gate: its known geometry arrays use
canonical `SVECTOR`/`PSXLONG` types, while the gap at `224..2C8` is explicitly
unknown. Two word-sized vectors at `2C8` and `2D8`, followed by state/count/
extent at `2E8/2EC/2F0`, agree with the separate sheet helper's accesses.
Projection flags precede depths at `2F4` and `338`, respectively.

The signed positive state gate at context `2000` controls geometry and
rendering. Each ribbon joins a source at height `-1024` to an endpoint at
height zero, positioned at successive quarters of the context displacement.
Seventeen points use the observed `1100` wave increment and `1024` inter-node
phase, with the view offset derived from `extent * 48 / 1024`.
Terminal projection uses point fifteen and the current point, not a guessed
additional point. Alternating `POLY_FT4` packets preserve depth/flag gates.
Count growth is `step * 2`, capped at sixteen; completing a node advances the
sheet count to `i * 2 + 3`, capped at eight. State two shrinks extent by
`step * 8`, and the final node can advance the global state to three.
The two wave words advance by `step * 650` and `step * 100` even when the
positive state gate is closed.

### Paired-origin sheets at `13D4`

The 152-byte `ModelVariantSheet` records start at `F00`. The signed runtime
count at `1FEC` controls traversal, with even/odd records using the two
origins in the same ribbon node. The node advances by 960 bytes only after
an odd record. Each sheet projects four `POLY_GT4` quads at `1E3C`;
retail depth is submitted directly, without the scale or normalization
present in other regional ring implementations. The otherwise unused
`ratan2` call is retained.

Even sheets grow to 4096 during node state zero; odd sheets grow to 8192
during node state one. In state two their scales derive from node extent
times four/eight. The eighth-sheet/global-state condition and the
unconditional packet-color writes are retained exactly.

### Timed streamers at `1B6C`

Two canonical 888-byte `Variant321Streamer` records start at `13C0` and end
at the strand array's `1AB0`. Seventeen-point generation uses a signed-short
reach of 128, the measured frame-dependent rotations, and word-sized world
translation at `1F34`. The large-width clamp is two, not the NA321 value
three. Each actual endpoint case completes its own projection offsets;
there is no artificial `if (work)` condition or self-assignment.

During state zero, positive extent at `1FFC` is recomputed as 1024 minus
the unsigned elapsed-time fraction between descriptor words `10` and `14`
through the guest pointer at `1FD8`. Signed nonpositive results clamp to
zero and advance state to one. No timing record size, allocation size, or
unobserved descriptor fields are inferred.

Compilation uses the authoritative `gcc_2_8_1_g0_split` profile,
GCC 2.8.1 / MASPSX 2.81. Historical compiler claims in comparison sources do
not determine this pipeline.

## Experiments and scope

The [attempt ledger](french-model-variant336-attempts.csv) preserves the
initial paired code-only calibration, a paired calibration with distinct
slot symbols and layout checks, and the terminal promoted-source fingerprints.
Both initial calibrations reproduce all 780 strand instruction bytes with
the 264-byte frame. The additional sheet candidate is exact at 1,164 bytes/
frame 272. The first ribbon candidate is 2,336 bytes/frame 296 with 339
differing words; current-point terminal indexing gives the exact 2,360-byte/
frame-304 result.

Five nonexact streamer experiments preserve results of 2,116/2,124/2,132/
2,132/2,128 bytes, all frame 328, with 266/130/223/223/231 differing words.
The two 2,132-byte results are byte-identical despite distinct source forms.
Completing projection offsets inside each real endpoint case resolves the
induction/register-allocation and scheduling differences: 2,124 bytes,
frame 328, zero differing words in both slots. Removing obsolete unused
locals retains this equality. Rejected sources remain scratch-only.

The regression fixture pins both physical identities, four C owners per
image, preserved entry/raw extents, source fingerprints, canonical and local
layouts, retail control flow, all helper frames, measured growth/timing
behavior, and the absence of observed local strand calls or line submission.
French aggregate expectations include 289 images, 1,583 C instances out of
1,859 inventoried functions, and 2,116,964 C bytes.

The entry per image remains assembly. The suffix remains unclassified
and byte-preserved, not padding or excluded code. The observed views do not
establish the work allocation size, every possible bank entry point, or
complete family-wide C coverage.
