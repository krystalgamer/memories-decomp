# French MODEL458 sheet helper

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
| `19EC..230C` | 2,336 | Reachable helper, assembly fallback |
| `230C..5000` | 11,508 | Unclassified raw suffix |

All four functions have closed, contiguous direct-entry/local-call CFGs. Their
local call targets are `C10`, `1150`, and `19EC`; 34 distinct external calls
resolve to real French resident function starts. The new helper contributes
2,688 C instruction bytes across the two slots. Neither the remaining assembly
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
unchanged. French totals are 1,631 C instances / 1,943 functions and 2,210,084
C instruction bytes; these are inventoried totals, not proof of exhaustive
overlay coverage.
