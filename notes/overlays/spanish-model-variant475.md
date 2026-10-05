# Spanish MODEL headers 475 and 625

Four complete ten-sector images contain an independently matched entry and five helpers:
models 116 and 576, compact records 116 and 526, stages 7/8 and slots 0/1.
The headers are 475/625 and the command is 641000. The
[instance inventory](spanish-model-variant475-instances.csv) records every
sector and SHA-256. The census checked sixteen known secondary windows in each
of 621 complete records; it does not establish completeness of all runtime
loading paths.

## Ownership and matching

| Image offset | Bytes per image | Owner |
| --- | ---: | --- |
| `0` | 4 | Raw header |
| `4` | 3732 | Entry C |
| `E98` | 2784 | Ribbon C |
| `1978` | 1212 | Sheet C |
| `1E34` | 940 | Quad C |
| `21E0` | 832 | Strand C |
| `2520` | 2072 | Streamer C |
| `2D38` | 8904 | Unclassified raw suffix |

The four images contribute twenty-four C instances / 46,288 instruction bytes,
no assembly instances in the inventoried spans, and 35,632 raw bytes.
Unknown suffix storage is not counted as code or matching C.

The six Spanish wrappers only rename symbols in the accepted local US458
sheet, quad and strand bodies. All were independently compiled with
`gcc_2_8_1_g0_split` (GCC 2.8.1 / MASPSX 2.81), individually relocated and
selected as real compiler objects in four complete byte-identical scratch
images. US compiler registrations were not imported. The
[attempt ledger](spanish-model-variant475-attempts.csv) preserves initial
masked comparisons separately from terminal relocated matches.

Initial ribbon calibration produced 2768 rather than 2784 bytes, with 493 masked
word differences at the actual ribbon offset. The Spanish endpoint branch uses
`k`/`k - 1`, guarded by `k == 12`, rather than literal indices 12/11. This retains
the target's secondary `ribbon + 0x30` address and produces all 2784 bytes under
the unchanged GCC 2.8.1 profile. The dedicated Spanish body preserves the shared
canonical ribbon declarations without changing other releases' sources.
Both slots were separately compiled and relocated in all four complete images.
The ledger retains the failed calibration, indexed-endpoint experiment
and terminal matches; no instruction output was patched.

Initial streamer calibration produced 2004 rather than
2072 bytes. Their heuristic minimum at `1E34` belongs to another helper and is
not a streamer match. The subsequently accepted French streamer and its
slot-one wrapper now compile unchanged to all four Spanish retail bodies.
The selected compiler objects, linked 2,072-byte extents, resident bindings
and complete images are verified with the same GCC 2.8.1 profile. Four
terminal ledger records preserve that evidence without discarding the
earlier failed experiments. This adds 8,288 matching C bytes; no direct
entry-call path or new classification of the suffix is inferred.

## Entry reuse and independent Spanish evidence

The accepted [French entry](french-model-variant475.md) and its slot-one wrapper
are reused unchanged from `src/overlays/french_model_variant/`. No regional
conditionals, copied declarations, compiler flags or patched instructions are
introduced. Both sources are freshly compiled with `gcc_2_8_1_g0_split`;
each complete 3,732-byte body is relocated and selected in two Spanish images.
The sixteen previous helper C owners remain selected. The forty earlier ledger
rows are preserved verbatim, followed by four exact calibrations and four
terminal records.

Fresh Spanish resident linking reproduces the complete retail executable.
All twenty-five entry imports resolve to actual sized function definitions in
selected resident input objects; their complete final bodies equal the Spanish
retail functions. This includes the distinct 44-byte `RotTransPers` owner,
packet constructors, model queries and texture upload. Regionally identical
overlay bytes are a lead, not a substitute for this resident-owner evidence.

Sixty-one fresh target-compiled constants check the entry's local declarations,
including the 56-byte descriptor, 484-byte quad, 740-byte ribbon, 820-byte
streamer, signed four-byte projection words and minimum `3034`-byte context.
The matrix starts at `2DE4`; target, starts, ends and deltas at
`2E04/2E0C/2E8C/2F0C`; configuration and part pointers at `2FCC/2FD4`;
slot and command at `3030/3032`. These describe accessed views, not allocation
capacity or lifetime isolation.

