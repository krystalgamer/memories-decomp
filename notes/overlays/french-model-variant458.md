# French MODEL458 sheet and streamer helpers

MODEL202 stages 7/8 load headers 458/608 from sectors 55932/55942. The packed
command is 624000, selecting descriptor zero and entry argument zero. The two
20,480-byte images load at `0x8013B000` / `0x8017B000`; their independent hashes
are recorded in [the instance inventory](french-model-variant458-instances.csv).

| Image offset | Bytes | Ownership |
|---|---:|---|
| `0..4` | 4 | Raw module header |
| `4..C10` | 3,084 | Entry, assembly fallback |
| `C10..1150` | 1,344 | Matching C sheet helper |
| `1150..19EC` | 2,204 | Reachable helper, assembly fallback |
| `19EC..230C` | 2,336 | Matching C streamer helper |
| `230C..5000` | 11,508 | Unclassified raw suffix |

All four functions have closed, contiguous direct-entry/local-call CFGs. Their
local call targets are `C10`, `1150`, and `19EC`; 34 distinct external calls
resolve to real French resident function starts. The two helpers contribute
7,360 C instruction bytes across the two slots. Neither the remaining assembly
nor the suffix is claimed as recovered C or excluded SDK code. The descriptor
at image offset `2408` is evidence inside the suffix, not a classification of
the entire suffix.

## Locally recovered layout and behavior

The helper receives the original entry context through the call at `AB8`.
Entry calls to `GsGetLw` at `D8` and `854`, the 32-byte matrix stride, and the
seven-iteration bounds establish seven world matrices at context `1C60`.
Entry stores at `91C..94C` establish the displacement vectors at `1E20`, with
16-byte stride. The destinations at `1D40` are populated by that same entry.

Seven 136-byte sheet records begin at `C00`: sixteen `SVECTOR` points and two
four-byte colors. Four calls to `RotTransPers4` per sheet reuse the second
`POLY_GT4` packet at `1B54`. The entry initializes exactly the two declared
packets using `SetPolyGT4` at `1F0` and `244`; the following `D0` bytes remain
unclassified in the helper view, rather than being guessed as more packets.

The view preserves the two initial `ratan2` calls even though their results are
unused. It interpolates each matrix translation with its signed displacement
and shared factor, then scales the sheet with a shared size and optional
one-eighth pulse. Depth is multiplied by eight and divided by ten before its
nonnegative check; projection flags retain their signed test. The color and
packet reuse order is unchanged.

The descriptor is 40 bytes, with signed timing words at offsets 20, 24, 28,
32, and 36: `52, 100, 110, 200, 370` in both images. The phase chain preserves
the compound phase/time conditions:
phase 0 grows size to 2048, phase 1 advances displacement factor to 1024,
phase 2 grows size to 8192, and phase 4 shrinks it to zero and selects phase 7.
There is no added guard against zero timing duration or signed overflow.
The `G32` timing link preserves the retail guest-address representation; this
is not a claim of arbitrary native-pointer safety.

The measured helper prefix ends at context offset `1F80`, below the primary
loader window beginning at offset `4000`. This is a bounded helper-view
separation check, not a full context allocation or lifetime proof; the entry
itself accesses later fields. The 16 unaccessed stack bytes between the
80-byte coordinate object and projection outputs are retained without
assigning them a guessed semantic type.

## Reconstruction and acceptance

The [attempt ledger](french-model-variant458-attempts.csv) records source
experiments and tooling failures separately. An initial borrowed linker file
omitted four SDK bindings. The first linked candidate was 1,312 bytes/frame272,
with 323 differing words per slot: its view gap was miscalculated by `400`,
its stack gap was absent, and its phase tests were nested rather than compound.
A layout preflight then exposed a probe-reader bug: old GCC emits the constant
array with a zero-sized `STT_NOTYPE` symbol. Reading its actual section
confirmed the measured layout.

Corrected layouts, stack gap, pulse diamond, and compound phase conditions
produced 1,340 bytes/frame288 with 41 differing words per slot. Only the final
fade comparison's operand load order remained causally different. Expressing
`fade_start <= elapsed` recovered the load delay and all 1,344 bytes in both
slots. Narrowing the canonical packet declaration to the two caller-confirmed
packets preserved that exact result.

Both canonical sources use `gcc_2_8_1_g0_split` with GCC 2.8.1/MASPSX 2.81.
The slot-1 wrapper only renames the verified function. The complete French
resident and all 317 configured overlay images match byte-for-byte after
integration with the accepted MODEL450 changes. All 12 sized C/assembly/raw
owners across the two new images and 37 resident loader/controller/callee
owners were checked independently. The 315 accepted image entries are
unchanged. That sheet-only checkpoint had 1,631 C instances / 1,943 functions and 2,210,084
C instruction bytes; these are inventoried totals, not proof of exhaustive
overlay coverage.

## Streamer helper

