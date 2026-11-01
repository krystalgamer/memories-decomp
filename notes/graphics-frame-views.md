# Graphics frame controls and ordering-table views

This component names the six frame-publication data views used by
`Graphics_SyncFrame`, `Graphics_BeginFrame` and their resident clients.
The identities do not change storage, access widths, frame arithmetic,
buffer ownership or existing addressing selectors.

## Frame-step publication

`gGraphics_FrameStepControl`, formerly `D_8009AFA4`, retains the NA
initialized `.sdata` `u8[4]` backing `{1,0,0,0}`. Existing clients select a
scalar first-byte view, an absolute scalar `section(".data")` view, or the
four-byte scene view. A nonzero first byte forces the published
`gGraphics_bFrameStep` to exactly two; it is then cleared. The setter
stores the low byte of its argument, not an arbitrary requested step.

The other bytes are not inferred from that first-byte role. Byte one is
unclassified. Byte two remains `D_8009AFA6`, and its getter
`func_80058DC0` remains address-named because no semantic consumer or
writer is established. Three watch traces under #418 saw neither a call
to the getter nor a write to the byte on the title loop, Library 3-D view
or duel board, and a 40-screen menu/campaign tour saw neither as well
([`trace-uncalled-functions.md`](trace-uncalled-functions.md)). Byte three is the credits controller's private
slot selection, not the separate `gModel_bActiveSlotIndex`. The following
halfwords are outside the four-byte NA backing. No replacement struct is
introduced, and native array views do not create storage.

`gGraphics_nBaseFrameStep`, formerly `D_8009B0D8`, remains one signed
32-bit word. `Main_Init` seeds it to one. Frame sync copies the VBlank
counter through the separate extra-step byte, normalizes that byte to
zero or one, then publishes byte-plus-one here. Counter-to-byte truncation
and low-byte wrap are unchanged.

This pre-override multiplier is distinct from the post-override
`gGraphics_bFrameStep` byte and `Model_GetFrameStep`'s clamp of that byte
to six. It is not the raw VBlank elapsed count. Word, halfword and byte
reads still come from the existing explicit casts; volatile, plain and
`section(".data")` declaration arms stay distinct. No arithmetic is
simplified from nominal one/two values.

## Display-swap guard

`gGraphics_bSkipDisplaySwap`, formerly `D_8009B0A8`, remains an unsigned
byte. `Main_InitHardware` clears it. When nonzero, frame begin skips the
draw-environment refresh, `GsSwapDispBuff` and conditional `PutDispEnv`
block. It does not skip `ResetGraph`, playback service, OT drawing or
the subsequent SDK buffer-selection/work-base publication.

The frame unit's tentative common definition is preserved: it is an
addressing/load-delay lever, not a new allocation. Its original automatic
NA absolute provider at `0x8009B0A8` is retained through explicit
canonical and compatibility bindings.
One local pre-include name binding keeps this variable at its frozen
owner's declaration position. Without it, GCC emits the common declarations
in a different order even though the executable still matches. The binding
preserves complete compiler/MASPSX output, not a filtered comparison.

## Three interior pointer slots

`gGraphics_pOrderingTable1/2/3` name the pointer slots formerly exposed as
`D_800E9D94/98/9C`. They are interiors of the existing
`gGraphics_apOrderingTables[4]`, not new descriptors or pointer storage.
Frame begin publishes four active `GraphicsFrameBuffer` descriptors and
clears them through `GsClearOt`.

The slot-one selector retains the intentional oversized four-pointer
array versus scalar `GsOT *G32` view. Slot two retains its incomplete array
versus scalar view; slot three remains a scalar pointer declaration.
These declarations control old-GCC addressing and must not be resized.
Model rendering still reaches slot three through slot-two-view element
one; already named primary-array reads are not rewritten.

| Image | Primary pointer array | Slots one/two/three |
| --- | --- | --- |
| NA | `0x800E9D90` | `0x800E9D94/98/9C` |
| Japan | `0x800E9C70` | `0x800E9C74/78/7C` |
| SLES-03947 | `0x8009C438` | `0x8009C43C/440/444` |
| Other PAL images | `0x8009C858` | `0x8009C85C/860/864` |

These addresses come from each native primary array and its established
four-byte pointer stride, checked against every explicit interior binding.
Japan has no original explicit logical slot-one alias, and the other four
PAL images have no original explicit logical slot-three alias. Canonical
bindings name those actual slots without rebinding potentially unrelated
literal NA-address names. Existing foreign providers and literal labels
remain unchanged.

## Preservation and validation

The primitive timing header stays separate from the SDK pointer-view
header. Both mirror existing selector/declaration contracts; all
pre-existing headers and overlay C remain frozen. Legacy overlay frame
imports, including exact import-count controls, retain their old providers.

Three existing card/scene witness or ownership-control modules receive
namespace/include migrations only. Reversing those migrations reconstructs
their complete accepted files, preserving generators, assertions,
relocation/data expectations, guard storage, case counts and mutations.
No test case, C fixture, source-string harness or probe is added.
Existing legacy-overlay controls stay unchanged.

The original registered data-owner and caller profiles, common symbols,
raw initializers, object order, function inventories, manifests, attempts
and old semantic rows remain. Whole compiler, filtered-intermediate and
MASPSX output plus complete fallback assembly are compared with only the
six identities and original per-context aliases reversed. No instruction,
register, directive or filename is filtered.

Final clean resident matching, data ownership/policy checks and fresh
applicable PR checks remain required. Regional retail inputs are not
available locally; no local cloud/full regional/overlay or witness suite
is run.

## Japanese bindings for the frame and save timing globals

Three of these globals sit at other addresses in the Japanese executable.
`config/slpm_86398/symbols.txt` binds the C names there directly, so every
declaration arm in `graphics_frame.h` uses the one name in all regions:

| Global | Japanese address | Arms |
|---|---|---|
| `gGraphics_dwSyncVBlankCounter` | `0x8009AFB8` | `.data` and volatile |
| `gSaveData_dwVBlankCounter` | `0x8009AFB4` | `.data` and volatile |
| `gGraphics_bClearBackground` | `0x8009AFC0` | plain |

Types, sections, volatility and every access are unchanged.

