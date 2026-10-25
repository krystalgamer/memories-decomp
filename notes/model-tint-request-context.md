# Model tint-request submission and redraw context

## Whole-slot request submission

`Model_QueueTintRequestForAllParts` is the former resident
`func_80059AF8`. It writes `mode & 0x7F` into the start descriptor's fourth
byte, then calls `Model_QueueTintRequest` with selection zero and a null
part-mask pointer. The allocator fills all eight mask bytes with `0xFF`
for that null pointer, establishing the all-parts name.

The two `ModelTintColor` arguments remain by value. The wrapper does not
normalize the end descriptor, validate a slot or mode, add queue space,
change duration, or alter the allocator's no-free-entry behavior. No
caller or gameplay event is inferred merely from its interface.

The existing wrapper translation unit is retained. All seven native
functions retain size `0x98`, the common source
and the named `gcc_2_8_1_g0` profile. The owning queue header now declares
the exact existing definition type; neither legacy queue compatibility
symbol nor its signature is removed.

## Request table

`gModel_aTintRequests` confirms the existing `tent_ModelTintRequests`
identity, not a new allocation. The established declaration remains ten
`0x18`-byte records at NA `0x800F2B50`. The allocator scans for a clear live
bit, fills the selected record, and stops; the updater interpolates,
temporarily redraws and restores the slot, then advances the request.
`Model_ResetSceneState` clears the live bit of every record.

The mask layout is deliberately not reinterpreted. The allocator writes
eight bytes beginning at record offset one, overlapping the high byte of
the existing `flags` halfword. The updater passes that same raw address
to the renderer. `pad_02` is not substituted for it, and no member,
alignment, mask bound, table size or initializer is changed. The Japanese
allocator's separate accepted body remains otherwise exact.

## Transient masked redraw

`gModel_PartMaskAddress` is the former `D_8009AF9C`, retaining its
initialized `s32` representation rather than becoming a stored native
pointer. The updater stores the current request's raw `+1` address, stores
request flag bit two into `gModel_bDrawMaskedPartsOnly` (formerly
`D_8009AF9B`), calls `Model_DrawSlot`, then clears the address and flag in
that order. Scene reset clears both through the same scalar views.

The renderer keeps its existing range and bit arithmetic. Entry zero
bypasses the mask. With the flag set, entries beyond the covered range and
covered entries whose bit is absent are skipped. Without it, a missing bit
still selects the original zero-matrix path rather than the skip path.
The nonzero address also retains the ordering-table-length update and
postdraw early return. Neither state is persistent model visibility.

Both initialized `.sdata` owners keep their width and order. Only the four
obsolete Japanese producer/renderer alias lines retire; the new names
retain native `0x8009AF03` and `0x8009AF04`, not the NA addresses.
No pointer/G32 conversion, mask relocation, bounds check, reset policy,
copied snapshot or shared-struct change accompanies these identities.

## Descriptor channels and saved animation position

`ModelTintColor.b0/b1/b2` are now `r/g/b`, and `b3` is `blend_mode`.
All four remain bytes at offsets zero through three; the descriptor
remains four bytes, passed by value and copied as a whole word.
`ModelTintRequest.field_0A` is now `animation_position_snapshot`, still
a halfword at `+0x0A` in the same `0x18`-byte request. Fixed member-width
and offset assertions accompany the names, without a new SDK type.

The two queue wrappers mask their mode argument with `0x7F` into the
start descriptor's `blend_mode`. The allocator copies both descriptors
and captures `slot->animation_position` before the per-frame updater
uses that snapshot in `Model_SeekSlotPartAnimation`. The updater saves
the current slot animation position and selector separately for its
restoration pass; it does not advance or replace the request snapshot.
Its flag bits 3..7 come from `slot->field_BF5` and override sequence
`sid` only when nonzero. They are not the descriptor's fourth byte
and are not a geometric part index.

The updater interpolates each RGB byte using the original sum of two
integer quotients, while copying only the start `blend_mode` unchanged
into the temporary slot tint. `Model_DrawSlot` passes that fourth byte
to its primitive-template/blend-word setup. Independently,
`Model_HasInsufficientBufferSpace` receives the same mode and reserves
twice the packet budget for mode 5. This establishes a blend selector,
not an animation selector or a direct two-bit GPU ABR value.

The end descriptor's `blend_mode` remains copied but unread. The
matching variant118/119 producers initialize only RGB before calling
`Model_QueueTintRequestForAllParts`. The French variant144 producer
also initializes only RGB, but calls `Model_QueueTintRequestForParts`
with its configured part list instead. Both wrappers install the same
masked start blend byte, but construct different part masks. The unused
end byte is deliberately not initialized by this change. Those three sources
serve 23 tracked image bindings, all retaining their existing named
compiler profiles. No gameplay event or new human execution is inferred.

Keep the raw `+1` mask address and flags overlap, partial queue writes,
full-word descriptor save/install/restore, original casts and arithmetic,
duration doubling/truncation, call order and queue exhaustion behavior.
Inactive slots still skip elapsed advancement; the insufficient-room
path still advances it. No new zero-duration guard, interpolation clamp,
mode normalization or initialization policy accompanies these names.

| Identity | NA | Japan | UK | Continental Europe |
|---|---|---|---|---|
| All-parts wrapper | `0x80059AF8` | `0x8005C928` | `0x8005DFBC` | `0x8005CC00` |
| Ten-request table | `0x800F2B50` | `0x800F03F0` | `0x800F3C80` | `0x800F4088` |
| Masked-only byte | `0x8009AF9B` | `0x8009AF03` | `0x8009BF23` | `0x8009C32B` |
| Raw mask-address word | `0x8009AF9C` | `0x8009AF04` | `0x8009BF24` | `0x8009C32C` |
