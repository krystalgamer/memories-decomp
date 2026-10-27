# Primitive handler modes and shared packet state

The original symbol-naming component grouped two registry-based callback
mappers and two anonymous draw-state words into one interface. Its canonical
declarations are isolated in `primitive_handler_modes.h`. The later complete
remapping-chain views are documented below.

## Registered callback mapping

`Model_MapToWireframePrimitiveHandler` (`0x8005FC1C`) and
`Model_MapToBasePrimitiveHandler` (`0x8005FE44`) take and return the original
`s32` callback-value representation. Neither argument is a packed primitive
key. Each retains the same open-coded reverse lookup through the eighty-entry
registry, the `GsU_00000000` sentinel handling, switch order and unchanged-input
fallback for missing or unsupported keys.

The wireframe mapper selects the existing line-rendering callback families.
All twelve mapped targets reference only the existing line primitive templates.
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

"Base" does not promise opaque GPU output: the global fade/template state
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
`Model_DrawSlot` assigns the current slot's `lit_color_cache_address` allocation address and
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

That symbol-naming component left existing headers, overlay C/import maps,
tests, historical evidence and trace bytes unchanged. The accepted handler trace records zero hits
for both mappers, so it supplies no positive runtime observation; the names
rest on local source, data and handwritten-instruction evidence.
No handwritten packet instruction is edited.

Acceptance compares complete ordinary compiler/MASPSX output, actual native
source/profile contexts including initialized data, and complete generated
fallback assembly, allowing only declared identity substitutions. A final
clean resident executable match remains mandatory; local probes do not replace
the PR's regional/overlay full-image checks.


## Complete registered-value remapping chain

The six complete registry/map/run definitions now distinguish registered
handler VALUES from packed type KEYS. ModelHandlerRegistryEntry.key is
STILL the packed type word; ModelHandlerRunEntry.handler_value is the
existing signed callback word formerly called key. metadata keeps its
original s32 type and all raw bits; there is no pointer conversion, new
record owner, descriptor layout or allocation.

The binders establish the distinction: Model_BindPrimitiveHandler and
Model_BindSharedPrimitiveHandler write the selected callback through the
GsTYPEUNIT-compatible pointer and register (original type, selected value).
GsScanUnit's original SDK instructions at 0x8008A064..0x8008A080 return the
type word plus its address, then clear the descriptor's scan flag at +4.
The low halfword at +4 still sets its next-descriptor advance at
0x8008A118..0x8008A12C: four bytes for the leading word plus four times that
halfword. This is the same payload-length role as the primitive/image
prefixes; the walker must NOT advance by sizeof(ModelHandlerRunEntry).

Model_RegisterHandlerKey preserves first matching key and first wholly
empty entry, refuses to update an existing key and stays a no-op on a full
table. Model_FindHandlerKey and BOTH mappers retain their complete original
eighty-entry scans, first duplicate-value match and specific SDK sentinel.
Zero is not substituted for that sentinel. Missing and unsupported mapped
keys retain the original input value; mapper-specific unsigned family masks,
case order and target families remain unchanged. The two maps are not inverses.

Model_RemapPrimitiveHandlers names the type_key from reverse lookup,
count_flags_word, signed shifted type_tag, descriptor_stride_words and
replacement_handler. It computes the stride BEFORE masking the high half.
Eligibility remains EXACTLY ((u32)tag<2 && low_kind!=0 && high_half!=0) OR
(tag==1 && low_kind==0): the shared zero-kind case is not folded into the
other high-half gate. Its callback receives the STORED HANDLER VALUE and
only result -1 leaves it unchanged. It does not publish the looked-up key.
The countdown remains count-1 through -1; no new negative-count policy,
type-range check, handler registry update, callback validation or retry.
The third argument remains unused, with the original caller setup intact.

Model_RunSlotHandlers keeps its active-slot guard, original unit-count loop
tests, nonnull group gate, full three-word header reads, nonzero-count remap
and exact -1 next-group sentinel. A zero next pointer is not a terminator.
Its u32 group_words/next_group_words retain their integer-word views and
casts; no guessed typed group or new nonnull pairing is introduced. The
palette-flash completion caller and its signed byte-cursor view remain
byte-for-byte unchanged, including argument order and its remap timing.

Only one existing prefix field, scoped definition identifiers, six existing
registry-comment identifiers and this owning note change, plus six existing
semantic NOTES cells. Public prototype argument labels and every signature
stay. The original source groups, SDK declarations, switch/private data,
commons, fields/widths, macros, selectors and all other function bytes stay.
Retired gameplay controls are not revived; no synthetic/generated-C/source-
string behavioral harness or local cloud fixture. Fresh complete native
objects and the final clean retail match remain required; regional and
overlay image validation belongs to ordinary exact-head PR checks.
