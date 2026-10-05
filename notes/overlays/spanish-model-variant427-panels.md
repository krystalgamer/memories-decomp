# Spanish MODEL427 six-panel renderer

Independently recover the 1,368-byte helper at `+0x28A0..+0x2DF8` in both
[MODEL427 images](spanish-model-variant427.md). A fresh pre-recovery scan
of 5,853 accepted regional C registrations found no accepted normalized
shape. No French or other regional C implementation was ported.

The first retail-derived candidate was 1,376 bytes, with 264 differing
words. Its indexed primary expression created a duplicate pointer snapshot.
A context byte cursor reproduced the original lifetime and every instruction;
the second slot also compiled exactly. Both use the existing
`gcc_2_8_1_g0_split` profile, GCC 2.8.1 / MASPSX 2.81, without allocation
controls, forced registers, padding or new flags.

## Original context and bounded views

Entry calls this helper at `+0xDE4`, preparing `a0` from `s5` in the
`+0xDE8` delay slot. The only reaching `s5` definition is entry `+0xC`,
the original context. The call requires the current time to have reached
descriptor offset `+0x18` and shared phase to be below five.

| View | Offset / extent |
|---|---|
| Three 936-byte primary records | `+0x0ED0..+0x19C8`; signed progress at record `+0x240` |
| Six 152-byte sheets | `+0x1FF8..+0x2388` |
| One reused, initialized GT4 | `+0x3594..+0x35C8` |
| Three matrices | `+0x3628..+0x3688` |
| Three padded word-vector directions | `+0x36F0..+0x3720` |
| Frame / unsigned time / step | `+0x373C / +0x3740 / +0x3748` |
| G32 descriptor / shared phase | `+0x3750 / +0x37C4` |

The shared `ModelVariantSheet` has four four-point arrays, outer and inner
RGBA, and signed size. Initialization proves six such records and configures
the actual 52-byte packet used here.

The primary cursor advances through context bytes, reaching `+0x24C0`
after six iterations. Only iterations zero through two interpret it as a
primary record. This avoids advancing a typed pointer beyond the proven
three-record array. The `0x37C8` state remains a partial view, not an
allocation-capacity claim, and fits before both corresponding load banks
at the verified resident context addresses.

## Rendering and shared timing

All six sheets are projected, including zero or negative sizes.
Odd frame parity adds signed `size / 8` to the uniform scale.
For the first three sheets, signed direction times primary progress,
divided by 1,024 with truncation, is added to the matching matrix
translation. The other three use the corresponding unmodified translation.
Rotation is zero.

Four quads per sheet reuse one GT4. Its first three corners use inner RGB,
and its fourth uses outer RGB. Depth is multiplied by eight and divided
by ten with signed truncation before testing nonnegative depth and flag.
The first three sheets also require nonnegative primary progress to sort.
The sorting priority narrows to sixteen bits.

For the first three sheets, phase two sets size to 4,096. At phase three,
size grows by `step * 512` toward 8,192; reaching that bound sets phase four.
For the other three, phase zero interpolates size toward 4,096 from
unsigned descriptor times and sets phase one on reaching that bound.
These shared phase writes affect subsequent loop iterations.

Both groups shrink once the current time reaches the descriptor's shrink
start, using unsigned arithmetic and division, then clamp nonpositive
signed results to zero. The measured descriptor timing words are
20, 100, 160, 360 and 420 at `+0x18..+0x28`; the growth and shrink
denominators are therefore 80 and 60 for this row.
Original unsigned wrap behavior and divide-by-zero trap instructions are
preserved, not replaced with new guards or a generic interpolation helper.

## Preservation and proof

This adds two C instances / 2,736 instruction bytes without adding images
or function boundaries. Both complete 20KiB images match. Accepted curtain
sources, headers, wrappers, linker bindings and the entire manifest are
unchanged; all other assembly and raw owners remain intact.

Four added regressions verify 36 target-compiled layout constants,
original-context provenance, initialized records and packet bounds,
descriptor timing and unsigned division instructions, selected C ownership,
ten external calls plus three local jumps per object, and terminal source
and transitive-header fingerprints. The existing family regressions cover
all input/final C/assembly/raw owners and actual resident function and
context-storage owners.

The [attempt ledger](spanish-model-variant427-panels-attempts.csv) preserves
three scratch comparisons and two complete-image terminal records.
