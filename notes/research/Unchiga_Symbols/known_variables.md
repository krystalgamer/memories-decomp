# Known variable / data names (SLUS_014.11)

GENERATED from config/symbol_addrs.txt -- regenerate with tools/gen_research_notes.py
after any naming change. Source tags: `idb2018` = idb_raymond_2018 import, `ramMap` =
datacrystal RAM map, `dotr` = DotR-style naming, `psyq` = original SDK symbol
(libsyms signature match), `fleet` = behavior-derived by this project. Evidence: NAMING.md.

83 named variables (75 game-meaningful, 0 sdk, 8 mechanical).

## Game-meaningful names

Names that say what something IS in the game.

| address | name | source |
|---|---|---|
| 0x8009078C | `fileNames` | idb2018 |
| 0x800909D4 | `terrainBoosts` | idb2018 |
| 0x80090C50 | `scriptCommandTable` | idb2018 |
| 0x80090E58 | `textSpriteSlotRanges` | idb2018 |
| 0x800916E0 | `aiFunctionTable` | idb2018 |
| 0x800917F0 | `oppData` | idb2018 |
| 0x8009B141 | `screenFadeOverlayOn` | idb2018 |
| 0x8009B1D5 | `playingSide` | idb2018 |
| 0x8009B258 | `gridCursorCol` | live |
| 0x8009B259 | `gridCursorRow` | live |
| 0x8009B27C | `scriptCommand` | idb2018 |
| 0x8009B290 | `scriptStreamPtr` | idb2018 |
| 0x8009B2A4 | `scriptState` | idb2018 |
| 0x8009B2C4 | `debugSoundId` | ramMap |
| 0x8009B327 | `dialogInputState` | idb2018 |
| 0x8009B336 | `dialogChoiceEnabled` | idb2018 |
| 0x8009B338 | `selectedCardId` | idb2018 |
| 0x8009B345 | `dialogChoiceCount` | idb2018 |
| 0x8009B34D | `dialogChoice` | idb2018 |
| 0x8009B361 | `opponentId` | idb2018 |
| 0x8009B364 | `terrain` | idb2018 |
| 0x8009B366 | `opponentGridCol` | live |
| 0x8009B367 | `opponentGridRow` | live |
| 0x8009B408 | `soundMode` | live |
| 0x800E9EA8 | `filePositions` | idb2018 |
| 0x800E9EC8 | `screenFade` | idb2018 |
| 0x800EA002 | `playerDisplayedLp` | ramMap |
| 0x800EA004 | `playerLp` | ramMap |
| 0x800EA022 | `cpuDisplayedLp` | idb2018 |
| 0x800EA024 | `cpuLp` | idb2018 |
| 0x800EA118 | `fusionResult` | ramMap |
| 0x800EB0F8 | `textRecords` | idb2018 |
| 0x800F5B98 | `aiMemory` | idb2018 |
| 0x800F5BE8 | `aiStruct` | idb2018 |
| 0x800F5BEC | `aiWord1` | idb2018 |
| 0x800F5BF0 | `aiWord2` | idb2018 |
| 0x800F5BFC | `aiByte2` | idb2018 |
| 0x800F5C00 | `aiWordArray2` | idb2018 |
| 0x800FE6F8 | `prngSeed` | idb2018 |
| 0x80177F94 | `playerPerm` | idb2018 |
| 0x80177FBC | `cpuPerm` | idb2018 |
| 0x80177FE8 | `playerDeckTmp` | idb2018 |
| 0x80178038 | `cpuDeckTmp` | idb2018 |
| 0x801781D8 | `cpuDeckPool` | idb2018 |
| 0x8017878C | `powDropPool` | idb2018 |
| 0x80178D40 | `bcdDropPool` | idb2018 |
| 0x801792F4 | `tecDropPool` | idb2018 |
| 0x801798A8 | `rankScoreDiffs` | idb2018 |
| 0x801799D8 | `ritualData` | idb2018 |
| 0x80179A04 | `rankScore` | idb2018 |
| 0x80179A14 | `cardDropId` | ramMap |
| 0x8017A1D8 | `equipTable` | idb2018 |
| 0x8017C2D8 | `fusionTable` | idb2018 |
| 0x801A7B64 | `playerMonsters` | idb2018 |
| 0x801A7B80 | `playerMonster2` | idb2018 |
| 0x801A7B9C | `playerMonster3` | idb2018 |
| 0x801A7BB8 | `playerMonster4` | idb2018 |
| 0x801A7BD4 | `playerMonster5` | idb2018 |
| 0x801A7E20 | `playerHand` | ramMap |
| 0x801A8000 | `handAiScript` | idb2018 |
| 0x801A8008 | `cardPasswords` | ramMap |
| 0x801AB000 | `activeCards` | idb2018 |
| 0x801B0000 | `textBank` | idb2018 |
| 0x801C0000 | `textBankOffsets` | idb2018 |
| 0x801D0200 | `playerDeck` | idb2018 |
| 0x801D0250 | `trunk` | idb2018 |
| 0x801D06F4 | `freeDuelUnlocks` | ramMap |
| 0x801D07BC | `lastCardDrops` | ramMap |
| 0x801D07E0 | `starchips` | idb2018 |
| 0x801D4244 | `statsArray` | idb2018 |
| 0x801D4D8E | `nameKeys` | idb2018 |
| 0x801D56A8 | `g_CardDropId2` | ramMap |
| 0x801D5800 | `globalTextOffsets` | idb2018 |
| 0x801D9000 | `glyphCodeTable` | idb2018 |
| 0x801EA800 | `musicTrack` | idb2018 |

