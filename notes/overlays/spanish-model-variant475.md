# Spanish MODEL headers 475 and 625

Four complete ten-sector images contain three independently matched helpers:
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
| `4` | 3732 | Entry assembly |
| `E98` | 2784 | Ribbon assembly |
| `1978` | 1212 | Sheet C |
| `1E34` | 940 | Quad C |
| `21E0` | 832 | Strand C |
| `2520` | 2072 | Streamer assembly |
| `2D38` | 8904 | Unclassified raw suffix |

The four images contribute twelve C instances / 11,936 instruction bytes,
twelve assembly instances / 34,352 instruction bytes, and 35,632 raw bytes.
Unknown suffix storage is not counted as code or matching C.

The six Spanish wrappers only rename symbols in the accepted local US458
sheet, quad and strand bodies. All were independently compiled with
`gcc_2_8_1_g0_split` (GCC 2.8.1 / MASPSX 2.81), individually relocated and
selected as real compiler objects in four complete byte-identical scratch
images. US compiler registrations were not imported. The
[attempt ledger](spanish-model-variant475-attempts.csv) preserves initial
masked comparisons separately from terminal relocated matches.

Ribbon calibration produced 2768 rather than 2784 bytes, with 493 masked word
differences at the actual ribbon offset. Streamers produced 2004 rather than
2072 bytes. Their heuristic minimum at `1E34` belongs to another helper and is
not a streamer match. Both remain assembly.

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
