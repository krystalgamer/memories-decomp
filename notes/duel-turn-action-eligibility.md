# Duel turn-action eligibility

## Scope

This note documents the resident-code contract behind three related gameplay
rules:

- a monster played from the hand may attack during that same turn;
- a monster may attack at most once per turn; and
- a monster that has attacked may not change position again that turn.

The contract is carried by bit `0x4000` in `DuelCardRecord.flags`, named
`DUEL_CARD_FLAG_USED_THIS_TURN` in
[`duel_card_layout.h`](../src/game/duel_card_layout.h). The conclusions below
come from the matching record constructor and reset code plus the resident
assembly for scene states 5, 7, and 9. Literal `0x4000` uses on unrelated
scene, input, sound, and transfer state are excluded.

## Flag lifecycle

### Placement starts unused

Matching
[`Duel_SetupCardRecord`](../src/game/duel_card_record_lifecycle.c) constructs a
field record by assigning:

```c
p->flags = DUEL_CARD_FLAG_OCCUPIED;
```

This assignment replaces the complete flag halfword. In particular, a newly
constructed record starts with `DUEL_CARD_FLAG_USED_THIS_TURN` clear.

The ordinary hand-play path in `func_8001BD88` hands control to scene state 7
(`func_80019D18`). That state calls `Duel_SetupCardRecord` at `0x8001A610`.
The following placement presentation and stat-display work does not add
`0x4000` to the card-record flag halfword. The visible `0x4000` operations in
that state are operations on the separate global scene word
`D_8009B23A`; the later card-record store at `0x8001B098` clears
`DUEL_CARD_FLAG_FACE_DOWN` and does not set the used bit.

The result is an occupied field record whose per-turn action bit is still
clear when field-action control becomes available. There is no placement
marker that the attack selector interprets as summoning sickness.

### Battle sets used

Scene state 9 (`func_8001F55C`) marks the attacker by ORing
`DUEL_CARD_FLAG_USED_THIS_TURN` into its record flags. The two battle routes
perform the equivalent write at `0x800200B4` and `0x80020698`. The latter
route immediately refreshes the card object's visible flags with
`Duel_ApplyCardObjectFlags`.

Both writes also restore the saved `0x0A00` position/guardian bits needed by
the battle path. Neither clears the existing flag word before adding
`0x4000`, so the occupied attacker remains present and becomes unavailable
to the field-action filters.

### The next draw phase clears used

Matching `func_8001898C` in
[`duel_phase_entry.c`](../src/game/duel_phase_entry.c) scans all 30 field
records at the next draw-phase entry. It clears
`DUEL_CARD_FLAG_USED_THIS_TURN` on occupied records while preserving their
other flags. Unoccupied records have their complete flag word cleared.

This makes `0x4000` a turn-local action latch:

1. card construction starts it clear;
2. battle commits set it on the attacker; and
3. the next turn's draw entry clears it.

## Attack selection

The human field-action loop is scene state 5 (`func_8001D670`). Its monster
attack branch loads a 32-bit word beginning at record offset `+0x14`, so the
high half is `DuelCardRecord.flags`, and applies:

```text
(word_at_0x14 & 0xC8000000) == 0x80000000
```

On the flag halfword this is exactly:

```text
(flags & 0xC800) == DUEL_CARD_FLAG_OCCUPIED
```

The mask selects:

| Flag | Meaning required by the comparison |
| --- | --- |
| `0x8000` | occupied must be set |
| `0x4000` | used this turn must be clear |
| `0x0800` | defense position must be clear |

Thus an occupied monster in attack position is accepted only while its used
bit is clear. A freshly played attack-position monster satisfies the test
because placement did not set `0x4000`.

After source and optional target setup, the normal targeted-attack route
selects scene state 9 at `0x8001E65C`. The direct-attack route, reached when
the opponent's monster row is empty, selects the same state at `0x8001E670`.
Both therefore converge on the state that marks the attacker used.

The attack test itself does not include
`DUEL_CARD_FLAG_FACE_DOWN`. Position is the relevant gate here; battle setup
handles the card presentation separately.

## Position changes use the same latch

The L1/R1 field-action paths call `func_8001700C`. Its matching body accepts a
record only when:

```c
(flags & DUEL_CARD_FLAG_OCCUPIED) &&
!(flags & DUEL_CARD_FLAG_USED_THIS_TURN)
```

An accepted position change installs `func_8001D240` on the card object.
Matching [`func_8001D240.c`](../src/game/func_8001D240.c) toggles only
`DUEL_CARD_FLAG_DEFENSE_POSITION`; it does not set the used bit.

Consequently:

- changing position before attacking does not spend the attack;
- after battle sets `0x4000`, the same card fails the position-change
  eligibility helper; and
- the attack-source mask also rejects it, enforcing one attack per turn.

## Visible feedback and resulting rules

Matching
[`Duel_ApplyCardObjectFlags`](../src/game/duel_apply_card_object_flags.c)
maps `DUEL_CARD_FLAG_USED_THIS_TURN` to the dimmed card colour. The visible
dim state therefore reflects the same record bit used by attack and
position-change eligibility, rather than a separate UI-only status.

The combined static contract establishes:

| Record state | Attack eligibility | Position-change eligibility |
| --- | --- | --- |
| newly played, occupied, attack position | yes | yes |
| occupied, unused, defense position | no | yes |
| occupied after attacking | no | no |
| unoccupied | no | no |

This is the code-level basis for the game's absence of summoning sickness,
its one-attack-per-monster limit, and its prohibition on changing a
monster's position after that monster attacks.
