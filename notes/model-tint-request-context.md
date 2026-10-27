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
Its flag bits 3..7 come from `slot->current_animation_row` and override sequence
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

## Submission argument names

All three complete submission definitions now use the names already
declared by model_queue_tint_request.h: slot, mode or selection, start,
end, duration and part_mask. The variadic wrapper's eight-byte local
mask has the same part_mask name; the allocator's existing slot pointer
and request-table end pointer are slot_state and request_end to avoid
colliding with the by-value argument names.

slot still indexes gModel_aSlots without validation, while only its low
bit reaches the request flags. mode is deliberately NOT blend_mode:
the wrappers copy its low seven bits into start.blend_mode, but the
variadic wrapper also forwards mode & 128 as selection. The allocator
still tests selection & 1, not bit seven. That existing mismatch is
preserved; these names do not assert that the wrapper enables the
masked-only redraw flag or repair its forwarding.

The variadic cursor still starts four bytes after the named duration
argument, reads four-byte part indices until the first negative value,
and sets unchecked bits in the eight-byte local mask. A null part_mask
still fills all eight request bytes with FF, beginning at raw request
offset one and overlapping the high flags byte. No standard varargs
helper, bound, mask relocation or new selection policy is introduced.

start and end remain four-byte descriptors passed and copied by value.
Only start.blend_mode is installed by the wrappers; the end byte is
still copied but unread. duration retains its in-place multiplication
by two and assignment to the request halfword, with no zero guard,
clamp, unit conversion or overflow change. The first free request is
still claimed, partial writes and animation snapshots retain their
order, and a full queue still returns without a status or allocation.

Only bounded bare argument/local identifiers in the two complete C
units and three existing registry NOTES cells change. The whole header,
qualified descriptor/request members, types, arithmetic, statement order,
accepted source grouping, named profiles, initialized data, callers and
all existing fixtures/controls remain exact.


## Submission and transient-redraw local views

The complete ForParts wrapper, allocator and updater now name existing
local views. No arguments, function identities, public declarations,
request members or storage change. The two redraw loops' part_row locals
remain TWO separate lexical bindings, not a shared cached selector.

The wrapper's argument_cursor still advances BEFORE loading the previous
four-byte argument. It stops at the first negative part_index, with the
same unchecked mask_byte_index and shift. The clear_cursor still walks
backwards through all eight local bytes. start.blend_mode and the wrapper's
mode & 128 forwarding remain distinct from the allocator's selection & 1;
no selection repair, bound or standard-varargs replacement is introduced.

The allocator distinguishes request, raw_requests, request_offset and
full_mask_byte. active_flags, slot_cleared_flags, slot_flags,
selection_cleared_flags, selection_flags and row_cleared_flags name the
original separately declared flag intermediates, not new writes or
changed masks. flag_work still holds both the initial flags and the
later row-packed word. The deliberately mixed mode and sel scratch remain:
mode first carries slot flag bits THEN the mask-loop index; sel first
carries selection flag bits THEN the raw request offset. Their assignments,
declarations, register qualifier and do-while-zero block stay exact.

The request's flags are still published twice BEFORE the animation snapshot,
zero elapsed, doubled duration and full-word descriptors. Only then are
eight mask bytes copied or filled from FF, starting at raw_requests plus
request_offset plus ONE. This still overlaps the high flags byte. Keep the
first-free search, signed end-pointer comparison, partial writes and silent
full-queue result; no record atomicity or initialization policy is added.

The updater distinguishes request_index/request_offset, requests/request,
and mask_base_address. Its raw mask address is STILL table base plus one
plus the independently advanced 0x18 offset, not a substituted member.
Inactive slots still skip the whole tail; insufficient buffer space still
jumps to the tail and advances elapsed. Duration division retains both
integer quotients with their original promotions, truncation and zero case.
Only the start blend byte enters draw_tint; the end blend byte stays unread.

flag_work still shifts in place. request_animation_row captures bits 3..7
BEFORE the shadow-disable call; draw_animation_row copies it AFTER that
call. These are row selectors, NOT geometric part indices. saved_animation_row
is the one existing saved selector across BOTH loops. The original
allocation explanation changes only its eight backtick identifiers.
No lifetime merge, split, cached selector or scheduling assumption is added.

saved_tint, saved_animation_position and saved_shadow_state retain their
separate capture times. request_animation_position is the request halfword;
draw_position and restore_position remain separate signed temporaries
initialized from their original halfword views. The two loops still reload
current_animation_row per part, optionally replace sid only when the
respective saved/request row is nonzero, and call the same seek helper.
The transient draw still publishes mask address then masked-only flag,
calls Model_DrawSlot, and clears address then flag. The restore loop,
selector restoration, shadow restoration, whole-word tint restoration and
elapsed/deactivation tail remain in their exact order.

Only scoped bare locals and the existing allocation-comment identifiers
change in the two grouped C units. All other comments, types, declarations,
lifetimes, qualified members, casts, operators, constants, labels, calls,
regional macros, public headers, compiler profiles and other grouped
definitions remain. The all-parts wrapper and every caller stay unchanged.
The entire semantic ledger stays byte-identical: no identity promotion or
NOTES churn. No fixture/control/oracle/mutation edits, retired-fixture
revival, local cloud execution or new behavioral harness accompany these
local views.
