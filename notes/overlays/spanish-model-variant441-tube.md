# Spanish MODEL441 tubular mesh

`func_8013C73C` / `func_8017C73C`, `+173C..+1DDC`, is a 1,696-byte independently
recovered tubular-mesh helper in the ten MODEL441 images listed in the instance
CSV. A fresh screen of 6,009 configured regional C entries found no same-size
body. No regional C was ported. The existing GCC 2.8.1 / MASPSX 2.81
`gcc_2_8_1_g0_split` profile matches both slots and every complete 20 KiB image.
After updating to accepted master, all eight newly accepted French MODEL76
entries were also checked: each is 2,964 bytes, not this 1,696-byte helper.

This adds ten C instances / 16,960 bytes while preserving the separately
recovered retained line helper. Together they contribute twenty C instances /
31,040 instruction bytes, representing two unique routines; seventy physical
functions remain ASM. Headers, raw tails, all existing bindings and line-source
fingerprints remain unchanged. Two trig aliases preserve all 37 resident
addresses and original names.

The subsequent [framebuffer-ring recovery](spanish-model-variant441-framebuffer-rings.md)
preserves this tube source and adds a third C helper per image, leaving sixty
physical ASM functions.

## Original context and packet ownership

Unlike the retained line helper, this routine has a recovered entry call:
`+1014`, inside the descriptor iteration loop when phase is positive.
Reaching-definition analysis proves the original context captured in `s2` at
`+C` supplies `a0` in the delay slot. Initialization uses `s2` for other work,
but those definitions do not reach this render call.

The helper captures that context in `s4` at `+1744`, and its stable `s1` packet
pointer is context `+2548`. Entry `+68/+6C` stores this pointer in stack slot
`A0`; no other entry store replaces it. Entry `+250` calls `SetPolyG4` with
that pointer, and `+25C` enables semitransparency. The 36-byte packet ends at
`256C`, immediately before the next packet.

Projection writes packed coordinate pairs at packet offsets `8/16/24/32`.
RGB stores occupy `4..6`, `12..14`, `20..22`, `28..30`. Submission calls the
existing packet-copy helper through its local `u32 *` API, with narrowed `u16`
depth and flags one, only when both depth and GTE flag are nonnegative.

## Private layout and geometry

Forty-one target-compiled constants cover all accessed layouts and packet
offsets. The private context view ends at `2720`, not necessarily the full
context. One `54C`-byte group at `5AC` ends at `AF8`, where line groups begin.
It contains a `[9][9]` point grid (row stride `48`, extent `288`), opaque bytes
`288..510`, and nine colors at `510`. The opaque region is not assigned a
guessed second-bank role.

The modulation view starts at `10DC`, with an accessed signed value at `+88`
(`1164`). Origin is `2690`; the path vector is `26A4`; projected direction is
`26B4`; another direction vector is `26B8`. Signed time/step are `26D4/26DC`,
the descriptor pointer is `26E4`, and signed width/progress/angle/phase occupy
`2704/2708/270C/271C`.

The four `ratan2` calls remain. Only the third result is used: the path's
`z/y` angle plus 3072. Radius is width divided by 16, additionally multiplied
by the modulation value and divided by 8192 when phase is at least three,
then narrowed to signed sixteen bits.

Nine rows interpolate along the path using `direction*progress/1024*j/8`.
Each row contains nine samples at 512-angle-unit steps, including its closing
sample; each next row advances the starting angle by another 512. The radial
terms retain their nested fixed-point shifts and trig evaluation order.
The resulting eight-by-eight cells submit sixty-four candidate Gouraud quads,
with colors indexed around the ring, not along the path.

Transformation uses zero rotation and origin translation. Retail also writes
three 4096 values to an unused `VECTOR` at stack `30..3C`; those original
assignments are retained, without adding a `ScaleMatrix` call or artificial
padding. The natural frame is 296 bytes.

## Signed timing and state transitions

The entry's descriptor table is `base+4850`, stride 48, selected by the command
remainder. All actual descriptors request one iteration:

| Model | Expansion start/end | Fade start/end |
|---|---|---|
| 166 | 76 / 88 | 180 / 200 |
| 360 | 168 / 180 | 360 / 400 |
| 487 | 128 / 140 | 360 / 430 |
| 590 | 76 / 88 | 180 / 200 |
| 709 | 84 / 96 | 400 / 460 |

These signed fields are at `20/24/28/2C`; all actual denominators are positive.
During phase one, progress below 1024 starts expanding at the first time,
using `(time-start)*1024/(end-start)`. Crossing 1024 clamps it and advances
phase to two. Positive width fades after its start using
`1024-(time-start)*1024/(end-start)`. Crossing zero clamps it, and changes
phase three to four. Each invocation subtracts `step*128` from the angle.
Signed division trap sequences and narrowing remain exact.

## Recovery and verification

The first reconstruction was 1,696 bytes with 125 differing words. Conditional
modulation-view lifetime reduced that to 121; observed loop-control angle
progression reduced it to twenty. Introducing a row local was rejected because
it changed the frame and register ownership. Natural `(points[j]+k)->field`
expressions instead preserve row-address grouping without another local and
recover every instruction.

Seventeen ledger rows retain all experiments and ten whole-image terminals.
One diagnostic run labeled slot one accidentally used the runner's slot-zero
default; it is explicitly recorded as a repeated slot-zero check. The subsequent
explicit slot-one compilation provides the actual independent proof.

Regressions verify all physical commands/timings, original-context flow, packet
initialization and bounds, coexisting/repeated private headers, forty-one
layouts, all selected/retained input and final owners, fifteen call relocations
to nine resident addresses, and the one local jump. The retained line helper
still has no recovered runtime caller; adding this live mesh does not alter
that limitation or add a dispatch.
