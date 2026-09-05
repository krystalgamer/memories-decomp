# Card Catalog

`notes/card-catalog.csv` records the requested factual metadata for all 722
cards in the North American release:

- card ID and in-game name;
- card type and, for monsters, attribute and level;
- both Guardian Stars;
- ATK and DEF;
- password and starchip cost.

Descriptions and artwork are intentionally excluded. A password or cost of
`N/A` means the card has no usable retail password and cannot be purchased
through the password screen; it does not mean that the card costs zero.

## Executable-backed fields

Names and gameplay fields were decoded from the verified
`game/SLUS_014.11` executable:

- `gText_aGlobalOffsets` at `0x801D5800` is indexed by card ID and points into
  the `0x801Dxxxx` global text bank. Card names are `0xFF`-terminated glyph
  strings.
- `gDuel_adwCardStats` at `0x801D4244` contains 722 packed words. ATK is bits
  `0`-`8` times 10, DEF is bits `9`-`17` times 10, Guardian Star 2 is bits
  `18`-`21`, Guardian Star 1 is bits `22`-`25`, and type is bits `26`-`30`.
- The byte table at `0x801D5332`, indexed directly by card ID, stores
  attribute in the high nibble and level in the low nibble.

All 722 decoded types, attributes, levels, Guardian Stars, ATK values, and DEF
values agree with the public Forbidden Memories database. For searchable CSV
text, the game's right-apostrophe glyph is normalized to ASCII `'`. Names then
agree except card 480: the executable glyph at index `0x55` maps through
`gText_adwGlyphCodeTable` to Shift-JIS `alpha`, so the catalog preserves the
in-game name `Kuwagata α` rather than the database's ASCII `Kuwagata a`.

## Password and cost cross-check

The retail password/cost table is stored in `WA_MRG.MRG`, which is not
available in this checkout. Those two columns are therefore externally
cross-checked rather than executable-backed.

The password and starchip fields were compared across:

- <https://yugioh-fm-db.pages.dev/cards>
- [`ThatPlayer2/Yu-Gi-Oh-Forbidden-Memories` at
  `4b6aa11`](https://github.com/ThatPlayer2/Yu-Gi-Oh-Forbidden-Memories/blob/4b6aa11ba205de177f39dfcadf28361af3712b3f/Dictionary/Cards.cs)
- [`TheGreatHeroStudios/ForbiddenMemoriesDuelCompanion` at
  `8f6888a`](https://github.com/TheGreatHeroStudios/ForbiddenMemoriesDuelCompanion/blob/8f6888a42b280bdb57ef9cb0560d133e4a9694ae/FMFC.DataLoader/Files/CardData.json)

The first two sources agree on every password and cost. The independent JSON
agrees on every available password and all but three available costs. The
three differences were checked against Yugipedia:

| Card | Catalog cost | Conflicting JSON | Corroboration |
|---|---:|---:|---|
| 213 Aqua Madoor | 260 | 250 | [Yugipedia](https://yugipedia.com/wiki/Aqua_Madoor_%28FMR%29) |
| 436 White Dolphin | 20 | 50 | [Yugipedia](https://yugipedia.com/wiki/White_Dolphin_%28FMR%29) |
| 678 Revival of Sennen Genjin | 100 | 50 | [Yugipedia](https://yugipedia.com/wiki/Revival_of_Sennen_Genjin_%28FMR%29) |

The catalog uses the corroborated values above. All three sources identify the
same 24 cards as having no usable password; those rows use `N/A` for both
password and starchip cost.

## Field mappings

The packed numeric domains decode as follows:

| Value | Type |
|---:|---|
| 0 | Dragon |
| 1 | Spellcaster |
| 2 | Zombie |
| 3 | Warrior |
| 4 | Beast-Warrior |
| 5 | Beast |
| 6 | Winged Beast |
| 7 | Fiend |
| 8 | Fairy |
| 9 | Insect |
| 10 | Dinosaur |
| 11 | Reptile |
| 12 | Fish |
| 13 | Sea Serpent |
| 14 | Machine |
| 15 | Thunder |
| 16 | Aqua |
| 17 | Pyro |
| 18 | Rock |
| 19 | Plant |
| 20 | Magic |
| 21 | Trap |
| 22 | Ritual |
| 23 | Equip |

Guardian Star values are `1` Mars, `2` Jupiter, `3` Saturn, `4` Uranus, `5`
Pluto, `6` Neptune, `7` Mercury, `8` Sun, `9` Moon, and `10` Venus. Attribute
values are `0` Light, `1` Dark, `2` Earth, `3` Water, `4` Fire, and `5` Wind;
non-monster records use the remaining attribute codes and are shown blank.
