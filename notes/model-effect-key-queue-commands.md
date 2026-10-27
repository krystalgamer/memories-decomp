# Effect-key queue commands

`Model_ApplyEffectKeyQueueCommand`, originally `func_8005F91C` at
`0x8005F91C`, dispatches three resident queue operations. The existing
four-argument interface remains unchanged: signed command, two nullable
`ModelEffectEndpoint *` records, and the magnitude/control word.

| Command | Existing effect |
|---|---|
| `0` | Invoke the existing camera request helper with `-1`; seed the cursor at the array and zero the count; conditionally capture the starting pose and enter append |
| `1` | Append one pair below the ten-key capacity; retain null-record handling and initialize the key's magnitude/progress/duration/readiness fields |
| `2` | Walk the array through `Model_PrepareEffectKey`; then clear the preparation byte and set the counter to `-1` |

Preset, noise, sequence and orbit helpers select begin/append operations;
intro paths also issue command zero with null records. The explicit third
operation is present in the dispatcher body. The name is deliberately
broader than append or duration calculation.

## Distinct state boundaries

Command zero is not `Model_ResetEffectKeyQueue`. With both records null
and a final word of `0` or `0x4000`, it returns after seeding the cursor and
clearing the count, before capture or preparation-byte publication. The
cursor can therefore remain at the array while the count is zero.

Appending keeps the original packed eight-byte copies and null behavior:
a null record clears only its kind, not the other requested components.
The second-record arm and readiness stores retain their original shape.
The magnitude word keeps its existing narrowing.

Preparation retains the byte cursor's `0x28` stride and live count test
after every preparer call. It does not snapshot the count, require every
input key to become ready, or replace the preparer's distinct global guard.
The shared two-flag gate and invalid-command no-op remain unchanged.

## Endpoint transport and preparation local views

The four contiguous pair/shake/sequence/dispatcher definitions now
name only their existing local views. first_endpoint/second_endpoint
retain signed negative-index-to-null selection BEFORE the state guard.
No new upper bound or endpoint validation. ordinal_or_mode deliberately
remains ONE binding for the signed byte ordinal THEN begin/append mode.
The pair's postincrement is not replaced by the parallel helper forms.

shake_halfwords stays ONE aligned s16[4] initializer, with the original
input narrowing, zeros and kind5. The SAME array address is passed for
both requested endpoints. Keep separate signed and unsigned ordinal
reads, including unsigned_ordinal capture on the negative/disabled path,
then the original captured-byte increment. No duplicated record, packed
local, helper extraction, new noise policy or byte-wrap normalization.

The sequence's batch_ordinal remains s8 and queue_mode remains s32.
Batch-state computation/increment STILL precedes count==0 return.
Each iteration sends the current records, advances pointers BEFORE
testing NEXT kinds, and preserves the first-stream goto short-circuit.
Keep explicit signed-halfword kind loads, negative-count continuation,
positive-count-only decrement and the later forced append mode1.
No first-record terminator test, early empty-call suppression or bound.

The dispatcher keeps one=1, key and all goto labels unchanged.
Begin still requests camera-1, seeds cursor/count, then may return for
two null records and magnitude0/0x4000 BEFORE pose capture or pending
publication. This is not the complete queue reset and does not gain
an unconditional pending clear. Invalid commands remain no-ops.

append_key_bytes names the existing OTHERWISE UNUSED byte binding;
its declaration and assignment stay. Append retains packed eight-byte
copies, null-kind-ONLY clears and BOTH readiness-store arms. queued_count
is reloaded AFTER those copies. Preserve magnitude narrowing, zero
duration/progress stores and final count publication, plus capacity10.
No clearing of stale requested components or new initialization policy.

prepare_key_bytes and next_key_bytes remain SEPARATE byte cursors.
The next cursor advances by0x28 BEFORE each preparer callback; key_index
still tests the LIVE count after every call, not a captured count.
The preparer's separate global-key guard is not bypassed. Pending clears
and ordinal-1 publication remain AFTER the whole walk. No early cursor
publication, array bounds repair or guarantee that all keys become ready.

Only scoped bare locals in these FOUR complete definitions change.
Arguments, member tokens, comments, types, constants, expressions,
scopes, labels and order remain. All other grouped definitions, headers,
SDK declarations, callers, fixtures and controls stay exact. Existing
queue controls cover headers/ABI, not these local names. No new harness
or local cloud-witness execution. The source/regional section below
records the EARLIER function-identity rename; this later local pass
changes no function identity or caller.

## Source and regional boundaries

Only the function identifier changes in production bodies and callers.
Existing branches, labels, unused locals, field writes, callback ordering,
nullable pointer types and arithmetic are retained.

The function stays in the contiguous 23-function `model_effect_state.c`
object under `gcc_2_8_1_g8_split`, with its `0x1C8` extent:

| Build | Address |
|---|---|
| North America | `0x8005F91C` |
| Japan | `0x8004F36C` |
| European base | `0x8004F728` |
| French, German, Italian, Spanish | `0x8004F92C` |

All manifests, source paths, profiles, stored layouts and regional
addresses remain intact. No overlay implementation, selector enum,
validation behavior, guest-pointer cast or integer-address ABI changes.
Strict owner controls cover the exact signature and nullable command calls;
whole-executable matching remains the code-generation gate.
