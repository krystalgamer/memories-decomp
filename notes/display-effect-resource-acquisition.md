# Display-effect resource acquisition and row blending

The remaining two callbacks in `display_effect_resource_setup.c` now have
evidence-backed names beside the existing resource probe, object builder
and phased loader. The record producers, reset owner, transfer callback and
transition callers establish their roles; the old descriptions alone do not.

| NA identity | Name | Role |
|---|---|---|
| `0x8003A440` | `DisplayEffect_SetObjectRowBlend` | Configure three nullable row objects' blend bits, depth and CLUT y |
| `0x8003A560` | `DisplayEffect_UpdateResourceAcquisition` | Effect step two: restore or load/cache a resource, then build its primary row |
| `0x8009B326` | `gDisplayEffect_ResourceCacheWriteSlot` | Shared signed-byte destination for a cache miss |
| `0x8015C410` | `gDisplayEffect_CachedResourceIds` | Unsized signed-byte view of five cache-owner IDs |

## Producers and three distinct acquisition paths

`Script_OpShowMenu` and `Text_HandleDisplayEffectCommand` write the signed
resource ID at `+0x30`, `resource_entry_index` at `+0x31`, `slot_index`
at `+0x3C` and step two at `+0x33`. These are separate values. The
step table retains its cast from `DisplayEffectVramState *` to the public
`MenuRecord *` callback ABI; neither record layout changes.

The first call uses `DisplayEffect_MarkInitialized`. It clears the shared
write-slot byte to zero, then walks five ID bytes and five RAM slot
records together from index four down to zero. Every negative ID updates
the write-slot byte, so a miss chooses the lowest negative index, or zero
if none is negative. This is not LRU, round-robin or per-record state.
The explicit walking pointer and original locals remain.

A matching ID immediately waits on the existing `IsIdleGPU(10)` loop,
loads the cached image and CLUT into VRAM, copies the cached `0x800`
metadata bytes to the selected resource buffer, sets state bit `0x40`
and returns. It does not inspect the file-transfer predicate, publish a
new cache ID or build objects on this call.

On a miss, the unchanged predicate
`(transfer_flags & FILE_TRANSFER_REQUEST_BLOCKED_MASK) | secondary_request`
controls the request. If blocked, clearing only state bit `0x80` permits
the next call to repeat initialization and the entire scan. Otherwise the
callback requests fifty sectors at `resource_id * 50 + first_sector`,
installs `DisplayEffect_LoadResourceStage`, selects the destination
metadata buffer and writes the existing primary-active flags. The
descriptor is still used without a new null check; no request, ID, side
or buffer bounds policy is introduced.

On a later call with state bit `0x40` still clear, a busy transfer returns.
Once the predicate clears, the callback sets that state bit and publishes
the resource ID at the shared write-slot index *before* its GPU wait and
`StoreImage` calls. It captures the image, CLUT and `0x800` metadata bytes
into that RAM slot. This does not fall through to object construction.

Only a subsequent call whose state bit `0x40` is already set invokes
`DisplayEffect_BuildResourceObjects` for row zero with `resource_entry_index`, ignores
its existing return value, clears the whole step byte and ORs `0x40` into
`field_32`. The two `0x40` bits live in different bytes. There is no new
same-call build, polling timeout, error result or buffer lifetime.

## Cache storage and regional views

`DisplayEffect_ResetMenuRecords` writes minus one into all five owner
bytes. It neither clears the shared write-slot byte nor erases cached
page storage. Its descending loop and original return artifact remain.

Each RAM slot in the `gRuntime_pSharedWorkBase` arena is still `0x18C10` bytes: image
`0x18000`, CLUT `0x400`, metadata `0x800`, then two `RECT`s. Acquisition
uses image rectangle `(832 - side * 192, 256, 192, 256)` and CLUT
rectangle `(clut_x, side * 2 + 240, 256, 2)`. A nonzero side selects
`D_801AF800` rather than `D_801AF000`; it is not clamped to Boolean for
the rectangle or request arithmetic.

NA and Europe retain the unsized `extern s8
gDisplayEffect_CachedResourceIds[]` at `0x8015C410`. Completing that array
to five bytes would make it small-data eligible in the grouped G8 unit
and change its split-addressing relocations. The write-slot declaration
remains `s8` and gp-relative. Japan instead accesses owner bytes through
`((s8 *)gRuntime_pSharedWorkBase)[0x7FFF0 + i]`, both here and in reset. That expression
is preserved; no standalone Japanese owner-array symbol is invented.

The first-sector constants remain NA 15182, Japan 15177 and Europe
17249. CLUT x remains 512 in NA/Japan and 640 in Europe. The phased
loader's existing upload operations, width-specific stores and transfer
programming are unchanged. Its three phases are not replaced by the
cache-hit inline upload or cache-miss capture.

