# Symbols Guide

> **External reference — mirror of Unchiga/ygofm-decomp.** This file is copied verbatim from that repository's `notes/research/` (the matching decomp the research was done in). Paths, tools and rules it names (`config/symbol_addrs.txt`, `tools/gen_symbols.py`, `tools/setup.sh`, `config/modules/`, `tools/gen_research_notes.py`, ...) are that project's, not this one's. The addresses and evidence are the claim; the names follow this repo's `notes/naming-conventions.md` (`Subsystem_VerbObject` / `gSubsystem_Role`) since 2026-09-03 and are ready for `config/slus_01411/symbols.txt` and `notes/semantic-symbol-map.csv` after the usual per-address review.

The confirmed symbols, organized by where in the game they actually run --
from matched code, live tracing of the running game, and confirmed community
knowledge. GENERATED from tools/guide/build_guide.py output (the shared Symbols
Guide artifact) ; edit build_guide.py, publish the HTML to the artifact URL in
config/symbols_guide_url.txt, and regenerate this file. Rows tagged `module:` are only true while that screen's module
is resident (config/modules/). Rosters: known_functions.md / known_variables.md.


## Everywhere — the engine

Runs on every screen. A live-computed core of 147 functions (GPU frame pump, SPU upkeep, pad, fades) beats under all of it; these are the named anchors.

| symbol | address | description |
|---|---|---|
| `Main_Loop` | 0x8002DD74 | The master dispatch: never returns, runs the update funcs each frame, then calls the current mode's loop through a function-pointer table. |
| `Main_Init` | 0x80012B50 | Boot-time setup before `Main_Loop` takes over. |
| `Main_VBlankCB` | 0x80012CD4 | Runs once per frame on the vertical blank. |
| `Input_UpdatePads` | 0x8003CCD8 | Publishes held, newly pressed, and timer-repeated button masks for both controllers. |
| `gInput_wPad1Held / gInput_wPad2Held` | 0x8009B3A4 / 0x8009B3A6 | Current held-button masks for controllers 1 and 2. |
| `gInput_wPad1Pressed / gInput_wPad2Pressed` | 0x8009B398 / 0x8009B39A | Rising-edge button masks for controllers 1 and 2. |
| `gInput_wPad1Repeat / gInput_wPad2Repeat` | 0x8009B394 / 0x8009B396 | Newly pressed buttons plus timer-generated repeats for controllers 1 and 2. |
| `gRand_dwSeed` | 0x800FE6F8 | The random-number seed — shuffles, drops, and AI coin-flips all trace back here. |
| `File_SetPositionTable` | 0x800136E4 | Boot: resolves every file in `gFile_apszName` to its disc position. |
| `gFile_apszName` | 0x8009078C | Names of the data files on the disc. |
| `gFile_anLba` | 0x800E9EA8 | Where each file lives on the CD. |
| `File_GetPosition` | 0x800138F4 | Looks a file's position up. |
| `File_Exists` | 0x8005C4F0 | Checks whether a file is present (wraps PsyQ `DsSearchFile`). |


### Text boxes and dialogs (also everywhere)

> How text works: every string on screen — a menu label, a prompt, the letter grid you pick a name from, the name you typed — is a text record driven by one typewriter. A record holds a string id, not text; the screen's strings live in a bank loaded with its module, and a global bank holds the always-needed ones (location and card names). Strings are glyph indices with control bytes: `00` space, `F8 xx yy` position/style, `FA` wait for X, `FB 02` a choice with two lines, `FB 80 lo hi lo hi` jump table by the answer, `FC xx yy` splice dynamic text, `FE` newline, `FF` end.

