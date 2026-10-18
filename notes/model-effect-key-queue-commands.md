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
