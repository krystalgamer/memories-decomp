# Known variable / data names (SLUS_014.11)

> **External-reference provenance.** This file originated in
> Unchiga/ygofm-decomp and retains its upstream evidence. Memories-decomp may
> add local reconciliations and corrections; upstream-only paths, tools, and
> rules are not this repository's policy. See [`README.md`](README.md).

GENERATED from config/symbol_addrs.txt -- regenerate with tools/gen_research_notes.py
after any naming change. Source tags: `idb2018` = idb_raymond_2018 import, `ramMap` =
datacrystal RAM map, `dotr` = DotR-style naming, `psyq` = original SDK symbol
(libsyms signature match), `fleet` = behavior-derived by this project. Evidence: NAMING.md.

The roster is divided into game-meaningful and mechanical sections. Derive
current totals from the rows rather than maintaining a duplicate snapshot
here.

## Game-meaningful names

Names that say what something IS in the game.

| address | name | source |
|---|---|---|
| 0x8009078C | `gFile_apszName` | idb2018 |
| 0x800909D4 | `gDuel_aTerrainBoost` | idb2018 |
| 0x80090C50 | `gScript_apfnCommand` | idb2018 |
| 0x80090E58 | `gTextBox_awSpriteSlotRange` | idb2018 |
| 0x8009151C | `gMovie_aStreamRanges` | matching C + disc |
| 0x800916E0 | `gAiScript_apfnCommand` | idb2018 |
| 0x800917F0 | `gDuel_aOpponentData` | idb2018 |
| 0x8009AF24 | `gDuel_abTrapAttackThresholds` | matching C + retail data |
| 0x8009AF29 | `gDuel_bWidespreadRuinAttackThreshold` | matching C + retail data |
| 0x8009AF30 | `gDuel_abLifePointRecoveryUnits` | matching C + retail data |
| 0x8009AF38 | `gDuel_abDirectDamageUnits` | matching C + retail data |
| 0x8009B0AC | `gGraphics_bActiveBuffer` | live + matching C |
| 0x8009B141 | `gFade_bOverlayOn` | idb2018 |
| 0x8009B146 | `gGraphics_sViewportX` | live + matching C |
| 0x8009B148 | `gGraphics_sViewportY` | live + matching C |
| 0x8009B164 | `gDuel_bQuitDialogState` | matching C + live |
| 0x8009B165 | `gDuel_bWinnerSide` | matching C + live |
| 0x8009B1D5 | `gDuel_bPlayingSide` | idb2018 |
| 0x8009B246 | `gDuel_wViewerCardID` | live + resident writers |
| 0x8009B258 | `gCardGrid_bCursorColumn` | live |
| 0x8009B259 | `gCardGrid_bCursorRow` | live |
| 0x8009B27A | `gCampaignSceneIndex` | matching C |
| 0x8009B27C | `gScript_wCommand` | idb2018 |
| 0x8009B290 | `gScript_pStream` | idb2018 |
| 0x8009B2A4 | `gScript_wState` | idb2018 |
| 0x8009B2C4 | `gDebug_nLastSoundID` | ramMap |
| 0x8009B2C8 | `gDebug_nSceneOrSoundID` | ramMap |
| 0x8009B327 | `gDialog_bInputState` | idb2018 |
| 0x8009B336 | `gDialog_bChoiceEnabled` | idb2018 |
| 0x8009B338 | `gDuel_wSelectedCardID` | idb2018 |
| 0x8009B345 | `gDialog_bChoiceCount` | idb2018 |
| 0x8009B34D | `gDialog_bChoice` | idb2018 |
| 0x8009B361 | `gDuel_bOpponentID` | idb2018 |
| 0x8009B364 | `gDuel_bTerrain` | idb2018 |
| 0x8009B365 | `gFreeDuel_bReturnFlags` | live |
| 0x8009B366 | `gFreeDuel_bCursorColumn` | live |
| 0x8009B367 | `gFreeDuel_bCursorRow` | live |
| 0x8009B37C | `gOptions_bState` | matching C |
| 0x8009B37D | `gOptions_bOutputType` | matching C + live |
| 0x8009B384 | `gOptions_bSelection` | matching C |
| 0x8009B394 | `gInput_wPad1Repeat` | matching C |
| 0x8009B396 | `gInput_wPad2Repeat` | matching C |
| 0x8009B398 | `gInput_wPad1Pressed` | matching C |
| 0x8009B39A | `gInput_wPad2Pressed` | matching C |
| 0x8009B39C | `gInput_bRepeatDelay` | matching C |
| 0x8009B39E | `gInput_wPad1RepeatBackup` | matching C |
| 0x8009B3A0 | `gInput_wPad1PressedBackup` | matching C |
| 0x8009B3A2 | `gInput_bRepeatInterval` | matching C |
| 0x8009B3A4 | `gInput_wPad1Held` | matching C |
| 0x8009B3A6 | `gInput_wPad2Held` | matching C |
| 0x8009B3A8 | `gInput_dwPendingHeld` | matching C |
| 0x8009B3AC | `gInput_wPad1HeldBackup` | matching C |
| 0x8009B408 | `gSD_bOutputType` | live |
| 0x8009B450 | `gMemCard_nIOResult` | matching C |
| 0x8009B45C | `g_SDValue` | dotr |
| 0x800E9EA8 | `gFile_anLba` | idb2018 |
| 0x800E9EC8 | `gFade_State` | idb2018 |
| 0x800EA002 | `gDuel_wPlayerLifePointDisplay` | ramMap |
| 0x800EA004 | `gDuel_wPlayerLifePoint` | ramMap |
| 0x800EA022 | `gDuel_wOpponentLifePointDisplay` | idb2018 |
| 0x800EA024 | `gDuel_wOpponentLifePoint` | idb2018 |
| 0x800EA118 | `gDuel_wFusionResultCardID` | ramMap |
| 0x800EB0F8 | `gTextBox_aRecords` | idb2018 |
| 0x800EF668 | `gInput_abRawPadBuffers` | matching C |
| 0x800F2AE0 | `gMemCard_aIOEventHandles` | matching C |
| 0x800F5B98 | `gAiScript_aMemory` | idb2018 |
| 0x800F5BE8 | `gAiScript_State` | idb2018 |
| 0x800F5BEC | `gAiScript_wWord1` | idb2018 |
| 0x800F5BF0 | `gAiScript_wWord2` | idb2018 |
| 0x800F5BFC | `gAiScript_bByte2` | idb2018 |
| 0x800F5C00 | `gAiScript_awWordArray2` | idb2018 |
| 0x800F5C80 | `gAi_wBestDifference` | matching C |
| 0x800F5C82 | `gAi_bBestAttacker` | matching C |
| 0x800F5C83 | `gAi_bBestTarget` | matching C |
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
| 0x80184568 | `gMain_apMenuEntries` | matching C + live |
| 0x80184594 | `gMain_bMenuID` | matching C + live |
| 0x801A7B64 | `gDuel_aPlayerMonsters` | idb2018 |
| 0x801A7B80 | `gDuel_PlayerMonster2` | idb2018 |
| 0x801A7B9C | `gDuel_PlayerMonster3` | idb2018 |
| 0x801A7BB8 | `gDuel_PlayerMonster4` | idb2018 |
| 0x801A7BD4 | `gDuel_PlayerMonster5` | idb2018 |
| 0x801A7E20 | `gDuel_aDeckCardRecords` | matching C + ramMap |
| 0x801A8000 | `gAiScript_abHandScript` | idb2018 |
| 0x801A8008 | `gLibrary_aPasswordCardData` | ramMap |
| 0x801AB000 | `gDuel_aActiveCards` | idb2018 |
| 0x801B0000 | `gText_aBank` | idb2018 |
| 0x801C0000 | `gText_aBankOffsets` | idb2018 |
| 0x801D0200 | `gDuel_awPlayerDeck` | idb2018 |
| 0x801D0250 | `gLibrary_abCardChest` | idb2018 |
| 0x801D060C | `gSaveData_aPlayerNameSjis` | matching C + live |
| 0x801D06F4 | `gFreeDuel_dwUnlockedDuelists` | ramMap |
| 0x801D07BC | `gDuel_awRecentCardDrops` | ramMap |
| 0x801D07DC | `gCampaignSavedSceneIndex` | matching C |
| 0x801D07E0 | `gLibrary_dwStarchips` | idb2018 |
| 0x801D4244 | `gDuel_adwCardStats` | idb2018 |
| 0x801D4D8E | `gCard_asNameSortKey` | idb2018 |
| 0x801D56A8 | `gDuel_CardDropID2` | ramMap |
| 0x801D5800 | `gText_aGlobalOffsets` | idb2018 |
| 0x801D9000 | `gText_adwGlyphCodeTable` | idb2018 |
| 0x801DC000 | `gLibrary_aCardArtRecord` | live |
| 0x801EA800 | `gSD_MusicTrack` | idb2018 |