| symbol | address | description |
|---|---|---|
| `gTextBox_aRecords` | 0x800EB0F8 | The text-box records, 0x64 bytes each: current text pointer (advances as glyphs are typed), glyph list, three widgets, flags (0x800 build requested, 0x4000 started, 0x2000 done), string id, rect, glyph cell size, and the record's glyph-sprite budget. |
| `gTextBox_awSpriteSlotRange` | 0x80090E58 | Cumulative glyph-sprite slot boundaries per record: 0, 255, 415, 575, 620. |
| `gText_aBank / gText_aBankOffsets` | 0x801B0000 / 0x801C0000 | The resident screen's strings (0xFF-terminated) and the u16 offset per string id; ids 0x500+ are shifted by 0x100, ids 0xD000+ index the second table region. |
| `gText_aGlobalOffsets` | 0x801D5800 | Offset table of the always-resident bank (ids 0x8000+, base 0x801D0000): location names are 0x8350 + index, Metropolis … Hiding. |
| `Text_LookupString` | 0x8003B744 | String id → pointer into the right bank. |
| `gText_adwGlyphCodeTable` | 0x801D9000 | One u32 per font glyph, low half = its Shift-JIS code. ‘A’ is glyph 24, ‘B’ 45, ‘a’ 3, ‘0’ 56. |
| `Text_SjisToGlyphCodes` | 0x8003BC40 | The inverse: a Shift-JIS string (like the player name) into glyph codes so the typewriter can show it. |
| `TextBox_Create / TextBox_CreateFlagged` | 0x80035BE4 / 0x80035C38 | Open a box: slot, string id, x, y, w, h (and extra flags). |
| `TextBox_SetRect / TextBox_InitRecord` | 0x80035AB8 / 0x80035AF0 | The two halves of create: geometry, then id, cell size and sprite budget. |
| `TextBox_BuildStep` | 0x800393B0 | The typewriter: first call resolves the string and allocates the widgets, then one glyph per call. `poll_call_393b0_until_bit13` spins it to the end at once. |
| `TextBox_SetPos / TextBox_Destroy` | 0x80039934 / 0x80035B7C | Move a box with its widgets; close it and free everything. |
| `Widget_SlideSine` | 0x80043230 | The sine slide dialogs use to enter and leave (the name prompt travels y 248 ↔ 176 on it). |


### Dialog choices (YES / NO boxes)

A choice box is an ordinary text box whose string ends in a choice attribute. The EXE attaches a cursor object and the screen reads the answer from one byte. The same cursor object doubles as the blinking “press X” arrow when a page fills.

| symbol | address | description |
|---|---|---|
| `gDialog_bChoice` | 0x8009B34D | The selected line: 0 = first (YES), 1 = second (NO). Live while you move, final on confirm. |
| `gDialog_bChoiceCount / gDialog_bChoiceEnabled` | 0x8009B345 / 0x8009B336 | How many lines, and a bit per line that may be picked. |
| `gDialog_bInputState` | 0x8009B327 | 0x40 confirmed (low bits = the choice), 0x80 cancelled. |
| `Dialog_OpenChoice` | 0x800374F4 | Spawns the cursor at the box's bottom-right corner (sprite 0x20C). |
| `Dialog_UpdateChoice` | 0x800371A8 | Per-frame: confirm, cancel, or hand the DPAD to the input handler. |
| `Dialog_ReadChoiceInput` | 0x8003700C | DOWN and UP move within the count, SE 6, re-highlight. |
| `Dialog_HighlightChoice / Widget_UpdatePulseColour` | 0x80036F80 / 0x80037110 | Cursor colour by whether the line is enabled; the triangle-wave blink from the frame counter. |


### Screen fades (also everywhere)

Every screen change goes through one fade system. The fade-out blocks: the mode loop calls it and nothing else on the screen runs until it lands, about 48 frames at step 8.

| symbol | address | description |
|---|---|---|
| `gFade_State` | 0x800E9EC8 | The fade block: colour, current level, target, flags (0x80 in flight, 0x01 strip mode), step, head, and 30 per-strip brightness bytes. |
| `Fade_WaitOut` | 0x80015B00 | What mode loops call: starts the fade to black and pumps frames until done. |
| `Fade_StartOut / Fade_InitOut` | 0x80015904 / 0x800158B8 | Start (strip mode, step 8) and init (head 255, target 0, all strips at the current level). |
| `Fade_Update` | 0x80015310 | Per-frame update; latches the RGB and the overlay flag when the target is reached. |
| `Fade_StepBands` | 0x800151D8 | The mirrored walker: strips `i` and `29-i` share a value, so fade-out closes from the top and bottom edges to the middle and fade-in opens from the middle. |
| `Fade_DrawOverlay` | 0x800154E4 | Draws 30 semi-transparent black 320×8 boxes, darkness 255 − strip. |
| `gFade_bOverlayOn` | 0x8009B141 | Keeps the overlay painting solid black between a fade-out and the next screen's fade-in — that is what hides the disc load. |


### Story flags (also everywhere)

