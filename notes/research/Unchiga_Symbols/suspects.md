# Suspected / WIP identifications — NOT yet applied

> **External reference — mirror of Unchiga/ygofm-decomp.** This file is copied verbatim from that repository's `notes/research/` (the matching decomp the research was done in). Paths, tools and rules it names (`config/symbol_addrs.txt`, `tools/gen_symbols.py`, `tools/setup.sh`, `config/modules/`, `tools/gen_research_notes.py`, ...) are that project's, not this one's. The addresses and evidence are the claim; the names follow this repo's `notes/naming-conventions.md` (`Subsystem_VerbObject` / `gSubsystem_Role`) since 2026-09-03 and are ready for `config/slus_01411/symbols.txt` and `notes/semantic-symbol-map.csv` after the usual per-address review.

Working ledger for names we suspect but have not proven, and for observed
roles that need one more session before naming. Promotion path: gather the
missing evidence -> move the row to findings.md as solid -> apply in the next
BULK rename pass (operator policy 2026-08-31: no per-name rebuilds; batch
the applies and byte-verify once).

## From the OPTION round-trip (2026-08-31)

| address | current name | observed | suspicion |
|---|---|---|---|
| `0x8003FEE0` | `call_80048658_255_0` | Body called SD_SEPlay(8, 0xFF) when Circle cancelled out of OPTION (ra 0x8003FEF8 inside it). The fleet name's "255_0" does not match observed args. | SE-play convenience wrapper, maybe `SD_SEPlayDefault(id)`; the cancel sound goes through it. Re-read the matched body before renaming. |
| `0x80013C28` | `func_80013C28` | Fired on X->OPTION build AND on password card-art reveal — both disc-load moments (with the 0x800137E4/13940/13998/1455C streaming family). | CD streaming/read-request path member, NOT a fade. |
| `0x80015xxx` family | `reset_obj_*` (fleet) | CONFIRMED REPEATABLE: identical 20-function exit path on both Circle-exits (teardown family + cancel blip + flag setters 0x80047430/0x80049108/0x8004A2F8/0x80084240). Also proven: exiting OPTION commits nothing to the 0x801D0xxx save region. | Menu-screen teardown path; names stay fleet-mechanical until per-function roles emerge. |
| `0x8004002C`-`0x800404CC` + `0x80041428` | various `func_`/`myst` | Fired only during the X->OPTION build-up (with the widget setter 0x80040410 triple). | Widget/object constructor-initializer family for building a menu screen. |
| `0x80040588`, `0x80040804` (triple), `0x80040D14`, `0x80042210`, `0x8004A40C/430/7C0`, `0x8004B374/70C`, `0x80035E20`, `0x8003C7A0/8CC` | various | Ambient ONLY while sitting in OPTION (with `Main_RunOptionsMenu` + GPU prim setters SetSemiTrans/SetPolyGT4/GsSortFlipSprite). | Options-screen widget update/draw helpers. |
| `0x80041C8C` | `func_80041C8C` (19/53 stub) | Press-only on main menu, but AMBIENT in OPTION. | Widget visual refresher, called whenever widgets animate; OPTION animates constantly. |

## From the STEREO/MONO toggle (2026-08-31)

