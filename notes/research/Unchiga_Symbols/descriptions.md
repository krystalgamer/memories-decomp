# Symbol descriptions — the naming gate

> **External reference — mirror of Unchiga/ygofm-decomp.** This file is copied verbatim from that repository's `notes/research/` (the matching decomp the research was done in). Paths, tools and rules it names (`config/symbol_addrs.txt`, `tools/gen_symbols.py`, `tools/setup.sh`, `config/modules/`, `tools/gen_research_notes.py`, ...) are that project's, not this one's. The addresses and evidence are the claim; the names follow this repo's `notes/naming-conventions.md` (`Subsystem_VerbObject` / `gSubsystem_Role`) since 2026-09-03 and are ready for `config/slus_01411/symbols.txt` and `notes/semantic-symbol-map.csv` after the usual per-address review.

Operator rule (2026-08-31): every properly named function and variable gets a
short description (a couple of sentences max) BEFORE its name is considered
done. Descriptions are written 10 at a time, grounded in the matched body /
live trace, and the operator corrects them in review. Format: address, name,
description. Batches are numbered; a symbol without a row here is not fully
named yet.

## Batch 1 (game-meaningful functions, address order)

| address | name | description |
|---|---|---|
| 0x80012B50 | `Main_Init` | One-time boot setup: brings up the core subsystems, installs the vblank callback, then hands off to `Main_Loop`. Never returns to its caller. |
| 0x80012CD4 | `Main_VBlankCB` | The VSync interrupt callback. Ticks the per-frame counters and flags that the game's pacing and update loops read. |
| 0x800136E4 | `File_SetPositionTable` | Boot-time init that also sets up the draw environment: installs the per-frame pump callback, then resolves each name in `gFile_apszName[]` to its disc location, building the file-position table (with debug prints). |
| 0x800138F4 | `File_GetPosition` | Looks up a file's disc location in the table `File_SetPositionTable` built at boot and returns that entry for the requested file. |
| 0x8001EE44 | `Duel_CalcGuardianStarBonus` | Guardian-star advantage between two monsters, in ATK/DEF points. Reads each side's two 4-bit star ids out of the packed `gDuel_adwCardStats` word and returns the resulting bonus. |
| 0x8001EF1C | `Duel_CalcBattleAttack` | A monster's effective ATK for battle resolution: base stat plus every applicable bonus (guardian star, terrain, equips). |
| 0x8001EF78 | `Duel_CalcBattleDefense` | Same as `Duel_CalcBattleAttack`, but for DEF. |
| 0x80021558 | `Duel_CalcRankScoreChange` | Scores one duel statistic: walks row `a0` of the `gDuel_awRankScoreChange` {threshold, value} pairs and returns the value paired with the first threshold the statistic falls under. |
| 0x80021598 | `Duel_CalcRankScore` | Computes the end-of-duel rank score: feeds each tracked duel statistic through `Duel_CalcRankScoreChange` and accumulates the total that decides the POW/TEC grade. |
| 0x80021810 | `Duel_SelectCardDrop` | Rolls the reward card: rolls a threshold in [1, 2048] and walks a 722-entry weight table (one drop-pool row) accumulating weights until the threshold is reached; returns that card's 1-based index (0 = none). |

## Batch 2 (functions, address order — approved batch 1 precedes)

(0x800218F0 and 0x8002CEE8 are excluded: their IDB names dispute the
matched bodies — tracked in suspects.md, not here, until 100%.)

