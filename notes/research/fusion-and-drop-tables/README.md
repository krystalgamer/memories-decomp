# Fusion, equip and drop tables

The three rule tables the duel reads from disc, decoded and joined against
`notes/card-catalog.csv`. Everything here regenerates from a retail disc with
the two scripts beside it; nothing is transcribed by hand.

```sh
python3 extract_tables.py <disc.bin>   # writes tables.json, asserts the invariants
python3 compile_tables.py              # writes the CSVs
```

`tables.json` is an intermediate and is not tracked.

## Files

| file | rows | what |
|---|---|---|
| `fusions.csv` | 25,131 | every recipe: both materials and the result, with names |
| `fusions_by_result.csv` | 204 | one row per obtainable result, its recipe count, and every recipe that makes it |
| `fusions_by_material.csv` | 722 | how many recipes each card appears in, on either side |
| `equips.csv` | 4,041 | every (equip, monster) pair the equip table accepts |
| `drops.csv` | 12,518 | every non-zero weight: duelist, pool, card, weight, chance |
| `drops_summary.csv` | 40 | per duelist, pool sizes and best weight |

`drops.csv` carries the deck pool alongside the three drop pools. It is the
same kind of table in the same record, and the opponent's deck is drawn from it
rather than being fixed (§5.11), so it belongs with them.

## Where the data is

Both tables live in `DATA/WA_MRG.MRG`, LBA 10102 per
`config/slus_01411/disc_layout.json`. Sector numbers below are relative to the
start of that file.

**Fusion and equip tables** — the duel blob, 235 sectors at sector `0x16C6`.
Seven consecutive copies exist, one per terrain; twelve of the thirteen chunks
are byte-identical between them and only the field picture differs, so any copy
will do. Inside the blob:

* `+0x22000`, 0x2800 bytes — equip table, loaded to `0x8017A1D8`
* `+0x24800`, 0x10000 bytes — fusion table, loaded to `0x8017C2D8`

**Drop tables** — one 3-sector (6144-byte) record per duelist, the first at
sector `0x1D33`, stride 3 sectors, ids 0-39. Inside a record:

| offset | size | table |
|---|---|---|
| `+0x0000` | 1460 | deck weights |
| `+0x05B4` | 1460 | S/A POW drop pool |
| `+0x0B68` | 1460 | B/C/D drop pool |
| `+0x111C` | 1460 | S/A TEC drop pool |
| `+0x16D0` | 200 | rank table |
| `+0x1798` | 104 | `0xFF` sector padding; identical in all 40 blocks |

Each weight table is 722 `u16` followed by 16 bytes of padding, indexed by card
id minus one. The rank table is not decoded here; see §6.2.

## Reading a weight

`Duel_SelectCardDrop` (`0x80021810`) rolls `(rand & 0x7FF) + 1`, i.e. 1-2048,
then walks the pool accumulating weights until the running sum reaches the roll.
It consumes one RNG value, not one per examined weight, and returns zero
without retrying if the threshold is never reached. Its argument selects
rank pool `0`, `1`, or `2` in the already-loaded duelist block; it is not
the duelist ID.

Under a uniform-threshold model, a card's weight is its nominal selection
share out of 2048. `chance_percent` reports
`round(weight * 100.0 / 2048, 4)`, not an empirical frequency for a specific
seed/timing route or a proof of independent draws. For deck-pool rows this
is a per-threshold share, not the probability that a card appears in the
finished deck after copy-limit rejection and shuffling. All 160 weight
tables sum to exactly 2048; `extract_tables.py` asserts it rather than
assuming it. See the [RNG contract](../../rng.md#reward-card-stream-consumption)
for the selector's behavior with edited totals.

## Reading the fusion table

`Duel_CheckFusion` (`0x80019A60`, `src/game/duel_card_checks.c`) sorts its two
arguments, so each pair is stored once under the smaller id:

```
off   = u16 at base + min(a,b) * 2      0 means this card fuses with nothing
rec   = base + off
count = rec[0]                          body at rec + 1
        ...unless rec[0] == 0, then
count = 0x1FF - rec[1]                  body at rec + 2   (counts 256-511)
```

The body is groups of five bytes carrying two (partner, result) pairs. Ids are
ten bits and the four high halves are packed two bits each into the group's
first byte:

```
partner1 = ((g[0] << 8) & 0x300) | g[1]
result1  = ((g[0] << 6) & 0x300) | g[2]
partner2 = ((g[0] << 4) & 0x300) | g[3]
result2  = ((g[0] << 2) & 0x300) | g[4]
```

`count` counts *pairs*, so it decrements by two per group and an odd count
leaves the last group's second pair unused.

## What this confirms

Every figure §5.4 and §6.4 of `the-game.md` state, reproduced from the disc
rather than quoted:

* 25,131 recipes producing 204 distinct results
* the three most-produced results — Nekogal #2 (1,200 recipes), Mystical Sand
  (1,174), Cyber Soldier (1,008)
* 4,041 equip pairs across 34 equips
* all 160 weight tables summing to 2048

Three structural facts fall out of the data rather than being assumed:

* blocks 8 and 35 — Heishin and Heishin 2nd — share all three drop pools but
  carry different deck weights
* block 39, Duel Master K, has drop pools byte-identical to block 6,
  Villager 3, and again a different deck
* block 0 is a complete copy of block 1, Simon Muran, decks included, which is
  what makes it unused rather than merely unreferenced

The sector arithmetic also lands independently on the offsets the recomp's
`tools/gen_drop_db.py` uses for the same records (`0x1D33 * 2048 + 1460`
= 15310260, stride 6144), which is a check on both.

## Caveat on the names

The initial block/name assignment used the community's recorded drop lists,
with 92-100 % overlap per duelist. Weight comparisons alone do not encode
the characters' names. The
[primary game description](../the-game.md#64-the-dropped-card) now also
corroborates the identities through matching Free Duel code and retail text:
the armed update for an available, settled grid index `i` selects global
string ID `0x8328 + i`.

| `duelist_id` | Global string ID | Retail string address | Label |
|---:|---|---|---|
| `37` | `0x834D` | `0x801D8D7A` | `DarkNite` |
| `38` | `0x834E` | `0x801D8D83` | `Nitemare` |

Both `drops.csv` and `drops_summary.csv` already use these labels. The
reversed labels in the archived GameShark win/loss table are not an
alternative duelist-block order. This independent source/text evidence
retires the earlier uncertainty about the pair; it does not remeasure
drop weights or claim a new runtime trace. The archived cheat labels and
the generated CSV data remain unchanged.
