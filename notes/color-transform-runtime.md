# Color Transform Runtime

## Scope

Five matching helpers at `0x8005A98C-0x8005B260` form one fixed-point colour
conversion and tint translation unit in `src/game/color_transform.c`.
`Color_HslToBgr555Pixel` produces the same object under its historical
`gcc_2_8_1_g0` profile and the unit's `gcc_2_8_1_g8` profile, so the recorded
profile difference is not a real boundary.

| Function | Input | Output | Established role |
|---|---|---|---|
| `Color_RgbToHsl` (`0x8005A98C`) | three channels plus their maximum value | `{h, lightness, saturation}` | RGB to fixed-point HSL |
| `Color_HslToRgb` (`0x8005ABA0`) | fixed-point HSL plus a channel maximum | three channels | fixed-point HSL to RGB |
| `Color_TintBgr555Pixel` (`0x8005AE68`) | one BGR555 colour, flags, scale | one BGR555 colour | hue/saturation transform preserving bit `0x8000` |
| `Color_HslToBgr555Pixel` | fixed-point HSL | one BGR555 colour | direct HSL-to-BGR555 packing |
| `Color_TintRgb` | three channels, flags, scale, maximum | three channels | the same transform on an unpacked RGB triple |

The shared game type remains named `HsvT`, but the formulas are HSL, not HSV.
`Color` and `HsvT` are defined once in `src/ygo_types.h`; `color.h` now owns
only this translation unit's function interfaces and constants. Moving the
unchanged layouts does not alter the code-generation-sensitive arithmetic.

## Fixed-point HSL representation

`src/game/color_constants.h` names the 12-bit fractional shift as
`COLOR_FIXED_SHIFT`, one fixed-point unit as `COLOR_FIXED_ONE` (`0x1000`),
and the midpoint/rounding bias as `COLOR_FIXED_HALF` (`0x800`). Hue is
measured in fixed-point RGB-edge units: `COLOR_HUE_SECTOR_COUNT` is six,
so `COLOR_HUE_FULL_TURN` is `0x6000`, not the `0x1000` trigonometric turn.
The conversion helpers, tint wrappers, and VRAM fast-path scale test share
these constants without changing their local types or arithmetic order.

`Color_RgbToHsl` first finds the maximum and minimum input channels. With
`limit` equal to the maximum channel value, it produces:

```text
lightness = (maximum + minimum) * 0x1000 / (2 * limit)
```

When all channels are equal, hue and saturation are zero. Otherwise:

```text
delta = maximum - minimum

if lightness <= 0x800:
    saturation = delta * 0x1000 / (maximum + minimum)
else:
    saturation = delta * 0x1000 / (2 * limit - maximum - minimum)
```

Hue uses a full turn of `0x6000`, with `0x1000` per RGB edge:

| Hue | Colour |
|---:|---|
| `0x0000` | red |
| `0x1000` | yellow |
| `0x2000` | green |
| `0x3000` | cyan |
| `0x4000` | blue |
| `0x5000` | magenta |

The maximum channel selects the base at `0x0000`, `0x2000`, or `0x4000`.
The difference between the other two channels supplies the signed position
within that two-edge span; negative red-sector values wrap by adding
`0x6000`.

`Color_HslToRgb` performs the inverse calculation. It derives the minimum and
maximum channel magnitudes from lightness and saturation, interpolates the
third channel across the selected hue edge, rounds each 12-bit fixed-point
result with `+0x800`, and clamps it to `0xFF`. Evaluating the two matching
integer formulas across all `32 * 32 * 32` BGR555 channel triples reproduces
every input channel exactly.

The rebuilt channel pair is the textbook HSL pair. Below lightness `0x800`
the matching body computes `minimum = lightness * (0x1000 - saturation)` and
`maximum = 2 * lightness - minimum`, which is `p = L * (1 - S)` and
`q = L * (1 + S)`; above it the body computes
`maximum = lightness * (0x1000 - saturation) + saturation` and
`minimum = 2 * lightness - maximum`, which is `q = L + S - L * S`. Comparing
the matching integer formula against floating-point HSL-to-RGB with
`hue = h / 0x6000`, `L = s / 0x1000` and `S = v / 0x1000` over 418176
`(hue, lightness, saturation)` samples at channel maximum `255` leaves a
worst per-channel deviation of `0.5`, which is the `+0x800` rounding alone.

## Tint controls

`Color_TintBgr555Pixel` and `Color_TintRgb` apply the same controls after converting
to HSL:

