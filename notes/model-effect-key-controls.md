# Effect-key queue controls

Three adjacent resident helpers have high-confidence semantic names:

| Original address | Name | Existing interface |
|---|---|---|
| `0x8005FAE4` | `Model_ResetEffectKeyQueue` | `void (void)` |
| `0x8005FB08` | `Model_IsEffectKeyQueueEmpty` | `s32 (void)` |
| `0x8005FB14` | `Model_GetCurrentEffectKeyAddress` | `s32 (void)` |

The owner remains `model_transfer_state.h`. No type, return convention,
expression, branch, load/store order, or caller cast changes.

## Evidence and limits

Reset clears the current-key pointer and queued count, then writes
`D_8009B079 = 0`, `D_8009B07A = -1`, `D_8009B07B = 0`, and
`D_8009B07C = 0` in that order. Camera setup invokes it after initializing
the shared view. It resets queue control state, not the stored key records
or camera pose. The neighboring bytes retain their address-based names;
their reset values alone do not establish narrower semantic names.

The empty predicate tests only whether the queued count is zero. The scene
updater's negated call gates keyframe execution; camera and intro state
paths use the true result as the no-queued-keys condition. It does not
cross-check, repair, or clear a stale cursor.

The address getter returns zero for a zero count, otherwise the current
pointer's signed word representation. `Address` is deliberate: the
existing integer-address ABI and caller casts remain intact, including
the ownership control rejecting a `Key *` function-pointer return.
A positive count does not add a nonnull-pointer guarantee.

## Regional and object boundaries

The three definitions remain in the same positions of the contiguous
22-function `model_effect_state.c` object, with their existing profiles
and `0x24 / 0x0C / 0x1C` extents.

| Build | Reset / empty / address |
|---|---|
| North America | `0x8005FAE4 / 0x8005FB08 / 0x8005FB14` |
| Japan | `0x8004F534 / 0x8004F558 / 0x8004F564` |
| European base | `0x8004F8F0 / 0x8004F914 / 0x8004F920` |
| French, German, Italian, Spanish | `0x8004FAF4 / 0x8004FB18 / 0x8004FB24` |

Japan's reset and address helpers retain the existing local exports
`func_8004F534` and `func_8004F564`. Adapter macros change only their shared
left-hand names; right-hand export targets, regional inventories and symbol
bindings stay intact. Japan's empty query already uses the common shared
name and follows its semantic rename. Other regional bindings use the
three shared names at their original addresses.

The camera/intro native witnesses and positive/negative ownership controls
change only identifiers. Their complete oracles, events, signed word
contracts, and pointer-return rejection remain unchanged. No overlay
implementation, guest-pointer annotation, storage, data layout, compiler
setting, or generated usage report is changed.
