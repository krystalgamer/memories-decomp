# Spanish MODEL headers 442 and 592

Four distinct Spanish images for models259/630 reuse the unchanged accepted
`variant425_{sheet,spiral,rays,webs,bands,spokes,rings,quad}.c` bodies through sixteen existing
French442 wrappers. A separately refined ribbon body is selected directly
for Spanish slot 0 and included by the slot-1 wrapper. Thirty-six
compiler-owned C instances cover 51,952
instruction bytes using named `gcc_2_8_1_g0_split`, GCC 2.8.1 and MASPSX
2.81. Previously accepted implementations, local/SDK declarations and
profiles are unchanged; French source provenance is not assumed Spanish
byte identity.

## Loader and boundaries

Models259/630 map to compact records259/580. Stages7/8 select ten-sector
slices180/190 within the 276-sector records, loaded at
`0x8013B000`/`0x8017B000`. The matched resident controller calls offset4
with context and decoded initial command, then update command-1.
The [instance ledger](spanish-model-variant442-instances.csv) records
actual Spanish archive slices, hashes, headers and requests608000.

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `4..1174` | 4464 | generated assembly | yes |
| `1174..1560` | 1004 | sheet C | yes |
| `1560..1F2C` | 2508 | spiral C | yes |
| `1F2C..2924` | 2552 | rays C | yes |
| `2924..2CE8` | 964 | webs C | yes |
| `2CE8..3334` | 1612 | ribbons C | no |
| `3334..3A40` | 1804 | bands C | no |
| `3A40..3D50` | 784 | spokes C | no |
| `3D50..40CC` | 892 | rings C | no |
| `40CC..4430` | 868 | quad C | no |

Strict walks cover every instruction and terminal return in all ten spans,
including retained ribbons at `+0x2CE8`. Sheet, spiral, rays and webs are entry-call
reachable C helpers; the other five remain retained code without a
demonstrated entry execution path.

Four entry assembly instances / 17,856 bytes remain untranslated. Real
input/final storage owners preserve every four-byte header and 3,024-byte
suffix, covering all 81,920 image bytes. The 12,096 suffix bytes remain
unclassified, not established non-code.

## Entry-called rays

The unchanged accepted French symbol-renaming wrappers around
`variant425_rays.c` independently reproduce all four 2,552-byte Spanish spans
with 456-byte frames under the same named profile. Four complete private links
retain all thirty-two previous compiler objects byte-for-byte. The sixteen
historical attempt rows remain verbatim; two terminal wrapper records are added.
No shared C, headers, profiles, bindings or raw extents change.

Forty fresh target-compiled constants check the 184-byte ray, its fields and
endpoints, SDK vectors/matrices/coordinates, signed four-byte projection words,
128-byte status array and 52-byte GT4 fields. Sixteen rays occupy
`DC0..1940`, ending at the ribbon records. Point arrays begin at `18/48`,
projected words at `30/60`, angles at `3C`, widths at `6C`, depths at `94`,
and signed halfword offsets at `A0/A6`. The packet remains at `2570..25A4`.

Fresh exact Spanish resident linking establishes all eleven actual ray callees,
their selected input definitions, sized final symbols and complete retail
function bodies. Both real context-pointer data owners are checked again.
Four fresh closed CFGs cover all 638 ray instructions. The actual entry call
at `100C`, with original-context `s3` in the delay slot, and the existing
descriptor/minimum-view regressions execute against Spanish inputs.

The original `status[16][2]` spelling is retained. The third point writes the
next status row; for the final ray, its pointer is `sp+150`, aliasing the
projection `p` output. Actual `RotTransPers4` loads the `p` and flag pointers
from argument slots `20/24`, stores `p` first and combined flags afterward.
The final aliased flag therefore overwrites `p`; subsequent single-point
projection rewrites `p` again. Drawing reads only status columns zero and one.
This is measured target behavior, not safe-array, original-type or whole-frame
lifetime evidence. GPU/GTE execution is not emulated here.

Actual initialization clears size at `273C`. Isolated execution of the retail
growth/clamp/phase instructions, including branch and load delays, agrees with
an independent scalar oracle in 104,448 cases across all four images; twelve
mutated instruction controls are rejected. Under zero initialization, a byte
step and no external size corruption, the seventeen sizes `0,256,..4096` are
closed. Growth adds `step * 256` whenever size is below 4096, regardless of
phase. Only reaching the clamp while phase is two advances it to three.
Starting a call already at size 4096 does not advance phase two. No stable
getter, whole-animation termination or complete writer audit is inferred.

Existing geometry, packet, flag/depth and caller anchors remain checked.
The focused state execution is not a new whole-ray native rendering oracle,
allocation-capacity proof or global lifetime-isolation claim.

## Independently verified views

Entry captures `a0 -> s3 -> s6` and initializes three 512-byte webs at
context0 through `+0x600`. Each has two five-by-six `SVECTOR` grids
at0 and `+0xF0`, 48-byte rows, color at `+0x1E0` and scale at `+0x1F4`.
These are not family435's 608-byte six-by-six records. Entry calls the
webs helper at module `+0xFF0`, passing `a0 = s3` in its delay slot.

