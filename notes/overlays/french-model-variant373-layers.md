# French MODEL373 five-layer helper

The directly entry-called `func_8013C0B0` / `func_8017C0B0` at image
`+0x10B0` owns 1,724 bytes in MODEL707 stages7/8, retail headers373/523.
The source and slot wrapper use the existing `gcc_2_8_1_g0_split` profile,
GCC2.8.1 and MASPSX2.81. All six earlier C helpers remain unchanged.

## Independently recovered layout and behavior

Five `0x1AC`-byte records begin at context`0x4EC`. Each contains51 `SVECTOR`
points in three consecutive seventeen-point rows, rotation at`+0x198`,
signed scale at`+0x1A0`, and a completion word at`+0x1A8`.
The helper reuses sixteen GT4 packets at`0x1E60` for every layer.
The partial context view ends at`0x2488`; it does not establish allocation
capacity or interpret the opaque gaps.

Positive-scale records retain the unused sine call. The unused active-buffer
query and both initial bearings are also preserved. Before phase2, translation
is the root origin with Y reduced by scale/12, and scale1024 is modified by
the sine pulse only over the inclusive1024..5120 range. Later phases use
the record rotation, target position and record scale.

Light `(192,192,192)` and dark `(192,128,64)` fade after scale5120.
Each of sixteen adjacent point pairs from the first two rows projects to a
GT4 with descending eight-pixel V coordinates. Light colors belong to
vertices0/1 and dark colors to2/3. Sorting requires nonnegative signed depth
and flag and passes only the low16 depth bits. The third row is regenerated
but not projected by this helper.

Before phase2, scale grows by `step*192` and wraps once at6144, except that
crossing after the strict unsigned configuration deadline clamps it and marks
completion. The product of all five completion words advances phase1 to2.
Phase2 regenerates the three rows with radii384/576/768 and Y0/16/16, then
sets each record's stagger to `-i*6144/5`. The unsigned radius expressions
preserve the observed logical shifts before narrowing to halfwords.
Later phases grow by `step*128`; wrapping also adds384/800 to rotation X/Z,
with scale clamped at6144 from phase5.

## Exact recovery

The accepted local MODEL373 points/grid supplied declaration and API structure;
record offsets, unsigned arithmetic, timing and projection accesses were
recovered from this helper's complete431-instruction body. No external
reference declarations or compiler claims were used.

The measured312-byte frame includes an untouched24-byte gap between the
coordinate and color locals. Explicit texture/bearing cursors preserve their
native setup. The positive vertex-byte cursor forms its offsets before adding
the layer base; reassociating those expressions loses two address instructions.
Both vertex and texture cursors are derived from the updated index. Their
source update order preserves the final four setup/update instructions.
These are real address and texture calculations, not artificial dependencies.

The [attempt ledger](french-model-variant373-layers-attempts.csv) preserves
all seven measured candidates and both production image terminals.
Only the new layer registrations, views, wrapper and directly related notes
change. The original entry and raw suffix remain unchanged.

Both complete20KiB production images reproduce the independent hashes in the
[instance ledger](french-model-variant373-instances.csv), with the original
1724-byte `STT_FUNC` owners at`0x8013C0B0`/`0x8017C0B0` and no assembly
fallback for these helpers. This adds3,448 C instruction bytes across both
images. That checkpoint left seven C functions per image and the entry in
assembly; the subsequent [entry recovery](french-model-variant373-entry.md)
completes all eight closed functions without changing these helpers.
