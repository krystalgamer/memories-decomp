# Model-slot appearance controls

The ambient setter selects a render request, the renderer publishes that
request and the ambient channels, and slot setup applies the palette tint
control and scale. These are four related appearance views, not new
rendering or color logic:

| Original member | Canonical member | Preserved representation |
| --- | --- | --- |
| `field_E11` | `draw_mode_request` | `u8`, `+0xE11` |
| `field_DA0` | `ambient_rgb_scale` | `s32[3]`, `+0xDA0` |
| `field_E0C` | `texture_tint_control` | `u8`, `+0xE0C` |
| `field_E0A` | `texture_tint_scale` | `u16`, `+0xE0A` |

`ModelSlot` stays `0xE20` bytes. Signed channels, array extent, halfword
storage, byte conversion, casts and offset assertions retain their
original forms.

## The draw request is not the published current mode

`Model_SetSlotDrawMode` writes the existing byte directly.
`Model_InitSlot` supplies `flags & 0x7F` only on its nonnegative-flags path.
`Model_DrawSlot` uses the request in its initial gate, copies it to
`gModel_bCurrentDrawMode`, and clears non-4 requests. Mode 4 retains its
existing persistent behavior.

The name distinguishes this slot-side request/latch from the published
global and the unrelated GPU-packet local variables named `draw_mode`.
It does not assign an enum, valid range, Boolean meaning, or universal
purpose to every numeric mode.

`Model_DrawSlotUnitsOnly` saves the raw byte, forces request 1 for its draw,
then restores the saved byte. This remains distinct from ordinary
consumption and clearing by `Model_DrawSlot`. No new reset, normalization
or persistent-mode assumption is introduced.

## Signed ambient channels

`Model_SetSlotAmbient` retains its independent channel clamps to
`[-MODEL_FIXED_ONE, MODEL_FIXED_THREE]`. Unless request 4 is present, it
chooses request 0 for the existing all-half-scale condition and request 3
otherwise. Neither those comparisons nor their priority changes.

The renderer passes `ambient_rgb_scale[0..2]` directly to `GsSetAmbient`.
These are signed scale channels, not packed RGB bytes or values restricted
to `0..255`. The existing slot-2 suppression when all three are exactly
negative one remains load-bearing and unchanged. Background rendering
and its native oracle consume the same channel values.

## Packed palette tint and unsigned scale

Reset keeps the existing tint control 7 and `COLOR_FIXED_ONE` scale,
including the signed-halfword reset-store view of the unsigned member.
The direct setup and staged-load paths supply these values to
`Model_ApplyTextureTint`.

The packed control ultimately supplies the hue selector/inversion flags to
`Color_TintBgr555Pixel`. It is not renamed to a plain hue index or converted
to a Boolean. The scale remains the pixel routine's `u16` multiplier;
all conversions, arithmetic, masks, channel handling and preserved bits
remain unchanged.

The palette routine keeps its per-region source/destination placement,
two-band read/recolor/upload path, wait loops and fast `MoveImage` path.
The fast-path conditions do not justify claiming universal pixel-for-pixel
equivalence with the per-pixel path or removing its zero-channel behavior.
No new palette format, opacity meaning or range validation is inferred.

Slot initialization does not blanket-reset these controls. Existing
negative-flags behavior and partial state retention remain, as do all
external aliases or raw writers not expressed through these member names.

## Scope and conservation

Only the corresponding identifiers change in the existing background
renderer witness. Its complete remaining source, byte offsets, sentinel
inputs, channel matrix, oracles, negative controls and compiler flags stay
identical. No new synthetic/generated-C/source-string harness, fixture or
control is added; actual cloud/runtime validation remains on the PR.

The loader/activity/animation fields, playback speed, mixed `tint`
record, tentative textured-primitive helper, packet/cache accounting,
SDK headers, source groups, compiler profiles, linker inputs and semantic
ledger are outside this change and remain frozen. These runtime members
do not receive invented fixed-VMA global rows.
