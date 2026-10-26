# Camera-relative coordinate-builder vector input

The third argument of `func_800580D4` is `SVECTOR *`. The body passes it to
the canonical `RotTransSV(SVECTOR *, SVECTOR *, PSXLONG *)` SDK function,
and the sole observed caller independently constructs the same record at
the start of its stack scratch block.

This recovers only the vector-input contract. The two leading selector
arguments retain their existing names/types and meaning; the output keeps
its existing compatible `GsCOORDUNIT` declaration.

## Packed caller storage

`func_80059B90` deliberately retains one byte array, not three new locals:

| Target offset | Existing SDK view | Extent |
|---|---|---|
| `0x00` | `SVECTOR` | `0x08` |
| `0x08` | `GsCOORDUNIT` | `0x50` |
| `0x58` | `MATRIX` | `0x20` |

The total remains `0x78`. The caller still clears just the vector prefix,
sets `vx`, passes selectors `0` and `15`, and uses the unit/matrix views
at their original offsets. Its three volatile halfword result reads and
the intervening fourth-halfword store remain in the original order.
Only the explicit `SVECTOR *` view moves from inside the callee to the
actual byte-storage boundary in this caller.

## Preserved transform and build boundaries

The callee removes one redundant SDK cast. Its slot selection, fallback
index, matrix calls, volatile slot-member reads, local unit lifetime,
output stores and parent pointer are unchanged. The separate flag scratch
and `PSXLONG *` boundary remain intact. No aliasing barrier is simplified.

Both functions in `model_slot_control.c` keep their contiguous
definitions and `gcc_2_8_1_g8_split` profile: the view-metric updater's
`0x19C` bytes followed by this helper's `0x1EC` bytes, totaling `0x388`.
No source move, section, address, layout, compiler setting or overlay
implementation changes.

Supported-GCC owner controls accept the precise SDK input and reject
byte/opaque function-pointer contracts. They also measure the real SDK
scratch extents. Whole-body/caller comparisons retain every token except
the argument type and boundary casts; complete executable matching is
required because earlier parameter/aliasing experiments in this grouped
unit were not byte-neutral.