| Control | Effect |
|---|---|
| `flags & COLOR_TINT_HUE_MASK` (`7`), values `0-5` | Replace the hue with the corresponding primary/secondary sector. |
| Hue selector `COLOR_TINT_GRAYSCALE` (`6`) | Replace the hue and set saturation to zero, producing greyscale. |
| Hue selector `COLOR_TINT_KEEP_HUE` (`7`) | Retain the source hue. |
| `flags & COLOR_TINT_INVERT` (`8`) | Rotate a forced hue by three sectors, then subtract every converted channel from the channel maximum. |
| `scale` | Multiply saturation by `scale / 0x1000`; it does not scale brightness. |

Both wrappers replace a zero output channel with one. The BGR555 wrapper has
two additional rules: input zero returns zero immediately, and bit `0x8000`
is copied unchanged from the input colour to the repacked result.

### Measured over every BGR555 input

`Color_TintBgr555Pixel` takes a 16-bit argument, so the whole input domain
fits in one sweep. Modelling the matching body over all `0x10000` halfwords
settles what each control does, rather than inferring it from the arithmetic:

| Claim | Result |
|---|---|
| Forced sectors `0-5` each leave one hue family dominant | Sector 0 leaves red dominant, 1 red and green, 2 green, 3 green and blue, 4 blue, 5 red and blue. The only other outcome is all three equal, which is the zero-saturation case. |
| `COLOR_TINT_GRAYSCALE` emits a grey | All three output channels are equal for every input. A zero `scale` does the same under any hue selector. |
| `COLOR_TINT_INVERT` is a complement | Before the zero clamp, `flags = f \| 8` is exactly the three-sector rotation of the same colour with every channel subtracted from `31`. The rotation is what cancels the complement's own three-sector hue shift, which is why `f` and `f \| 8` leave the same channels dominant. |
| Bit `0x8000` is copied, not computed | Holds for every input under flags `0`, `3`, `6`, `7`, `0xE` and `0xF`. |
| `scale` only removes saturation | The spread between the brightest and dimmest output channel never falls as `scale` rises through `0`, `0x400`, `0x800`, `0xC00` and `0x1000`. |

`Color_HslToBgr555Pixel` is the direct packing counterpart: it converts one fixed-point
HSL triple with channel maximum `31` and packs the resulting channels into
BGR555 bits `0-14`.

`color_constants.h` names the five-bit channel width and derives the channel
mask (`31`), green shift (`5`), blue shift (`10`), and STP mask (`0x8000`).
Red occupies bits `0-4`, green bits `5-9`, and blue bits `10-14`. The channel
mask is also the maximum passed to HSL conversion. Both packing paths use
these constants, but only `Color_TintBgr555Pixel` copies the input STP bit; the direct
packer has no input colour from which to preserve it.

## VRAM application

Matching `Model_ApplyTextureTint` applies the BGR555 transform to VRAM colour bands.
For buffer selector `0` or `1`, it processes two `256 x 4` rectangles:

1. Read from x `selector * 256`, y `248` and `252` through `StoreImage2`.
2. Transform all `0x400` halfwords in each rectangle with `Color_TintBgr555Pixel`.
3. Upload the results to the same x coordinate at y `240` and `244` through
   `LoadImage2`.

When the full flags argument is at least `COLOR_TINT_KEEP_HUE` (`7`),
`COLOR_TINT_INVERT` (bit `3`) is clear, and the saturation scale is at least
`COLOR_FIXED_ONE` (`0x1000`), the function skips per-colour conversion and
moves the complete `256 x 8` source band from y `248` to y `240` with
`MoveImage`. The first comparison deliberately remains against the full
argument, not `flags & COLOR_TINT_HUE_MASK`; the two tests are not equivalent
when higher flag bits are set.

That fast path is not bit-identical to the slow one. At `flags = 7` and
`scale = 0x1000` the per-pixel transform returns its input with every zero
channel raised to one, which the sweep above confirms for all `0xFFFF`
non-zero halfwords, and that changes 5953 of them. `MoveImage` copies those
pixels unchanged instead. The retail code accepts the difference; it is not a
claim that the two paths agree.

The coordinates above describe the North American/Japanese path.
Europe instead uses x `0x280` and destination base
`(side_index << 4) + 0xD0`. Its two four-row bands read at base+8/base+12
and upload at base/base+4; its fast path moves the complete eight-row
source band from base+8 to base. These are transfer layouts, not a new
claim about the user-facing ownership of either band.

## Card-image readback

