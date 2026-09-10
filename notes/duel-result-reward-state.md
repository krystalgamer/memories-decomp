# Duel result display and reward state

## Scope

The pointer at `D_8009B1E8` refers to the transient result-state record built
by the end-of-duel state machine. Two matching consumers already established
its display-object and score prefix. The unmatched `func_800218F0` extends
the same record through offset `+0x3D`, covering page selection, rank axis,
starchip prize, and the selected card drop.

`DuelResultDisplayState` in `duel_result_display.h` now models the complete
`0x40`-byte extent used by the result flow.

## Layout

| Offset | Field | Evidence |
|---:|---|---|
| `+0x00` | `root` | `Duel_ShowResultPage` passes it to the display-mode setter |
| `+0x04` | `children[10]` | Page switching toggles each child's renderable bit; the result constructor clears ten slots |
| `+0x2C` | `side_scores[2]` | `Duel_CalcRankScore` initializes both to 50 and accumulates all rank adjustments |
| `+0x34` | `page_text_ids[3]` | Score setup stores `0x44`, result variant, `0x45`; page display passes the selected byte to `TextBox_Create` |
| `+0x37` | `page_index` | Initialized to zero, cycled modulo three by directional repeat, passed to `Duel_ShowResultPage` |
| `+0x38` | `rank_tier` | Derived as distance from the D rank, producing `0..4` |
| `+0x39` | `is_tec_rank` | Set when the original score is below 50 |
| `+0x3A` | `starchip_prize` | Set to `rank_tier + 1`, producing `1..5` |
| `+0x3B` | padding | No independent field semantics established |
| `+0x3C` | `dropped_card_id` | Receives `Duel_SelectCardDrop` and is later passed to `Duel_AwardCard` |
| `+0x3E` | padding | Rounds the observed record to `0x40` bytes |

## Page selection

The result state starts on page zero. Directional repeat changes the byte at
`+0x37`:

- one direction increments it;
- the opposite direction decrements it;
- values wrap within `0..2`.

The state machine then calls matching `Duel_ShowResultPage`. That function:

1. applies the page index to the root display object;
2. makes all result children renderable for page zero and non-renderable for
   pages one and two; and
3. creates a text box from `page_text_ids[page_index]`.

The page-index name therefore follows both its bounded input behavior and its
matching consumer, rather than only the three stored text IDs.

## Rank axis and tier

After `Duel_CalcRankScore` produces each side's score, the winner's score is
converted into two fields:

1. if the score is below 50, `is_tec_rank` is set and the value is mirrored
   as `99 - score`;
2. values are clamped below 100;
3. `(value - 50) / 10` becomes `rank_tier`.

The result is:

| `rank_tier` | POW rank | TEC rank | Starchips |
|---:|---|---|---:|
| 0 | D-POW | D-TEC | 1 |
| 1 | C-POW | C-TEC | 2 |
| 2 | B-POW | B-TEC | 3 |
| 3 | A-POW | A-TEC | 4 |
| 4 | S-POW | S-TEC | 5 |

The stored tier is not the ten-rank index by itself; the axis byte supplies
the POW/TEC half.

## Drop-pool selection

The normal player-win path derives the three drop-pool indices from the same
fields:

```text
rank_tier < 3       -> pool 1 (B/C/D)
rank_tier >= 3 POW  -> pool 0 (S/A POW)
rank_tier >= 3 TEC  -> pool 2 (S/A TEC)
```

`Duel_SelectCardDrop` returns a signed card ID, which is stored at `+0x3C`,
published to the result display staging word, and later passed unchanged to
`Duel_AwardCard`.

## Starchip persistence

The live save state begins at `gDuel_awPlayerDeck`. Its `SaveDataState`
starchip field is at offset `+0x5E0`, absolute address `0x801D07E0`, also
exposed by the established symbol `gLibrary_dwStarchips`.

The campaign credit path:

1. reads `starchip_prize`;
2. adds it to the winner save's `starchips`;
3. clamps the result to `SAVE_DATA_STARCHIP_MAX` (`999999`); and
4. awards `dropped_card_id`.

The password shop reads and deducts the same `gLibrary_dwStarchips` object.
Its declaration now lives in `save_data.h`; the module-specific `shop.h`
declaration was redundant.

The alternate post-duel branch increments per-save win/loss halfwords at
`+0x518/+0x51A` instead. It does not award starchips or a card. Which branch
is taken depends on the duel mode/opponent state already documented in the
gameplay guide.

## Confidence boundary

Confirmed:

- every field offset and access width through `+0x3D`;
- page-index range and matching display consumer;
- rank-tier/axis derivation and the `1..5` starchip prize;
- three-way drop-pool selection;
- save offset `+0x5E0`, unsigned 32-bit balance, and `999999` clamp.

The result-state pointer is assigned to the reused ritual-data buffer by the
unmatched state machine. The side-indexed save pointers at `D_8009B1D8` and
`D_8009B1DC` remain address-based: their two-entry relationship is clear, but
changing the matching setup source to one array declaration has not been
measured and is not required for this field contract.
