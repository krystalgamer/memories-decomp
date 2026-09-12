# Magic-effect group dispatch

The card-use runtime does not interpret card text. `func_80026BA4` maps the
selected card ID into the 104-entry effect-ID range, and `func_80026B34` uses
`gDuelEffect_abGroupByEffectId` to select one of fifteen two-handler slots in
`gDuelEffect_apfnGroupHandler`. Group 14 is allocated but unused by every
effect ID in the table.

The card membership already documented in
[`research/the-game.md`](research/the-game.md) and the handlers' complete
field, life-point, ritual, and presentation behavior establish these names:

| Group | Cards | Application handler |
| ---: | --- | --- |
| 0 | equips, traps, and effect IDs without play-time work | `func_80024E4C` |
| 1 | six terrain cards | `DuelEffect_ApplyTerrain` |
| 2 | five LP recovery cards | `DuelEffect_ApplyLifePointRecovery` |
| 3 | five direct-damage cards | `DuelEffect_ApplyDirectDamage` |
| 4 | Dark Hole; Dragon Capture Jar | `DuelEffect_ApplyBoardDestruction` |
| 5 | seven type/threshold monster-removal cards | `DuelEffect_ApplyMonsterRemoval` |
| 6 | Stop Defense | `DuelEffect_ApplyStopDefense` |
| 7 | Raigeki | `DuelEffect_ApplyRaigeki` |
| 8 | Dark-piercing Light | `DuelEffect_ApplyDarkPiercingLight` |
| 9 | Spellbinding Circle; Shadow Spell | `DuelEffect_ApplyStatPenalty` |
| 10 | Swords of Revealing Light | `DuelEffect_ApplySwords` |
| 11 | Cursebreaker | `DuelEffect_ApplyCursebreaker` |
| 12 | all 24 rituals | `DuelEffect_StartRitual`; `DuelEffect_ApplyRitual` |
| 13 | Harpie's Feather Duster | `DuelEffect_ApplyHarpiesFeatherDuster` |

## Shared reveal callback

`DuelEffect_ApplyDarkPiercingLight` and `DuelEffect_ApplySwords` both assign
`DuelEffect_UpdateRevealCard` to occupied field-card display objects. The
callback clears the temporary display-marker state, reapplies the card-object
flags, and removes itself. This is presentation work shared by the two reveal
effects, not another dispatch-table group.

## Evidence boundaries

The group names come from exclusive card membership where a group contains one
card, and from the complete card-ID branch or data table where a group contains
several cards. `DuelEffect_ApplyRitual` remains unmatched assembly, but its
identity is constrained by the group-12 slot and its calls that request the
three `Duel_CheckRitual` output records, remove those tributes, and stage the
result monster. The address-based source and target filenames for integrated
candidates remain unchanged.

The Swords countdown and lifetime are documented separately in
[`swords-of-revealing-light.md`](swords-of-revealing-light.md).