Matching `func_8001944C` has a separate readback path: `StoreImage2` reads a
rectangle into `D_8015C424`, a loop sets `COLOR_BGR555_STP_MASK` on every
halfword, twelve corner pixels are cleared to 0 (the three at each corner:
(0, 0), (0, 1), (1, 0) and their mirrors at columns 138/139 and rows
194/195), and `LoadImage2` uploads the block.
`duel_display.h` names its width as `DUEL_CARD_READBACK_WIDTH_WORDS` (`140`)
and height as `DUEL_CARD_READBACK_HEIGHT` (`196`). Their product,
`DUEL_CARD_READBACK_WORD_COUNT`, is `0x6B30` halfwords (`0xD660` bytes).
Both transfer rectangles and the processing loop use this same extent.

These are VRAM transfer dimensions, not the encoded card-art record's image
dimensions. The buffer's separate `+0x8000` address bias is not the STP bit
despite its equal value. The fixed VRAM coordinates, individually addressed
clears, source statement order, and allocation-only register pins remain
unchanged.

## Evidence boundary

The formulas, flag meanings, channel packing, zero handling, rectangle
coordinates, and transfer sizes come from local matching C. The user-facing
purpose of each transformed VRAM band and the original Konami names remain
unassigned. `Color_RgbToHsl` is named for the conversion its formulas
establish, not for an original name. The other descriptive helper names
are likewise recovered roles, not original Konami names.

## Conversion, tint and regional transfer input/local views

Five complete colour definitions and BOTH regional Model_ApplyTextureTint
definitions now name their existing arguments/locals. HsvT, its s/v member
names, every shared type and all public prototypes remain untouched:
the established HSL interpretation is still lightness in s and saturation
in v. No function/global/member identity promotion or semantic-ledger edit.

RGB conversion distinguishes channel values/indices, extrema, their sum,
twice_channel_limit and lightness. channel_delta_work STILL holds the
minimum before becoming maximum-minus-minimum; hue_work holds a scaled
channel-difference numerator before the complete signed hue result.
Neither binding is split. Keep the low-16-bit lightness branch test,
denominator choices, signed division/remainder, negative-hue adjustment,
original temporary aggregate copy and every declaration/lifetime.

Inverse conversion names hue/lightness/saturation and maximum/minimum/
interpolated channel indices. channels_fixed remains the original UNSIGNED
array; edge_position/edge_weight retain signed arithmetic and the original
conditional reflection. Keep the two lightness arms, multiply/divide order,
rounding and 0xFF caps. There is no new input-range validation, zero-limit
guard, wider arithmetic, overflow guarantee or repaired negative-hue path.

Packed and unpacked tint paths share tint_control/saturation_scale,
hsl_state/output_rgb, hue_selector/invert_mask/grayscale_request and
forced_hue names without unifying their storage or call signatures.
The packed path retains input-zero early return, exact channel extraction,
its separate local channel_limit, direct real conversion calls, zero-channel
clamps, packing_rgb copy and input STP-bit preservation. The unpacked path
retains Color_RgbToHsl_void and Color_HslToRgb_wide caller views, its input
limit and output-pointer copy/return. Inversion and grayscale/control order
stay. The direct packer keeps full-width lightness_word/saturation_word
parameters, BOTH explicit low-halfword masks and the widened caller view.
Do not silently replace any alias call with the definition's real prototype.

Regional VRAM definitions distinguish side_index, tint_control,
saturation_scale, pixel_buffer/pixel_cursor/pixel_index and band_offset;
Europe additionally names destination_base_y and its block-local
source_row_offset. Two existing European-comment identifier references
follow the argument; no new C commentary. Both complete original bodies
remain, including stack-buffer extent, all SDK polling loops, transfer
rectangle stores, two four-row passes and the separate eight-row fast move.

The unsigned side_index<2 guard still rejects every other integer bit
pattern. Fast-path eligibility still examines FULL signed tint_control and
saturation_scale BEFORE the slow call's u8/u16 casts. Do not mask the
initial comparison, normalize controls, move casts or equate fast and slow
paths: the existing zero-channel-clamp difference remains. VRAM coordinates
are regional, while colour conversion remains the same registered unit.

Scoped bare-identifier and existing-comment inverses restore each complete
original C file byte-for-byte, including untouched grouped definitions,
inline helpers, caller-view declarations, macro arms and private data.
All headers, qualified members, types/qualifiers, declaration order/lifetimes,
casts/operators/constants/labels/calls, profiles, storage, callers and the
ENTIRE semantic ledger remain. Existing measured sweeps above are historical
research, not newly executed behavioral evidence. No fixture/control/oracle/
mutation changes, retired-fixture revival, local cloud tests or new
synthetic/generated-C/source-string behavioral harness accompany these views.
