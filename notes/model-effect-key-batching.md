# Model effect-key batching

`Model_SetEffectKeyBatchingEnabled` is the former `func_8005F588`.
Inside its unchanged two-byte transfer guard, zero stores `-1` in
`gModel_nEffectKeyBatchOrdinal`; any nonzero input stores zero.
Scene controllers enable batching before submitting a preset orbit followed
by a paired endpoint request.

`Model_NextEffectKeyQueueMode` is the former `func_8005F564`. A negative
ordinal returns queue-begin mode zero without advancing. Otherwise it
postincrements the signed byte and returns whether the old value was
positive: the first enabled command begins, and later commands append.
This function has no transfer guard; adding one would change its contract.
The `int` definition and compatible `s32` declaration remain unchanged.

The other queue builders retain their separate signed/unsigned byte
reads, increments and casts instead of being rewritten to call this helper.
The ordinal is not the live key count, playback cursor or a bounded index.
No saturation, range check or new wrap policy is introduced.

`gModel_bEffectKeyPreparationPending` is the former `D_8009B079`.
Queue begin sets it after capturing the pose. When permitted, the updater
prepares every queued key, clears the pending byte and restores the batch
ordinal to `-1`. Explicit queue preparation and reset also clear the byte
and restore `-1`. Existing null/empty early returns and publication order
remain intact.

Both bytes retain their initialized `.sdata` definitions: `u8` zero for
preparation pending and `s8` minus one for the ordinal. Native bindings,
the contiguous 23-function state objects, compiler profiles and unrelated
guard bytes remain unchanged. Only obsolete Japanese bridges for the
renamed batch APIs retire; the provider and scene-controller caller use
the same canonical names.
