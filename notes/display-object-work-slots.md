# Shared display-object work slots

`display_object_work_slots.h` supplies the single declaration of
`D_800E9EF0` to three matching resident translation units and the retained
`DuelScene_UpdateExodiaResult` candidate. It is an incomplete array of `DisplayObject *`,
not a new allocation or an object pool. The linker address and assembly-owned
storage remain unchanged. No new semantic symbol name is assigned.

## Evidence and consumers

| Consumer | Evidence for the contract |
|---|---|
| `DuelScene_UpdateCardUse` | Reads slots 0 and 1 as display objects, installs objects returned by `func_800291E0` and `func_80019564`, modifies their render attributes, flags, scale halves and colour word, and releases both with `func_8004036C`. |
| `Main_RunTrade` | Stores the object returned by `func_800400AC` in slot 0, animates that same object, then releases it and clears the slot. The former `gTradeObj` macro was only a cast of the table's first word, not a distinct symbol. |
| `func_8002CB50` | Copies exactly five words starting at this address and appends a zero word to the destination. It does not allocate or release the objects. |
| Retained `DuelScene_UpdateExodiaResult` | Stages five card display objects using the three-byte pose records at `D_80090918`, then indexes the slots to read each object's `+0x30/+0x32` position for a sparkle effect. |

The five-slot count describes this shared work window, not the length of the
display-object pool. The candidate's initialization and sparkle loop both
use that count, as does the copier. The following word, `D_800E9F04`, is
separately cleared by the presentation and is not a sixth pointer slot.
The header checks the 20-byte pointer span and every newly named object-field
offset used by these consumers.

`duel_screen_tables.h` now records the corresponding source-table shape.
`D_80090918` contains five `DuelExodiaCardPose` records selected by card IDs
`0x11` through `0x15`. Each record maps the card to a work-slot index and
stores the x/y values with the `0x1A`/`0x1E` biases that `func_80018FEC`
removes before starting motion. A separate trailing byte preserves the
original table's 16-byte extent and the address of the following duel-result
sprite table. The retained candidate keeps its raw-byte view for now because
its documented near-match depends on the existing `anim * 3` expression.

## Preserved code-generation boundaries

The declaration deliberately stays incomplete. `Main_RunTrade` requires
split absolute `%hi/%lo` addressing, with the high half kept across the tail.
A scalar pointer or a `.data` annotation is not substituted for its array
view. The other consumers now share that declaration rather than keeping
private `u8 *[]`, `int []` and `u8 []` views.

The copier keeps its `s32 *` destination ABI and writes each pointer as an
integer word before the terminating zero. Its source cursor alone is now
`DisplayObject **`; this does not impose a pointer-array type on callers'
destination buffers.

The duel animation uses the existing `DisplayObject` members instead of
raw `+4`, `+8`, `+0xC`, `+0x44` and `+0x46` addressing. The halfword stores
retain their unsigned views, and the fade reads only the low byte of the
colour word before writing the replicated word. Repeated table loads,
statement order and the load-bearing tentative `D_8009B150` definition
remain intact. Existing register constraints are unchanged, not extended.

The candidate's slot cursors are typed and its two position reads use the
same existing members; unrelated byte views and its known near-miss behavior
remain intact. It now obtains `D_800E9EF0` directly from the header, so only
that obsolete private-extern entry is removed from its schema-2 dependency
map. Its object fingerprint remains
`5b8db9b3dbaf377624bf31a2890b934f9fff263f86a4f6db4db586c8d30d6615`;
no target bytes, compiler profile, matching status or assembly fallback
changes.
