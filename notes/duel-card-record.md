# Duel card record table (`D_801A7AD8`)

## Scope and evidence policy

This phase structures only `D_801A7AD8`, the duel/card table at
`0x801A7AD8`. Primary evidence is the verified North American
`game/SLUS_014.11` target disassembly, exact-matching C users in `src/game`,
and the final byte-for-byte build comparison. GMS, the Unchiga tree, and the
old reference tree were used only to corroborate offsets and access widths;
their guessed declarations were not copied.

`notes/global-usage.csv` records 52 game functions using this address. The
typed-migration snapshot documented below contains 20 matching-C users and 32
assembly users; ongoing decompilation may change that split, so the generated
report is the authority for current status. `func_8002C9B4` is an additional
matching C source whose address formation names `D_801A7AD8` only inside an
inline-assembly string, so it is not one of those 52 report rows.

## Conservative shared layout

`src/game/duel_card.h` defines `DuelCardRecord`, with a checked size of
`0x1C`, checked offsets for every exposed field, explicit unknown padding at
`+0x08..+0x0B` and `+0x19..+0x1B`, and an unsized typed extern. The known
initialization loop covers 30 records; the extern remains unsized because
several functions use interior aliases or derived subranges.

| Offset | Width | Shared field | Exact evidence |
| --- | ---: | --- | --- |
| `+0x00` | 4 | `object` | `func_8001778C` clears it with `sw`; `func_80025F3C`, `func_8002778C`, `func_800278A0`, and `func_80027DF8` load it as an object pointer. `func_8002C938` exports the same word as an opaque value. |
| `+0x04` | 4 | `data` | `func_8001778C` clears it with `sw`; `func_800249E0` stores a pointer into `gDuel_aDeckCardRecords`; `func_80017DB4` loads it and then reads a byte from the pointed-to object. |
| `+0x08` | 4 | padding | No field type is asserted. |
| `+0x0C` | 2 | `card_id` | `func_800249E0` stores it with `sh` and later uses `lh`; `Duel_CollectFieldCardsByType` uses `lh`; `func_80027DF8` uses both `lhu` for copying and `lh` for signed table indexing. The shared field therefore fixes the width while exact users retain explicit signed views where required. |
| `+0x0E` | 2 | `attack` | `func_800249E0` stores the first card-stat component with `sh`; exact stat calculation users consume the record. |
| `+0x10` | 2 | `defense` | `func_800249E0` stores the second card-stat component with `sh`; exact stat calculation users consume the record. |
| `+0x12` | 2 | `stat_modifier` | `func_800249E0` clears it with `sh`; the exact stat comparison path passes the record to the stat calculators. |
| `+0x14` | 2 | `terrain_modifier` | `func_800249E0` stores the terrain result with `sh`. Some state tests intentionally load a 32-bit word beginning here, spanning this field and `flags`. |
| `+0x16` | 2 | `flags` | Repeated `lhu`/`sh` accesses in `func_80017E3C`, `Duel_ApplyCardObjectFlags`, `func_8001EFD4`, cursor/effect helpers, scans, and mutators establish the width and bit masks. |
| `+0x18` | 1 | `table_index` | `func_800249E0` stores the selected table index with `sb`. |
| `+0x19` | 3 | padding | No field type is asserted. |

The target initialization at `0x8001778C` advances both cursors by `0x1C`
and stops at 30. Independent record address calculations use the same
stride: `func_800249E0` scales an index by 28,
`Duel_CollectFieldCardsByType` selects
five-record slices within 15 records per side, and `func_80027DF8` selects
the two 15-record side blocks.

Corroboration agrees without defining the shared type:

- GMS declares a guessed `int[]`, but consistently indexes records as seven
  words, uses halfword 6 for the `+0x0C` ID, and the high half of word 5 for
  the `+0x16` flags. Its guessed declaration was rejected.
- Unchiga sources independently use a `0x1C` stride and the same
  `+0x00`, `+0x04`, `+0x0C`, and `+0x16` widths. Its recompilation records
  for `Duel_UpdateCardPickCursor` (`0x800240B0`, originally
  `cursor_card_pick_tick`) and
  `find_table801a7ad8_entry_matching_id` report zero differing words.
- The old reference assembly and C repeat these accesses, but were treated
  only as a cross-check against the target and exact local build.

## Typed migration snapshot

At this snapshot, all 20 pure-C report users include `duel_card.h` and
use its typed extern:

`func_8001778C`, `func_80017DB4`, `func_80017E3C`,
`Duel_ApplyCardObjectFlags`, `func_80019BD0`, `func_8001D240`,
`func_8001EFD4`, `func_80023090`, `Duel_UpdateCardPickCursor`, `func_800249E0`,
`func_80025B28`, `func_80025F3C`, `func_80026A3C`,
`func_80026C0C`,
`Duel_CollectFieldCardsBelowType`, `Duel_CollectFieldCardsByType`,
`func_8002778C`, `func_800278A0`, `func_80027DF8`, and `func_8002C938`.

Raw local views retained for exact code generation:

- `func_80023090` keeps byte-addressed construction for its first record
  pointer because it only scales the index before passing that pointer to the
  guardian comparison helper; it does not access a shared field directly.
- `func_80026C0C` uses the shared typed extern, but casts its base to a byte
  pointer and keeps the pinned `$a0` address construction so the record-scale
  result and final pointer share the retail live range.
- `func_8001778C` keeps byte cursors for the paired `+0x00`/`+0x04` stores
  and independent `+0x16` cursor; its increments and bound use the shared
  size/count constants.
- `func_80017DB4` keeps the target's 32-bit read beginning at `+0x14` so the
  `0xA0000000` mask remains a single `lw`-based test.
- `func_80025F3C` likewise keeps the target's 32-bit `+0x14` read for the
  `0x90000000` state test.
- `func_8002C938` keeps its explicit byte-address construction and fixed
  register variables, then uses `DuelCardRecord` once the address is formed.

`func_8002C9B4` remains wholly unchanged because its `D_801A7AD8` address
formation is inline assembly. Its local record view and the interior
`D_801A7B64` alias are therefore deliberately not migrated.

## Assembly users in the migration snapshot

The snapshot contains 32 assembly users:

- `func_80016784`, `func_80017F04`, `func_8001825C`, `func_8001898C`,
  `func_80018DB4`, `func_80019608`, `func_80019D18`, `func_8001B170`,
  `func_8001B938`, `func_8001BAF0`, `func_8001BD88`, `func_8001D670`,
  `func_8001F0D0`, `func_8001F364`, and `func_8001F55C`.
- `func_80022674`, `func_800229F4`, `func_80023144`,
  `func_800235C0`, `func_80024E58`, `func_80025028`, `func_8002538C`,
  `DuelEffect_UpdateFieldMarker`, `func_800257A0`, `func_8002596C`, `func_80025BEC`,
  `func_80025D30`, `func_800260D0`, and `func_800262D4`.
- `func_80027508`, `func_800279BC`, and `Duel_CheckRitual`.