## Mechanical names

Fleet-written descriptions of verified *mechanics*, not game meaning (e.g. `flag80_test_and_set` tests-and-sets bit 0x80 of something). Accurate but low readability; each is a candidate for a meaningful upgrade when evidence appears.

| address | name | source |
|---|---|---|
| 0x80090E0C | `pow10_table` | fleet |
| 0x8009B34D | `g_DialogChoice_b` | fleet |
| 0x8009B36C | `freeDuelTargetCol` | fleet |
| 0x8009B36D | `freeDuelTargetRow` | fleet |
| 0x8009B408 | `soundMode_s` | fleet |
| 0x800E9EC8 | `g_ScreenFade_arr` | fleet |
| 0x800EB0F8 | `g_TextRecords_arr` | fleet |
| 0x801D0618 | `storyFlags` | fleet |

## Module symbols

Only true while that screen's module is resident (config/modules/README.md). Source: live trace.

### free_duel

| address | name |
|---|---|
| 0x80169030 | `freeDuelGridAvailable` |
| 0x80169058 | `freeDuelThumbWidget` |
| 0x80169060 | `freeDuelSparklePool` |
| 0x801690A0 | `freeDuelCursorWidget` |
| 0x801690A4 | `freeDuelScreenFlags` |

### main_menu

| address | name |
|---|---|
| 0x80184594 | `menuCursor` |

### name_entry

| address | name |
|---|---|
| 0x8016AB38 | `nameEntryCellTable` |
| 0x8016ABC0 | `nameEntryRowJump` |
| 0x8016D400 | `nameEntryFlags` |
| 0x8016D401 | `nameEntryCol` |
| 0x8016D402 | `nameEntryRow` |
| 0x8016D404 | `nameEntryCursorWidget` |
| 0x8016D408 | `nameEntryNameBytes` |
| 0x8016D418 | `nameEntryNamePtr` |
| 0x8016D426 | `nameEntrySavedRow` |
| 0x8016D42C | `nameEntryNameLen` |
| 0x8016D434 | `nameEntryCursorTargetX` |
| 0x8016D436 | `nameEntryCursorTargetY` |
| 0x8016D43C | `nameEntryCaretWidget` |
| 0x8016D4D2 | `nameEntryPendingDialog` |

### overworld

| address | name |
|---|---|
| 0x801691A8 | `overworldLocationTable` |
| 0x80169608 | `overworldMoveState` |
| 0x8016960C | `overworldLocation` |
| 0x80169618 | `overworldLocationPrev` |

### password

| address | name |
|---|---|
| 0x8016D410 | `passwordDigits` |

