# Known variable / data names (SLUS_014.11)

> **External reference — mirror of Unchiga/ygofm-decomp.** This file is copied verbatim from that repository's `notes/research/` (the matching decomp the research was done in). Paths, tools and rules it names (`config/symbol_addrs.txt`, `tools/gen_symbols.py`, `tools/setup.sh`, `config/modules/`, `tools/gen_research_notes.py`, ...) are that project's, not this one's. The addresses and evidence are the claim; the names follow this repo's `notes/naming-conventions.md` (`Subsystem_VerbObject` / `gSubsystem_Role`) since 2026-09-03 and are ready for `config/slus_01411/symbols.txt` and `notes/semantic-symbol-map.csv` after the usual per-address review.

GENERATED from config/symbol_addrs.txt -- regenerate with tools/gen_research_notes.py
after any naming change. Source tags: `idb2018` = idb_raymond_2018 import, `ramMap` =
datacrystal RAM map, `dotr` = DotR-style naming, `psyq` = original SDK symbol
(libsyms signature match), `fleet` = behavior-derived by this project. Evidence: NAMING.md.

83 named variables (75 game-meaningful, 0 sdk, 8 mechanical).

## Game-meaningful names

Names that say what something IS in the game.

| address | name | source |
|---|---|---|
| 0x8009078C | `gFile_apszName` | idb2018 |
| 0x800909D4 | `gDuel_aTerrainBoost` | idb2018 |
| 0x80090C50 | `gScript_apfnCommand` | idb2018 |
| 0x80090E58 | `gTextBox_awSpriteSlotRange` | idb2018 |
| 0x800916E0 | `gAiScript_apfnCommand` | idb2018 |
| 0x800917F0 | `gDuel_aOpponentData` | idb2018 |
| 0x8009B141 | `gFade_bOverlayOn` | idb2018 |
| 0x8009B1D5 | `gDuel_bPlayingSide` | idb2018 |
| 0x8009B258 | `gCardGrid_bCursorColumn` | live |
| 0x8009B259 | `gCardGrid_bCursorRow` | live |
| 0x8009B27C | `gScript_wCommand` | idb2018 |
| 0x8009B290 | `gScript_pStream` | idb2018 |
| 0x8009B2A4 | `gScript_wState` | idb2018 |
| 0x8009B2C4 | `gDebug_nLastSoundID` | ramMap |
| 0x8009B327 | `gDialog_bInputState` | idb2018 |
| 0x8009B336 | `gDialog_bChoiceEnabled` | idb2018 |
| 0x8009B338 | `gDuel_wSelectedCardID` | idb2018 |
| 0x8009B345 | `gDialog_bChoiceCount` | idb2018 |
| 0x8009B34D | `gDialog_bChoice` | idb2018 |
| 0x8009B361 | `gDuel_bOpponentID` | idb2018 |
| 0x8009B364 | `gDuel_bTerrain` | idb2018 |
| 0x8009B366 | `gFreeDuel_bCursorColumn` | live |
| 0x8009B367 | `gFreeDuel_bCursorRow` | live |
| 0x8009B408 | `gSD_bOutputType` | live |
| 0x800E9EA8 | `gFile_anLba` | idb2018 |
| 0x800E9EC8 | `gFade_State` | idb2018 |
| 0x800EA002 | `gDuel_wPlayerLifePointDisplay` | ramMap |
| 0x800EA004 | `gDuel_wPlayerLifePoint` | ramMap |
| 0x800EA022 | `gDuel_wOpponentLifePointDisplay` | idb2018 |
| 0x800EA024 | `gDuel_wOpponentLifePoint` | idb2018 |
| 0x800EA118 | `gDuel_wFusionResultCardID` | ramMap |
| 0x800EB0F8 | `gTextBox_aRecords` | idb2018 |
| 0x800F5B98 | `gAiScript_aMemory` | idb2018 |
| 0x800F5BE8 | `gAiScript_State` | idb2018 |
| 0x800F5BEC | `gAiScript_wWord1` | idb2018 |
| 0x800F5BF0 | `gAiScript_wWord2` | idb2018 |
| 0x800F5BFC | `gAiScript_bByte2` | idb2018 |
| 0x800F5C00 | `gAiScript_awWordArray2` | idb2018 |
| 0x800FE6F8 | `gRand_dwSeed` | idb2018 |
| 0x80177F94 | `gDuel_awPlayerDeckShuffle` | idb2018 |
| 0x80177FBC | `gDuel_awOpponentDeckShuffle` | idb2018 |
| 0x80177FE8 | `gDuel_awPlayerShuffledDeck` | idb2018 |
| 0x80178038 | `gDuel_awOpponentShuffledDeck` | idb2018 |
| 0x801781D8 | `gDuel_awOpponentDeckPool` | idb2018 |
| 0x8017878C | `gDuel_awSaPowCardDrops` | idb2018 |
| 0x80178D40 | `gDuel_awBcdCardDrops` | idb2018 |
| 0x801792F4 | `gDuel_awSaTecCardDrops` | idb2018 |
| 0x801798A8 | `gDuel_awRankScoreChange` | idb2018 |
| 0x801799D8 | `gDuel_awRitualData` | idb2018 |
| 0x80179A04 | `gDuel_anRankScore` | idb2018 |
| 0x80179A14 | `gDuel_wCardDropID` | ramMap |
| 0x8017A1D8 | `gDuel_awEquipTable` | idb2018 |
| 0x8017C2D8 | `gDuel_aFusionTable` | idb2018 |
| 0x801A7B64 | `gDuel_aPlayerMonsters` | idb2018 |
| 0x801A7B80 | `gDuel_PlayerMonster2` | idb2018 |
| 0x801A7B9C | `gDuel_PlayerMonster3` | idb2018 |
| 0x801A7BB8 | `gDuel_PlayerMonster4` | idb2018 |
| 0x801A7BD4 | `gDuel_PlayerMonster5` | idb2018 |
| 0x801A7E20 | `gDuel_aPlayerHand` | ramMap |
| 0x801A8000 | `gAiScript_abHandScript` | idb2018 |
| 0x801A8008 | `gLibrary_aPasswordCardData` | ramMap |
| 0x801AB000 | `gDuel_aActiveCards` | idb2018 |
| 0x801B0000 | `gText_aBank` | idb2018 |
| 0x801C0000 | `gText_aBankOffsets` | idb2018 |
| 0x801D0200 | `gDuel_awPlayerDeck` | idb2018 |
| 0x801D0250 | `gLibrary_abCardChest` | idb2018 |
| 0x801D06F4 | `gFreeDuel_dwUnlockedDuelists` | ramMap |
| 0x801D07BC | `gDuel_awRecentCardDrops` | ramMap |
| 0x801D07E0 | `gLibrary_dwStarchips` | idb2018 |
| 0x801D4244 | `gDuel_adwCardStats` | idb2018 |
| 0x801D4D8E | `gCard_asNameSortKey` | idb2018 |
| 0x801D56A8 | `gDuel_CardDropID2` | ramMap |
| 0x801D5800 | `gText_aGlobalOffsets` | idb2018 |
| 0x801D9000 | `gText_adwGlyphCodeTable` | idb2018 |
| 0x801EA800 | `gSD_MusicTrack` | idb2018 |