Fresh reads of all four Spanish physical slices reproduce their manifest
hashes. Their actual metadata request is `641000`, selecting descriptor zero
at `2E34`, with timings 20, 80, 160, 280, 460 and 560. Each entry has a closed
933-instruction CFG and a 224-byte frame. Calls at `CD4`, `D10` and `D30`
pass the original context to ribbons, quads and sheets. No strand or streamer
entry-call path is invented.

The canonical source retains both frame-step reads, direct command indexing,
opposite-slot branches and signed packed projection extraction. The 64 bytes
of stack storage preceding the projection inputs remain unrecovered local
storage; their original types are not claimed. Earlier initializer and
behavioral evidence below remains applicable to the unchanged target
instructions, but its execution counts are not presented as new entry runs.
No arbitrary-command safety, complete writer audit or whole-frame spill
lifetime proof is inferred.

## Layout, initialization and packets

Eighty-seven target-compiled constants establish the local primitive, matrix,
coordinate, packet and record layouts. Sixteen 156-byte sheets occupy
`1904..22C4`; six 132-byte strands follow at `22C4..25DC`. Each strand contains
thirteen eight-byte points and 28 opaque trailing bytes. The quad view occupies
`0..1E4`, including four eight-point arrays, shared RGB, levels and hidden words.

The sheet initializer writes four quadrants per record, outer RGB
`255,255,255`, inner RGB `0,64,255`, and zero words at record offsets
`88`, `8C`, `90` and `98`. Vector padding, color fourth bytes and opaque word
`94` remain untouched. Quad initialization runs eight iterations in one
484-byte record: corners use coordinates +/-128, shared RGB is 128, levels
are `-j * 256`, and hidden words and eight words at `194` are zero.
The origin at `1B4/1B8/1BC` is repeatedly overwritten, not an eight-record array.

The reused GT4 packet is `2CB8..2CEC`, FT4 is `2D98..2DC0`, and GsLINE is
`2DC0..2DD0`. The second GT4 constructor call initializes the sheet packet.
The quad constructor is the actual twenty-byte function at `80082EA8`, not
the unrelated `80082E48` call. Actual constructors and subsequent
`SetSemiTrans(1)` / `SetShadeTex(0)` produce GT4 length 12/code `3E` and FT4
length 9/code `2E`. UV pairs are `(64,0),(127,0),(64,63),(127,63)` for sheets
and `(0,64),(63,64),(0,127),(63,127)` for quads.

Isolated entry-instruction execution covers all four images, real SDK calls,
branch/load delays, complete context/stack guards and selected trig-table data
owners. Actual `rsin` reserves 24 stack bytes and calls `80086664`; its saved
return address occupies caller `sp-8`. The initial harness omitted that frame,
failed explicitly, and was corrected rather than treating the store as noise.

Sheet, quad and strand frames are respectively `118`, `128` and `108` bytes.
Sheet/strand projection outputs `p/flag` occupy `D0..D8`; quad outputs occupy
`E0..E8`, with a separate reserved sixteen-byte gap at `30..40`. Direct stack
stores do not overwrite these output windows. This is not a claim about every
spill lifetime or all indirect callee memory effects.

### Ribbon-specific evidence

Thirty-six additional target-constant checks confirm the 740-byte ribbon view:
thirteen-point arrays `a/sa/angle/b/sb/width` start at `0/68/9C/D0/138/16C`;
RGB is at `1A0`, count/state/length at `1C0/1C8/1CC`, and
depth/flag/x-offset/y-offset arrays at `248/27C/2B0/2CA`.
Eight records occupy `1E4..1904`; vector padding and opaque spans retain their
existing declarations.

Actual initialization sets RGB 128, count `-i * 16`, state zero and length 1024.
Its angle is `1024 + floor((i + 1) * 1024 / 9)` for odd records and
`1024 - floor(i * 1024 / 9)` for even records. This differs from the draw
helper's `1024 +/- i * 1800 / count`. Only observed stores within opaque spans
are modeled; they are not assigned speculative types. Sixteen guarded actual
entry/SDK executions and 24 rejected mutations verify all four images.
An initially symmetric angle oracle failed on the odd records and was corrected
from the actual carried-counter dataflow, with its failure retained.

The two ribbon FT4s occupy `2D20..2D70`. Their actual constructors and
`SetSemiTrans(1)` / `SetShadeTex(1)` produce length 9/code **`2F`**, not the
quad packet's `2E`. UVs are `(0,128),(0,175),(48,128),(48,175)`; the second
packet adds 48 to V. Sixteen actual packet-initializer cases and twenty
negative controls preserve all unselected packet/context/stack bytes.