## Mechanical names

Fleet-written descriptions of verified *mechanics*, not game meaning (e.g. `flag80_test_and_set` tests-and-sets bit 0x80 of something). Accurate but low readability; each is a candidate for a meaningful upgrade when evidence appears.

| address | name | source |
|---|---|---|
| 0x80011580 | `gFile_szModelMrgPath` | fleet |
| 0x800117C8 | `gFile_szSuMrgPath` | fleet |
| 0x80090B50 | `gCredits_awSecretNumbers` | fleet |
| 0x80090E0C | `gText_anPow10` | fleet |
| 0x8009B34D | `gDialog_bChoice_b` | fleet |
| 0x8009B36C | `gFreeDuel_bTargetColumn` | fleet |
| 0x8009B36D | `gFreeDuel_bTargetRow` | fleet |
| 0x8009B408 | `gSD_bOutputType_s` | fleet |
| 0x800E9EC8 | `gFade_State_arr` | fleet |
| 0x800EB0F8 | `gTextBox_aRecords_arr` | fleet |
| 0x801D0618 | `gCampaign_abStoryFlags` | fleet |
| 0x801D071C | `gFreeDuel_aDuelistRecords` | fleet |
| 0x801D5332 | `gDuel_abCardLevelAttr` | fleet |
| 0x801D5708 | `gText_abColorSlots` | fleet |

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
| 0x8016ABD4 | `gNameEntry_aStarterSets` |
| 0x8016D3DC | `gNameEntry_apStarterSets` |
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
| 0x8016D420 | `gPassword_pDigitCursorWidget` |
| 0x8016D428 | `gPassword_nDigitIndex` |
