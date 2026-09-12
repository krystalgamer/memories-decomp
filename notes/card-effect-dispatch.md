# Card-effect dispatch contract

`DuelEffect_StartCardEffect` is the sole producer of the state consumed by
`DuelEffect_UpdateCardEffect`. It accepts card IDs in the two fifty-card
effect blocks (`301..350` and `651..700`) plus Dark Magic Ritual (`721`).
Unsupported IDs leave the current state unchanged.

For the first block, the normalized index is `card_id - 301`. For the second
block it is `card_id - 601`, placing it immediately after the first fifty
entries. Dark Magic Ritual uses the final index, `100`. The producer stores:

| Address | Name | Contract |
| --- | --- | --- |
| `0x8009B1A8` | `gDuel_sCardEffectIndex` | Normalized index into the 104-entry effect-to-group table |
| `0x8009B1D2` | `gDuel_wEffectCardID` | Original card ID consumed by card-specific handlers |
| `0x8009B220` | `gDuel_wCardEffectFlags` | Active, handler-selection, initialization and per-handler phase flags |

The caller selects the first or second handler with a Boolean argument.
`DuelEffect_StartCardEffect` writes `DUEL_CARD_EFFECT_FLAG_ACTIVE` and adds
`DUEL_CARD_EFFECT_FLAG_SECOND_HANDLER` for the second phase.

`DuelEffect_UpdateCardEffect` runs only while the active bit is set. It maps
`gDuel_sCardEffectIndex` through `gDuelEffect_abGroupByEffectId`, multiplies
the group by two, optionally selects the second entry, and invokes that slot
from `gDuelEffect_apfnGroupHandler`. Its return value is the handler-updated
`gDuel_wCardEffectFlags`, so callers use zero as completion without a separate
status translation.

`DuelEffect_MarkInitialized` owns bit `0x80`: the first call sets it and
returns zero; later calls return one. `DuelEffect_ClearCardEffect` clears the
whole state word and fills the no-op group-table entries. The lower phase bits
remain handler-specific and are not assigned broader names.

The presentation candidate at `0x80019608` starts phase one before its card
animation and phase two afterwards. The duel update candidate at `0x80024200`
blocks further duel work while either the card-effect dispatcher or the
separate display-effect state machine remains active.
