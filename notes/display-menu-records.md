# Shared display-effect menu records

These two existing identities connect the script menu opcode, text-command
display effects and their common record lifecycle:

| Former identity | Canonical name | Preserved view |
| --- | --- | --- |
| `D_800EB010` | `gDisplayEffect_aMenuRecords` | incomplete `MenuRecord[]` |
| `D_8009B274` | `gScript_pMenuRecord` | stored `MenuRecord *G32` |

They do not introduce new record storage or replace the separate
`gText_DisplayEffectRecord` pointer.

## Shared records and lifecycle

`Script_OpShowMenu` chooses record 0 or 1 with operand bit 7. It releases an
already active record, resets the shared records, applies the existing
partial state reset and arms lifecycle step 2. Its raw marker, mode bits,
side selector and coordinate stores remain unchanged.

`Text_HandleDisplayEffectCommand` independently uses the same records.
Ordinary effect IDs select/search the first two records; portrait IDs use
record 2. The existing stop, adjustment and start branches retain their
raw IDs, masks, state stores and coordinate defaults. This route publishes
`gText_DisplayEffectRecord`, not the script menu continuation pointer.

`DisplayEffect_ResetMenuRecords` and `DuelEffect_MarkObjectIfActive` walk
indices 2 through 0. The latter tests the signed marker and marks the
caller's record on the first nonnegative marker, retaining the regional
flag difference. `DisplayEffect_ProcessMenuRecords` visits three records
in forward order, optionally updates them under bit `0x40` and dispatches
nonzero lifecycle steps through the existing `& 0x1F` table selection.

Those consumers establish three accessed slots with the existing
`0x4C` stride, not a reason to replace the incomplete declaration with a
new sized allocation. The existing `MenuRecord`, narrower
`DisplayEffectState`/VRAM views, integer grid words, pointer casts, signed
markers and lifecycle fields remain untouched.

The debug bust-up entry also passes the first record to the existing
updater. Its numeric debug gate and empty per-frame helper do not acquire
a new purpose from this table rename.

## Script continuation pointer

On first entry, `Script_OpShowMenu` stores the selected record in
`gScript_pMenuRecord` **before** its release/reset/arming calls. Subsequent
invocations process the shared records and clear the script command word
when the selected record's lifecycle step reaches zero.

This is a borrowed continuation reference to a shared record, not an owner,
allocator, per-instance script object or synonym for all menu state.
The pointer remains stored after completion; it is not newly cleared,
validated or made reentrant. No null/bounds guard, publication reorder or
collapse into `gText_DisplayEffectRecord` is introduced.

The stored pointer retains `G32`. The isolated header only supplies the
canonical declarations alongside the frozen legacy header; it does not
change the existing record layout or member representations.

## Native identities and preservation

| Image | Record table | Script continuation pointer |
| --- | --- | --- |
| NA | `800EB010` | `8009B274` |
| Japan | `800EAEF0` | `8009B164` |
| SLES_039.47 | `800EBE88` | `8009C214` |
| Other PAL | `800EC2A8` | `8009C634` |

All fourteen bindings retain the original logical providers. The Japanese
pointer uses its actual `gJapanese_D_8009B274` provider; the table retains
its independent provider. French literal aliases remain distinct from the
logical PAL addresses. Old names continue to bind, and no storage,
initialization, section or allocation extent is added.

The six complete C files differ only by the scoped names and isolated
include. Old headers, wrappers, overlays/imports, compiler profiles,
registered function groups, field/stream arithmetic and callback order
remain unchanged. All existing test modules are frozen; no harness,
fixture, probe, testcase or control migration is added.

Native-profile compiler/assembler conservation is not independent full
regional-image proof. Regional retail inputs are unavailable locally, so
fresh exact-head PR checks remain required. Cloud/regional/overlay/witness
suites are not executed locally.


## Effect identity and child-variant countdown

The shared signed byte at +0x30 is effect_id in MenuRecord and the wider
DisplayEffectVramState view. Script_OpShowMenu stores its first operand;
Text_HandleDisplayEffectCommand compares and stores the same identifier
while selecting the existing ordinary or portrait slots. Portrait setup
subtracts CAMPAIGN_DIALOG_PORTRAIT_FIRST_EFFECT_ID from it. Resource
acquisition compares it with cached IDs, uses it in the original
50-sector package calculation and records it in the selected cache entry.
Text continuation retains its separate literal 0x41 portrait gate.
These are mechanical effect/resource identities, not a new menu enum or
an assertion that every value is valid.

Reset and release still store -1. Walkers still use the signed >=0 marker
test; script and text stores still narrow into s8 with no new bounds rule.
IDs that narrow negative retain that behavior. Negative markers, resource
cache sentinels, portrait threshold arithmetic and all partial reset,
release and stale-state behavior remain. DisplayObject's unrelated
field_30 union and other owners are not renamed.

The shared u16 at +0x3E is variant_countdown in MenuRecord,
DisplayEffectState and DisplayEffectVramState. Reset seeds it with the
same rand() mask/base expression. DisplayEffect_UpdateMenuRecord only
works this countdown when grid[0][1] exists, after the existing flag-0x10
early return. Its active-variant branch waits for that object's field_5A
to reach zero, reseeds the countdown, clears flag 0x01 and selects variant
zero. Otherwise it decrements, stores the narrowed halfword and tests the
same signed-halfword result before setting flag 0x01 and selecting variant
one. No new initialization or extra RNG call is added.

The name does not claim frames, elapsed time, a universal blink, or the
meaning of DisplayObject.field_5A. The update gate, resource variants,
halfword wrap/signed test, repeated reads and no-child behavior remain.
The separate grid[0][2] counter and variant argument are unchanged.

This follow-up changes the two member identities consistently across
their existing views and qualified users. All record types, signedness,
widths, offsets, padding, aliases, existing assertions, callback casts,
archive arithmetic and regional differences retain their prior forms.
The earlier table/pointer identity checkpoint remains distinct; no
global storage, ownership, array extent or new behavioral rule is added.
