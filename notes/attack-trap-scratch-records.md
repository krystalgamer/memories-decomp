# Attack-trap candidate scratch records

`Duel_SelectAttackTrap` uses the reusable staging buffer at `0x8015C424`.
Its candidate table lives at buffer offset `0x1BC68`, which retail reaches
as a `0x18000` high part plus a `0x3C68` load displacement. The table is the
`attack_traps.table` member of the buffer's `DuelCardStagingBuffer` union in
`src/game/duel_card_staging.h`; it reuses the storage of the sorted
combined-deck IDs once deck population is done with them.

## The two six-entry arrays

| Relative to `0x18000` | Member | Evidence |
|---|---|---|
| `0x3C68..0x3C73` | `table.entries.card_ids[6]` | The initial loop clears six halfwords; occupied trap cards with IDs `681..686` store their ID at `card_id - DUEL_ATTACK_TRAP_FIRST_CARD_ID`. Zero means absent. |
| `0x3C74..0x3C87` | Opaque padding | No interpretation or initialization is added. |
| `0x3C88..0x3C93` | `table.entries.record_indices[6]` | For the same trap type, the scan stores the display object's `kind_index.card_record_index` as a halfword. Selection later reads the chosen index's low byte. |

The `0x20` separation is independently visible in both producer and
consumer arithmetic: the store subtracts `0x299` rather than the first
trap ID `0x2A9`, and the selected read adds `0x10` halfword slots. Thus
word slots `0..5` are IDs, `6..15` remain opaque, and `16..21` are record
indices. `DuelAttackTrapTable.words` is the matching halfword view of
that same `0x2C`-byte storage, not another table or allocation.

The candidate scan writes indices only when a matching ID is found. The
initial loop still clears only IDs, and selection still requires a
nonzero ID; stale index entries and all padding remain untouched.

## Exact access shape and input type

The clearing loop and the final selection read index the union member
directly; GCC splits the `0x1BC68` member offset into the retail
`lui 0x1` / `ori 0x8000` high part and `0x3C68` displacement. Two shapes
remain explicit because the member form changes codegen:

- The candidate scan's two stores compute `slot offset + base` as an
  integer and cast it to `DuelCardStagingBuffer *`: retail emits
  `addu offset, base`, while indexing the member swaps the operands.
- The reverse threshold scan keeps a byte cursor and a local holding the
  `0x18000` high part, reading `(DuelAttackTrapTable *)(q + high + 0x3C68)`.
  Retail keeps that high part in a register across the loop; a plain member
  read rematerialises `lui`/`ori` on every iteration.

The final record-index read uses the table's byte view to preserve retail's
`lbu`, despite the producer's `sh`. The table offsets and size are checked.

The attacker's input is now `const DisplayObject *`. Its only use is
`kind_index.card_record_index`, which selects the attacker's card record for stat calculation;
the battle caller already has a `DisplayObject *` and no longer casts it
down to bytes. Thresholds, ordering, fallback-trap handling and selected
outputs do not change.

[retired `test_attack_trap_layout.py`](https://github.com/krystalgamer/memories-decomp/blob/4b5667dae10ffb9c72df1a242ee060e654214ae1/tools/project/tests/test_attack_trap_layout.py) compiles the real layouts
with GCC 2.8.1, checks both array endpoints and their biased word aliases,
and accepts the object-pointer contract while rejecting the old byte
contract. Full North American executable matching remains the acceptance
gate.
