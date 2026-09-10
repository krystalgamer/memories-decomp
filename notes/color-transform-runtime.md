# Color Transform Runtime

## Scope

Five matching helpers at `0x8005A98C-0x8005B260` form one fixed-point colour
conversion and tint translation unit in `src/game/color_transform.c`.
`func_8005B054` produces the same object under its historical
`gcc_2_8_1_g0` profile and the unit's `gcc_2_8_1_g8` profile, so the recorded
profile difference is not a real boundary.

| Function | Input | Output | Established role |
|---|---|---|---|
| `func_8005A98C` | three channels plus their maximum value | `{h, lightness, saturation}` | RGB to fixed-point HSL |
| `func_8005ABA0` | fixed-point HSL plus a channel maximum | three channels | fixed-point HSL to RGB |
| `func_8005AE68` | one BGR555 colour, flags, scale | one BGR555 colour | hue/saturation transform preserving bit `0x8000` |
| `func_8005B054` | fixed-point HSL | one BGR555 colour | direct HSL-to-BGR555 packing |
| `func_8005B0B4` | three channels, flags, scale, maximum | three channels | the same transform on an unpacked RGB triple |

The local source calls the intermediate type `HsvT`, but the formulas are
HSL, not HSV. This note uses the mathematical field roles while leaving the
code-generation-sensitive local declarations unchanged.

## Fixed-point HSL representation

`src/game/color_constants.h` names the 12-bit fractional shift as
`COLOR_FIXED_SHIFT`, one fixed-point unit as `COLOR_FIXED_ONE` (`0x1000`),
and the midpoint/rounding bias as `COLOR_FIXED_HALF` (`0x800`). Hue is
measured in fixed-point RGB-edge units: `COLOR_HUE_SECTOR_COUNT` is six,
so `COLOR_HUE_FULL_TURN` is `0x6000`, not the `0x1000` trigonometric turn.
The conversion helpers, tint wrappers, and VRAM fast-path scale test share
these constants without changing their local types or arithmetic order.

`func_8005A98C` first finds the maximum and minimum input channels. With
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

`func_8005ABA0` performs the inverse calculation. It derives the minimum and
maximum channel magnitudes from lightness and saturation, interpolates the
third channel across the selected hue edge, rounds each 12-bit fixed-point
result with `+0x800`, and clamps it to `0xFF`. Evaluating the two matching
integer formulas across all `32 * 32 * 32` BGR555 channel triples reproduces
every input channel exactly.

## Tint controls

`func_8005AE68` and `func_8005B0B4` apply the same controls after converting
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

`func_8005B054` is the direct packing counterpart: it converts one fixed-point
HSL triple with channel maximum `31` and packs the resulting channels into
BGR555 bits `0-14`.

`color_constants.h` names the five-bit channel width and derives the channel
mask (`31`), green shift (`5`), blue shift (`10`), and STP mask (`0x8000`).
Red occupies bits `0-4`, green bits `5-9`, and blue bits `10-14`. The channel
mask is also the maximum passed to HSL conversion. Both packing paths use
these constants, but only `func_8005AE68` copies the input STP bit; the direct
packer has no input colour from which to preserve it.

## VRAM application

Matching `func_800582C0` applies the BGR555 transform to VRAM colour bands.
For buffer selector `0` or `1`, it processes two `256 x 4` rectangles:

1. Read from x `selector * 256`, y `248` and `252` through `StoreImage2`.
2. Transform all `0x400` halfwords in each rectangle with `func_8005AE68`.
3. Upload the results to the same x coordinate at y `240` and `244` through
   `LoadImage2`.

When the full flags argument is at least `COLOR_TINT_KEEP_HUE` (`7`),
`COLOR_TINT_INVERT` (bit `3`) is clear, and the saturation scale is at least
`COLOR_FIXED_ONE` (`0x1000`), the function skips per-colour conversion and
moves the complete `256 x 8` source band from y `248` to y `240` with
`MoveImage`. The first comparison deliberately remains against the full
argument, not `flags & COLOR_TINT_HUE_MASK`; the two tests are not equivalent
when higher flag bits are set.

## Card-image readback

Matching `func_8001944C` has a separate readback path: `StoreImage2` reads a
rectangle into `D_8015C424`, a loop sets `COLOR_BGR555_STP_MASK` on every
halfword, selected halfwords are cleared, and `LoadImage2` uploads the block.
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
unassigned, so the functions retain address-based symbols.
