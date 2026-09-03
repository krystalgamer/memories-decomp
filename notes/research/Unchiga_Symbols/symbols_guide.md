# Symbols Guide

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
| `mainLoop` | 0x8002DD74 | The master dispatch: never returns, runs the update funcs each frame, then calls the current mode's loop through a function-pointer table. |
| `gameInit` | 0x80012B50 | Boot-time setup before `mainLoop` takes over. |
| `vertBlankCallback` | 0x80012CD4 | Runs once per frame on the vertical blank. |
| `prngSeed` | 0x800FE6F8 | The random-number seed — shuffles, drops, and AI coin-flips all trace back here. |
| `setFilePosTable` | 0x800136E4 | Boot: resolves every file in `fileNames` to its disc position. |
| `fileNames` | 0x8009078C | Names of the data files on the disc. |
| `filePositions` | 0x800E9EA8 | Where each file lives on the CD. |
| `filePosition` | 0x800138F4 | Looks a file's position up. |
| `fileExists` | 0x8005C4F0 | Checks whether a file is present (wraps PsyQ `DsSearchFile`). |


### Text boxes and dialogs (also everywhere)

> How text works: every string on screen — a menu label, a prompt, the letter grid you pick a name from, the name you typed — is a text record driven by one typewriter. A record holds a string id, not text; the screen's strings live in a bank loaded with its module, and a global bank holds the always-needed ones (location and card names). Strings are glyph indices with control bytes: `00` space, `F8 xx yy` position/style, `FA` wait for X, `FB 02` a choice with two lines, `FB 80 lo hi lo hi` jump table by the answer, `FC xx yy` splice dynamic text, `FE` newline, `FF` end.

| symbol | address | description |
|---|---|---|
| `textRecords` | 0x800EB0F8 | The text-box records, 0x64 bytes each: current text pointer (advances as glyphs are typed), glyph list, three widgets, flags (0x800 build requested, 0x4000 started, 0x2000 done), string id, rect, glyph cell size, and the record's glyph-sprite budget. |
| `textSpriteSlotRanges` | 0x80090E58 | Cumulative glyph-sprite slot boundaries per record: 0, 255, 415, 575, 620. |
| `textBank / textBankOffsets` | 0x801B0000 / 0x801C0000 | The resident screen's strings (0xFF-terminated) and the u16 offset per string id; ids 0x500+ are shifted by 0x100, ids 0xD000+ index the second table region. |
| `globalTextOffsets` | 0x801D5800 | Offset table of the always-resident bank (ids 0x8000+, base 0x801D0000): location names are 0x8350 + index, Metropolis … Hiding. |
| `textStringLookup` | 0x8003B744 | String id → pointer into the right bank. |
| `glyphCodeTable` | 0x801D9000 | One u32 per font glyph, low half = its Shift-JIS code. ‘A’ is glyph 24, ‘B’ 45, ‘a’ 3, ‘0’ 56. |
| `sjisToGlyphCodes` | 0x8003BC40 | The inverse: a Shift-JIS string (like the player name) into glyph codes so the typewriter can show it. |
| `textBoxCreate / textBoxCreateFlagged` | 0x80035BE4 / 0x80035C38 | Open a box: slot, string id, x, y, w, h (and extra flags). |
| `textBoxSetRect / textBoxInitRecord` | 0x80035AB8 / 0x80035AF0 | The two halves of create: geometry, then id, cell size and sprite budget. |
| `textRecordBuildStep` | 0x800393B0 | The typewriter: first call resolves the string and allocates the widgets, then one glyph per call. `poll_call_393b0_until_bit13` spins it to the end at once. |
| `textBoxSetPos / textBoxDestroy` | 0x80039934 / 0x80035B7C | Move a box with its widgets; close it and free everything. |
| `widgetSlideSine` | 0x80043230 | The sine slide dialogs use to enter and leave (the name prompt travels y 248 ↔ 176 on it). |


### Dialog choices (YES / NO boxes)

A choice box is an ordinary text box whose string ends in a choice attribute. The EXE attaches a cursor object and the screen reads the answer from one byte. The same cursor object doubles as the blinking “press X” arrow when a page fills.

