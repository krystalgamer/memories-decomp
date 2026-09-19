# Model background renderer ownership

`func_8004DE24` uses the third canonical model slot:
`&D_800F2C40[2] == 0x800F4880`, since each of the three slots is `0xE20`
bytes. The complete array ends at `0x800F56A0`, the separate camera
coordinate unit. The renderer no longer declares an independent
`D_800F4880` object. The canonical array expression matches every retail
instruction; no linker alias or duplicate allocation is needed.

`D_800F56A0`, returned by `Model_GetCameraCoordinateUnit`, is the actual SDK
`GsCOORDUNIT`, `0x50` bytes. Its `matrix`
subobject begins at `+4`; `D_800F56A4` must not be separately allocated.
The model slot's `field_D18` points to another `GsCOORDUNIT`; the renderer
reads `matrix.t[1]` at coordinate offset `0x1C` and adds 300. The three
`field_D70` entries have the `GsF_LIGHT` size/layout, `field_DA0` supplies
the three ambient words, and `field_E1F` is the active byte.

`func_80059220` is owned by `screen_projection.h`, already included by its
definition in `model_buffer_getters.c`. Its existing return type is `void *`,
and its body returns `&D_800FE148`, a real SDK `MATRIX`. Conversion to the
`MATRIX *` parameter of `GsSetLsMatrix` is an ordinary C object-pointer
conversion, not an inferred alternate ABI.

## Supported inputs and producers

The selected model record is the complete `0xB2` bytes described by
`model_graphics_state.h`. `Model_SetSlotProperties`, in
`func_80052D2C.c`, selects it from `D_80091008` using stride `0xB2`.
The owning `model_record_tables.c` defines 1,248 bytes: seven complete
records and two trailing bytes. Their initialized texture depth codes are
0 or 1, and their tile periods are 128 or 256.

The renderer reads the record at `+0xAC` and `+0xA8/+0xAA`, and reads the
coordinate Y value, **before** the slot-active guard. Both pointers must
therefore be valid even when the slot is inactive. The model initializer at
`func_8004CB0C` initially clears `field_D18`, then installs a coordinate from
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
  the corresponding byte of `D_8009B0A0`; this renderer does not validate it.

These are supported-format and caller-lifetime assumptions, not claims that
arbitrary corrupt records, null pointers, or oversized table lengths are safe.

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