The separately recovered helper at `19EC` is called at entry offset `A90`,
with the original context reloaded at `A8C` and a nop delay slot at `A94`.
It constructs three 628-byte records at context `106C`, each containing
thirteen spine points, projected words, angles, displaced points, projected
widths, colors, depths, and halfword offsets. Entry offsets `698..6D4`
independently confirm all thirteen four-byte color records at record `1D4`,
the three-record bound, and the `274` stride. Unaccessed gaps remain raw.

The helper builds each coil using a 96-unit radius, signed `length / 64`,
two evolving angles and a 125-unit point twist. Its flag-dependent rotation
and phase selection retain signed `% 4` and `% 10` behavior. The first pass
is followed by seven transform/project/draw passes, using the same measured
world matrices, displacement vectors, factor and scale as the sheet helper.
Local projection flags are three rows of thirteen words, not record fields.
The endpoint projects points 11/12; each other point projects itself and its
successor. Width becomes two when the shared length exceeds 512; otherwise
the projected horizontal difference supplies it. Both depth and projection
flags must be nonnegative before submission.

All twelve segments of each streamer reuse the first `POLY_FT4` at `1BD8`.
The two-packet declaration is independently supported by entry `340/344`,
the 40-byte increment at `3A0`, and the second initialization at `3A4/3A8`.
The view ends at `1F74`; this remains a partial helper view, not an allocation
or lifetime proof. It neither changes nor reclassifies the raw suffix.

Accepted streamer implementations supplied control-flow prior art, not a
same-sized byte donor or substituted layout. The first paired candidate was
2,324 bytes/frame496, with 553 differing words per slot. Separating selected
phase from advancing spin and reach from radius recovered their lifetimes.
Ordering the transform index before the radius recovered the measured stack
slots and frame496. A bottom-tested loop was unchanged, while the existing
no-CSE-follow-jumps profile regressed. Keeping offset recovery inside both
projection arms allowed old GCC to merge the common tail with the original
back-edge instruction and scratch reloads: 2,336 bytes, zero differing words
in both slots. Canonical source/header/wrapper recompilation retained the
exact result under `gcc_2_8_1_g0_split`.

The first production link exposed a metadata omission: the sheet-only symbol
files still used address aliases for `rsin`, `rcos`, and `RotTransPers`.
The standalone probe already used their independently verified SDK addresses.
Replacing those three local aliases in the linker file and both Splat symbol
files preserves the same 34 resident targets and allows the unchanged exact C
to link in production. The failed slot-0 gate is retained in the ledger.

The streamer integration adds 4,672 C instruction bytes without changing any
of the 317 image registrations. The complete resident and all 317 configured
overlays remain byte-identical; all 12 image owners and 37 resident
loader/controller/callee owners are independently checked. French totals are
1,633 C instances / 1,943 functions and 2,214,756 C instruction bytes.
The entry and nine-point ribbon helper remain assembly in both slots, and
both 11,508-byte suffixes remain unclassified.

## Strand helper `1150`

The `0x89C` helper between the sheets and streamers is first-ever game code:
no release had matching C for its body. Both stage images (`8013C150` and
`8017C150`) now compile from `variant458_strands.c` and its slot1 wrapper.

Six `0x200`-byte strand records start at work offset `0`. Each holds nine
points (`a`, projected `sa`, `angle`, offset points `b`, projected `sb`,
`width`), one colour at `120`, depths at `1A8`, signed screen offsets at
`1CC/1DE`, and a growing visible count at `1F4`. The view also measures two
`POLY_FT4` packets at `1BD8`, six transform origins at `1D40`, the `size`
word at `1E08`, six per-strand directions at `1E90`, the shared axis words,
`speed` at `1F18`, `spin` at `1F5C`, two phase words at `1F74/1F78`, and the
state word at `1F7C`. Unaccessed gaps remain raw.

The first pass places point `k` at `direction * k / 8` with an offset point
along `ratan2(axis_z, axis_x) + 3072`, projects each point with the strand
origin, and derives angle, width and screen offsets in the MODEL400 ribbon
shape (point 8 pairs with point 7). Two `rsin` results are discarded by the
original (alternating-parity sway and a phase advancing by 1300), and the
projected word receives a packed sine displacement. The draw pass submits
up to `count` quads, alternating the two packets with the MODEL400
`(s16)(k % 2) == 1` increment, through `func_8005B260` with depth and flag
checks. State 3 grows each count by `speed` up to 8; the last strand moves
the state to 4. Both phases and `spin` advance afterwards.

The packet sort call at `0x8004D5B8` was named only by a local address alias.
Its independently verified resident name `func_8005B260` (declared in
`gpu_packets.h`) replaces that alias in the shared MODEL458 linker file and in
both Splat symbol files; the resident binding set is otherwise unchanged.

Attempts kept the 520-byte frame from the first typed view. A separate sway
variable fixed the discarded first argument; the MODEL400 shape fixed the
draw pass. The remaining difference was loop-invariant motion: the target
keeps the flag-row base inline and hoists `strand + 32` for point 8. A
word-sized row index for the origin transform and projection-flag row gives
the exact movable decisions: 2,204 bytes, zero differing words in both slots.
The clean French overlay gate keeps every configured image byte-identical.
French overlay totals are 3,960 C instances and 5,087,724 C bytes.