| symbol | address | description |
|---|---|---|
| `dialogChoice` | 0x8009B34D | The selected line: 0 = first (YES), 1 = second (NO). Live while you move, final on confirm. |
| `dialogChoiceCount / dialogChoiceEnabled` | 0x8009B345 / 0x8009B336 | How many lines, and a bit per line that may be picked. |
| `dialogInputState` | 0x8009B327 | 0x40 confirmed (low bits = the choice), 0x80 cancelled. |
| `dialogChoiceOpen` | 0x800374F4 | Spawns the cursor at the box's bottom-right corner (sprite 0x20C). |
| `dialogChoiceTick` | 0x800371A8 | Per-frame: confirm, cancel, or hand the DPAD to the input handler. |
| `dialogChoiceInput` | 0x8003700C | DOWN and UP move within the count, SE 6, re-highlight. |
| `dialogChoiceHighlight / objPulseColourUpdate` | 0x80036F80 / 0x80037110 | Cursor colour by whether the line is enabled; the triangle-wave blink from the frame counter. |


### Screen fades (also everywhere)

Every screen change goes through one fade system. The fade-out blocks: the mode loop calls it and nothing else on the screen runs until it lands, about 48 frames at step 8.

| symbol | address | description |
|---|---|---|
| `screenFade` | 0x800E9EC8 | The fade block: colour, current level, target, flags (0x80 in flight, 0x01 strip mode), step, head, and 30 per-strip brightness bytes. |
| `screenFadeOutWait` | 0x80015B00 | What mode loops call: starts the fade to black and pumps frames until done. |
| `screenFadeOutStart / screenFadeOutInit` | 0x80015904 / 0x800158B8 | Start (strip mode, step 8) and init (head 255, target 0, all strips at the current level). |
| `screenFadeUpdate` | 0x80015310 | Per-frame update; latches the RGB and the overlay flag when the target is reached. |
| `screenFadeStepStrips` | 0x800151D8 | The mirrored walker: strips `i` and `29-i` share a value, so fade-out closes from the top and bottom edges to the middle and fade-in opens from the middle. |
| `drawScreenFadeOverlay` | 0x800154E4 | Draws 30 semi-transparent black 320×8 boxes, darkness 255 − strip. |
| `screenFadeOverlayOn` | 0x8009B141 | Keeps the overlay painting solid black between a fade-out and the next screen's fade-in — that is what hides the disc load. |


### Story flags (also everywhere)

| symbol | address | description |
|---|---|---|
| `storyFlags` | 0x801D0618 | The save block's story-progress bit array, MSB-first: byte `id >> 3`, bit `0x80 >> (id & 7)`. |
| `storyFlagTest` | 0x8002CCA8 | Tests one flag; bit 0x8000 of the id means “must be clear”. Overworld exits, script gates and modules all go through it. |


### The sound driver (also everywhere)

| symbol | address | description |
|---|---|---|
| `SD_Init` | 0x800492D8 | Sound-system init. |
| `SD_Term` | 0x80049694 | Sound-system shutdown. |
| `SD_LoadData` | 0x80046768 | Loads `sd_bgm.dat` / `sd_se.dat` / `master.xa` from disc. |
| `SD_SEPlay` | 0x80048658 | Plays sound effect `id` at volume `vol`. Known ids: 6 cursor move, 7 confirm, 8 cancel, 0xB page advance, 0xC typing, 0x2F option toggle / grid move, 0x30 confirm a site. |
| `SD_SEPlayFull` | 0x8003FEE0 | Convenience wrapper: plays an effect at full volume — always `SD_SEPlay(id, 0xFF)`. |
| `musicTrack` | 0x801EA800 | The current music track. (Below this, the SPU layer keeps original Sony PsyQ names.) |


## Main menu

NEW GAME / LOAD / 2P DUEL / TRADE / OPTION. The menu's own logic runs from a module loaded from disc into `0x80180000+` — the EXE supplies widgets, sound, and drawing.