## Row blending is not an opaque-mode setter

The helper keeps its `u8 **`, `u32`, `s32` signature and its two distinct
descending loops over slots 2, 1, 0, skipping null objects. Integer grid
rows retain their explicit caller casts. No pointer-grid retyping, count
argument, allocation or release is added.

For zero attribute, it clears only `GsALON | GsATWO | GsAONE`, then
sets `GsALON`, passes the original signed-byte depth local to
`DisplayObject_SetDepthOffset`, restores `COLOR_RGB24_NEUTRAL_GREY` and
decrements the existing CLUT-y halfword. Zero is therefore not opaque,
and repeated calls are not idempotent.

For nonzero attribute, it replaces those same blend bits with the raw
supplied word and narrows depth at the existing call site. Exactly
`GsALON | GsATWO` assigns CLUT y `0xFD`; every other nonzero value
increments the existing CLUT y. No RGB store occurs on that arm. Other
attribute bits, the raw argument and halfword wrap behavior remain.
The transition callers' additive/subtractive pairs and zero-argument
completion calls retain their order and object ownership.

## Native identities and conservation

| Identity | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Row blend | `0x8003A440` | `0x80039AC0` | `0x8003A5D0` | `0x8003A7D0` |
| Acquisition | `0x8003A560` | `0x80039BE0` | `0x8003A6F0` | `0x8003A8F0` |
| Shared write slot | `0x8009B326` | `0x8009B216` | `0x8009C2BF` | `0x8009C6DF` |
| Owner IDs | `0x8015C410` | arena `+0x7FFF0` | `0x8015C410` | `0x8015C410` |

The Japanese native acquisition address is not the unrelated NA
`func_80039BE0` fade callback. Likewise the French
`D_8009B326_French` alias and the Japanese
`gMemCard_bTransferRetriesRemaining` binding at the NA-number address
remain untouched. Bindings are selected by native
address, not by coincident numeric names.

All 940 complete compiler/MASPSX outputs match after reversing only these
four identifiers, with no instruction, directive, relocation or filename
filtering. All seven inventories retain every other field and row, and
unrelated symbol/import bindings remain. Source paths, grouped object
order, manifests, compiler profiles, layouts and table positions do not
change. The complete accepted semantic ledger gains four evidence/caveat
rows, without rewriting its existing records.

Clean full-executable retail matching remains the acceptance gate. No
tests, fixtures, negative controls or captures change, and no synthetic or
source-string harness is added. Regional/cloud checks remain PR-side.
See [display-effect-lifecycle-helpers.md](display-effect-lifecycle-helpers.md)
for partial initialization, reset and release contracts.

The original four-name proof precedes accepted #7637 integration. Its
five transition identities, corrected headers, native bindings and complete
ordered ledger edits are retained. The combined-tree proof explicitly
aligns only those five accepted names in the saved baseline before
reversing the four owned resource names. Every complete artifact remains
in the comparison, and a fresh clean gate covers the integrated tree.


## Resource-entry and shared-slot member views

The +0x31 byte is resource_entry_index in MenuRecord,
DisplayEffectState and DisplayEffectVramState. It is the first index
passed to the three-level resource-table probe and object constructor,
not the effect/package ID at +0x30 or a claimed facial expression.
Script stores flags & 15; text adjustment stores flags & 3, while an
ordinary start only overwrites the reset zero when flags & 0x10,
using (flags >> 1) & 3. Portrait start retains its early return.
All masks, conditional stores, narrowing and the existing partial reset
remain; no archive-dependent bounds policy is added.

The +0x3C byte is slot_index in MenuRecord and DisplayEffectVramState.
Script publishes the selected record's 0/1 index; text publishes its
chosen 0/1/2 index, including the portrait value two. This explains the
side/resource-bank consumers without retyping the byte as Boolean.
Zero selects bank zero and nonzero bank one, but image/CLUT coordinates
retain their raw slot multiplications. The loader still forwards this
byte into the transfer descriptor's generic position word; phases zero
and two use that original value in the same upload-coordinate formulas.
That descriptor word is not renamed globally or given a universal role.

Fade-in placement, crossfade direction and continuation placement retain
their original zero/nonzero slot tests. Crossfade rebuilds the two new
rows from resource_entry_index and the two old rows from the live
object's field_67; neither is substituted for the other. Existing
fade-in/out back-row selectors also keep their live-object reads.
The row builder still returns early on a missing first entry and leaves
its output array untouched on that path; no null filling is added there.

The lifecycle reset still clears only resource_entry_index, not
slot_index. Global menu-record reset and record release do not gain
stores to either member. Stale values, incomplete arrays, all signed
position views, integer object-grid casts, regional constants and the
separate effect identity/variant-countdown roles remain unchanged.