| address | current name | observed | suspicion |
|---|---|---|---|
| `0x8009B37D` (byte) | `D_8009B37D` | Flips with the setting; func_8003C628 writes it FROM gSD_bOutputType(0x8009B408); func_8003C568 uses it as an options-label table index. | Derived working copy: `g_SoundModeCur`-ish. Name after gSD_bOutputType lands. |
| `0x80044DC0` | `func_80044DC0` | Called by SD_SetOutputType with the master volume field; matched body builds the 4-byte CdMix packet, channel slot picked by the mode byte. | `SD_UpdateCdMix`-ish CD-audio re-mixer. |
| `0x8007CDC0` -> `0x8007A048` | (mid-entries) | CD volume apply path beneath the CdMix packet (0x8007A048 sits in CD_vol's extent; entries are mid-function dispatch quirks). | Library-side CD volume application; verify extents before naming anything. |
| `0x8003C568` | `func_8003C568` | Runs ~1 frame after the toggle; body picks a label from a local table by D_8009B37D. | Options-screen label refresh (draws STEREO/MONO highlight). |
| `0x8003C628` | `func_8003C628` | Body syncs D_8009B37C/D from gSD_bOutputType with sign-bit reset semantics. | Options-state sync/init. |
| `0x80047F38(1, 0x11, 0xFF)` | `func_80047F38` | Fired inside the SE-play path on every blip. | SE voice setup/allocation. |
| `0x8009B37C` (byte) | `D_8009B37C` | 1 while the OPTION screen is alive, drops to 0 exactly on Circle-exit (stable-byte diff); func_8003C628 touches it beside the derived mode copy. | Options-screen-active/session flag. |
| `D_8009B45C->f48` | (struct field) | The sound-state struct's output-mode byte (written by SD_SetOutputType, read by the CdMix builder). | Document as `outputType` when the struct gets a header. |

## From the TRADE card flow (2026-08-31)

| address | current name | observed | suspicion |
|---|---|---|---|
| `0x8003F8D4/F9A4/FCD8` + `0x8004A27C/0x8004B49C` | various | Ambient only while the memory-card caution dialog was up (with SetSemiTrans/SetPolyGT4). | Modal-dialog draw/update helpers. |
| `0x8003E46C-0x8003FAxx` + `0x80035AB8-0x80039E9C` cluster (~40 fns) | various (`set_flag_bits_mode`, pins, `func_`) | Fired only during BEGIN LOAD -> slot1 read -> slot2 NO DATA (full set in tmp/trade_session.json "load"). No PsyQ `_card_*`/`InitCARD` wrappers appeared -- card I/O reaches BIOS HLE without them, so the game-side card session logic is THIS cluster. | Memory-card load-session state machine + NO DATA dialog path. Rich naming ground for a dedicated session. |
| `0x80094xxx` (16 bytes), `0x800FExxx` (28 bytes) | -- | Stable bytes changed across the card flow. | Card-session status/state candidates; probe with targeted reads next card session. |

## Description-gate disputes (from batch 2, 2026-08-31)

| address | current name | dispute |
|---|---|---|
| `0x8002CEE8` | `Main_RunDuel` | Matched body reads as disc-swap/lid-open state machine (D_8009B26C bit 0x40 gate), not a duel mode tick. Adjudicate by live trace: enter a duel and check ambience. |
| `0x800218F0` | `Duel_MaybeEndOfDuel` | Matched body reads as per-frame duel-round setup/dispatch, not end-of-duel. Same adjudication: live duel trace. |
| `calc_wrap_scroll_delta` (unit) | -- | Defines `Duel_CalcGuardianStarMatchup`; its "scroll helper" reading is wrong per func_8001EE44.c's note (6/4 wraps = the guardian-star wheels). Unit rename candidate for the bulk pass. |

## Unverified-body AI opcodes (excluded from descriptions until proven)

| address | name | suspected role (IDB + family context, NOT verified) |
|---|---|---|
| `0x80071008` | `AiScript_CalcCardPower` | Evaluates a card's battle power into a VM slot. |
| `0x80071B64` | `AiScript_FindKiller` | Finds a monster that beats a given target. |
| `0x80071CB0` | `AiScript_FindDefenseStopper` | Finds a defender able to stop an attacker. |
| `0x80072A48` | `AiScript_FindBestCombo` | Runs the full best-combo search (results feed the BestDiff trio's neighborhood). NOTE: AiScript_FindKiller may actually have a landed body — unit `equip_score_best_slot.c` per sibling comments; verify and graduate. |

## From the SAVE flow (2026-08-31)

| address | current name | observed | suspicion |
|---|---|---|---|
| `0x801DC383+` | -- | Source region of the memory-card write stream (card tracker: kernel store_pc, frame-by-frame). | The assembled save-file image / card staging buffer (`g_SaveStaging`-ish). Confirm extent by diffing a save after changing the deck. |
| `0x801D3xxx` | -- | 78 bytes changed only during SAVE. | Save metadata / checksum area. |

## From the password deep dive (2026-08-31)

| address | current name | observed | suspicion |
|---|---|---|---|
| `0x801D0698` area | -- | Byte set 0->0x20 on card-2 purchase; re-use then rejected. | Used-password bitfield (base/stride unknown; map by buying 2-3 more cards and watching bits). |
| `0x80042210/20` | `func_80042210` | Builds the digit-cursor sprite in scratchpad 0x1F800320, draws via GsSortFlipSprite; password-ambient. | Password cursor-sprite drawer; likely generic "draw flip-sprite widget" used by other screens too. |
| cost-10 table rows | -- | Their BCD passwords are rejected with virgin used-flags (e.g. card 24 "32271410"). | Non-passwordable card entries with placeholder rows, or a second validation table. |
| `0x8009B510/518` | -- | Digit-cursor X pair (+24/cell). | Cursor-sprite coordinate record (name with the widget struct later). |

## From Build Deck sorting (2026-08-31)

| address | current name | observed | suspicion |
|---|---|---|---|
| `0x80032BD4` | `compare_rec_two_level_std` | RESOLVED: it is the fleet-matched std-convention two-level comparator; used for several sort modes, BuildDeck_CompareCard for the rest. No rename needed. |
| `0x80100004` | (in `playerMonModel` buffer) | 722 x 16-byte trunk-view records, qsort target for all modes; per-mode cached orders recopied on mode change. | Trunk view/sort arena — document as scratch reuse of the model buffer. |

## From the deck pane (2026-08-31)

| address | current name | observed | suspicion |
|---|---|---|---|
| `0x8009B0AC` | `gGraphics_bActiveBuffer` | Alternates 0/1 every frame in a stationary Library screen; `func_80012E5C` stores `GsGetActiveBuff()` here. | RESOLVED: active graphics double-buffer index, not a pane selector. |

## SE id catalog (facts, kept here until the table earns a home)

Via SD_SEPlay(id, 0xFF): 6 = menu cursor move, 7 = confirm (X), 8 = cancel
(Circle), 0x2F = option toggle. Menu module calls it directly for 6 and 7 (ra 0x801809D0 /
0x80180A48); cancel goes through the 0x8003FEE0 wrapper.

## From the free-duel scrollbar session (2026-09-02)

| address | current name | observed | suspicion |
|---|---|---|---|
| `0x8009B146` (s16) | -- | Zeroed alongside the scroll offset 0x8009B148 at screen init (0x801682F8); never written during vertical moves. | Horizontal scroll offset twin, or a stale field. Check with LEFT/RIGHT past the viewport. |
| `0x8009B365` (byte) | -- | 0x80 set on confirm (duel start), 0x40 set on Build Deck; bit 0x80 tested at screen init to decide whether to credit a record. | Free-duel screen state/result flags. |
| `0x8009B3A4` (u16) | `D_8009B3A4` | Pad EDGE word: dpad bits in the high nibble (0x1000 UP, 0x2000 RIGHT, 0x4000 DOWN, 0x8000 LEFT) consumed by the module input handler. `0x8009B398` is the held/button word (0x20 cancel, 0xC0 confirm). | `g_PadPressed` / `g_PadHeld`-class names; confirm the bit layout on another screen before naming. |
| `0x800F07E8`, `0x800F0858` | widget pool slots | Thumb widget and cursor widget instances (pool D_800F0548, 0x70-byte records: +0x30 x, +0x32 y, +0x36/38 vel, +0x60 counter, +0x62 frac). | Name the pool struct first; these are instances. |