| symbol | address | description |
|---|---|---|
| `mainMenuLoop` | 0x8002D588 | The main-menu mode tick. Dialogs (like TRADE's memory-card prompt) are modals inside it — the game never leaves this mode for them. |
| `menuCursor` (module: main_menu) | 0x80184594 | The shared menu cursor: 0–4 on the main menu, 5–10 on the post-load menu (CAMPAIGN=5 … SAVE=10). |


## Options

| symbol | address | description |
|---|---|---|
| `optionsMenuLoop` | 0x8002D6C8 | The OPTION screen's mode tick. |
| `SD_SetOutputType` | 0x80046FA0 | The STEREO/MONO setter (0=stereo, 1=mono): writes the driver's mode byte and re-mixes CD audio on the spot. |
| `soundMode` | 0x8009B408 | The stored stereo/mono setting. |


## Campaign — story script and overworld

> What a cutscene is: not a screen module. After the name is confirmed the mode byte goes to `0xC2` and `campaignLoop` runs a bytecode script: it picks a script through a table of self-relative offsets (in the same 0x801A8000 bank the AI scripts use), latches a stream cursor and dispatches opcodes through a 24-entry table. The dialogue is an ordinary bank string typed by the typewriter; the pictures are swapped by opcode 5. Simon's “Run away / Keep listening” never reaches the script at all — it is a jump table inside the text (`FB 80`), with all four “Run away” exits pointing at the same “Drat! He's gone” ending.

| symbol | address | description |
|---|---|---|
| `campaignLoop` | 0x8002CE64 | Mode 0xC2: the story/cutscene tick (IDB name, live-confirmed). |
| `scriptRunTick` | 0x8002FA54 | The bytecode driver: selects a script, latches the stream, runs one opcode per call. |
| `scriptCommandTable / scriptCommand` | 0x80090C50 / 0x8009B27C | 24 opcode handlers and the opcode being serviced (bit 0x8000 = busy). |
| `scriptStreamPtr / scriptState` | 0x8009B290 / 0x8009B2A4 | Stream cursor; 0x8000 running, 0x4000 waiting on a text box, low bits = script number. |
| `scriptOpShowImage` | 0x8002E730 | Opcode 5: swap the cutscene picture — copy the staged image in VRAM, or fade and pull the next one from disc. |
| `campaignOverworldLoop` | 0x8002D2D8 | Mode 0xC5: the fogged 3D map you walk between locations (IDB name, live-confirmed). |
| `overworldLocation` (module: overworld) | 0x8016960C | Where you stand: 0–9 the world-map sites, 10–15 the town (Town Plaza, Shrine, Duel Ground, Card Shop, Pharaoh's Palace, Hiding). The name box shows global string 0x8350 + index. |
| `overworldLocationTable` (module: overworld) | 0x801691A8 | 16 records of 0x42 bytes: lock flag, camera, marker, then four exits — story flag, target x/y, DPAD mask, destination (16 = none), move type. |
| `overworldPickExit` (module: overworld) | 0x80168E0C | Walks the four exits in order: skip unused, skip if the flag test fails, take the first whose mask matches the pressed direction. From the Palace: DOWN Duel Ground; RIGHT Shrine while flag 71 is clear, Hiding once flag 90 is set. |
| `overworldSetLocation / overworldDpadCamera` (module: overworld) | 0x8016866C / 0x80168388 | Commit a move (the camera lerps between sites); the free-look camera on the world map. |


## Build deck & trunk

Mode slot 7. Editing is STAGED: X-moves change working state only; the deck commits to the save block — compacted and sorted by card id — when you exit (any route). Leaving with fewer than 40 cards triggers the “YOUR DECK ISN'T READY!” modal, and every duel entrance re-checks the committed deck.

| symbol | address | description |
|---|---|---|
| `buildDeckMenuLoop` | 0x8002D370 | The deck-building screen's mode tick. |
| `working deck count` | 0x801D560C | The staged deck size (40 ↔ 39 during edits); guards read it inside the screen, and the committed deck elsewhere. |
| `playerDeck` | 0x801D0200 | Your 40-card deck (part of the save block). |
| `trunk` | 0x801D0250 | Your trunk — per-card counts of everything you own (part of the save block). |
| `compDeckCard` | 0x80032B60 | One of the two generic list comparators (with `compare_rec_two_level_std`): START cycles 7 sort orders, each mode rebuilds the records' sort keys and picks a comparator; trunk orders are computed once then cached, the deck re-sorts live. |
| `nameKeys` | 0x801D4D8E | Per-card sort keys for that comparison. |


## Card library

Mode slot 4. The card-detail page is a modal inside it; the full-size card art streams from the disc when the page opens.

| symbol | address | description |
|---|---|---|
| `libraryMenuLoop` | 0x8002D0E0 | The Library (card catalog) screen's mode tick. |
| `gridCursorCol / Row` | 0x8009B258 / 59 | The card grid's cursor position, one byte each (10-wide grid). The selected CARD is tracked by the global `selectedCardId`. |
| `setLibraryUsed` | 0x8002BF3C | Marks a card as seen in the catalog. |
| `toggleLibraryUsed` | 0x8002CCE4 | Flips that seen-flag. |


## Password screen

Mode slot 10 — a real mode switch. Passwords are one-time-use per save; a wrong or reused code is refused inside the screen's own module.

| symbol | address | description |
|---|---|---|
| `passwordMenuLoop` | 0x8002D684 | The password screen's mode tick (a name dispute settled by tracing). |
| `passwordDigits` (module: password) | 0x8016D410 | The 8 entry digits, one byte each. |
| `cardPasswords` | 0x801A8008 | The password + starchip-cost table the entry screen checks against — entering a password spends `starchips` from your save. |


## Your save file

The persistent block at `0x801D02xx–0x801D07xx` — what actually goes to the memory card. These aren't tied to one screen: they're earned, spent, and edited all over the game.

| symbol | address | description |
|---|---|---|
| `playerDeck` | 0x801D0200 | Your 40-card deck — stored compacted and SORTED by card id, trailing zeros when short (commit-on-exit proven by byte diff). |
| `trunk` | 0x801D0250 | Per-card ownership counts (Build Deck, Library, drops all touch it). |
| `player name` | 0x801D060C | Up to 5 characters as two-byte Shift-JIS (fullwidth ‘B’ = 0x8261), zeroed on New Game; the name box on any screen is rebuilt from it through `sjisToGlyphCodes`. |
| `storyFlags` | 0x801D0618 | Story-progress bits (see the engine section). |
| `freeDuelUnlocks` | 0x801D06F4 | Which duelists are unlocked in Free Duel (0xFFFFFFFF = all). |
| `duelistRecords` | 0x801D071C | Your record vs every duelist — 39 {u16 wins, u16 losses} pairs indexed by `opponentId`, both capped 9999. |
| `lastCardDrops` | 0x801D07BC | The cards you most recently WON/acquired (drops and password buys both shift in; drives the trunk's New! tags and the NEW sort; capacity ~15-16 per the operator, extent unverified). |
| `starchips` | 0x801D07E0 | Your starchip balance — spent on password exchanges (deduction byte-verified live). |
| `used-password flags` | ~0x801D0698 | Set when a password is redeemed; re-entry then refuses (“already put in that password”). Bit layout still being mapped. |


## Free duel & name entry

Mode slot 6. An 8×5 opponent grid (Build Deck tile at top-left); the cursor roams every tile, but only duelists whose unlock bit is set are drawn. Selecting with an illegal deck is refused before `opponentId` is even written. Both screens run from modules that share the 0x80160000 range.

| symbol | address | description |
|---|---|---|
| `freeDuelMenuLoop` | 0x8002D3F8 | Free Duel opponent-select mode tick; calls the module's entry `freeDuelModuleEntry` (0x80168FB4) by fixed address. |
| `opponentGridCol / Row` | 0x8009B366 / 67 | The committed grid cell, one byte per axis. |
| `freeDuelTargetCol / Row` | 0x8009B36C / 6D | The pending cell the DPAD writes; the cursor glides to it over 8 frames, then it becomes the committed pair. |
| `freeDuelScreenTick` (module: free_duel) | 0x80168C7C | Per-frame: cursor tween, scrollbar, then DPAD / confirm / cancel. |
| `freeDuelScrollbarUpdate` (module: free_duel) | 0x80168004 | Keeps the grid scrolled to the cursor, then places the thumb at `7 + (cursor_y − 40) × 72 / 364` — it glides because it follows the tweened cursor. |
| `freeDuelCursorPlace / freeDuelCursorTweenTick` (module: free_duel) | 0x80168090 / 0x80168A9C | Cell to pixels (col×56+20, row×52+40); the 8-frame glide and commit. |
| `freeDuelGridAvailable` (module: free_duel) | 0x80169030 | 8×5 bytes: which cells can be selected. |
| `nameEntryLoop` | 0x8002D62C | The name-entry screen's mode tick. |
| `nameEntryCol / Row` (module: name_entry) | 0x8016D401 / 02 | The crosshair on a 15×9 letter grid, wrapping both ways; pixels are col×20+22, row×18+24. |
| `nameEntryNameLen / nameEntryNamePtr` (module: name_entry) | 0x8016D42C / 0x8016D418 | Characters typed (cap 5) and the pointer to the save-block name. |
| `textGlyphAt` (module: name_entry) | 0x8016868C | How a letter is picked: the grid is itself a text box, so X looks up the glyph under the cursor and takes its Shift-JIS code. No character table anywhere. |
| `nameEntryDialogTick` (module: name_entry) | 0x80169734 | Builds the “Your NAME is …” box from a pending string id and polls `dialogChoice`: NO returns to typing, YES leaves the screen. |


## Duel — setup

| symbol | address | description |
|---|---|---|
| `shuffleDeck` | 0x800243F4 | Shuffles one deck (via the 40-byte permutation buffers). |
| `shuffleBothDecks` | 0x800245A0 | Shuffles yours and the computer's at duel start. |
| `playerPerm / cpuPerm` | 0x80177F94 / FBC | The two shuffle-permutation buffers. |
| `playerDeckTmp / cpuDeckTmp` | 0x80177FE8 / 0x80178038 | Shuffled working copies of each deck. |
| `cpuDeckPool` | 0x801781D8 | The card pool the computer's deck is drawn from. |


## Duel — in progress

| symbol | address | description |
|---|---|---|
| `playingSide` | 0x8009B1D5 | Whose side is acting. |
| `opponentId` | 0x8009B361 | Which duelist you're facing. |
| `terrain` | 0x8009B364 | The current field terrain. |
| `selectedCardId` | 0x8009B338 | The currently selected card — game-wide: it is also the Library grid's cursor (RIGHT +1, DOWN +10, proven live). |
| `playerHand` | 0x801A7E20 | Your hand (5 entries, 6 bytes each). |
| `activeCards` | 0x801AB000 | The cards in play on the field. |
| `playerMonsters` | 0x801A7B64 | Your five monster-zone slots. |
| `playerLp / playerDisplayedLp` | 0x800EA004 / 002 | Your life points — actual vs the on-screen ticking number. |
| `cpuLp / cpuDisplayedLp` | 0x800EA024 / 022 | The computer's, same split. |
| `fusionResult` | 0x800EA118 | Result of the last fusion attempt. |


## Duel — battle math

| symbol | address | description |
|---|---|---|
| `statsArray` | 0x801D4244 | Per-card packed stats records (ATK/DEF/type/guardian stars). |
| `baseCardStat` | 0x8002CBF4 | Base ATK or DEF ×10 from `statsArray`, plus terrain, clamped to 9999. |
| `gsBonus` | 0x8002CB80 | The Guardian Star wheel: two star ids → +500 / −500 / 0 (a 6-star cycle and a 4-star cycle). |
| `monGsBonus` | 0x8001EE44 | The same bonus evaluated for two monsters on the field. |
| `monBattleEffAtk / Def` | 0x8001EF1C / F78 | Effective ATK / DEF in battle, bonuses applied. |
| `getTerrainBoost` | 0x8002497C | The +/−500 field bonus by monster type on the current terrain. |
| `terrainBoosts` | 0x800909D4 | The table behind it. |
| `fusionTable` | 0x8017C2D8 | Every fusion combination in the game (u16 array). |
| `equipTable` | 0x8017A1D8 | Which equips work on which monsters. |
| `ritualData` | 0x801799D8 | The ritual recipes. |


## Duel — the computer opponent

> What the AI interpreter is: every duelist ships a bytecode script on the disc — their personality. When it's the computer's turn, the game loads that script into `handAiScript` and `aiMain` executes it instruction by instruction: test the board, branch, roll chances, and finally emit a move. The ~80 `aiInst*` functions are the instruction set of that little virtual machine. It runs only here — mid-duel, on the CPU's decision. Its cleverest pieces: `aiInstFindBestA` pits every AI monster against every opposing slot (ATK difference + guardian-star bonus) and leaves the best score/attacker/target in three slots the script reads back, and `aiCompleteFusion` recursively solves fusion chains from the AI's own hand.

| symbol | address | description |
|---|---|---|
| `aiMain` | 0x80070650 | The interpreter loop — dispatches each opcode via `aiFunctionTable`. |
| `aiFunctionTable` | 0x800916E0 | The opcode dispatch table (one entry per `aiInst*`). |
| `handAiScript` | 0x801A8000 | The loaded script being executed. The story scripts' offset table lives in the same bank, so the name is narrower than the bank. |
| `aiStruct` | 0x800F5BE8 | The VM's state block (position, stack, flags). |
| `aiMemory` | 0x800F5B98 | The VM's scratch registers. |
| `aiStructInit` | 0x800705D8 | Resets the VM before a decision. |
| `aiHandSize` | 0x80070710 | The opponent's configured hand-size parameter from `oppData`. |
| `oppData` | 0x800917F0 | Per-duelist AI parameter records (9 bytes each). |
| `aiCompleteFusion` | 0x800727C0 | The computer working out a fusion chain from its own hand. |
| `aiInst* (~80 opcodes)` | 0x80070988+ | The instruction set: jumps (`aiInstJumpEq/Rand/Mid`…), call/return with a real stack, VM arithmetic, card/type sets, board scanners (`aiInstStrongest/Weakest/FindCard`…), the matchup search (`aiInstFindBestA`), and the move-emitters (`aiInstFieldPlay`, `aiInstFaceUpPlay`, the combo pushers). |


## Duel — battle animation

| symbol | address | description |
|---|---|---|
| `animatedBattleLoop` | 0x8002D180 | The 3D battle-animation mode tick (dispatches on the fusion recipe id). |


## End of duel — rank & rewards

| symbol | address | description |
|---|---|---|
| `calcRankScore` | 0x80021598 | Scores how you dueled — the number behind the S/A/B… POW/TEC grade. |
| `rankScoreChange` | 0x80021558 | Scores one statistic against the `rankScoreDiffs` thresholds. |
| `rankScore / rankScoreDiffs` | 0x80179A04 / 98A8 | The running score and the per-deed point table. |
| `cardDrop` | 0x80021810 | Rolls your reward card — a weighted draw (threshold 1–2048) over a 722-entry pool row. |
| `powDropPool / bcdDropPool / tecDropPool` | 0x8017878C… | The three drop tables, picked by your win style. |
| `cardDropId` | 0x80179A14 | The rolled drop (the last-10 history lives in the save block). |


## Game over & credits

| symbol | address | description |
|---|---|---|
| `gameOverLoop` | 0x8002D730 | The game-over screen's mode tick. |
| `creditsLoop` | 0x8002DA1C | The credits roll's mode tick. |
| `hirataLoop` | 0x8002D7C4 | Empty in retail — a compiled-out developer mode, named for the Konami programmer whose source path survives in the debug strings. |


## Debug leftovers

Development machinery that shipped in the retail binary.

| symbol | address | description |
|---|---|---|
| `debugSoundId` | 0x8009B2C4 | Debug menu: last sound id played. |
| `FntOpen / FntPrint / FntFlush / SetDumpFnt` | 0x8007E9B0… | The PsyQ on-screen debug text channel — the game's printf-to-screen. |
| `aiInstPrint` | 0x800736C4 | An AI opcode that prints two debug strings (the “check_point” machinery) — AI scripts could narrate themselves during development. |
| `PCread` | 0x80073758 | Dev-kit leftover: file reads from the attached PC's filesystem (LIBSN); inert on retail hardware. |

---
Placements were proven in the 2026-08-31 and 2026-09-02 live trace sessions (see the Trace Dossier and `notes/research/findings.md`); the rest are from matched code and the community RAM map. Evidence system: The Symbol Ledger. Symbols tagged with a module live in `config/modules/<module>.txt` because those screens share one address range. Excluded on purpose: the 30 still-disputed names and anything hedged. The build stays byte-identical to the original through every rename.