One 456-byte band at `+0x1CA0` is followed by one 152-byte sheet at
`+0x1E68`, six 144-byte rings at `+0x1F00`, four 144-byte spokes at
`+0x2260` and one 144-byte quad at `+0x24A0`. The band is not the
492-byte padded form used by family435. Its point rows, projected
coordinates, four-byte color entries and depths have independently
verified offsets and endpoints. Spokes read the sheet size at
`0x1E68 + 0x88 = 0x1EF0`.

Eight 108-byte `ModelVariantRibbonShort` records occupy
`+0x1940..+0x1CA0`. Their points, projected coordinates, angles, widths,
depths and halfword offsets have verified field and array endpoints.
The ribbon `POLY_G3` packet occupies `+0x2530..+0x254C`, immediately
before the quad packet.

The line packet is at `+0x266C`, band `POLY_GT4` at `+0x2570` and
quad `POLY_G4` at `+0x254C`. Web translation reads
`+0x26A8/+0x26AC/+0x26B0`, whereas band/ring/quad translation reads
`+0x2694/+0x2698/+0x269C`. These distinct views are not conflated.
Step and phase accesses include `+0x2700/+0x2748`.

The 284 Spanish instruction anchors per image verify entry/helper
capture, record initialization and strides, loop bounds, descriptor
arithmetic, sheet timing and the sheet/web calls. The 188 independently target-compiled
constants verify all seven canonical record types, accessed fields,
grid/array endpoints, SDK vectors/matrices, stored coordinate pointers,
packet fields and four-byte pointer/integer widths. Actual storage is a
752-byte `.rodata` section with four arrays at0/292/448/588; GCC's size-zero
NOTYPE labels are checked against the real section extent and all values.

Every actual request608000 selects command0. The 56-byte descriptor is
at module `+0x452C + (command % 1000) * 56`; entry stores its pointer
at context `+0x2714`. All selected windows fit their preserved suffix
owners. Direct entry accesses establish the minimum context view
`+0x2760` (10,080 bytes).

Fresh exact Spanish resident proofs verify 36 real callee input/final
owners, three matching initializer/controller/loader owners, and both
context-pointer owners at `0x80010024/28`. Those pointers belong to input
`.data` in `spanish_raw_80010000.o`, despite the mixed executable output
`.main`. Contexts `0x80136000/0x80176000` and their minimum views do
not overlap the selected MODEL, primary or secondary loads. This is not
allocation-capacity or whole-game lifetime-isolation evidence.

## Sheet rendering and timing

The unchanged sheet body independently matches all four Spanish spans:
1,004 bytes each, adding four C owners / 4,016 bytes. All six retained
helpers were freshly rebuilt, including the actual accepted Spanish ribbon
sources rather than the French fixed-index candidate.

The single sheet contains four eight-byte vertices in each corner array at
offsets0/32/64/96, outer color at128, inner color at132 and size at136.
The fourth color bytes and bytes140..152 remain uninterpreted by this helper.
Entry initializes the record and zeroes its size. Its fixed 52-byte
`POLY_GT4` at `+0x25A4..+0x25D8` is initialized by the real
`SetPolyGT4` at `0x80082EE8`; projection writes coordinates at packet
offsets8/20/32/44. The first three corners use the inner RGB and the fourth
uses the outer RGB, with direct color writes ending at byte42.

The 256-byte frame holds rotation40..48, scale48..64, matrix64..96,
local-screen matrix96..128, coordinate128..208, projection result208..212,
flag212..216 and saved registers216..256. The OT stays in `s8`.
Ten static calls resolve to nine distinct real resident callees. The
record loop executes once and the corner loop submits four quads through
the same packet pointer. Odd frames add signed `size / 8` to scale.
Translation reads `+0x26A8/+0x26AC/+0x26B0`, not the other helpers'
`+0x2694/+0x2698/+0x269C` view.

Sorting requires both signed transformed depth and signed flag to be
nonnegative. The original signed `depth * 8 / 10` multiply-high sequence
and truncation toward zero remain unchanged, followed by unsigned
16-bit sorting depth.

On the negative-command update path, entry preserves the original context
in `s3` and calls the sheet at `+0xFE0`, with `a0 = s3` in the delay
slot, only when unsigned time reaches descriptor field `+0x1C` (44).
For phase0 and size below4096, growth is the original unsigned quotient
`((time - 44) << 12) / (80 - 44)`, with signed clamp4096 and phase1.
Otherwise decay starts at unsigned time320 and subtracts the unsigned
quotient `((time - 320) << 12) / (440 - 320)` from4096, clamps at0,
and changes phase3 to4. Both original zero-divisor traps remain.
During phase1, `+0x2734` advances by `step << 4` to1536, then phase2.
These timings come from all four actual 56-byte descriptors, not assumed
cross-region behavior. Direct sheet accesses establish `+0x274C`;
the larger entry minimum remains `+0x2760`.

