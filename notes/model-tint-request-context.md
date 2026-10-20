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

The original address-based C filename remains for existing matching
evidence. All seven native functions retain size `0x98`, the common source
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
