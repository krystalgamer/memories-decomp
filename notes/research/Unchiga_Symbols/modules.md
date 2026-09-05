# Screen modules -- per-module symbols

> **External reference — mirror of Unchiga/ygofm-decomp.** This file is copied verbatim from that repository's `notes/research/` (the matching decomp the research was done in). Paths, tools and rules it names (`config/symbol_addrs.txt`, `tools/gen_symbols.py`, `tools/setup.sh`, `config/modules/`, `tools/gen_research_notes.py`, ...) are that project's, not this one's. The addresses and evidence are the claim; the names follow this repo's `notes/naming-conventions.md` (`Subsystem_VerbObject` / `gSubsystem_Role`) since 2026-09-03 and are ready for `config/slus_01411/symbols.txt` and `notes/semantic-symbol-map.csv` after the usual per-address review.

Several screens run from a module loaded from disc into the SAME address
range, so a module function/variable name is only true while that module is
resident. `config/symbol_addrs.txt` can hold one name per address, which is
why the free-duel names landed there on 2026-09-02 (0x80168004..0x80168FB4,
0x80169030..0x801690A4) now collide with the name-entry module below.
Module symbols live in `config/modules/<module>.txt` (tools/module_symbols.py);
this file keeps the evidence and layout notes. EXE-side symbols go through gen_symbols.py.

## Load map (observed)

| range | who |
|---|---|
| 0x80160000..0x8017B000 | one screen module at a time: free duel, name entry, password (`gPassword_abDigits` 0x8016D410 lives in the password module's data at 0x8016D4xx, the same block the name-entry module uses) |
| 0x80180000.. | main menu module (`gMain_bMenuID` 0x80184594) |
| 0x801B0000 / 0x801C0000 | the resident screen's string bank and offset table |

## Free duel module (config/modules/free_duel.txt)

See findings F79-F86: `FreeDuel_Entry` 0x80168FB4, `FreeDuel_UpdateScreen`
0x80168C7C, `FreeDuel_UpdateCursorTween` 0x80168A9C, `FreeDuel_PlaceCursor`
0x80168090, `FreeDuel_UpdateScrollbar` 0x80168004, `gFreeDuel_bTargetColumn/Row`
0x8009B36C/6D (EXE bss, not module), `gFreeDuel_abGridAvailable` 0x80169030,
`gFreeDuel_bScreenFlags` 0x801690A4, `gFreeDuel_pThumbWidget` 0x80169058,
`gFreeDuel_pCursorWidget` 0x801690A0.

## Name entry module (config/modules/name_entry.txt, findings F100-F101)

| address | name | what |
|---|---|---|
| 0x80168FB4 | `name_entry_module_entry`? | not traced yet -- the EXE's `Main_RunNameEntry` (0x8002D62C) calls the module by fixed address; confirm the entry |
| 0x8016913C | `NameEntry_UpdateScreen` | cursor tween (counter at widget+0x60, 8 frames), then DPAD from `gInput_wPad1Held`: RIGHT/LEFT wrap over 15 columns, UP/DOWN wrap over 9 rows, right-hand column uses the row-jump table |
| 0x8016909C | `NameEntry_AdjustLength(delta, ?)` | len += delta with a 5-char cap; moves the caret widget (x = len*16 + 107); SE 0x0C |
| 0x8016868C | `TextBox_GetGlyphAt(slot, x, y)` | finds the glyph entry of text record `slot` at pixel (x, y); +0 of the entry is the Shift-JIS code |
| 0x80168CDC | `NameEntry_SpawnGlyphSprite` | spawns the animated copy of the picked letter (find_free_slot_0x10_0x60 + get_or_init slot) |
| 0x801698C0.. | `name_entry_prompt_slide` | slides the 'Input your NAME!' box (text slot 2) between y 248 and 176 with `Widget_SlideSine`, then `TextBox_Destroy` |
| 0x8016D400 | `gNameEntry_bFlags` | 0x20 prompt up, 0x02 prompt sliding, 0x80 END pressed, 0x10 leaving (after Fade_WaitOut) |
| 0x8016D401 / 0x8016D402 | `gNameEntry_bColumn` / `gNameEntry_bRow` | grid cursor cell |
| 0x8016D404 | `gNameEntry_pCursorWidget` | -> 0x800F0548 |
| 0x8016D408 | `gNameEntry_nNameBytes` | 2 per character |
| 0x8016D418 | `gNameEntry_pName` | -> 0x801D060C, the save block's player name, u16 Shift-JIS per char |
| 0x8016D426 | `gNameEntry_SavedRow` | row remembered while in the right-hand column |
| 0x8016D42C | `gNameEntry_nNameLen` | characters typed, max 5 |
| 0x8016D434 / 0x8016D436 | `gNameEntry_wCursorTargetX/Y` | x = col*20+22 (+20 for cols >= 11), y = row*18+24 |
| 0x8016D43C | `gNameEntry_pCaretWidget` | the underline under the name |
| 0x8016AB38 | `gNameEntry_abCellTable` | 9 rows x 15: 0 = letter, 4 = arrow cell (row 4), 0x46 = END (row 6, big cursor), negative = skip that many cells leftwards |
| 0x8016ABC0 | `gNameEntry_abRowJump` | [row*2 + down]: rows 0-4 -> 6, rows 5-8 -> 4, for the arrow/END column |
| 0x80169734 | `NameEntry_UpdateDialog` | builds text box 2 from the pending string id at 0x8016D4D2 (bit 0x8000 = scripted, 0x4000 = no choice), opens the choice, polls done + `gDialog_bChoice` |
| 0x8016D4D2 | `gNameEntry_wPendingDialog` | u16: string id + flags of the dialog to show (245 = 'Your NAME is ...') |
| 0x801D060C | `g_PlayerName` (save block) | EXE-side symbol candidate: 5 x u16 SJIS + terminator; zeroed on New Game |

## Overworld module (config/modules/overworld.txt, findings F117-F120)

Resident while `Main_RunCampaignMap` (mode 0xC5) runs. `gCampaignMap_aLocationTable`
(0x801691A8, 16 x 0x42) is the map graph; `CampaignMap_PickExit` (0x80168E0C) resolves
a DPAD edge to a destination through the exit list and `Campaign_TestStoryFlag`;
`CampaignMap_SetLocation` (0x8016866C) commits it; `CampaignMap_MoveCameraDpad` (0x80168388)
is the free-look camera on the world map.
