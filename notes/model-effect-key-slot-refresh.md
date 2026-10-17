# Slot-derived effect-key values

`Model_RefreshEffectKeySlotValues`, originally `func_8005FB30` at
`0x8005FB30`, refreshes the resolved slot values of at most two requested
effect-key endpoints. The name and canonical record ownership follow the
existing `Key.requested[2]` / `Key.resolved[2]` layout and the preparer's
call before its progress/readiness work.

## Key selection and exact effects

An explicit key wins over the global current key. A null argument captures
the current pointer once; if both are null the existing behavior is a
no-op. This fallback does not inspect the queue count and must not be
replaced by the count-gated integer-address getter.

Only signed kinds 2 and 3 select slot values. In those cases the first
requested halfword is the signed model-slot index, and the copier writes
all four halfwords into the corresponding resolved record at `0x10` or
`0x18`. Other requested kinds, request records, timing, readiness and
control state are unchanged.

The selected key base remains captured across copier calls, while each
next requested record is read live. An external call can change the
global cursor or the next input record without changing the captured
destination. No records are prefetched or redirected through a newly
loaded cursor.

## Canonical record and access widths

The refresher now uses `ModelEffectEndpoint`, the actual requested-record
type already embedded in `Key`, instead of the sole legacy
`ModelTransferItem` view. The unused legacy declaration/assertions are
removed. No stored layout changes.

Both old fields were **signed** halfwords. Explicit `s16` views of the
packed endpoint's `kind` and `x` retain the original `lh` accesses, rather
than packed byte reads. A valid `Key` is halfword-aligned and the two
requested records have an eight-byte stride, preserving the existing
alignment requirement, now asserted by the key owner. The resolved fourth
word remains opaque.

## Verification and regional boundaries

The independent ILP32 witness executes the actual extracted production
function at `-O0` and `-O2`, sweeping every 16-bit kind in each lane. Its
131,078 cases also cover explicit/global/null key selection, zero-count
fallback, both four-word destinations, full guarded-object preservation,
and mutation of the cursor/next record during copying. Narrow-stride and
ignored-explicit-key source mutations must fail.

The same grouped object/order/profile and `0x94` extent remain:

| Build | Address |
|---|---|
| North America | `0x8005FB30` |
| Japan | `0x8004F580` |
| European base | `0x8004F93C` |
| French, German, Italian, Spanish | `0x8004FB40` |

No source move, overlay implementation, callback sequence, selector
validation, integer-address ABI, compiler setting or data allocation is
changed. Full executable matching remains the code-generation gate.
