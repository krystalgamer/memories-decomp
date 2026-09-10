# Starter-deck pool tables

## Scope

The name-entry overlay builds a new game's 40-card deck from seven weighted
pool records. Two screen packages contain byte-identical copies of the data:

| WA offset | Package context |
|---:|---|
| `0xF92BD4` | name-entry package |
| `0xFBDBD4` | password package |

Each copy is `0x2808` bytes: seven `0x5B8`-byte
`NameEntryStarterDeckPool` records. Its SHA-256 is:

```text
842e022e4a4bb2e5ecf633f5c5d80e3a041ab837cd3bbd65d01b67783a4d45a4
```

## Checked layout

`src/overlays/password/name_entry_starter_deck.h` defines:

```c
typedef struct {
    u16 draw_count;
    u16 weights[722];
    u8 padding[18];
} NameEntryStarterDeckPool;
```

The compile-time size check fixes the `0x5B8` stride:

```text
2 + 722 * 2 + 18 = 1464 = 0x5B8
```

All seven retail padding tails are zero in both copies. Immediately after
each copy is the same eight-word pointer list:

```text
0x8016ABD4
0x8016B18C
0x8016B744
0x8016BCFC
0x8016C2B4
0x8016C86C
0x8016CE24
0x00000000
```

The non-null pointers differ by exactly `0x5B8`. Matching
`NameEntry_BuildStarterDeck` walks until the null pointer rather than using a
hard-coded pool count. The overlay symbols are therefore named
`gNameEntry_aStarterDeckPools` and `gNameEntry_apStarterDeckPools`.

## Retail rows

| Row | Draw count | Nonzero-card category | Weight sum |
|---:|---:|---|---:|
| 0 | 16 | Monsters with ATK+DEF `450-1050` | 2048 |
| 1 | 16 | Monsters with ATK+DEF `1100-1550` | 2048 |
| 2 | 4 | Monsters with ATK+DEF `1600-2050` | 2048 |
| 3 | 1 | Monsters with ATK+DEF `2100-2450` | 2048 |
| 4 | 1 | Dark Hole and Raigeki | 2048 |
| 5 | 1 | The six terrain cards | 2048 |
| 6 | 1 | 28 Equip cards | 2048 |

The draw counts total 40. No card appears in more than one row.

The stored records contain 722 weights, but the matching generator scans only
indices `0..719`. The last two weights are zero in retail, and each first-720
sum remains 2048, so this mismatch does not shorten a stock deck. Edited data
can produce a missing card whenever a threshold exceeds the first-720 sum.

## Selection and retry contract

For each requested card:

1. compute threshold `(rand() & 0x7FF) + 1`;
2. scan weights from card index zero, calling and discarding one additional
   `rand()` result before adding each weight;
3. accept the first cumulative sum that reaches the threshold;
4. if that card already has three copies, increment the remaining draw count
   so the attempt is retried;
5. otherwise append card ID `index + 1` to `gDuel_awPlayerDeck`;
6. call `Library_UpdateCardUsedFlag(index + 289)`.

The flag argument is `0x120 + card_id`, so every distinct card accepted into
the starter deck is marked seen in the Library.

The discarded calls make RNG consumption card-dependent: selecting card ID
`n` consumes one threshold call plus `n` scan calls. Fourth-copy retries
consume another complete attempt. A deck predictor must reproduce those
calls, not merely sample the static probabilities.

## Confidence boundary

Confirmed from matching source and both retail table copies:

- record size, fields, stride, pointer-list termination, and row order;
- draw counts and every row's 2048 weight sum;
- first-720 scan bound and three-copy retry behavior;
- accepted deck IDs and Library seen-flag writes.

The ATK+DEF/category labels come from comparing every nonzero weight with the
verified card catalogue. They describe the retail rows, not validation rules:
the generator itself never tests card type, ATK, DEF, or row membership.
