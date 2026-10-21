# Primitive handler modes and shared packet state

Two registry-based callback mappers and the draw pass's two remaining anonymous
primitive-state words form one interface. Canonical declarations are isolated
in `primitive_handler_modes.h`; existing headers remain unchanged.

## Registered callback mapping

`Model_MapToWireframePrimitiveHandler` (`0x8005FC1C`) and
`Model_MapToBasePrimitiveHandler` (`0x8005FE44`) take and return the original
`s32` callback-value representation. Neither argument is a packed primitive
key. Each retains the same open-coded reverse lookup through the eighty-entry
registry, the `GsU_00000000` sentinel handling, switch order and unchanged-input
fallback for missing or unsupported keys.

The wireframe mapper selects the existing line-rendering callback families.
For example, the handwritten handler at `0x80069E44` loads
`gModel_dwLineF4Template` and `gModel_dwDrawModeTemplate`, projects three
vertices and writes `SXY0/SXY1/SXY2/SXY0` into a closed line packet.
`Model_UpdateCreditsMonsterSlot` installs this mapping through
`Model_RunSlotHandlers` after loading and orienting a slot.

The base mapper selects the corresponding base callback family for registered
groups `0x00200000/0x00300000` and `0x01200000/0x01300000`. Its selected
handlers correspond to `0x00000000/0x00100000` and
`0x01000000/0x01100000`, retaining the existing material-kind variants.
The forced-semitransparent triangle handler at `0x80063F90` explicitly ORs
GPU command bit `0x02000000`; its base counterpart at `0x800612C0` uses
the current base polygon template instead.

“Base” does not promise opaque GPU output: the global fade/template state
may still enable blending. These mappers are not inverses, and the base mapper
does not recover original shading after a wireframe conversion.
`Model_DrawSlot` uses the base mapper when its existing palette-flash countdown
reaches zero. Both callers retain their `s32 (*)(s32)` callback variables and
the existing `Model_RemapPrimitiveHandlers` traversal.

## Second-pass CLUT word

`gModel_dwSecondPassClutBits`, formerly `D_8009AFDC`, is zero except for the
existing mode-5 template selection. Its values remain `0x3C200000` in
North America/Japan and `0x3C280000` in Europe.

The ordinary triangle handler loads this word, then, in its additional-packet
arm, preserves the original UV word's low half and ORs the override into the
upper CLUT half. It copies the projected coordinates and submits the additional
packet with the original blend-mode adjustment. Some forced-semitransparent
siblings use the nonzero word as an enable and derive their CLUT word from the
primitive record. This is not the separate TPAGE blend-rate word, and no
universal new packet operation is inferred.

## Lit-color cache address word

`gModel_dwLitColorCacheBase`, formerly `D_8009AFE0`, retains its `u32` spelling.
`Model_DrawSlot` assigns the current slot's `field_DF0` allocation address and
clears it for draw modes at least three.

The handwritten triangle handlers add a normal-derived offset divided by two
to this address. Their lighting path writes GTE `NCCS`/`NCCT` result words to
the cache when its base is nonzero; their mode-zero path reads those words
back for the packet. It is an address word, not render flags, depth bias or a
single color value. No pointer declaration or new cache capacity is introduced.

## Storage, native providers and preservation

North America's initialized owner remains the same fourteen separately named
`u32` words in `model_primitive_templates.c`, in identical order with explicit
`.sdata` attributes. The final two values remain zero. The draw pass still
defines the cache word as an uninitialized common: its old-assembler
load/store-padding effect is preserved, and it still merges with the
initialized definition rather than allocating new storage.

North American legacy data aliases refer to the canonical C definitions.
Regional aliases instead refer to the original image providers: the Japanese
`gJapanese_D_8009AFDC/gJapanese_D_8009AFE0` pair and Europe's
`D_8009BF64/D_8009BF68`. The distinct French literal-address aliases are not
substituted, and absent German numeric bindings are not guessed.

The complete North American five-function registry object remains intact.
Each regional image retains its two-mapper dispatch object and three
independently guarded registry/helper objects. All source paths, definitions,
profiles, extents, sibling bodies and dispatch guards remain. Only obsolete
resident spelling macros are retired.

All existing headers, overlay C/import maps, tests, historical evidence and
trace bytes remain unchanged. The accepted handler trace records zero hits
for both mappers, so it supplies no positive runtime observation; the names
rest on local source, data and handwritten-instruction evidence.
No handwritten packet instruction is edited.

Acceptance compares complete ordinary compiler/MASPSX output, actual native
source/profile contexts including initialized data, and complete generated
fallback assembly, allowing only declared identity substitutions. A final
clean resident executable match remains mandatory; local probes do not replace
the PR's regional/overlay full-image checks.
