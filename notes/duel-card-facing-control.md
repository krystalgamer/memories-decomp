# Card-view facing control

The card-rendering view's +0x10 member has explicit `word` and `callback`
forms. Its four-byte union preserves the raw control bits and stores the
actual `void (DisplayObjectConfig *, u32)` function pointer through `G32`.
The card-display factory and world-space field-card factory both install
`DisplayObject_SetResourceVariantFromSign` via the callback form.

This is deliberately not a callback-only interpretation or a type change
to canonical `DisplayObject`. `DisplayObject_ProjectRotationFacing` treats
zero as disabled, a positive nonzero word as mirrored-reprojection control,
and a negative word as a callback address before applying its area test.
`Library_CreateFadingCardOutline` actually stores 1, even on list/class 6.
List membership therefore does not prove callback-only ownership.
The canonical word, positive control values and generic projection code
remain untouched; the narrower view retains all values through `word`.

The signature comes from the actual sign-bit helper's owner, not a
guessed generic callback: its config view names the +0x69 byte, and its
second parameter remains the full `u32` result bits. No generic callback
ABI or +0x4C interpretation is changed. The source helper still performs
only the sign-bit store.

Supported GCC probes check both +0x10 overlays, four-byte widths, complete
partial-view size and downstream offsets, accepting the actual callback
and rejecting alternate first/second parameter and return types.
The native witness round-trips zero, positive and high-bit raw words,
then calls only the complete actual sign-bit setter through the stored
`CALL32` view. Two fully backed canonical/card unions, four upper-word
patterns and every low halfword yield 524,288 calls at each of O0/O2.
Fixed-offset byte expectations protect both full records and their guards.
Wrong/inverted sign, wrong selector, lost flags and guard writes must fail.
No generic projection, GTE, renderer, allocator or engine is executed.


## Pre-mirror backface cache

The card-only DuelCardDisplayObject byte at +0x69 is is_back_facing.
Both card factories retain their zero initialization and install the
same sign-bit callback. DisplayObject_ProjectRotationFacing calls that
callback with the ORIGINAL signed NCLIP area before it mirrors X/Z and
reprojects. The callback receives those bits as u32 and stores value >> 31,
so it writes exactly 0 or 1, not an arithmetic-shift 0xFF mask.

Duel_DrawCardFrame resets the generic +0x69 byte before projection. After
the callback, zero permits the front frame/art/stat path; nonzero selects
the back-frame UV and CLUT. The cache describes pre-mirror facing, not
the record's face-down flag, the final mirrored area or persistent card
state. All clears, early returns, callback gating and drawing order stay.

DisplayObjectConfig calls the same physical byte resource_variant:
that resource selector view and the card-only cache have distinct names.
The canonical pool view remains field_69 because card-list selected-icon
indices, result sprite kinds and other users reuse it. The row-set's
reused card-display pointer receives no new card-only interpretation.
No generic callback signature, raw +0x10 word, flag, stored pointer type,
padding, stale-value policy or literal control changes.
