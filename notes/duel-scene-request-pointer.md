# Duel scene request pointer

Every observed non-null write to `D_8009B17C` traces to
`DuelEffect_AllocateRequest`, which gets a `DuelEffectRequest` from the
eight-entry `0x20`-byte pool and returns its byte view. The other write
clears the handle. No overlay implementation references this global.
Its stored pointee therefore uses `DuelEffectRequest *G32`; the pointer's
four-byte width, eligibility/placement and observed lifetime are unchanged.

Some field-effect handlers retain `DuelFieldEffectObject` or
`DuelEffectObject` partial views. Those names are not allocation evidence:
their values stored in this global also come from the request allocator.
Through this handle they access only the request prefix, including the
payload halfwords, flags at `0x1C` and progress byte at `0x1D`. Other uses
of the larger views really do refer to display objects, so the shared view
types and persistent arrays are not globally changed.

The allocator's byte-return API is deliberately retained. Actual request
locals publish directly; byte/partial-view producers keep explicit casts
at the stored-handle boundary. The two direct byte-index status reads use
the canonical unsigned `progress` (formerly `field_1D`), avoiding scaled
request-array indexing. Its handler-defined counts and separation from
finished/ACTIVE status are documented in the
[resident/bank handoff](duel-effect-request-handoff.md).
All other partial casts, capture/store/call order, masks and scalar math
remain unchanged. The battle's now-unused byte-publication macro is removed.

The ritual fixture changes only its handle declaration, setup conversion
and address-comparison view. It still uses the same request buffer and
event/address oracle; no expected event or scenario changes. No layout,
allocator, payload naming, allocation/null-safety guarantee or historical
source fingerprint is altered.
