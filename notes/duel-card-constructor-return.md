# Card constructor return view

The factory at North American `0x80017F04` returns its existing
`DuelCardDisplayObject *` local directly. Both ordinary and ambient
owner declarations expose that same actual card-rendering view, not
an untyped byte pointer or a claim about the complete display record.
`Duel_CreateHandCardDisplayObject` already returns this view and no longer needs to cast
the result back to it.

The ambient caller still passes only `(u8 *)card`, retaining its
old-style declaration and incoming `$a1/$a2` convention. Explicit
canonical-object views at hand/placement/ritual clients remain.
Mixed-use byte locals and debug globals keep their accepted views,
with explicit byte conversions at the factory boundary. No argument
width, field, layout, global type, allocation, helper order, profile
or metadata changes are included.

The accepted ritual observer uses the same typed return without
changing any event, guard, numeric or mutation oracle. New supported
GCC controls accept the actual view and reject byte/canonical return
substitutions in the appropriate declaration arms.

The native witness executes only the complete actual factory with
observing pool/helper stubs. It uses two fully backed canonical/card
view unions, guarded actual card/data/stat storage, every coordinate
halfword pattern and all 30 record/722 stat indices over 131,072 cases
at each of O0/O2. Fixed-offset byte expectations check the stores,
pointer identity, exact helper order, opaque-helper mutations and
unchanged surrounding storage. Wrong-return/index/position/order
and guard-store negatives must fail. It does not execute the real
allocator, the two state helpers, callbacks, graphics or engine.

Complete factory/caller compiler/MASPSX equality and a fresh clean
whole-executable exact match remain the acceptance gates.