| symbol | address | description |
|---|---|---|
| `gCampaign_abStoryFlags` | 0x801D0618 | The save block's story-progress bit array, MSB-first: byte `id >> 3`, bit `0x80 >> (id & 7)`. |
| `Campaign_TestStoryFlag` | 0x8002CCA8 | Tests one flag; bit 0x8000 of the id means “must be clear”. Overworld exits, script gates and modules all go through it. |


### The sound driver (also everywhere)

| symbol | address | description |
|---|---|---|
| `SD_Init` | 0x800492D8 | Sound-system init. |
| `SD_Term` | 0x80049694 | Sound-system shutdown. |
| `SD_InitState` | 0x80046768 | Initializes the sound-driver state (`g_SDValue`), its buffers and callbacks; names `sd_bgm.dat` / `sd_se.dat` / `master.xa`, which the request path then reads. |
| `SD_SEPlay` | 0x80048658 | Plays sound effect `id` at volume `vol`. Known ids: 6 cursor move, 7 confirm, 8 cancel, 0xB page advance, 0xC typing, 0x2F option toggle / grid move, 0x30 confirm a site. |
| `SD_SEPlayFull` | 0x8003FEE0 | Convenience wrapper: plays an effect at full volume — always `SD_SEPlay(id, 0xFF)`. |
| `gSD_MusicTrack` | 0x801EA800 | The current music track. (Below this, the SPU layer keeps original Sony PsyQ names.) |


## Main menu

NEW GAME / LOAD / 2P DUEL / TRADE / OPTION. The menu's own logic runs from a module loaded from disc into `0x80180000+` — the EXE supplies widgets, sound, and drawing.