## Mechanical names

Fleet-written descriptions of verified *mechanics*, not game meaning (e.g. `flag80_test_and_set` tests-and-sets bit 0x80 of something). Accurate but low readability; each is a candidate for a meaningful upgrade when evidence appears.

| address | name | source |
|---|---|---|
| 0x80090E0C | `gText_anPow10` | fleet |
| 0x8009B34D | `gDialog_bChoice_b` | fleet |
| 0x8009B36C | `gFreeDuel_bTargetColumn` | fleet |
| 0x8009B36D | `gFreeDuel_bTargetRow` | fleet |
| 0x8009B408 | `gSD_bOutputType_s` | fleet |
| 0x800E9EC8 | `gFade_State_arr` | fleet |
| 0x800EB0F8 | `gTextBox_aRecords_arr` | fleet |
| 0x801D0618 | `gCampaign_abStoryFlags` | fleet |

## Module symbols

Only true while that screen's module is resident (config/modules/README.md). Source: live trace.

### free_duel

| address | name |
|---|---|
| 0x80169030 | `gFreeDuel_abGridAvailable` |
| 0x80169058 | `gFreeDuel_pThumbWidget` |
| 0x80169060 | `gFreeDuel_apSparklePool` |
| 0x801690A0 | `gFreeDuel_pCursorWidget` |
| 0x801690A4 | `gFreeDuel_bScreenFlags` |

### main_menu

| address | name |
|---|---|
| 0x80184594 | `gMain_bMenuID` |

### name_entry

| address | name |
|---|---|
| 0x8016AB38 | `gNameEntry_abCellTable` |
| 0x8016ABC0 | `gNameEntry_abRowJump` |
| 0x8016D400 | `gNameEntry_bFlags` |
| 0x8016D401 | `gNameEntry_bColumn` |
| 0x8016D402 | `gNameEntry_bRow` |
| 0x8016D404 | `gNameEntry_pCursorWidget` |
| 0x8016D408 | `gNameEntry_nNameBytes` |
| 0x8016D418 | `gNameEntry_pName` |
| 0x8016D426 | `gNameEntry_SavedRow` |
| 0x8016D42C | `gNameEntry_nNameLen` |
| 0x8016D434 | `gNameEntry_wCursorTargetX` |
| 0x8016D436 | `gNameEntry_wCursorTargetY` |
| 0x8016D43C | `gNameEntry_pCaretWidget` |
| 0x8016D4D2 | `gNameEntry_wPendingDialog` |

### overworld

| address | name |
|---|---|
| 0x801691A8 | `gCampaignMap_aLocationTable` |
| 0x80169608 | `gCampaignMap_MoveState` |
| 0x8016960C | `gCampaignMap_Location` |
| 0x80169618 | `gCampaignMap_LocationPrev` |

### password

| address | name |
|---|---|
| 0x8016D410 | `gPassword_abDigits` |

