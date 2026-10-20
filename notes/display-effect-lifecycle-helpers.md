# Display-effect lifecycle, object rows and step dispatch

Four existing helpers complete the lifecycle interface beside
`DisplayEffect_ReleaseRecord`. The existing step table receives a name
without changing its initializer, dispatch expression or narrow-view casts.

| NA identity | API | Exact role |
|---|---|---|
| `0x80039F1C` | `DisplayEffect_MarkInitialized` | Mark state bit `0x80`, returning zero first and one thereafter |
| `0x80039F44` | `DisplayEffect_ResetRecordState` | Partial scalar reset and randomized delay seed |
| `0x80039F90` | `DisplayEffect_ReleaseObjectRow` | Release/null exactly three supplied slots, descending |
| `0x80039FF8` | `DisplayEffect_FinishStepWhenIdle` | Finish the step only when the low two activity bits are clear |
| `0x80090F68` | `gDisplayEffect_apfnSteps` | Existing initialized display-effect step table |

## Marking is not a readiness wait

The mark helper reads the byte at `DisplayEffectState.state`, also
`MenuRecord.display_effect_step` at `+0x33`. If bit `0x80` is clear, it
stores the original byte OR `0x80` and returns zero. Otherwise it returns
one without that store. All other state bits remain.

Zero means the caller should run its existing initialization branch; it
does not mean an error or a deferred not-ready result. Existing consumers
can initialize and advance their transition in the same invocation.
Their actual branches, updates and call order are not changed.

## Reset has a precise partial write set

The reset helper retains this order:

1. `field_34 = 0x68`;
2. `field_32 = 0`;
3. `state = 0`;
4. `field_31 = 0`;
5. `field_36 = 0xB2`;
6. `field_3E = (rand() & DISPLAY_EFFECT_DELAY_MASK) + DISPLAY_EFFECT_DELAY_BASE`.

The existing constants are mask `0xFF` and base 60. Every invocation
consumes that RNG call, even when earlier values happen to match. There
is no initialization guard, cached delay, changed RNG sequence or
idempotent fast path.

The signed record marker, all object rows, side/mode byte, other tail
fields and both shared selectors remain untouched. Producers retain their
later ID/slot/step stores. This is not a complete arena or object reset.

## Row release is exactly three slots

The row helper keeps its original `void **` parameter and signed index
local. It walks 2, 1, 0, calling `DisplayObject_ReleaseIfPresent` and then
storing null for each slot, including slots that were already null.

Callers keep the explicit views of the integer grid rows. There is no
new count argument, null-array guard, skip path, pointer-grid type or
all-row teardown. The existing fixed-record release calls it for row
zero; transition handlers retain their independent secondary-row cleanup.
The enclosing arena is not freed.

## Idle step completion does not release

When `field_32 & 3` is nonzero, the finish helper makes no stores. Once
those activity bits clear, it ORs `0x10` into the existing flags and
writes literal state zero. It does not initialize the latch, alter other
fields, release objects or reset selectors.

Idle here means only the original per-record resource activity test,
whose writers are `DisplayEffect_UpdateMenuRecord`. It is not global
device or frame-loop inactivity. Keep the raw mask and every retained flag.

## Existing table and ABI views

The NA table has thirteen initialized entries. Its first two entries
reference the same idle-finish helper through the existing casts from
the narrower `DisplayEffectState *` signature. The VRAM helper keeps its
own existing cast. Every other callback remains in its original position.

`DisplayEffect_ProcessMenuRecords` still skips negative markers, performs
the conditional variant update, and only dispatches a nonzero step through
`step & 0x1F` using its existing local table cursor. Keep the declaration
unsized: the mask's domain is not proof of 32 valid entries.

`DisplayEffectState` remains the existing narrow view. Its padding,
offsets, width and helper signatures are not replaced by `MenuRecord`.
The row helper's void-pointer ABI and the table's full-record callback
ABI likewise remain distinct.

The selected-record and transition contracts are documented in
[text-display-effect-states.md](text-display-effect-states.md).
The resource acquisition step, shared cache and row-blend helper are covered in
[display-effect-resource-acquisition.md](display-effect-resource-acquisition.md).

## Native identities and acceptance

| Identity | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Mark initialized | `0x80039F1C` | `0x8003959C` | `0x8003A0AC` | `0x8003A2AC` |
| Partial reset | `0x80039F44` | `0x800395C4` | `0x8003A0D4` | `0x8003A2D4` |
| Release row | `0x80039F90` | `0x80039610` | `0x8003A120` | `0x8003A320` |
| Finish step | `0x80039FF8` | `0x80039678` | `0x8003A188` | `0x8003A388` |
| Step table | `0x80090F68` | `0x80090E18` | `0x80091F80` | `0x80092394` |

The French native table is not the unrelated
`D_80090F68_French` alias at the NA-number address. All unrelated native
bindings and classification stay intact.

Every complete compiler/MASPSX output is compared after reversing only
five identifiers, without filename, instruction, directive or relocation
filtering. Full clean retail matching remains mandatory. All non-name
inventory fields/order, profiles, manifests, source paths/grouping,
layouts and initialized entries remain. The complete accepted ledger gains
five evidence/caveat rows. Tests, controls and captures do not change;
regional/cloud validation remains PR-side.

The original five-name proof precedes accepted #7633 integration.
Its model-tint identity, native bindings and complete ordered ledger edits
are preserved. The combined-tree comparison aligns only that accepted
name in the saved baseline, then reverses the five lifecycle identities;
all whole outputs remain in the comparison and the tree gets a fresh
clean retail gate.