## Spiral geometry, projection and state

The unchanged accepted spiral wrappers independently reproduce all four
2,508-byte Spanish spans. Their `VERSION_FRENCH` arm separates the angle
call, timing-pointer assignment and angle addition; this is verified Spanish
instruction scheduling, not an assumed regional equivalence. The seven prior
C helpers per image, including the refined Spanish ribbons, are unchanged.

Sixteen 124-byte arms occupy `+0x600..+0xDC0`. Each has two eight-byte
vertices at16, projected coordinates at32, angles at40, displaced vertices
at48, displaced coordinates at64, widths at72, signed depths at100 and
halfword screen offsets at108/112. Unused fields and padding remain untouched.
Radius512, sweep-derived angles and signed spread divisions128/32 preserve
the original integer arithmetic and halfword narrowing.

The 440-byte frame has independently bounded argument/local windows:
arguments16..40, rotation40..48, scale48..64, matrix64..96,
local-screen matrix96..128, coordinate128..208, `status[16][2]`208..336,
projection result336..340 and separate single-point flag340..344.
This does not classify every spill slot or prove whole-game lifetimes.
Each arm makes two four-point projections with duplicated vertex/output pairs,
plus two single-point projections. Primary flags occupy distinct eight-byte
status rows, not the 124-byte arm stride. The separate single-point flag does
not control visibility.

Both quad halves reuse the first 52-byte GT4 at `+0x2570..+0x25A4`,
initialized by the real `SetPolyGT4` with length12 and command`0x3C`.
The sheet uses the following packet, not this one. Submission requires
nonnegative signed depth and primary flag for point0; depth0 and flag0
are accepted, and sorting priority narrows to16 bits. Packet command,
UV fields and tails are preserved.

Entry's stack home`+0x84` holds the sheet/timing record at context`+0x1E68`.
Its initializer zeros the four-byte size at record`+0x88`, context`+0x1EF0`;
the different stack home`+0x88` holds the ring pointer. The size contributes
to signed-halfword scale, including the frame-parity size/8 term.
Entry zeros the sweep, spread, parity, frame step, phase and shrink factor.
On the negative-command path, unsigned time must reach descriptor boundary44,
then sheet`+0xFE0` precedes spiral`+0xFE8`, both receiving the original context.
Two distinct frame-step getter calls remain; the second result updates`+0x2700`.
At phase2 or later, a shrink factor below2048 advances by step*128 and
clamps at2048. The signed-halfword sweep advances by16 with narrowing.

An additional43 target-compiled constants,44 helper and29 scalar instruction
anchors per image establish these views, calls and initialization paths.
Eleven actual resident dependency owners and the entry packet setter were
verified against selected input objects, the resident link and retail bodies.
A local ILP32 oracle checks65,536 cases spanning every signed sweep value,
structured scale/spread/phase/step edges, all10,240 context/guard bytes and
every submitted52-byte packet. It observes4,194,304 projection calls and
rejects14 compiled mutations. Deterministic mock trigonometry/projection
checks argument and state contracts; it is not GPU/GTE emulation or retail
execution.

## Exactness and scope

The [attempt ledger](spanish-model-variant442-attempts.csv) records sixteen
terminal wrapper matches. Full unmasked links, actual compiler/assembly/raw
owners, sized functions, dependency fingerprints, layouts and resident
owners are checked independently.

The ribbon calibration used accepted `variant425_ribbons.c` with local
declarations, then recovered the first-edge indexing from Spanish
instructions. Both slots were checked against both model images:

| Experiment | Compiled / target bytes | Differing words per image |
|---|---:|---:|
| Accepted fixed-index first edge | 1600 / 1612 | 221 |
| Indexed first edge, `sa[k + 1]` | 1612 / 1612 | 0 |
| Promoted body with existing resident binding alias | 1612 / 1612 | 0 |

The indexed form reproduces the separate screen pointer at stack `+0xF8`
and the `0x128` frame without pinned registers or inline assembly.
The initial calibration link exposed a missing `RotTransPers` name;
existing Spanish bindings establish its address `0x80087868`. The
promoted body uses the existing `func_french_80087868` symbol, preserving
all 36 bindings and their independently verified resident owners.

Regional regressions reuse the French source/boundary fixture, whose
descriptor test now selects the configured region's archive rather than
always reading France. Spanish-specific checks cover all fallback bindings,
actual descriptor arithmetic and the observed minimum context.
Sheet regressions additionally check the actual caller, packet initializer,
frame, complete static call sequence, signed sorting gates and descriptor
timings. Spiral regressions additionally cover the original-context caller,
status/p/flag windows, projection targets and preserved fallback alias.
The spiral adds `RotTransPers = 0x80087868` while retaining
`func_french_80087868` for existing ribbons and assembly: 37 names still
refer to the same36 resident addresses.
Previously accepted module records are preserved; progress snapshots
remain separate. Unknown game code, further Spanish runtime discovery and
the expanded seven-release campaign remain open.