| symbol | address | description |
|---|---|---|
| `Main_RunMenu` | 0x8002D588 | The main-menu mode tick. Dialogs (like TRADE's memory-card prompt) are modals inside it — the game never leaves this mode for them. |
| `gMain_bMenuID` (module: main_menu) | 0x80184594 | The shared menu cursor: 0–4 on the main menu, 5–10 on the post-load menu (CAMPAIGN=5 … SAVE=10). |


## Options

| symbol | address | description |
|---|---|---|
| `Main_RunOptionsMenu` | 0x8002D6C8 | The OPTION screen's mode tick. |
| `Options_HandleInput` | 0x8003C7A0 | Toggles stereo/mono on the sound row, confirms other rows, and cancels back out. |
| `gOptions_bState / gOptions_bSelection` | 0x8009B37C / 0x8009B384 | Options-screen state-machine value and selected row. |
| `gOptions_bOutputType` | 0x8009B37D | Working copy of the stereo/mono choice used to position the option widgets. |
| `SD_SetOutputType` | 0x80046FA0 | The STEREO/MONO setter (0=stereo, 1=mono): writes the driver's mode byte and re-mixes CD audio on the spot. |
| `gSD_bOutputType` | 0x8009B408 | The stored stereo/mono setting. |


## Campaign — story script and overworld

> What a cutscene is: not a screen module. After the name is confirmed the mode byte goes to `0xC2` and `Main_RunCampaign` runs a bytecode script: it picks a script through a table of self-relative offsets (in the same 0x801A8000 bank the AI scripts use), latches a stream cursor and dispatches opcodes through a 24-entry table. The dialogue is an ordinary bank string typed by the typewriter; the pictures are swapped by opcode 5. Simon's “Run away / Keep listening” never reaches the script at all — it is a jump table inside the text (`FB 80`), with all four “Run away” exits pointing at the same “Drat! He's gone” ending.

| symbol | address | description |
|---|---|---|
| `Main_RunCampaign` | 0x8002CE64 | Mode 0xC2: the story/cutscene tick (IDB name, live-confirmed). |
| `Script_RunTick` | 0x8002FA54 | The bytecode driver: selects a script, latches the stream, runs one opcode per call. |
| `Script_OpSound` | 0x8002EC74 | Sound opcode: plays an immediate effect or updates and replays the script's current sound command. |
| `gScript_apfnCommand / gScript_wCommand` | 0x80090C50 / 0x8009B27C | 24 opcode handlers and the opcode being serviced (bit 0x8000 = busy). |
| `gScript_pStream / gScript_wState` | 0x8009B290 / 0x8009B2A4 | Stream cursor; 0x8000 running, 0x4000 waiting on a text box, low bits = script number. |
| `Script_OpShowImage` | 0x8002E730 | Opcode 5: swap the cutscene picture — copy the staged image in VRAM, or fade and pull the next one from disc. |
| `Main_RunCampaignMap` | 0x8002D2D8 | Mode 0xC5: the fogged 3D map you walk between locations (IDB name, live-confirmed). |
| `gCampaignMap_Location` (module: overworld) | 0x8016960C | Where you stand: 0–9 the world-map sites, 10–15 the town (Town Plaza, Shrine, Duel Ground, Card Shop, Pharaoh's Palace, Hiding). The name box shows global string 0x8350 + index. |
| `gCampaignMap_aLocationTable` (module: overworld) | 0x801691A8 | 16 records of 0x42 bytes: lock flag, camera, marker, then four exits — story flag, target x/y, DPAD mask, destination (16 = none), move type. |
| `CampaignMap_PickExit` (module: overworld) | 0x80168E0C | Walks the four exits in order: skip unused, skip if the flag test fails, take the first whose mask matches the pressed direction. From the Palace: DOWN Duel Ground; RIGHT Shrine while flag 71 is clear, Hiding once flag 90 is set. |
| `CampaignMap_SetLocation / CampaignMap_MoveCameraDpad` (module: overworld) | 0x8016866C / 0x80168388 | Commit a move (the camera lerps between sites); the free-look camera on the world map. |


## Build deck & trunk

Mode slot 7. Editing is staged while the deck is incomplete. Once an edit
restores 40 cards, the compacted and card-id-sorted result can synchronize to
the save block before the screen exits. The not-ready dialog's EXIT route can
also commit an incomplete deck, and every duel entrance re-checks the
committed deck.

| symbol | address | description |
|---|---|---|
| `Main_RunBuildDeckMenu` | 0x8002D370 | The deck-building screen's mode tick. |
| `working deck count` | 0x801D560C | The staged deck size (40 ↔ 39 during edits); guards read it inside the screen, and the committed deck elsewhere. |
| `gDuel_awPlayerDeck` | 0x801D0200 | Your 40-card deck (part of the save block). |
| `gLibrary_abCardChest` | 0x801D0250 | Your trunk — per-card counts of everything you own (part of the save block). |
| `BuildDeck_CompareCard` | 0x80032B60 | One of the two generic list comparators (with `compare_rec_two_level_std`): START cycles 7 sort orders, each mode rebuilds the records' sort keys and picks a comparator; trunk orders are computed once then cached, the deck re-sorts live. |
| `gCard_asNameSortKey` | 0x801D4D8E | Per-card sort keys for that comparison. |


## Card library

Mode slot 4. The card-detail page is a modal inside it; the full-size card art streams from the disc when the page opens.

| symbol | address | description |
|---|---|---|
| `Main_RunLibraryMenu` | 0x8002D0E0 | The Library (card catalog) screen's mode tick. |
| `gCardGrid_bCursorColumn / Row` | 0x8009B258 / 59 | The card grid's cursor position, one byte each (10-wide grid). The selected CARD is tracked by the global `gDuel_wSelectedCardID`. |
| `Library_MarkOwnedCards` | 0x8002BF3C | Marks a card as seen in the catalog. |
| `Library_UpdateCardUsedFlag` | 0x8002CCE4 | Flips that seen-flag. |


## Password screen

Mode slot 10 — a real mode switch. Passwords are one-time-use per save; a wrong or reused code is refused inside the screen's own module.

| symbol | address | description |
|---|---|---|
| `Main_RunPasswordMenu` | 0x8002D684 | The password screen's mode tick (a name dispute settled by tracing). |
| `gPassword_abDigits` (module: password) | 0x8016D410 | The 8 entry digits, one byte each. |
| `gLibrary_aPasswordCardData` | 0x801A8008 | The password + starchip-cost table the entry screen checks against — entering a password spends `gLibrary_dwStarchips` from your save. |


## Your save file

The persistent block at `0x801D02xx–0x801D07xx` — what actually goes to the memory card. These aren't tied to one screen: they're earned, spent, and edited all over the game.

| symbol | address | description |
|---|---|---|
| `gDuel_awPlayerDeck` | 0x801D0200 | Your deck in the save block — synchronized compacted and SORTED by card id once a staged edit returns to 40 cards, or on the not-ready dialog's EXIT route with trailing zeros when short. |
| `gLibrary_abCardChest` | 0x801D0250 | Per-card ownership counts (Build Deck, Library, drops all touch it). |
| `player name` | 0x801D060C | Up to 5 characters as two-byte Shift-JIS (fullwidth ‘B’ = 0x8261), zeroed on New Game; the name box on any screen is rebuilt from it through `Text_SjisToGlyphCodes`. |
| `gCampaign_abStoryFlags` | 0x801D0618 | Story-progress bits (see the engine section). |
| `gFreeDuel_dwUnlockedDuelists` | 0x801D06F4 | Which duelists are unlocked in Free Duel (0xFFFFFFFF = all). |
| `gFreeDuel_aDuelistRecords` | 0x801D071C | Forty grid records of {u16 wins, u16 losses}; slot 0 is the Build Deck tile, duelist IDs 1–39 use the remaining slots, and normal updates cap each counter at 999. |
| `gDuel_awRecentCardDrops` | 0x801D07BC | Sixteen recently acquired card IDs (drops and password buys both shift in); exact C compacts all 16 slots, and the list drives the trunk's New! tags and NEW sort. |
| `gLibrary_dwStarchips` | 0x801D07E0 | Your starchip balance — spent on password exchanges (deduction byte-verified live). |
| `gLibrary_wViewerCardID` | 0x8009B246 | The card the chest / library viewer is showing (u16); the recomp clamps its two writers for clone ids. |
| `gLibrary_aCardArtRecord` | 0x801DC000 | CD-DMA target of a card's 2D record from LBA 10817+7×id: 102×96 8bpp art, its 256-colour CLUT, the baked 96×14 4bpp title, a 16×88 strip; LoadImage then uploads them to (256,256), (512,240), (256,352), (312,256). |
| `used-password flags` | ~0x801D0698 | Set when a password is redeemed; re-entry then refuses (“already put in that password”). Bit layout still being mapped. |


## Free duel & name entry

Mode slot 6. An 8×5 opponent grid (Build Deck tile at top-left); the cursor roams every tile, but only duelists whose unlock bit is set are drawn. Selecting with an illegal deck is refused before `gDuel_bOpponentID` is even written. Both screens run from modules that share the 0x80160000 range.

| symbol | address | description |
|---|---|---|
| `Main_RunFreeDuelMenu` | 0x8002D3F8 | Free Duel opponent-select mode tick; calls the module's entry `FreeDuel_Entry` (0x80168FB4) by fixed address. |
| `Main_InitFreeDuelMenu` | 0x8003B9BC | The tick's one-shot init: queues the CD request, then calls the module's `FreeDuel_Init` with the portrait buffer (arena slot 0, 0x80100000). |
| `gFreeDuel_bCursorColumn / Row` | 0x8009B366 / 67 | The committed grid cell, one byte per axis. |
| `gFreeDuel_bTargetColumn / Row` | 0x8009B36C / 6D | The pending cell the DPAD writes; the cursor glides to it over 8 frames, then it becomes the committed pair. |
| `FreeDuel_UpdateScreen` (module: free_duel) | 0x80168C7C | Per-frame: cursor tween, scrollbar, then DPAD / confirm / cancel. |
| `FreeDuel_UpdateScrollbar` (module: free_duel) | 0x80168004 | Keeps the grid scrolled to the cursor, then places the thumb at `7 + (cursor_y − 40) × 72 / 364` — it glides because it follows the tweened cursor. |
| `FreeDuel_PlaceCursor / FreeDuel_UpdateCursorTween` (module: free_duel) | 0x80168090 / 0x80168A9C | Cell to pixels (col×56+20, row×52+40); the 8-frame glide and commit. |
| `FreeDuel_Init` (module: free_duel) | 0x8016824C | Screen init: bumps the returning duelist's W/L, builds the availability table from the met-flags, uploads the 40 portraits (48×48 8bpp + 64-colour CLUT each) to VRAM pages 18/20, spawns one sprite per available cell. |
| `gFreeDuel_abGridAvailable` (module: free_duel) | 0x80169030 | 8×5 bytes: which cells can be selected. |
| `Main_RunNameEntry` | 0x8002D62C | The name-entry screen's mode tick. |
| `gNameEntry_bColumn / Row` (module: name_entry) | 0x8016D401 / 02 | The crosshair on a 15×9 letter grid, wrapping both ways; pixels are col×20+22, row×18+24. |
| `gNameEntry_nNameLen / gNameEntry_pName` (module: name_entry) | 0x8016D42C / 0x8016D418 | Characters typed (cap 5) and the pointer to the save-block name. |
| `TextBox_GetGlyphAt` (module: name_entry) | 0x8016868C | How a letter is picked: the grid is itself a text box, so X looks up the glyph under the cursor and takes its Shift-JIS code. No character table anywhere. |
| `NameEntry_UpdateDialog` (module: name_entry) | 0x80169734 | Builds the “Your NAME is …” box from a pending string id and polls `gDialog_bChoice`: NO returns to typing, YES leaves the screen. |


## Duel — setup

| symbol | address | description |
|---|---|---|
| `Duel_ShuffleDeck` | 0x800243F4 | Shuffles one deck (via the 40-byte permutation buffers). |
| `Duel_ShuffleBothDecks` | 0x800245A0 | Shuffles yours and the computer's at duel start. |
| `gDuel_awPlayerDeckShuffle / gDuel_awOpponentDeckShuffle` | 0x80177F94 / FBC | The two shuffle-permutation buffers. |
| `gDuel_awPlayerShuffledDeck / gDuel_awOpponentShuffledDeck` | 0x80177FE8 / 0x80178038 | Shuffled working copies of each deck. |
| `gDuel_awOpponentDeckPool` | 0x801781D8 | The card pool the computer's deck is drawn from. |


## Duel — in progress

| symbol | address | description |
|---|---|---|
| `gDuel_bPlayingSide` | 0x8009B1D5 | Whose side is acting. |
| `gDuel_bOpponentID` | 0x8009B361 | Which duelist you're facing. |
| `gDuel_bTerrain` | 0x8009B364 | The current field terrain. |
| `gDuel_wSelectedCardID` | 0x8009B338 | The currently selected card — game-wide: it is also the Library grid's cursor (RIGHT +1, DOWN +10, proven live). |
| `gDuel_aPlayerHand` | 0x801A7E20 | Your hand (5 entries, 6 bytes each). |
| `gDuel_aActiveCards` | 0x801AB000 | The cards in play on the field. |
| `gDuel_aPlayerMonsters` | 0x801A7B64 | Your five monster-zone slots. |
| `gDuel_wPlayerLifePoint / gDuel_wPlayerLifePointDisplay` | 0x800EA004 / 002 | Your life points — actual vs the on-screen ticking number. |
| `gDuel_wOpponentLifePoint / gDuel_wOpponentLifePointDisplay` | 0x800EA024 / 022 | The computer's, same split. |
| `gDuel_wFusionResultCardID` | 0x800EA118 | Result of the last fusion attempt. |


## Duel — battle math

| symbol | address | description |
|---|---|---|
| `gDuel_adwCardStats` | 0x801D4244 | Per-card packed stats records (ATK/DEF/type/guardian stars). |
| `Duel_GetBaseCardStat` | 0x8002CBF4 | Base ATK or DEF ×10 from `gDuel_adwCardStats`, plus terrain, clamped to 9999. |
| `Duel_CalcGuardianStarMatchup` | 0x8002CB80 | The Guardian Star wheel: two star ids → +500 / −500 / 0 (a 6-star cycle and a 4-star cycle). |
| `Duel_CalcGuardianStarBonus` | 0x8001EE44 | The same bonus evaluated for two monsters on the field. |
| `Duel_CalcBattleAttack / Def` | 0x8001EF1C / F78 | Effective ATK / DEF in battle, bonuses applied. |
| `Duel_GetTerrainBoost` | 0x8002497C | The +/−500 field bonus by monster type on the current terrain. |
| `gDuel_aTerrainBoost` | 0x800909D4 | The table behind it. |
| `gDuel_aFusionTable` | 0x8017C2D8 | Every fusion combination in the game (u16 array). |
| `gDuel_awEquipTable` | 0x8017A1D8 | Which equips work on which monsters. |
| `gDuel_awRitualData` | 0x801799D8 | The ritual recipes. |


## Duel — the computer opponent

> What the AI interpreter is: every duelist ships a bytecode script on the disc — their personality. When it's the computer's turn, the game loads that script into `gAiScript_abHandScript` and `AiScript_Run` executes it instruction by instruction: test the board, branch, roll chances, and finally emit a move. The ~80 `aiInst*` functions are the instruction set of that little virtual machine. It runs only here — mid-duel, on the CPU's decision. Its cleverest pieces: `AiScript_FindBestAttack` pits every AI monster against every opposing slot (ATK difference + guardian-star bonus) and leaves the best score/attacker/target in three slots the script reads back, and `Ai_CompleteFusion` recursively solves fusion chains from the AI's own hand.

| symbol | address | description |
|---|---|---|
| `AiScript_Run` | 0x80070650 | The interpreter loop — dispatches each opcode via `gAiScript_apfnCommand`. |
| `gAiScript_apfnCommand` | 0x800916E0 | The opcode dispatch table (one entry per `aiInst*`). |
| `gAiScript_abHandScript` | 0x801A8000 | The loaded script being executed. The story scripts' offset table lives in the same bank, so the name is narrower than the bank. |
| `gAiScript_State` | 0x800F5BE8 | The VM's state block (position, stack, flags). |
| `gAiScript_aMemory` | 0x800F5B98 | The VM's scratch registers. |
| `AiScript_Init` | 0x800705D8 | Resets the VM before a decision. |
| `Ai_GetHandSize` | 0x80070710 | The opponent's configured hand-size parameter from `gDuel_aOpponentData`. |
| `gDuel_aOpponentData` | 0x800917F0 | Per-duelist AI parameter records (9 bytes each). |
| `Ai_CompleteFusion` | 0x800727C0 | The computer working out a fusion chain from its own hand. |
| `aiInst* (~80 opcodes)` | 0x80070988+ | The instruction set: jumps (`AiScript_JumpEqual/Rand/Mid`…), call/return with a real stack, VM arithmetic, card/type sets, board scanners (`AiScript_FindStrongest/Weakest/FindCard`…), the matchup search (`AiScript_FindBestAttack`), and the move-emitters (`AiScript_PlayFieldCard`, `AiScript_PlayFaceUp`, the combo pushers). |


## Duel — battle animation

| symbol | address | description |
|---|---|---|
| `Main_RunAnimatedBattle` | 0x8002D180 | The 3D battle-animation mode tick (dispatches on the fusion recipe id). |


## End of duel — rank & rewards

| symbol | address | description |
|---|---|---|
| `Duel_CalcRankScore` | 0x80021598 | Scores how you dueled — the number behind the S/A/B… POW/TEC grade. |
| `Duel_CalcRankScoreChange` | 0x80021558 | Scores one statistic against the `gDuel_awRankScoreChange` thresholds. |
| `gDuel_anRankScore / gDuel_awRankScoreChange` | 0x80179A04 / 98A8 | The running score and the per-deed point table. |
| `Duel_SelectCardDrop` | 0x80021810 | Rolls your reward card — a weighted draw (threshold 1–2048) over a 722-entry pool row. |
| `gDuel_awSaPowCardDrops / gDuel_awBcdCardDrops / gDuel_awSaTecCardDrops` | 0x8017878C… | The three drop tables, picked by your win style. |
| `gDuel_wCardDropID` | 0x80179A14 | The rolled drop (the 16-card recent-acquisition history lives in the save block). |


## Game over & credits

| symbol | address | description |
|---|---|---|
| `Main_RunGameOver` | 0x8002D730 | The game-over screen's mode tick. |
| `Main_RunCredits` | 0x8002DA1C | The credits roll's mode tick. |
| `Main_RunHirata` | 0x8002D7C4 | Empty in retail — a compiled-out developer mode, named for the Konami programmer whose source path survives in the debug strings. |


## Debug leftovers

Development machinery that shipped in the retail binary.

| symbol | address | description |
|---|---|---|
| `gDebug_nLastSoundID` | 0x8009B2C4 | Debug menu: last sound id played. |
| `FntOpen / FntPrint / FntFlush / SetDumpFnt` | 0x8007E9B0… | The PsyQ on-screen debug text channel — the game's printf-to-screen. |
| `AiScript_Print` | 0x800736C4 | An AI opcode that prints two debug strings (the “check_point” machinery) — AI scripts could narrate themselves during development. |
| `PCread` | 0x80073758 | Dev-kit leftover: file reads from the attached PC's filesystem (LIBSN); inert on retail hardware. |

---
Placements were proven in the 2026-08-31 and 2026-09-02 live trace sessions (see the Trace Dossier and `notes/research/findings.md`); the rest are from matched code and the community RAM map. Evidence system: The Symbol Ledger. Symbols tagged with a module live in `config/modules/<module>.txt` because those screens share one address range. Excluded on purpose: the 30 still-disputed names and anything hedged. The build stays byte-identical to the original through every rename.