| address | name | description |
|---|---|---|
| 0x800243F4 | `Duel_ShuffleDeck` | Shuffles one deck. Called per side with the deck, a slot in `gDuel_awPlayerShuffledDeck`, and a permutation buffer (the RAM map's 40-byte "deck shuffle permutation" arrays). |
| 0x800245A0 | `Duel_ShuffleBothDecks` | Duel-start convenience: calls `Duel_ShuffleDeck` once for the player and once for the CPU, handing each its own temp/permutation buffers. |
| 0x8002497C | `Duel_GetTerrainBoost` | Terrain bonus lookup with a range guard: requires the `gDuel_bTerrain` flag byte nonzero and monster type < 20, then falls through into the table-lookup body at 0x800249A4 (not a standalone function). |
| 0x8002BF3C | `Library_MarkOwnedCards` | Marks a card as owned/seen: sets the card's byte in the 722-entry `gLibrary_abCardChest` flag table and runs the change-notify path shared with the deck table. |
| 0x8002CB80 | `Duel_CalcGuardianStarMatchup` | The guardian-star wheel: given two star ids, returns +500 / -500 / 0. Ids 1-6 form a 6-star advantage cycle, 7-10 a 4-star cycle; across the two cycles the result is 0. Its matching source is `duel_calc_guardian_star_matchup.c`. |
| 0x8002CBF4 | `Duel_GetBaseCardStat` | Reads card record `gDuel_adwCardStats[idx-1]`, extracts ATK (low 9 bits) or DEF (bits 9-17) per the `shift` arg, multiplies by 10, adds `Duel_GetTerrainBoost` for the card's type, and clamps to 0-9999. |
| 0x8002CCE4 | `Library_UpdateCardUsedFlag` | Flips a card's seen-flag byte in the `gLibrary_abCardChest` table (the inverse partner of `Library_MarkOwnedCards`), with the same notify path. |
| 0x8002D0E0 | `Main_RunLibraryMenu` | Mode tick for the card Library screen, per the loop-family pattern (`Main_Loop` dispatches one `*Loop` per mode). Not yet live-confirmed the way `Main_RunOptionsMenu` was. |

## Batch 3 (functions — the mode-loop family)

| address | name | description |
|---|---|---|
| 0x8002D180 | `Main_RunAnimatedBattle` | Mode tick for the 3D battle animation. First call sets up the screen geometry and dispatches on the fusion recipe id (special-casing recipe 0x309); later calls poll the animation handlers until done. |
| 0x8002D2D8 | `Main_RunCampaignMap` | Mode tick for the campaign map. Standard shape for this family: one-shot init gated on the D_8009B26C 0x40 flag, then per-frame polling until the mode ends. |
| 0x8002D370 | `Main_RunBuildDeckMenu` | Mode tick for the deck-building screen; works directly against `gDuel_awPlayerDeck` and the build-deck helpers. |
| 0x8002D3F8 | `Main_RunFreeDuelMenu` | Mode tick for the Free Duel opponent-select screen (same one-shot-init + poll family shape). |
| 0x8002D588 | `Main_RunMenu` | Mode tick for the title/main menu — live-confirmed ambient. Delegates the actual menu logic to the disc-loaded menu module (calls into 0x8018001C / polls 0x80180390) and advances `rand()` every frame. |
| 0x8002D62C | `Main_RunNameEntry` | Mode tick for the name-entry screen: registers its input handler on first call, then polls the entry module until the name is committed. |
| 0x8002D6C8 | `Main_RunOptionsMenu` | Mode tick for the OPTION screen — live-confirmed ambient. Sibling of `Main_RunNameEntry`; calls `Options_Init` once, then polls `Options_Update`. |
| 0x8002D730 | `Main_RunGameOver` | Mode tick for the game-over screen; registers its handler and polls, same family shape. |
| 0x8002D7C4 | `Main_RunHirata` | An empty function in the retail build — the compiled-out developer mode slot, named for the Konami programmer whose source path (`src\hirata\`) survives in the binary's debug strings. |
| 0x8002DA1C | `Main_RunCredits` | Mode tick for the credits roll (loop-family pattern; body not yet studied in detail). |

## Batch 4 (functions)

(0x80056504 `Model_LoadMonsterMerge` skipped: hedged name, not 100% — stays
on the suspects side until proven.)

| address | name | description |
|---|---|---|
| 0x8002DD74 | `Main_Loop` | The game's infinite task-dispatch loop; never returns. Each pass runs the four update funcs, then dispatches the current mode through the `D_80090B64[]` function-pointer table indexed by the low 5 bits of the mode flag byte — that table is where every `*Loop` lives. |
| 0x80032B60 | `BuildDeck_CompareCard` | Card comparator for sorting deck/trunk lists: compares a primary key first, then breaks ties through a secondary-key translation table (`gCard_asNameSortKey`, indexed by secondary−1) — alphabetical order. |
| 0x80046768 | `SD_InitState` | Sound-state bootstrap: initializes the sound-state struct at `g_SDValue`, its work buffers, callbacks and defaults. The sound file names `sd_bgm.dat`, `sd_se.dat` and `master.xa` are its own strings, but the disc reads themselves happen in the request path, not in this body (memories-decomp analysis; formerly `SD_LoadData`). |
| 0x80048658 | `SD_SEPlay` | Plays sound effect `id` at volume `vol` (0xFF = full). Live-proven: every menu blip is a call here; the chain bottoms out in PsyQ `_spu_note2pitch` / `SpuGetVoiceEnvelope`. Body itself not yet matched. |
| 0x800492D8 | `SD_Init` | Sound-system init: runs two internal setup calls, then disables SPU reverb via the BIOS (`SpuSetReverbModeType`). |
| 0x80049694 | `SD_Term` | Sound-system shutdown: two internal cleanup calls, then the BIOS `SpuQuit()`. Mirror of `SD_Init`. |
| 0x8005C4F0 | `File_Exists` | Thin wrapper around PsyQ `DsSearchFile` with the two args swapped, mapping the library result to a simple status (−1 = not found or error). |
| 0x8007058C | `AiScript_ReadByte` | AI interpreter fetch: reads the next byte operand from the opponent's AI script stream and advances the VM's read position. |
| 0x800705AC | `AiScript_ReadShort` | Same fetch, but a 16-bit operand — used by the jump/compare instructions that carry immediate values. |
| 0x800705D8 | `AiScript_Init` | Resets the AI VM's state block (`gAiScript_State`) before a decision pass; defined alongside the duel-buffer init. |

## Batch 5 (functions — AI interpreter core, 20 rows)

| address | name | description |
|---|---|---|
| 0x80070650 | `AiScript_Run` | The AI VM's interpreter loop: fetches the next opcode from the opponent's script and dispatches it through `gAiScript_apfnCommand[opcode]` (jalr), until an end instruction stops the pass. |
| 0x80070710 | `Ai_GetHandSize` | Returns the opponent's hand-size parameter: byte 0 of their 9-byte `gDuel_aOpponentData` record, indexed by `gDuel_bOpponentID`. A configured value, not a live count. |
| 0x80070738 | `Ai_GetWinningCardRange` | Maps a range-category code to card-id bounds via a 10-entry jump table — the "winning cards" flavor of the range pair. |
| 0x800707C4 | `Ai_GetCardRange` | Direct sibling of `Ai_GetWinningCardRange` with the same jump-table shape: category code in, card-id range out. |
| 0x80070870 | `Ai_IsCardInSet` | Membership test: scans the script's current CARD set (12-byte records, leading id field) for a given card id. |
| 0x800708C4 | `Ai_IsTypeInSet` | Same membership test against the script's TYPE set. |
| 0x80070920 | `Ai_IsCardInSets` | Combined membership check across both the card set and the type set. |
| 0x80070988 | `AiScript_Jump` | Opcode: unconditional jump — reads a target from the stream and seeks the VM's read position there. |
| 0x800709C0 | `AiScript_JumpGreaterEqual` | Opcode: conditional jump, taken when the compared value is >= the script operand. |
| 0x80070A40 | `AiScript_JumpGreater` | Opcode: conditional jump, taken on strictly-greater comparison. |
| 0x80070AC0 | `AiScript_JumpEqual` | Opcode: conditional jump on equality. |
| 0x80070B3C | `AiScript_JumpNotEqual` | Opcode: conditional jump on inequality. |
| 0x80070BB8 | `AiScript_JumpBetween` | Opcode: range jump — reads a [lo, hi] pair and jumps when the tested `gAiScript_aMemory` value falls inside it. |
| 0x80070C60 | `AiScript_JumpRandom` | Opcode: probabilistic jump — rolls `rand() % 100` against a script threshold. |
| 0x80070D00 | `AiScript_Call` | Opcode: subroutine call — pushes the return position onto the VM stack (asserting via the "error sub routine" path on overflow) and jumps. |
| 0x80070DA8 | `AiScript_Return` | Opcode: return — pops the VM stack back to the pushed position (asserting on underflow). |
| 0x80070E20 | `AiScript_SetRandom` | Opcode: reads a [lo, hi] bound pair from the stream and stores a random value in that range into an `gAiScript_aMemory` slot. |
| 0x80070EB4 | `AiScript_Subtract` | Opcode: subtracts one `gAiScript_aMemory` entry from another and stores the difference into a third slot. |
| 0x80070F1C | `AiScript_PlayFieldCard` | Opcode: emits the chosen FIELD play — pulls four script-selected `gAiScript_aMemory` values and stamps them into the play-command record at `D_800EAE88` (slot, card, position, category). |
| 0x80070FF8 | `AiScript_EndHand` | Opcode: end-of-hand-script marker — an empty function; the dispatch loop reacts to the opcode itself. (First matched by MaChInEgUn3's independent decomp.) |

## Batch 6 (functions — AI opcodes, board queries & pickers)

(0x80071008 AiScript_CalcCardPower, 0x80071B64 AiScript_FindKiller, 0x80071CB0
AiScript_FindDefenseStopper excluded: bodies unmatched, behavior not 100% verified —
tracked in suspects.md.)

| address | name | description |
|---|---|---|
| 0x80071000 | `AiScript_EndField` | Opcode: end-of-field-script marker — empty function, the loop reacts to the opcode itself. (First matched by MaChInEgUn3's independent decomp.) |
| 0x80071194 | `AiScript_TestHighStat` | Opcode: reads a card id from `gAiScript_aMemory`, checks its 5-bit type field in `gDuel_adwCardStats` (bits 26-30); for monster types (< 20) evaluates `Duel_GetBaseCardStat` and stores the resulting stat. |
| 0x80071248 | `AiScript_LoadCardType` | Opcode: copies a card's 5-bit type field (from its packed `gDuel_adwCardStats` word) into an `gAiScript_aMemory` slot. |
| 0x800712B4 | `AiScript_LoadCardID` | Opcode: copies the card id (leading field) of an `gDuel_aActiveCards` entry into an `gAiScript_aMemory` slot. |
| 0x80071320 | `AiScript_SetFaceDown` | Opcode: tests an `gDuel_aActiveCards` entry's face-down flag and stores the boolean into an `gAiScript_aMemory` slot. |
| 0x800713B8 | `AiScript_LoadLifePoint` | Opcode: copies the s16 life-points field (offset 0x14 of the 32-byte duel-side record) into an `gAiScript_aMemory` slot. |
| 0x80071424 | `AiScript_LoadOpponentID` | Opcode: stores `gDuel_bOpponentID` into `gAiScript_aMemory[idx]` — lets scripts branch per opponent. |
| 0x80071460 | `AiScript_LoadTerrain` | Opcode: stores the current `gDuel_bTerrain` byte into `gAiScript_aMemory[idx]` — identical shape to `AiScript_LoadOpponentID`, different global. |
| 0x8007149C | `AiScript_LoadDeckSize` | Opcode: counts the occupied entries (nonzero leading field) across a record range — the AI's remaining-cards count — into a VM slot. |
| 0x8007154C | `AiScript_TestPinned` | Opcode: tests the flag byte at offset 0x19 of the duel-side record (the "pinned" state) and stores the result. |
| 0x800715C4 | `AiScript_StartCombo` | Opcode: scans the 5-entry combo window in the VM state block (offset 0x38, shared with `AiScript_PushComboCard`) and writes the found entry — or a default — into `gAiScript_aMemory`. |
| 0x8007164C | `AiScript_LoadBestDifference` | Opcode: copies the precomputed best-combo score from `gAi_wBestDifference` into a `gAiScript_aMemory` slot. |
| 0x80071688 | `AiScript_LoadBestAttacker` | Opcode: copies the precomputed best-attacker slot from `gAi_bBestAttacker` into a `gAiScript_aMemory` slot. |
| 0x800716C4 | `AiScript_LoadBestTarget` | Opcode: copies the precomputed best-target slot from `gAi_bBestTarget` into a `gAiScript_aMemory` slot — the trio a combo search leaves behind. |
| 0x80071700 | `AiScript_FindStrongest` | Opcode: the big max-scanner (137 insns): takes 5 operands, gets a card-id range from `Ai_GetWinningCardRange`, scans `gDuel_aActiveCards[lo..hi]` with flag-gated skips, and writes the strongest entry's slot to the output. |
| 0x80071924 | `AiScript_FindWeakest` | Opcode: sibling of `AiScript_FindStrongest` scanning for the weakest qualifying entry. |
| 0x80071EB8 | `AiScript_CountCards` | Opcode: counts cards matching the scripted criteria over a scanned range into a VM slot. |

## Batch 7 (functions — AI searchers, fusion solver, play emitters, 20 rows)

(0x80072A48 `AiScript_FindBestCombo` excluded: body unmatched, not 100% — suspects.)

| address | name | description |
|---|---|---|
| 0x80071FC8 | `AiScript_FindFirstCard` | Opcode: scans `gDuel_aActiveCards` over an `Ai_GetCardRange` window and stores the first slot matching the scripted target/type criteria (two `gAiScript_aMemory` operands). |
| 0x8007214C | `AiScript_FindCard` | Opcode: same scan shape, but matches on card id — first entry whose id equals the looked-up value, with flag-based skips. |
| 0x800722CC | `AiScript_MatchType` | Opcode: scans for a slot owned by a looked-up value, excluding entries via type-gated flag bits (0x4000/0x1000) and set membership (`Ai_IsCardInSets`). |
| 0x8007249C | `AiScript_FindEquipTarget` | Opcode: finds a field slot whose monster the scripted equip card can attach to (the equip-compatibility lookup), storing the slot. |
| 0x800725C4 | `AiScript_CheckRitual` | Opcode: runs the ritual recipe lookup for the scripted card and stores the boolean result in a VM slot. |
| 0x80072640 | `AiScript_FindFirstMonster` | Opcode: scans the field records for the first qualifying monster and stores its slot. |
| 0x800726F4 | `AiScript_FindFirstType` | Opcode: sibling scan — first entry of the scripted type. |
| 0x800727C0 | `Ai_CompleteFusion` | The AI's fusion-chain solver: recursive combo search over the `gAiScript_State` rows, resolving each key through `gDuel_aActiveCards`, skipping used/rejected rows, testing fusion/equip validity at each depth. |
| 0x80072DC0 | `AiScript_EvaluateFusion` | Opcode: preps and scores a fusion attempt — fills the combo-search header at `gAiScript_State+0x9C` from four operands plus `Ai_GetHandSize`, evaluates `Duel_GetBaseCardStat` both ways (ATK and DEF), and keeps the higher. |
| 0x80072F1C | `AiScript_SkipHand` | Opcode: advances the script cursor 4 bytes, discarding them — the hand-script skip. |
| 0x80072F54 | `AiScript_SkipField` | Opcode: identical 4-byte skip for field scripts. |
| 0x80072F8C | `AiScript_PlayFaceUp` | Opcode: emits a face-up play — fills the play-command record `D_800EAE88[0..4]` with five `gAiScript_aMemory`-selected values. |
| 0x80073050 | `AiScript_SetPosition` | Opcode: low-byte copy between VM slots — how scripts set a play's position (attack/defense) value. |
| 0x8007308C | `AiScript_FindBestAttack` | Opcode: the best attack-matchup search. Pits each own monster (slots 1-5) against each opposing slot, scoring ATK difference + `Duel_CalcGuardianStarMatchup`, tracking the best score/attacker/target into `gAi_wBestDifference`, `gAi_bBestAttacker` and `gAi_bBestTarget`. |
| 0x80073220 | `AiScript_PushComboCard` | Opcode: clears the play-command record, then collects the found combo — scans the `gAiScript_State+0x38` window (bounded by `gAiScript_State[0xA3]`) and pushes values >= 11 into `D_800EAE88[]`. "C" = with clear. |
| 0x800732A0 | `AiScript_PushComboEmpty` | Opcode: the same combo collect WITHOUT the leading clear ("Nc" = no clear). |
| 0x80073300 | `AiScript_HandNop` | Opcode: hand-script no-op — empty function. |
| 0x80073308 | `AiScript_FieldNop` | Opcode: field-script no-op — empty function. |
| 0x80073310 | `AiScript_AddCard` | Opcode: inserts a card id into the script's card set (scan-or-insert over the set window). |
| 0x80073380 | `AiScript_ClearCards` | Opcode: clears the script's card set (the set table's writable region). |

## Batch 8 (functions — final: last AI opcodes, sound setters, SDK identifications)

| address | name | description |
|---|---|---|
| 0x800733A8 | `AiScript_AddType` | Opcode: inserts a type id into the script's TYPE set (scan-or-insert over the second set window). |
| 0x80073420 | `AiScript_ClearTypes` | Opcode: clears the script's type set. |
| 0x80073464 | `AiScript_SetUnknownFlag` | Opcode: sets the flag byte at `0x800F5C7F` (inside `gAiScript_State`'s tail). Mechanism certain; what the flag gates is still unknown — hence the inherited "Unk". |
| 0x80073474 | `AiScript_ClearUnknownFlag` | Opcode: clears that same flag byte. |
| 0x80073480 | `AiScript_MoveCard` | Opcode: sets the scripted flag bit on an `gDuel_aActiveCards` entry — per the IDB reading, marking the card as moved/used this turn. |
| 0x800734DC | `AiScript_LoadOpponentData` | Opcode: writes a scaled `gDuel_aOpponentData` lookup into a VM slot — resolves (opponent*9 + field) with an optional offset from a second slot. |
| 0x800735A0 | `AiScript_Store` | Opcode: stores a 16-bit immediate from the script stream into `gAiScript_aMemory[idx]`. |
| 0x800735DC | `AiScript_SetRegister` | Opcode: copies one `gAiScript_aMemory` entry to another (two stream-read indices). |
| 0x80073624 | `AiScript_Add` | Opcode: adds one `gAiScript_aMemory` entry to another and stores the sum in a third slot. |
| 0x800736C4 | `AiScript_Print` | Opcode: debug print — emits two strings (the "check_point" machinery) via the console `printf` path. |
| 0x80046FA0 | `SD_SetOutputType` | Sets the sound output mode (0=stereo, 1=mono): writes the driver state's mode byte, toggles its flag, and re-derives the CD-audio mix. Live-proven on the OPTION screen toggle. |
| 0x8003FEE0 | `SD_SEPlayFull` | Convenience wrapper: plays sound effect `id` at full volume — always `SD_SEPlay(id, 0xFF)`. Live-proven with ids 8 (cancel) and 0x2F (option toggle). |
| 0x80073758 | `PCread` | Chunked file-read wrapper over `_SN_read` — the LIBSN PC-host debugging filesystem (dev-kit leftover; inert on retail hardware). |
| 0x800740F0 | `FlushCache` | BIOS syscall trampoline: flushes the instruction cache (standard PsyQ kernel call). |
| 0x80074380 | `ChangeClearRCnt` | BIOS syscall trampoline: root-counter (timer) interrupt-clear mode control. |
| 0x80074A70 | `ReturnFromException` | BIOS syscall trampoline (B0h): return from exception. |
| 0x80074A80 | `ResetEntryInt` | BIOS syscall trampoline (B0h): restore the default exception exit. |
| 0x80074A90 | `HookEntryInt` | BIOS syscall trampoline (B0h): install a custom exception exit. |
| 0x80075B20 | `DeliverEvent` | BIOS syscall trampoline: fire a kernel event (used inside the SPU library's IRQ path). |
| 0x80075BE0 | `SpuSetReverb` | PsyQ libspu: enables/disables SPU reverb processing. |
| 0x80076D10 | `WaitEvent` | BIOS syscall trampoline: block until a kernel event fires. |
| 0x8007E350 | `PadChkVsync` | PsyQ libpad: the per-vsync pad servicing hook. |
| 0x8007E600 | `CdIntToPos_8007E600` | Second linked copy of PsyQ `CdIntToPos`: converts a sector number to a CD MSF position, returning its pointer argument (libsyms owns the bare name at 0x8007A710). |
| 0x8007E710 | `CdPosToInt_8007E710` | Second linked copy of PsyQ `CdPosToInt`: MSF position back to a sector number. |
| 0x8007E8D0 | `SetDumpFnt` | PsyQ debug-font library: selects the font stream used by `FntPrint` dumps. |
| 0x8007E9B0 | `FntOpen` | PsyQ debug font: opens an on-screen text window at the given position/size. |
| 0x8007EC68 | `FntFlush` | PsyQ debug font: draws the accumulated text into the frame. |
| 0x8007EF84 | `FntPrint` | PsyQ debug font: printf into the debug text window (the game's on-screen debug output). |
| 0x800857C0 | `GsSetProjection` | PsyQ libgs: sets the 3D projection distance. |
| 0x8008B4E0 | `InitCARD2` | PsyQ memory-card BIOS entry: initializes the card subsystem (async variant). |
| 0x8008E680 | `strcmp` | The C library string compare (LIBC A56 object, signature-identified). |
| 0x8008FBD0 | `exit` | The C library process-exit stub (matching libsyms' own identification at this address). |

## Addendum (post-bulk-apply)

| address | name | description |
|---|---|---|
| 0x8002D684 | `Main_RunPasswordMenu` | Mode tick for the password screen — live-confirmed ambient (mode slot 10), the first CONFLICT row adjudicated by tracing. Standard loop-family shape; the entry grid itself runs in the screen's disc-loaded module. |

## Batch: free-duel module pipeline (2026-09-02, live-traced, Linux seat)

| address | name | description |
|---|---|---|
| 0x80168FB4 | `FreeDuel_Entry` | The fixed-address entry `Main_RunFreeDuelMenu` calls into the disc-loaded free-duel module: seeds `rand()` then runs `FreeDuel_UpdateScreen`. |
| 0x80168C7C | `FreeDuel_UpdateScreen` | Per-frame screen logic. While a dialog is up (flag 0x20) it only waits for dismissal; otherwise runs the cursor tween and scrollbar, then reads `gInput_wPad1Held` to move the pending cell (clamped 0..4 / 0..7) and `gInput_wPad1Pressed` for cancel (SE 8) and confirm (Build Deck tile, deck-incomplete refusal, or duel start with opponent index row*5+col). |
| 0x80168A9C | `FreeDuel_UpdateCursorTween` | When the pending cell differs from the committed one, sets an 8-frame counter and per-frame velocities so the cursor widget glides to (col*56+20, row*52+40); on landing copies the pending cell into `gFreeDuel_bCursorColumn/Row`, re-places the cursor and plays SE 0x2F. |
| 0x80168090 | `FreeDuel_PlaceCursor` | Puts the cursor widget at the committed cell's pixel position. With the commit flag set, also refreshes the selected duelist's win/loss readout from `duelistRecords` and the name-string id, guarded by `gFreeDuel_abGridAvailable`. |
| 0x80168004 | `FreeDuel_UpdateScrollbar` | Every frame: nudges `gGraphics_sViewportY` so the cursor stays within 40..144 px of the viewport, then sets the scrollbar thumb's y to 7 + (cursor_y - 40) * 72 / 364, i.e. 72 px of travel over the 7-row grid. Follows the tweened cursor, so the thumb glides. |
| 0x8009B36C | `gFreeDuel_bTargetColumn` | Pending grid column written by the input handler; becomes `gFreeDuel_bCursorColumn` when the tween lands. |
| 0x8009B36D | `gFreeDuel_bTargetRow` | Pending grid row, same lifecycle. |
| 0x80169030 | `gFreeDuel_abGridAvailable` | Module data: 8 rows x 5 columns of bytes, nonzero where a grid cell can be selected (Build Deck at (0,0), duelists per unlock bits). |
| 0x801690A4 | `gFreeDuel_bScreenFlags` | Module data byte: 0x40 while the cursor tween is in flight (input ignored), 0x20 while the refusal dialog is open. |
| 0x80169058 | `gFreeDuel_pThumbWidget` | Module data: pointer to the scrollbar thumb widget instance (0x800F07E8 in the widget pool). |
| 0x801690A0 | `gFreeDuel_pCursorWidget` | Module data: pointer to the grid cursor widget instance (0x800F0858). |

## Batch: screen fade system (2026-09-02, live-traced, Linux seat)

| address | name | description |
|---|---|---|
| 0x800E9EC8 | `gFade_State` | The screen-fade state block (0x28 bytes): u32 colour, u8 current level, u8 target level, u8 flags (0x80 fade in flight, 0x01 strip-wipe mode, 0x10/0x20 colour latch), u8 step per frame, s16 head, u8 strips[30]. |
| 0x800151D8 | `Fade_StepBands` | Per-frame strip walker: from the head value, 15 strips each 8 apart, clamped to [current, target], mirrored into strips[i] and strips[29-i]; then moves the head by step * D_8009B0D8. |
| 0x80015310 | `Fade_Update` | Per-frame fade update: runs the strip walker (strip mode) or a flat level ramp, and when current reaches target clears the in-flight bit, latches the working RGB from the target RGB, and sets or clears the overlay-on flag. |
| 0x800158B8 | `Fade_InitOut` | Arms a fade to black: head 255, target 0, in-flight flag set, all 30 strips filled with the current level, step 12. |
| 0x80015904 | `Fade_StartOut` | `Fade_InitOut` plus step 8 and strip mode -- the menu-transition fade. |
| 0x80015B00 | `Fade_WaitOut` | Starts the strip fade-out and then pumps the four per-frame update functions until the in-flight bit drops. Blocking; ~48 frames. |
| 0x8009B141 | `gFade_bOverlayOn` | Set when a fade starts or the screen is black, cleared when a fade-in completes; the overlay drawer keeps painting black while it is set even with no fade in flight. |

## Batch: text-box / dialog machine (2026-09-02, live-traced on name entry, Linux seat)

| address | name | description |
|---|---|---|
| 0x800EB0F8 | `gTextBox_aRecords` | Array of text-box records (0x64 bytes, index = box slot): current text pointer, glyph-entry list, box/highlight/shadow widget pointers, flags, string id, rect, glyph cell size, and the record's glyph-sprite slot base/count. |
| 0x80090E58 | `gTextBox_awSpriteSlotRange` | Cumulative glyph-sprite slot boundaries per text-box slot: 0, 255, 415, 575, 620. Record i owns slots [tbl[i], tbl[i+1]). |
| 0x801D9000 | `gText_adwGlyphCodeTable` | One u32 per font glyph; the low half is the Shift-JIS code the glyph draws. Scanned by `Text_SjisToGlyphCodes`. |
| 0x801B0000 | `gText_aBank` | The current screen's string bank: 0xFF-terminated glyph strings with control bytes (00 space, F8 position/colour, FE newline). Loaded with the screen's module. |
| 0x801C0000 | `gText_aBankOffsets` | u16 byte offset into `gText_aBank` per string id. |
| 0x8003B744 | `Text_LookupString` | Returns the bank pointer for a string id: base + offsets[id]; ids >= 0xD000 index the table relative to 0xD000. |
| 0x80035BE4 | `TextBox_Create` | Opens a text box: sets its rect then initialises the record with the string id. |
| 0x80035C38 | `TextBox_CreateFlagged` | `TextBox_Create`, then ORs extra flags into the record's flag word. |
| 0x80035AB8 | `TextBox_SetRect` | Stores the box position and size into the record. |
| 0x80035AF0 | `TextBox_InitRecord` | Initialises a record: string id, default 8x12 glyph cell, sprite slot base/count from `gTextBox_awSpriteSlotRange`, counters cleared. |
| 0x80035B7C | `TextBox_Destroy` | Closes a box: releases its glyph list and frees the box, highlight and shadow widgets. |
| 0x80039934 | `TextBox_SetPos` | Moves a box and its three widgets to a new position. |
| 0x800393B0 | `TextBox_BuildStep` | The typewriter: on the first call resolves the string and allocates the widgets; every call emits one glyph sprite; sets the done bit at the terminator. |
| 0x80043230 | `Widget_SlideSine` | Slides a widget between a stored origin and a target along a sine of the given step; dialogs use it to enter and leave. |
| 0x8003BC40 | `Text_SjisToGlyphCodes` | Converts a Shift-JIS u16 string into glyph codes (0xF0xx for high indices), 0xFF-terminated, so dynamic text can be typed by the machine. |

## Batch: dialog choice machine (2026-09-02, live-traced on the name confirmation, Linux seat)

| address | name | description |
|---|---|---|
| 0x8009B34D | `gDialog_bChoice` | Index of the selected choice line in the open dialog (0 = first). Updated on every DPAD move, finalised from `gDialog_bInputState` on confirm; screens read it after the box reports done. |
| 0x8009B345 | `gDialog_bChoiceCount` | Number of selectable lines in the open choice box; the upper bound for DOWN. |
| 0x8009B336 | `gDialog_bChoiceEnabled` | Bitmask, one bit per line, of choices that may be picked; drives the cursor's highlight colour. |
| 0x8009B327 | `gDialog_bInputState` | The choice box's input result byte: bit 0x40 confirmed with the chosen line in the low 3 bits, bit 0x80 cancelled. |
| 0x800374F4 | `Dialog_OpenChoice` | Creates the choice cursor object for a text box at its bottom-right corner (sprite 0x20C) and returns it; the box stores it at +0x30. |
| 0x800371A8 | `Dialog_UpdateChoice` | Per-frame tick of the cursor object: spawns its sprite on first call, then handles confirm and cancel from `gDialog_bInputState` or passes the DPAD to `Dialog_ReadChoiceInput`. |
| 0x8003700C | `Dialog_ReadChoiceInput` | Moves the selection down or up within the count (or cycles with the repeat bit), plays SE 6 and re-highlights; returns 0 when nothing relevant was pressed. |
| 0x80036F80 | `Dialog_HighlightChoice` | Sets the cursor's colour fields to 0xC0 or 0xC0C0 depending on whether the selected line is enabled, then refreshes the object. |
| 0x80037110 | `Widget_UpdatePulseColour` | Draw callback that folds the global frame counter into a triangle wave and writes it across an object's colour and shadow lanes -- the blinking cursor. |

## Batch: story script engine (2026-09-02, live-traced in the campaign intro, Linux seat)

| address | name | description |
|---|---|---|
| 0x8002CE64 | `Main_RunCampaign` | Mode tick for the campaign (mode byte 0xC2): runs the story script and its text boxes each frame. Live-confirmed as the ambient loop of the intro cutscene. |
| 0x8002FA54 | `Script_RunTick` | Bytecode driver: if no script is running, resolves the selected script through the self-relative offset table and latches its stream; otherwise executes the current opcode through `gScript_apfnCommand`, gated by `gScript_wState`. |
| 0x8002E730 | `Script_OpShowImage` | Script opcode 5: shows the cutscene picture -- copies the staged image into the display area, or when nothing is busy releases the previous one, starts a fade and requests the next picture from disc. |
| 0x8002EC74 | `Script_OpSound` | Reads an encoded sound command from `gScript_pStream`, plays an immediate sound effect or updates/replays the current sound command, and clears `gScript_wCommand` when the operation is complete. |
| 0x80090C50 | `gScript_apfnCommand` | Table of the 24 script opcode handlers (0x8002E3DC .. 0x8002FED0), indexed by `gScript_wCommand`. |
| 0x8009B27C | `gScript_wCommand` | The opcode currently being serviced; bit 0x8000 marks it busy across frames. |
| 0x8009B290 | `gScript_pStream` | Byte-stream cursor of the running script. |
| 0x8009B2A4 | `gScript_wState` | Script engine state: 0x8000 a script is running, 0x4000 waiting for a text box to finish, low bits select the script. |

## Batch: campaign overworld (2026-09-02, live-traced, Linux seat)

| address | name | description |
|---|---|---|
| 0x8002CCA8 | `Campaign_TestStoryFlag` | Tests one bit of `gCampaign_abStoryFlags`: byte (id & 0x7FF) >> 3, bit 0x80 >> (id & 7); if bit 0x8000 of the id is set the result is inverted. |
| 0x801D0618 | `gCampaign_abStoryFlags` | The save block's story-progress bit array, MSB-first, read by `Campaign_TestStoryFlag`. |
| 0x801D5800 | `gText_aGlobalOffsets` | u16 offsets, base 0x801D0000, for string ids 0x8000 and up: the always-resident bank holding location and card names. |
