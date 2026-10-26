# Model background renderer ownership

`Model_DrawBackground` uses the third canonical model slot:
`&D_800F2C40[2] == 0x800F4880`, since each of the three slots is `0xE20`
bytes. The complete array ends at `0x800F56A0`, the separate camera
coordinate unit. The renderer no longer declares an independent
`D_800F4880` object. The canonical array expression matches every retail
instruction; no linker alias or duplicate allocation is needed.

`gModel_CameraCoordinateUnit`, returned by `Model_GetCameraCoordinateUnit`, is the actual SDK
`GsCOORDUNIT`, `0x50` bytes. Its `matrix`
subobject begins at `+4`; `D_800F56A4` must not be separately allocated.
The model slot's `root_coordinate` points to another `GsCOORDUNIT`; the renderer
reads `matrix.t[1]` at coordinate offset `0x1C` and adds 300. The three
`flat_lights` entries have the `GsF_LIGHT` size/layout, `ambient_rgb_scale` supplies
the three ambient words, and `is_active` is the active byte.

`Model_GetLightSourceMatrix` is owned by `screen_projection.h`, already included by its
definition in `model_slot_support.c`. Its existing return type is `void *`,
and its body returns `&gGraphics_LightSourceMatrix`, a real SDK `MATRIX`. Conversion to the
`MATRIX *` parameter of `GsSetLsMatrix` is an ordinary C object-pointer
conversion, not an inferred alternate ABI.

## Supported inputs and producers

The selected model record is the complete `0xB2` bytes described by
`model_graphics_state.h`. `Model_SetSlotProperties`, in
`model_scene_controllers.c`, selects it from `gModel_abBackgroundRecords` using stride `0xB2`.
The owning `model_record_tables.c` defines 1,248 bytes: seven complete
records and two trailing bytes. Their initialized texture depth codes are
0 or 1, and their tile periods are 128 or 256.

The renderer reads the record at `+0xAC` and `+0xA8/+0xAA`, and reads the
coordinate Y value, **before** the slot-active guard. Both pointers must
therefore be valid even when the slot is inactive. The model initializer at
`Model_InitSlot` initially clears `root_coordinate`, then installs a coordinate from
the slot's 80-byte entry run during model setup. Reset state alone is not a
valid input to this renderer.

The measured texture fields are enable bits at `+0xA1`, width at `+0xA6`,
height/depth at `+0xA8`, U/page bits at `+0xAA`, V/page bits at `+0xAC`,
and CLUT coordinates at `+0xAE/+0xB0`. Required arithmetic conditions are:

- Texture depth code `((height_depth >> 13) & 3)` is in **0..2**. Code 3
  would make the C shift count negative; masking that shift changes retail
  code and is not promoted.
- When the visible tiling loop runs, `(u16)(width / 10u)` is nonzero.
- The selected ordering table is valid and its `length` is below 32.
  `Graphics_BeginFrame` publishes the descriptor and copies its length from
  the corresponding byte of `gGraphics_abOrderingTableLengths`; this renderer does not validate it.

These are supported-format and caller-lifetime assumptions, not claims that
arbitrary corrupt records, null pointers, or oversized table lengths are safe.

## Typed selected-record ownership

`gModel_pBackgroundRecord`, formerly `D_8009AF88`, is a stored
`ModelBackgroundRecord *G32`, not a byte-buffer cursor. Its initialized
`.sdata` owner remains `model_graphics_state.c`; the historical COMMON
definition in `func_8004E7B0.c` remains COMMON under its existing named
compiler profile. The latter still resolves to the strong owner rather than
allocating a second handle. The declaration in `model_graphics_state.h`
forward-declares the record tag so unrelated graphics users need not import
the complete model-background layout.

The initializer selects the start of `gModel_abBackgroundRecords`, and
`Model_SetSlotProperties` selects `index * 0xB2` bytes into the same backing.
Those two producer boundaries retain explicit byte-to-record conversions.
The backing stays exactly 1,248 bytes: seven complete `0xB2` records and two
trailing bytes, not an eighth record or a padded replacement allocation.
The unrelated auxiliary-model lookup also retains its original byte table.

All selected-record readers now use the shared layout directly:

| Offset | View | Observed consumers |
| --- | --- | --- |
| `0xA0` | `u8 selector` | Animated-battle scene and VRAM-copy switches; the backing-table byte also selects the auxiliary model file |
| `0xA1` | `u8 flags` | Bit 0 enables floor bands and shadow clipping; bit 1 disables shadows when debug setup publishes the inverse |
| `0xA2` | `s16 shadow_y_offset` | Added to slot 2's coordinate Y for the default ground-shadow plane |
| `0xA4` | `u16 stage_height` | Copied into the debug controller's signed height scratch without changing either view |
| `0xA6` | `u16 texture_width` | Scroll modulus and renderer tile period |
| `0xA8` | `u16 texture_height_flags` | Low 13 bits bound clipping; upper bits retain depth/attribute behavior |
| `0xAA` | `u16 texture_u_page` | Existing packed U/page view, unchanged |
| `0xAC` | `ModelBackgroundVPage` | Low byte feeds V before the active guard; the complete halfword supplies texture-page bits after scrolling |
| `0xAE` | `u16 clut_x` | Sprite CLUT X |
| `0xB0` | `u16 clut_y` | Sprite CLUT Y |

The union at `0xAC` represents the actual byte/halfword overlap; replacing
the byte load with a halfword load and truncation is not assumed equivalent.
The record remains `0xB2` bytes, with each recovered member's offset checked.
The first `0xA0` bytes remain opaque, and the selector is deliberately not
named after just one of its three observed roles.

Setup, scrolling, VRAM animation, debug placement, background tiling and
model-shadow drawing share this contract. The renderer no longer needs its
raw halfword macro or private record-view/field macros. Existing operation
order, signedness, load widths, clipping arithmetic, inactive-slot reads and
pointer reloads across calls are preserved; no guards or supported-input
conditions are added or relaxed.

## SDK objects and load-bearing initialization

The source uses actual ILP32 SDK types: five `CVECTOR` colours, eight
`SVECTOR` vertices, one `GsSPRITE`, three `POLY_G4` packets, four `long`
projection outputs, and the `BackgroundNormals` wrapper owned by
`src/game/model_background.h`. The 20-byte automatic colour
initializer precedes a function-local 16-byte static normal initializer, then
the automatic aggregate copy. Keeping that sequence preserves the private
36-byte data region and the retail stack frame. A direct automatic normal
array emitted a clearing call and did not preserve the data region.

`NormalColorCol` always receives all three SDK arguments; the light-matrix
setter receives one. The colour outputs are the sprite RGB bytes and the
twelve polygon vertex colours. `RotAverage4` receives four actual vertices,
four packed-XY output pointers and two `long *` outputs.

Unused sprite transformation fields and vertex pad members remain
uninitialized as in retail. Native witnesses must inspect only the fields
the corresponding SDK call consumes, not compare entire stack objects.
The eight-byte vertex assignments deliberately retain their SDK stride.
The direct texture shift before the height load, clipping minima, aggregate
initializers, and late `vertices[6].vx` store remain in their exact source order.
