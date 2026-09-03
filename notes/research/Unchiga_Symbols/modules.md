# Screen modules -- per-module symbols

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
| 0x80160000..0x8017B000 | one screen module at a time: free duel, name entry, password (`passwordDigits` 0x8016D410 lives in the password module's data at 0x8016D4xx, the same block the name-entry module uses) |
| 0x80180000.. | main menu module (`menuCursor` 0x80184594) |
| 0x801B0000 / 0x801C0000 | the resident screen's string bank and offset table |

## Free duel module (config/modules/free_duel.txt)

See findings F79-F86: `freeDuelModuleEntry` 0x80168FB4, `freeDuelScreenTick`
0x80168C7C, `freeDuelCursorTweenTick` 0x80168A9C, `freeDuelCursorPlace`
0x80168090, `freeDuelScrollbarUpdate` 0x80168004, `freeDuelTargetCol/Row`
0x8009B36C/6D (EXE bss, not module), `freeDuelGridAvailable` 0x80169030,
`freeDuelScreenFlags` 0x801690A4, `freeDuelThumbWidget` 0x80169058,
`freeDuelCursorWidget` 0x801690A0.

## Name entry module (config/modules/name_entry.txt, findings F100-F101)

| address | name | what |
|---|---|---|
| 0x80168FB4 | `name_entry_module_entry`? | not traced yet -- the EXE's `nameEntryLoop` (0x8002D62C) calls the module by fixed address; confirm the entry |
| 0x8016913C | `nameEntryScreenTick` | cursor tween (counter at widget+0x60, 8 frames), then DPAD from `0x8009B3A4`: RIGHT/LEFT wrap over 15 columns, UP/DOWN wrap over 9 rows, right-hand column uses the row-jump table |
| 0x8016909C | `nameEntryAdjustLength(delta, ?)` | len += delta with a 5-char cap; moves the caret widget (x = len*16 + 107); SE 0x0C |
| 0x8016868C | `textGlyphAt(slot, x, y)` | finds the glyph entry of text record `slot` at pixel (x, y); +0 of the entry is the Shift-JIS code |
| 0x80168CDC | `nameEntrySpawnGlyphSprite` | spawns the animated copy of the picked letter (find_free_slot_0x10_0x60 + get_or_init slot) |
| 0x801698C0.. | `name_entry_prompt_slide` | slides the 'Input your NAME!' box (text slot 2) between y 248 and 176 with `widgetSlideSine`, then `textBoxDestroy` |
| 0x8016D400 | `nameEntryFlags` | 0x20 prompt up, 0x02 prompt sliding, 0x80 END pressed, 0x10 leaving (after screenFadeOutWait) |
| 0x8016D401 / 0x8016D402 | `nameEntryCol` / `nameEntryRow` | grid cursor cell |
| 0x8016D404 | `nameEntryCursorWidget` | -> 0x800F0548 |
| 0x8016D408 | `nameEntryNameBytes` | 2 per character |
| 0x8016D418 | `nameEntryNamePtr` | -> 0x801D060C, the save block's player name, u16 Shift-JIS per char |
| 0x8016D426 | `nameEntrySavedRow` | row remembered while in the right-hand column |
| 0x8016D42C | `nameEntryNameLen` | characters typed, max 5 |
| 0x8016D434 / 0x8016D436 | `nameEntryCursorTargetX/Y` | x = col*20+22 (+20 for cols >= 11), y = row*18+24 |
| 0x8016D43C | `nameEntryCaretWidget` | the underline under the name |
| 0x8016AB38 | `nameEntryCellTable` | 9 rows x 15: 0 = letter, 4 = arrow cell (row 4), 0x46 = END (row 6, big cursor), negative = skip that many cells leftwards |
| 0x8016ABC0 | `nameEntryRowJump` | [row*2 + down]: rows 0-4 -> 6, rows 5-8 -> 4, for the arrow/END column |
| 0x80169734 | `nameEntryDialogTick` | builds text box 2 from the pending string id at 0x8016D4D2 (bit 0x8000 = scripted, 0x4000 = no choice), opens the choice, polls done + `dialogChoice` |
| 0x8016D4D2 | `nameEntryPendingDialog` | u16: string id + flags of the dialog to show (245 = 'Your NAME is ...') |
| 0x801D060C | `g_PlayerName` (save block) | EXE-side symbol candidate: 5 x u16 SJIS + terminator; zeroed on New Game |

## Overworld module (config/modules/overworld.txt, findings F117-F120)

Resident while `campaignOverworldLoop` (mode 0xC5) runs. `overworldLocationTable`
(0x801691A8, 16 x 0x42) is the map graph; `overworldPickExit` (0x80168E0C) resolves
a DPAD edge to a destination through the exit list and `storyFlagTest`;
`overworldSetLocation` (0x8016866C) commits it; `overworldDpadCamera` (0x80168388)
is the free-look camera on the world map.
