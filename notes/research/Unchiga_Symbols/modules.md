# Screen modules -- per-module symbols

> **External-reference provenance.** This file originated in
> Unchiga/ygofm-decomp and retains its upstream evidence. Memories-decomp may
> add local reconciliations and corrections; upstream-only paths, tools, and
> rules are not this repository's policy. See [`README.md`](README.md).

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

The imported module file and F100/F101 retain their historical labels.
Current matching source lives in the `password` scope. Complete-body analysis
now distinguishes keyboard input from outer-dialog handling, and supersedes
the old byte-count, name-length and END-bit interpretations below; see the
[current contracts](../../../src/overlays/password/README.md#keyboard-input-and-glyph-effects).

| address | name | what |
|---|---|---|
| 0x80168FB4 | historical `name_entry_module_entry` guess | This address is inside `NameEntry_SpawnGlyphSprite`, not an independently established entry function; resident entry points are documented in the name-entry README. |
| 0x8016913C | `NameEntry_UpdateKeyboard` (reference: `NameEntry_UpdateScreen`) | selection-frame tween, navigation, caret controls, insertion and completion request; the outer dialog owns acceptance and text rebuilding |
| 0x801689B4 | `NameEntry_UpdateCaretTween` | moves the insertion caret to its stored target over the requested update count, then snaps and clears the callback |
| 0x8016909C | `NameEntry_AdjustLength(delta, bound)` | bounds/moves the caret word index (observed 0-5 with bound 6), updating x = index*16 + 107; does not erase words or maintain a terminator |
| 0x8016868C | `TextBox_GetGlyphAt(slot, x, y)` | finds the glyph entry of text record `slot` at pixel (x, y); +0 of the entry is the Shift-JIS code |
| 0x80168CDC | `NameEntry_SpawnGlyphSprite` | allocates a 16x16 glyph copy and stores its source-node pointer; callers choose the effect; the node-code read precedes the null test |
| 0x801698C0.. | `name_entry_prompt_slide` | slides the 'Input your NAME!' box (text slot 2) between y 248 and 176 with `Widget_SlideSine`, then `TextBox_Destroy` |
| 0x8016D400 | reference `gNameEntry_bFlags` | 0x80 pending character transfer/text refresh, 0x40 finish request, 0x20 final-confirmation path, 0x10 completion; these are separate from per-object state bits |
| 0x8016D401 / 0x8016D402 | `gNameEntry_bColumn` / `gNameEntry_bRow` | grid cursor cell |
| 0x8016D404 | `gNameEntry_pCursorWidget` | -> 0x800F0548 |
| 0x8016D408 | `D_8016D408` (reference: `gNameEntry_nNameBytes`) | u8 tag copied to each nonnull-node glyph sprite and incremented, including effects; not an encoded byte count |
| 0x8016D418 | `gNameEntry_pName` | -> `gSaveData_aPlayerNameSjis`, the save block's u16 Shift-JIS player name |
| 0x8016D426 | `gNameEntry_SavedRow` | row remembered while in the right-hand column |
| 0x8016D42C | `D_8016D42C` (reference: `gNameEntry_nNameLen`) | caret/insertion word index, observed 0-5; not a count of encoded bytes or necessarily the current string length |
| 0x8016D434 / 0x8016D436 | `gNameEntry_wCursorTargetX/Y` | x = col*20+22 (+20 for cols >= 11), y = row*18+24 |
| 0x8016D43C | `gNameEntry_pCaretWidget` | the underline under the name |
| 0x8016AB38 | `gNameEntry_abCellTable` | 9 rows x 15: 0 = letter, 4 = arrow cell (row 4), 0x46 = END (row 6, big cursor), negative = skip that many cells leftwards |
| 0x8016ABC0 | `gNameEntry_abRowJump` | [row*2 + down]: rows 0-4 -> 6, rows 5-8 -> 4, for the arrow/END column |
| 0x80169734 | `NameEntry_UpdateDialog` | builds text box 2 from the pending string id at 0x8016D4D2 (bit 0x8000 = scripted, 0x4000 = no choice), opens the choice, polls done + `gDialog_bChoice` |
| 0x8016D4D2 | `gNameEntry_wPendingDialog` | u16: string id + flags of the dialog to show (245 = 'Your NAME is ...') |
| 0x801D060C | `gSaveData_aPlayerNameSjis` | Twelve-byte save field, six u16 SJIS words; zeroed on New Game. The keyboard can write slot 5 without appending a source terminator, so these bodies do not establish a five-character maximum. |

## Password module (config/modules/password.txt, findings F25 and F158)

| address | name | what |
|---|---|---|
| 0x8016D410 | `gPassword_abDigits` | eight password digits, one byte each |
| 0x8016D420 | `gPassword_pDigitCursorWidget` | widget whose x target follows the selected digit |
| 0x8016D428 | `gPassword_nDigitIndex` | selected digit index, 0..7 |

## Overworld module (config/modules/overworld.txt, findings F117-F120)

Resident while `Main_RunCampaignMap` (mode 0xC5) runs. `gCampaignMap_aLocationTable`
(0x801691A8, 16 x 0x42) is the map graph; `CampaignMap_PickExit` (0x80168E0C) resolves
a DPAD edge to a destination through the exit list and `Campaign_TestStoryFlag`;
`CampaignMap_SetLocation` (0x8016866C) commits it; `CampaignMap_MoveCameraDpad` (0x80168388)
is the free-look camera on the world map.
