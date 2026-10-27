# Attack-trap candidate scratch records

`Duel_SelectAttackTrap` uses the reusable staging buffer at `0x8015C424`
through a high-base bias of `0x18000`. The small displacement `0x3C68`
then reaches the candidate table at buffer offset `0x1BC68`.
`DuelAttackTrapScratchBlock` in `src/game/duel_card_staging.h` is an offset
view of that region, not a claim about the whole allocation's lifetime.

## The two six-entry arrays

| Relative to the biased base | Member | Evidence |
|---|---|---|
| `0x3C68..0x3C73` | `table.entries.card_ids[6]` | The initial loop clears six halfwords; occupied trap cards with IDs `681..686` store their ID at `card_id - DUEL_ATTACK_TRAP_FIRST_CARD_ID`. Zero means absent. |
| `0x3C74..0x3C87` | Opaque padding | No interpretation or initialization is added. |
| `0x3C88..0x3C93` | `table.entries.record_indices[6]` | For the same trap type, the scan stores the display object's `field_6A` record index as a halfword. Selection later reads the chosen index's low byte. |

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

The high bias, small displacement, byte cursors and biased word selectors
remain separate. A measured direct-array-base candidate failed the
whole-image size gate (`0x1D07E8` instead of `0x1D0800`). The accepted view
keeps the retail cursor arithmetic and typed word slots, rather than
cancelling the index bias or rebasing every local to an array.

The reverse cursor advances by halfwords, so the block's alignment is
asserted to match `u16`. The final record-index read takes a byte view of
the named word member to preserve retail's `lbu`, despite the producer's
`sh`. The array offsets, total table/block sizes and alignment are checked.

The attacker's input is now `const DisplayObject *`. Its only use is
`field_6A`, which selects the attacker's card record for stat calculation;
the battle caller already has a `DisplayObject *` and no longer casts it
down to bytes. Thresholds, ordering, fallback-trap handling and selected
outputs do not change.

[retired `test_attack_trap_layout.py`](https://github.com/krystalgamer/memories-decomp/blob/4b5667dae10ffb9c72df1a242ee060e654214ae1/tools/project/tests/test_attack_trap_layout.py) compiles the real layouts
with GCC 2.8.1, checks both array endpoints and their biased word aliases,
and accepts the object-pointer contract while rejecting the old byte
contract. Full North American executable matching remains the acceptance
gate.