The ribbon frame is `138` bytes. Actual argument views are rotation `28..30`,
scale `30..40`, matrix `40..60`, local-screen matrix `60..80`, coordinate
`80..D0`, and projection outputs `D0..D8`. All direct stack accesses are
aligned and within the frame; this is not a complete spill-liveness proof.
Eleven actual resident callees were checked against selected input objects,
86 resolved relocations and the exact resident image. The distinct
`RotTransPers` at `80087868` is 44 bytes: it reads eight bytes at its vector
argument, writes four bytes at each of the three output pointers and returns
arithmetic-shifted SZ3/4. Its GTE execution is not emulated by the host oracle.

An independent ILP32 numeric-offset oracle compares 602,112 cases, 43,051,008
SDK projection calls, complete 12,416-byte guarded contexts and every submitted
FT4. Twenty mutations are rejected. Deterministic SDK mocks are not retail
gameplay or GPU/GTE emulation, and adversarial helper inputs are not all claimed
reachable. The final projection rewrites point 11 after its earlier bend;
packed X-wave addition can carry into Y. Single-point projection depth/flags
are ignored; the four-point projection controls submission, including zero depth.

From the eight observed initial records, all frame steps **0..255** and either
retirement-phase decision close in 71 record states over 36,352 transitions.
Count remains at most twelve and length within 0..1024, keeping point indices
below thirteen and packet selection within the two FT4s. This does not require
stable getter reads or the narrower publisher-only 0..6 assumption: even the
two-read getter always returns a byte or six. The bound still assumes the
entry-published step, unchanged descriptor count eight and no external record
corruption; allocation, aliasing and lifetime completeness remain unproved.

## Callers and behavioral limits

Complete conservative CFGs cover all six function spans in every image.
Entry reaches ribbons, quads and sheets, not strand or streamers. Original
context captures survive the negative-command path; initialization overwrites
some registers but bypasses the draw calls. No strand entry caller is invented.
Actual resident callers and both context-pointer data owners were preserved.
The largest observed entry store establishes a minimum `3034`-byte view, not
allocation capacity or whole-game lifetime isolation.

The 56-byte descriptor at image offset `2E34` contains timing values
20, 80, 160, 280, 460 and 560. Quad growth divides by 60, sheet growth by 80,
and sheet shrink by 100. Quads have no explicit count clamp to eight.
Sheets mutate shared phase sequentially: one record reaching full size can
change how later records update in the same call.

Sheets/quads accept zero depth and reject negative depth or projection flags;
quads also reject hidden records. Strand ignores the projection flag and
accepts only depths 1..2047. Strand duplicates its two point/output pairs,
preserves point padding and opaque tails, and narrows span updates to signed
halfwords before threshold comparisons.

The actual frame-step getter at `8005BF24` performs two byte loads. Arbitrary
reads `0,255` return 255, so it is not an unconditional clamp. Actual raw
resident initialization sets the counter at `8009C333` to zero. The preserved
`Graphics_SyncFrame` publication block produces 1 or 2 without intervening
calls, assuming no external modification of its scratch state. If these are
the only writes, independent getter reads remain bounded; read stability is
unnecessary under that contract. A complete alias/interrupt write audit has
not been established. Conditional steps 0..6 yield 128 quad states and 23
strand states; the strand bound remains a helper contract without a demonstrated
entry path.

Independent ILP32 host oracles compare complete 12,416-byte guarded contexts
and submitted packet bytes: 326,592 quad, 570,752 strand and 516,096 sheet
cases, with 52 rejected mutations. These use deterministic SDK mocks, not
GPU/GTE emulation or retail execution. Extreme direct-helper inputs are not
claimed reachable. A sheet fixture correlation initially missed the
state-one/shown-zero branch; decoupled fixtures reject the previously surviving
initial-size mutation. Separate actual-instruction publisher probes cover
262,144 cases and retain the global writer-completeness caveat.

Fresh Spanish resident matching and archived selected inputs cover thirteen
helper callees, four packet routines, the frame getter/publisher, three
resident callers, both context pointers and the raw counter initializer.
Production overlay matching, ownership preservation, repository policy checks
and the final clean North American gate remain separate acceptance requirements.
