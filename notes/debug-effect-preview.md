# Developer effect-preview controls and resources

The complete three-function `debug_effect_screen.c` group contains
`DebugEffect_UpdateViewControls`, `DebugEffect_UpdatePreview` and the already
named `DuelScene_UpdateEffectPreview` scene entry. Canonical declarations are
isolated in `debug_effect_preview.h`; every existing header remains unchanged.

## View controls and preview update

`DebugEffect_UpdateViewControls`, formerly `func_800220B8`, handles the existing
orbit/reference controls, modifier-button step sizes and SELECT's renderer
flag-0 toggle. `DebugEffect_UpdatePreview`, formerly `func_800222F4`, runs
that helper while START is held and manages four preview pages and effect
request creation. The scene entry then prints the operand HUD.

START is not exclusive: the controller continues processing its original
first-call/rebuild and input chain after updating the view. SELECT can affect
both the view helper and page selection in one call. No early return, input
priority change, button substitution, guard or frame advance is added.

## The editable bytes are operands, not coordinates

The first edited byte is passed to `DuelEffect_AllocateRequest`, whose existing
allocator stores it in the request's `id` at `0x18`. The second is assigned to
`field_1A`; its effect-specific meaning is not inferred. The actual request
position is assigned separately according to the preview page.

The local aliases are consequently `gDebugEffect_bEditedOperand` and
`gDebugEffect_abOperands`. These are only source macros over the existing
`gDebugEffect_abPreviewState`, not new data objects. The old layout selector
`DEBUG_EFFECT_COORDINATE_OFFSET` retains its name and value for compatibility,
as do every other selector and the historical wording in frozen headers.

LEFT/RIGHT toggles the edited index with XOR 1. UP/DOWN retains the exact
unsigned-byte increment/decrement expressions and wrapping behavior. Reentry
clears the second operand, page and edited index but preserves the first
operand; no effect-ID reset or range limit is introduced.

The separate `DEBUG_EFFECT_PAGE_BYTE` shared outcome-byte view is unchanged,
including its zeroing and modulo-eight increment. It is not confused with
the four-page selector. The controller also retains its existing unchecked
allocation result and all original request payload stores.

## Preview cards and HUD resources

`gDebugEffect_pPreviewCard0` and `gDebugEffect_pPreviewCard1` retain the original
`u8 *G32` handle types. Only pages 2/3 assign card-display objects to these
handles, and the original clear/release order is preserved. Page 1 uses the
separate card-preview resource manager and a local object, not these handles.

`gDebugEffect_aPreviewCardRecords` is the original incomplete record-array
view at North American `0x801A7B80`, six `0x1C`-byte records into
`gDuel_aCardRecords`. The controller uses that view and its next record.
These are borrowed slots 6/7, not a private two-record allocation. The
Japanese view retains its original `D_801B7B80` provider. No bound, pointer
variable or inline pool-plus-six expression replaces the old array identity.

The HUD resources retain their exact incomplete `u8[]` types:

| Original NA address | Canonical resource | Role |
| --- | --- | --- |
| `0x80010074` | `gDebugEffect_szOperandsFormat` | Existing two-value EFFECT format |
| `0x80010090` | `gDebugEffect_szSecondOperandMarker` | Marker when edited index is nonzero |
| `0x800100A8` | `gDebugEffect_szFirstOperandMarker` | Marker when edited index is zero |

North American retail bytes confirm the existing two `%2d` fields, marker
spacing and color markup. No bytes, field widths, terminators, casts or
`FntPrint` arguments change. European providers use their original
`0x80010060/0x8001007C/0x80010094` bindings rather than copied NA addresses.
Every canonical resource aliases the original image provider.

## Regional layout and matching boundary

All seven groups retain source definition order, `0x23C/0x324/0x5C` extents
and `gcc_2_8_1_g8_split`. NA/Japan/Germany use the existing six-byte state
layout with operands at 2/3 and page at 4. French, Italian, Spanish and
English manifests all use the Spanish source's seven-byte layout: operands
at 4/5 and page at 6. The state-array definition/initializer remains unchanged.
Japan retains its native button swaps, and European page-1 geometry remains
distinct from NA/Japan.

Only the three obsolete Japanese helper/record spelling macros are retired.
The neighboring-function comment in `duel_result_runtime.c` receives an
identifier update only. Existing headers, overlay C/import maps/metadata,
tests, traces and historical fingerprints remain unchanged.

Acceptance compares complete ordinary compiler/MASPSX output and actual native
source/profile contexts, preserving every expression and selector except the
documented identities and operand-wording corrections. A final clean
byte-identical resident executable remains mandatory; local probes do not
replace the PR's regional/overlay image checks.
