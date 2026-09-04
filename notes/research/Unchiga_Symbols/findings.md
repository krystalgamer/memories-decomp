# Live-trace findings — names pending operator approval

> **External reference — mirror of Unchiga/ygofm-decomp.** This file is copied verbatim from that repository's `notes/research/` (the matching decomp the research was done in). Paths, tools and rules it names (`config/symbol_addrs.txt`, `tools/gen_symbols.py`, `tools/setup.sh`, `config/modules/`, `tools/gen_research_notes.py`, ...) are that project's, not this one's. The addresses and evidence are the claim; the names follow this repo's `notes/naming-conventions.md` (`Subsystem_VerbObject` / `gSubsystem_Role`) since 2026-09-03 and are ready for `config/slus_01411/symbols.txt` and `notes/semantic-symbol-map.csv` after the usual per-address review.

Every entry here was observed LIVE in the recomp (fn-entry trace + RAM
snapshots + injected input), with the evidence stated. Nothing is applied to
the build until its row says APPROVED and the name survives the byte-verify
pipeline. Status: PENDING / APPLIED / REJECTED / HOLD. Operator policy 2026-08-31:
apply solid names directly; operator corrects when needed.

## Session 2026-08-31 — main menu, up/down only

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F1 | `0x80184594` (1 byte) | Main-menu cursor. Read live: 0=NEW GAME, 1=LOAD, 2=2P DUEL, 3=TRADE, 4=OPTION; +1 per down, -1 per up, wraps both directions. Lives in the loaded menu module's data, so it may be the shared cursor of every module-driven menu (open question — re-check in other menus). Currently `menuSelection` (idb2018; RAM map "Menu ID"). | `gMain_bMenuID` | APPLIED |
| F2 | `func_80048658(id, vol)` | Plays a sound effect: called with (6, 0xFF) on every cursor move — SE #6 is the menu blip; chain reaches `_spu_note2pitch` / `SpuGetVoiceEnvelope`. Existing wrappers `call_80048658_255_0`/`call_80048658_flagged` fit "play SE id at volume". DotR vocabulary for this role: `SD_SEPlay*`. | `SD_SEPlay` | APPLIED |
| F3 | `func_800482B0(id, 0, vol)` | SE playback internal, called by the F2 chain (ra 0x80048758). | (hold — internals need one more session) | HOLD |
| F4 | `func_8004803C(id, 0, 0)` | SE playback internal, deeper in the same chain (ra 0x800484E8). | (hold) | HOLD |
| F5 | `func_80040410(obj, state)` | Widget/object state setter: writes `obj+0x69 = state`, clears bit 0x10 of flags at `obj+8` (matched C). Called twice per cursor move on the two highlight widgets (0x800F0698 / 0x800F0858) with swapped states. | (hold — want the widget pool named first) | HOLD |
| F6 | `func_80041C8C(obj, dataPtr, state<<24)` | Per-widget visual refresh after F5; still unmatched (19/53 near-miss stub). | (hold) | HOLD |

### OPTION round-trip additions (same session)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F7 | `0x8002D6C8` `Main_RunOptionsMenu` | LIVE-CONFIRMED: it is the ambient mode loop while sitting in the OPTION screen. First live validation of the idb2018 loop-family names. | (keep `Main_RunOptionsMenu`) | CONFIRMED |
| F8 | `SD_SEPlay` ids | SE 6 = cursor move, SE 7 = confirm (X), SE 8 = cancel (Circle); all at vol 0xFF. | (fact, no name) | CONFIRMED |
| F9 | `gMain_bMenuID` scope | During the whole OPTION visit the byte stayed 4 (the slot we entered from) and the menu module at 0x80180000 was NOT reloaded (arena md5 stable). The OPTION screen keeps its own selection state elsewhere. | (name unchanged) | CONFIRMED |

### STEREO/MONO toggle (same session, OPTION screen)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F10 | `func_80046FA0(mode)` | Fired on every stereo/mono toggle with a0=0 (STEREO) / a0=1 (MONO), both directions. Matched body: writes soundState(D_8009B45C)->f48, toggles flag 0x815, re-derives CD volume via func_80044DC0. Exact DotR vocabulary for this role. | `SD_SetOutputType` | APPLIED |
| F11 | `0x8009B408` (byte) | Flips 0<->1 with the setting (0=STEREO, 1=MONO), live, both directions, stable across double-checks. func_8003C628 derives D_8009B37D from it (sign-bit = reset semantics), so this is the authoritative stored setting. | `gSD_bOutputType` | APPLIED |
| F12 | `0x8003FEE0(id)` | Observed twice with different ids (8 = cancel, 0x2F = option toggle): body always calls SD_SEPlay(id, 0xFF). Current fleet name `call_80048658_255_0` is factually wrong about the args. | `SD_SEPlayFull` | APPLIED |
| F13 | `SD_SEPlay` id 0x2F | The option-toggle blip (vol 0xFF), via SD_SEPlayFull. | (fact) | CONFIRMED |
| F14 | `select_sound_preset` (0x800171A8) | Did NOT fire during stereo/mono toggling — it is NOT the output-type setter (kills the earlier SD_SetOutputType hunch for it). | (null result) | CONFIRMED |

### TRADE memory-card flow (same session, one card only)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F15 | `Main_RunMenu` modality | The 1P/2P MEMORY CARD caution dialog, BEGIN LOAD, slot-1 load, slot-2 NO DATA and its dismissal ALL run inside `Main_RunMenu` (ambient through every phase). Menu dialogs are modals, not mode switches. | (architecture fact) | CONFIRMED |
| F16 | `0x8002D7CC` (tradeLoop conflict) | Never fired during the whole card flow -- the trade screen proper requires BOTH cards to load. Conflict stays open; we now know its precondition. | (null result) | CONFIRMED |

### LOAD flow + secondary menu (same session)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F17 | secondary menu mode | The post-load menu (CAMPAIGN/FREE DUEL/BUILD DECK/LIBRARY/PASSWORD/SAVE) runs in the SAME mode as the main menu: `Main_RunMenu` ambient, mode byte 0x8009B26C unchanged (0xC8 -> low 5 bits = 8 = Main_RunMenu's slot in `D_80090B64[]`). The menu module swaps layouts, not modes. | (architecture fact) | CONFIRMED |
| F18 | `gMain_bMenuID` continuation | In the secondary menu, hovering the TOP item (CAMPAIGN) reads cursor = 5 — the shared cursor continues past the main menu's 0-4. PROVEN: SAVE (bottom item) reads cursor=10, so the secondary menu occupies indices 5-10 (CAMPAIGN=5 .. SAVE=10) of the shared cursor. | (fact) | CONFIRMED |
| F19 | save block liveness | Direct read after load: starchips=5, sorted 40-card deck, trunk = 722 distinct / 2166 total (3x every card — an all-cards save). The RAM map's 0x801D02xx-07xx block is the LIVE save state. The load itself produced a zero-byte RAM diff because the same save was already resident (loaded during the earlier TRADE slot-1 read). | (fact) | CONFIRMED |
| F20 | yes/no + LOAD COMPLETE dialogs | Both are `Main_RunMenu` modals, dismissed by X, consistent with F15. | (fact) | CONFIRMED |

### SAVE flow (same session)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F21 | save write path | X -> save? -> X -> overwrite? -> X -> SAVE COMPLETE, all `Main_RunMenu` modals (mode 0xC8 throughout). The card write really ran: 5,952 write events captured via the recomp's card tracker, pumped by BIOS-kernel code reading from a staging image around `0x801DC383+`. The write-phase function cluster is preserved in tmp/save_session.json. | (facts) | CONFIRMED |
| F22 | `0x801D3xxx` region | ~78 bytes changed there during the save (the only non-display churn near the save block) — likely save metadata/checksum staging. | (suspect lead) | CONFIRMED-OBSERVED |

### PASSWORD screen (same session)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F23 | `0x8002D684` | CONFLICT ADJUDICATED: it is the ambient mode loop of the password screen (live). The IDB's `Main_RunPasswordMenu` is correct; our `mask_test_call_800eb26c` was the mechanical read of the same loop. Rename queued for the bulk pass. | `Main_RunPasswordMenu` | APPLIED |
| F24 | mode indices | Entering PASSWORD switched the mode byte 0xC8 -> 0xCA — a REAL mode change (unlike every dialog so far). Mode-table indices measured: 8 = main/secondary menu, 10 = password screen. | (fact) | CONFIRMED |
| F25 | `0x8016D410` | The password entry digits: 8 bytes, one per digit (value 0-9), first digit at +0. Proven by reversible up/down spins (0->1->2->0). Lives in the password screen's own loaded module (~0x8016xxxx arena — another disc-loaded screen module, sibling of the menu module at 0x80180000). | `gPassword_abDigits` | APPLIED |
| F26 | screen facts | 8-digit entry field, starchip balance displayed from the save block, X=OK / Circle=END. Cursor at PASSWORD menu item read 9, completing the 5-10 proof. | (facts) | CONFIRMED |

### PASSWORD validation (same session, Blue-Eyes 89631139 entered)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F27 | validation split | X on 00000000: nothing but the SE chain fires — rejection is internal to the password module, no EXE lookup visible, no dialog. X on a real password: a 104-function pipeline runs — the CD request layer fires (later shown by F33 to be the ambient heartbeat, not an art fetch: pane art is RAM-resident), widget spawns build the card pane, `int_to_digits` renders the 999999 cost. Card shown, EXCHANGE/QUIT offered, starchips untouched until exchange. | (facts) | CONFIRMED |
| F28 | `int_to_digits` | Live-confirmed in its named role: fired exactly when the cost number appeared on screen. | (keep name) | CONFIRMED |
| F29 | `func_80013C28` | Second sighting: fired during OPTION screen build AND during card-art reveal — both disc-load moments. The screen-transition hypothesis is dead; it belongs to the CD streaming path. | (suspect refined) | CONFIRMED-OBSERVED |

### EXCHANGE refusal (same session)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F30 | insufficient-funds path | X on EXCHANGE with 5 < 999999 starchips: silent refusal — zero new EXE functions beyond an SPU poll, no dialog, starchips and trunk byte-verified untouched. The affordability check is inside the password module; the real purchase path (deduct + trunk increment) remains untraced. NOTE: on this all-cards x3 save, even an affordable purchase may refuse at the copy cap, so tracing a successful exchange likely needs a different save. | (facts) | CONFIRMED |

### PASSWORD deep dive (99999-starchip session)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F31 | purchase path | Bought card 2 (Mystical Elf, pw 15025844, cost 160), byte-exact: starchips 99999->99839, trunk[card] 3->4 (no 3-copy cap on trunk), used-flag set, acquisition logged. ONE-TIME USE CONFIRMED: re-entry rejected with "already put in that password" (operator-witnessed). | (facts) | CONFIRMED |
| F32 | `gDuel_awRecentCardDrops` | On purchase the list SHIFTED with card 2 inserted at the front — it is really the last-10 cards ACQUIRED (drops and password buys both). Description upgrade queued. | (semantic fix) | CONFIRMED |
| F33 | card art source | Zero CD reads during any reveal — the card art is RAM-resident. The earlier "disc streaming" attribution was a false positive: those functions are the periodic CD heartbeat (12-byte reads at LBA 9730), which also poisoned a reveal-detection marker this session. Instrument lesson: never use heartbeat members as event markers. | (facts) | CONFIRMED |
| F34 | digit cursor sprite | LEFT/RIGHT moves an X-coordinate pair at 0x8009B510/0x8009B518 (+24px per digit cell, echoed into packed sprite data ~0x800CBBxx). Each animation tick, the widget fn at ~0x80042210/20 builds the sprite record in the SCRATCHPAD (0x1F800320) and draws it via GsSortFlipSprite — the "rotation" is flip-frame animation. | (mechanism) | CONFIRMED |
| F35 | `0x801D0698` | 0 -> 0x20 exactly on the purchase; with F31's rejection, this is the used-password flag store (bit mapping/extent still unmapped). Cost-10 table rows were rejected with virgin flags — those cards look non-passwordable (separate mechanism, open). | used-password flags | CONFIRMED-OBSERVED |

### LIBRARY card view (same session)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F36 | `Main_RunLibraryMenu` | Live-confirmed ambient in the Library (third loop validated). Library = mode slot 4 (0xC8 -> 0xC4 on entry); the card-detail view is a MODAL within it (mode unchanged). The grid cursor is NOT gMain_bMenuID (menu value persists) — see F40: it is the global `gDuel_wSelectedCardID`. | (keep name) | CONFIRMED |
| F37 | card art, two paths | The library card view streams from disc: ~392 real 2048-byte sector reads (LBA ~28534) into staging at 0x8012C800-0x8013F800 and 0x801A8000 (that arena doubles as a general load buffer). The password screen's smaller art pane used NO disc reads. Full art = disc; pane art = RAM-resident. | (facts) | CONFIRMED |
| F38 | shared card-display core | 81 functions run in BOTH card displays: the CD request layer (func_800137E4/13940/13998/13C28/1455C + the unnamed libcd text_* stack, DsPacket, CdGetSector, parcpy/rescpy), the widget constructors (find_free_slot_0x10_0x60, init_obj_id_pos_flags, init_container_entry_objs...), GsSortPoly for the art quad, and the SE plays. This is "draw a card" as a family. | (family map) | CONFIRMED |
| F39 | library-view-only cluster | 53 functions unique to the detail view — including the 0x80035AB8-0x80039xxx group that renders the description TEXT (password view shows no description) and the 0x8002A6B8-0x8002B194 widgets (stars, guardian-star icons). The text-engine hunt starts here. | (lead) | CONFIRMED-OBSERVED |

### LIBRARY grid scroll (same session)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F40 | `gDuel_wSelectedCardID` (0x8009B338) | LIVE-CONFIRMED as the library grid cursor: 1->2->3 on RIGHT, +10 on DOWN (10-wide grid), fully reversible. It holds the selected card's ID (1-based) — the idb2018 name validated in its natural habitat. | (keep name) | CONFIRMED |
| F41 | `0x8009B258/59` (2 bytes) | Packed grid coordinates: low byte = column (+1 per RIGHT), high byte = row (+1 per DOWN), both reversible. Scope (library-only vs shared card-grid widget) unverified — check in Build Deck before naming. | `gCardGrid_bCursorColumn/Row` | APPLIED |
| F42 | `0x801D5608` | Mirrors the selected card id exactly (1->11->1). This is the RAM map's "? (used in function 0x21598/Duel_CalcRankScore)" row — a current-card context copy shared with the rank scorer's output area. | (suspect refined) | CONFIRMED-OBSERVED |

### LIBRARY navigation model (same session)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F43 | grid navigation decoded | The "8 sections" are pure rendering — no section variable exists. State is just (col, row, gDuel_wSelectedCardID): `gCardGrid_bCursorColumn` spans BOTH panels 0-19 (right panel = cols 10-19 showing the next hundred), `gCardGrid_bCursorRow` counts ABSOLUTELY (9 -> 10 crossing the view boundary, not capped). Measured: RIGHT from card 10 -> 101 (col 9->10); RIGHT at col 19 snakes to next row (110 -> 11); DOWN at the bottom row scrolls the view a full two-panel block (100 -> 210, row 9 -> 10). Sections tile 2 columns x 4 rows (operator-confirmed): section = (row/10)*2 + (col>=10), sel = section*100 + (row%10)*10 + (col%10) + 1 — reproduces every measured move. | (facts; names already applied) | CONFIRMED |
| F44 | `0x8009B35A`, `0x800ECE7C/98` | Additional bytes tracking the view scroll (display-offset counters near the selection globals). | (suspects) | CONFIRMED-OBSERVED |

### LIBRARY 3D monster viewer (same session)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F45 | card-page load | Opening a card's detail page streams ~392 sectors from a per-card disc location (card 1: LBA 28534, card 101: LBA 56134) into the standard staging (0x8012Fxxx-0x8013Fxxx + 0x801A8000). The red arrow appears when the load completes; SQUARE then opens the 3D viewer with ZERO additional disc access — art and monster model arrive in the same load. | (facts) | CONFIRMED |
| F46 | 3D viewer family | 105-function ambient set while the model rotates: the full GTE matrix stack (MulMatrix, Push/PopMatrix, RotMatrix*_gte), `rsin`/`rcos`/`ratan2` driving the orbiting camera via `GsSetRefView2`, hierarchical model render through `GsSortUnit`/`GsGetLwsUnit`, lighting, `RCpolyFT4A` rasterization — plus 54 unnamed functions that ARE the model/animation layer (prime naming ground). The `D_800F2C40` slot pollers run here too (likely the emote's monster-cry SE). | (family map) | CONFIRMED |
| F47 | `Model_LoadMonsterMerge` (0x80056504) | Did NOT fire anywhere in the library model path — its hedge stays. The IDB name likely refers to the DUEL-side loader that fills `playerMonModel`/`enemyMonModel` (0x80100000/0x80140000), a separate path. `func_80013C28` third sighting: fired with (channel=1, record 0x800F5DB0) — confirmed CD stream request member. | (negative + refinement) | CONFIRMED |

### BUILD DECK — trunk sorting (same session)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F48 | `Main_RunBuildDeckMenu` | Live-confirmed ambient (fourth loop validated). Build Deck = mode slot 7 (0xC8 -> 0xC7). | (keep name) | CONFIRMED |
| F49 | trunk sort mechanism | START cycles 7 sort modes. Each mode's order is computed ONCE via PsyQ `qsort(0x80100004, 722, 16, cmp)` then CACHED (14 presses total -> exactly 6 computed sorts, one per non-default mode). The trunk VIEW is 722 x 16-byte records in the 0x80100000 scratch arena (the "playerMonModel" buffer moonlighting). One comparator serves every mode: `0x80032BD4`, a mode-dispatching comparator; `BuildDeck_CompareCard` fires beneath it only on the name-ordered modes (576/133 calls observed). CORRECTED by F57: the qsort call cascade is quicksort's own recursion, and the comparator story is two siblings, not one dispatcher. | (superseded detail) | CONFIRMED |
| F50 | sort-mode variable | Not yet pinned — no byte cycled cleanly 0-6 across presses; likely a free-running press counter used mod 7, or module-held state. Open. | (open) | HOLD |

### BUILD DECK — chest list navigation (same session)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F51 | list navigation mechanics | The chest list shows 8 rows. DOWN/UP move the highlight within the window; R1/L1 scroll a full page (+/-8); R2/L2 skip chunks of +/-50. All reversible, verified at 351 Yaranzo. | (facts) | CONFIRMED |
| F52 | `gDuel_wSelectedCardID` in build deck | Here it tracks the BOTTOM VISIBLE row of the window (358 while the cursor sat on 351 at top; +8 per R1 page, +50 per R2 chunk, unchanged by in-window cursor moves). Same global, different role per screen: grid cursor in the library, window edge in the chest list. | (role note) | CONFIRMED |
| F53 | `gCardGrid_bCursorColumn/Row` scope | The pair does NOT move during chest-list navigation — it belongs to card GRIDS (library-style), not lists. Scope question from F41 answered. | (scope answer) | CONFIRMED |
| F54 | list cursor variable | Authoritative storage not pinned (render-side derivatives at 0x800A0609/0A, 0x800F073A, 0x8009B0B6 lag by a step; pointer hunt found only stack transients). Likely module-held. Open, alongside the sort-mode variable (F50). | (open) | HOLD |

### BUILD DECK — deck pane (same session)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F55 | deck pane navigation | Same mechanics as the chest list: 8 visible rows, in-window cursor moves leave `gDuel_wSelectedCardID` alone; page/chunk scrolls update it to the BOTTOM VISIBLE row's card id (verified against the screenshot: sel=145 = the bottom row "The Bewitching P."). F52's role reading confirmed on a second list. | (facts) | CONFIRMED |
| F56 | `0x8009B0AC` | Flipped on the chest->deck pane switch (1->0), and previously on the library's left->right panel crossing (0->1). Two sightings as an active-pane indicator; polarity semantics unconfirmed. | `g_ActivePane` (suspect) | HOLD |

### BUILD DECK — deck sorting (same session)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F57 | deck sort + comparator model | The deck view sorts at 0x80102D50 (40 x 16B records) and RE-SORTS on nearly every START (unlike the trunk's cached orders). Two comparators observed across the mode cycle: `BuildDeck_CompareCard` (0x80032B60) for three consecutive modes, `compare_rec_two_level_std` (0x80032BD4) for the others — both generic two-level record comparators with gCard_asNameSortKey tie-breaks (fleet-matched bodies agree). So each mode REBUILDS the records' primary keys and picks a comparator flavor; the earlier "mode-dispatching comparator" and "two-level CARD TYPE sort" readings in F49 are corrected — the observed qsort cascades are quicksort recursion. | (model) | CONFIRMED |

### BUILD DECK — card moves (same session)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F58 | deck editing is staged | X on a deck card removed it (40->39), X on a trunk card added it (39->40) — but `gDuel_awPlayerDeck` in the save block stayed byte-identical through BOTH edits. Editing works on staged/view state; the save-block deck must be committed on screen exit (verify when leaving). The trunk rows' in-deck count column updates live. | (edit-buffer model) | CONFIRMED |
| F59 | `0x801D560C` | The working deck count (40->39->40 in lockstep with the edits), sitting in the 0x801D5608 current-context block right after the selected-card mirror. View copy at 0x80105AA0. | `deckCount` (working) — suspect | HOLD |

### BUILD DECK — restore & exit (same session)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F60 | edit-repair + exit | The accidental double-registers (3-frame presses double-fire on EDIT actions — instrument lesson; 2-frame pulses register once) were fully repaired: both stray Yaranzos removed, cards 9 and 58 re-added, staged deck verified EXACTLY equal to the original multiset. Exit (Circle) captured: cancel blip, teardown family, mode back to 8. Because the staged deck equaled the original, the commit wrote identical bytes — the F58 commit-on-exit hypothesis stays OPEN for a run with a genuinely changed deck. One commit-path lead: on exit, Main_RunBuildDeckMenu's body called `call_80047430_neg8_0` with a SAVE-BLOCK pointer (0x801D07DA, 0x10). Full exit set in tmp/bd_exit.json. | (facts + lead) | CONFIRMED |
| F61 | trunk "New!" tag | CORRECTED by operator: New! marks cards you LAST WON (recently acquired, list holds ~15-16) — not returned cards. Ties to the last-acquired list (gDuel_awRecentCardDrops region; capacity may exceed the RAM map's 10 entries — verify extent). NEW sort mode presumably surfaces these. | (fact, operator-corrected) | CONFIRMED |

### BUILD DECK — deck-ready guard (same session)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F62 | exit guard | Circle with the working deck count != 40 opens the "YOUR DECK ISN'T READY!" modal (BUILD DECK / EXIT) INSTEAD of exiting — mode byte stays 0xC7, so it's another in-mode modal. The guard reads the working count (0x801D560C), not the save block. Capture in tmp/notready.json. | (fact) | CONFIRMED |

### FREE DUEL menu (same session)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F63 | `Main_RunFreeDuelMenu` | Live-confirmed ambient (fifth loop validated). Free Duel = mode slot 6 (0xC8 -> 0xC6); the SELECT OPPONENT! dialog is another in-mode modal. | (keep name) | CONFIRMED |
| F64 | `gFreeDuel_dwUnlockedDuelists` | Live-verified as a duelist unlock BITMASK: reads 0x40 (one bit) on this save, and the grid shows exactly one duelist (Simon Muran) + the Build Deck tile. The operator's full 8x5 board (through Duel Master K, with per-CPU WIN/LOSS) is the all-unlocked rendering. Mask may span >4 bytes for 39 duelists — extent unverified. | (name validated) | CONFIRMED |
| F65 | grid cursor scope | `gCardGrid_bCursorColumn/Row` did NOT track cursor presses here — but with a 2-tile grid the navigation sample is too small; scope test deferred to a fuller roster. | (deferred) | HOLD |

### FREE DUEL grid cursor (same session)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F66 | `0x8009B366/67` | The free-duel opponent-grid cursor: col byte +1 on RIGHT, row byte +1 on DOWN, both cleanly reversible. Sits in the duel-globals block right after `gDuel_bTerrain`. The card-grid pair (gCardGrid_bCursorColumn/Row) stays frozen here — each grid family has its own cursor block (F65 resolved). `gDuel_bOpponentID` does NOT track hover — it must be set on selection. | `gFreeDuel_bCursorColumn/Row` | APPLIED |
| F67 | `0x8009B36C/6D` | A second col/row pair moving in lockstep with the cursor — likely the previous-position pair used for un-highlighting. | prev pair (suspect) | HOLD |

### FREE DUEL refusal + deck commit proof (same session)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F68 | duel-start guard | X on an opponent with an illegal deck: "YOUR DECK ISN'T READY!" banner, mode stays 0xC6 (modal), `gDuel_bOpponentID` never set — the guard short-circuits before selection. The guard reads the COMMITTED deck (the build-deck working count is 0 outside that screen). | (fact) | CONFIRMED |
| F69 | F58 CLOSED — commit proven | `gDuel_awPlayerDeck` (save block) read back with the operator's 39-card edit: compacted, ascending, trailing zero. Deck edits COMMIT on build-deck exit (including the not-ready dialog's EXIT route). The staged-edit model is now complete: stage in working arrays -> commit compacted+sorted on exit. | (proof) | CONFIRMED |

## Structural notes (not names)

- The main-menu logic itself executes from a menu module loaded from disc
  into `0x80180000+` (call sites 0x80180924/0x801809C0/0x801809D0). It is
  OUTSIDE SLUS_014.11's static code — a third code space next to the main EXE
  and the duel overlay. Its data includes the cursor byte (0x80184594) and
  widget art/anim data around 0x801AF8xx.
- The recomp debug server's `press` command button mask: 0x10 moves the menu
  cursor DOWN, 0x40 UP (do not assume the standard PSX pad word).
- Menu-move widget pool objects live around 0x800F0698/0x800F0858 (0x1C0
  apart — likely an object array; pool base unknown yet).
- Menu-module dump captured live (tmp/menu_module_80180000.bin, 0x30000 span,
  ~24KB nonzero). The cursor handler's own store instructions located: sb to
  0x4594(base) at module PC 0x80180890 (one direction) and 0x801808C0 (the
  other), setter at 0x80180070; EXE call-outs at 0x80180924/9C0/9D0 follow.
  Handler body ~0x80180800-0x801809FF — first concrete disassembly anchor for
  the menu-module code space.

## Session 2026-09-01 — end-of-duel credit: W/L records, starchips (rematch mod work)

Context: built a results-screen rematch trigger in the recomp; it bypasses the
natural results exit, which surfaced exactly what that exit does. Evidence is
live RAM reads/writes on the recomp plus stock-exe disassembly; the community
GameShark DB was used as a cross-check and caught making an off-by-one itself.

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F70 | `0x801D071C` (39 × 4 bytes) | Per-duelist free-duel records, indexed by `gDuel_bOpponentID`: u16 wins at +0, u16 losses at +2, both capped 0x270F. Pinned live: a natural win vs Simon landed at 0x801D0720 while `gDuel_bOpponentID` read 1 on his results screen; writes to 0x724/0x726 showed on Teana's grid row (id 2). Cross-check: GameShark "DarkNite max wins" 0x801D07B0 = base + 37*4. WARNING: the community "Simon 0 losses" code (0x801D0724) is an off-by-one in THEIR db — that address is id 2's wins. | `duelistRecords` | APPLIED |
| F71 | `0x8009B1D8` (2 words) | Per-side save-struct pointers for end-of-duel credit, set during the duel bootstrap (0x80017D3C..0x80017D80): 1P-vs-CPU duel → {0x801D0200, NULL}; a mode gated on `D_8009B361 < 0` → {0x801D1200, 0x801D2200}. Consumed by the 0x80021F24 block (F23). | `g_DuelSavePtrs` | APPLIED |
| F72 | `0x8009B165` (1 byte) | Winner side for the F23 block: used as index into `g_DuelSavePtrs` (winner gets wins++ at ptr+0x518, loser = index^1 gets losses++ at ptr+0x51A). Read 0 live on a player-win results screen; CPU-win value not yet observed. | (hold — `g_DuelWinnerSide` once a loss is observed) | HOLD |
| F73 | `0x80021E7C..0x80021FAC` (in `Duel_MaybeEndOfDuel` 0x800218F0) | One-shot end-of-duel credit block, latched by bit 0x2000 of the state half 0x8009B23A. Branch at 0x80021EA4: `D_8009B360 >= 0` OR `gDuel_bOpponentID < 0` → W/L record path (wins/losses u16, cap 0x270F, via `g_DuelSavePtrs` + 0x518/0x51A); else → starchips path: `gLibrary_dwStarchips += unk2E0->0x3A`, cap 0xF423F (999999), then the drop award `jal 0x80021894`. Note: in a 1P free duel this block's record path addresses ptr0+0x518 = 0x801D0718, which is NOT the per-duelist record hit at natural exit — the per-duelist increment site is still unlocated (see F25). | (fact) | CONFIRMED |
| F74 | `Duel_MaybeEndOfDuel` tick gap | The fn stops being entered for 8+ frames between the spoils (card award) page and the RESULTS OF DUEL banner. Cost a recomp bug: an outcome flag cleared on "results fn quiet for 8 frames" wiped the drop-roll evidence before the results screen was even up. | (fact) | CONFIRMED |
| F75 | per-duelist record increment | Unlocated. No resident instruction carries imm 0x718/0x71A/0x71C; a live-RAM 0x270F/0x2710 cap sweep on a results screen found only F23, the battle-math stat-cap family (0x800170C8 + Duel_CalcBattleAttack/Def, see F76), and unexamined sites 0x80028E44 / 0x8002CC84 / 0x80050008 / 0x80059C40 / 0x80071980 — yet `duelistRecords[gDuel_bOpponentID]` increments exactly once on the natural results exit (watched 2→3→4 vs Simon). Writer presumed to use a precomputed pointer with no distinctive immediates, or to be one of the unexamined sites. duel_overlay.bin has no 0x270F/0x2710 at all. | (unlocated) | OPEN |
| F76 | `0x800170C8..0x8001712C` | Leaf helper: given a struct in $a0, returns (sum of s16s at +0x10,+0x12,+0x14) <<16 \| (sum at +0xE,+0x12,+0x14), each clamped to [0, 0x270F]. RESOLVED while porting to the symbols guide: its two callers at 0x8001EF1C/0x8001EF78 are the already-named `Duel_CalcBattleAttack`/`Duel_CalcBattleDefense` (they add `Duel_CalcGuardianStarBonus` 0x8001EE44 and clamp again), so this is BATTLE MATH — the halves are base ATK<<16\|DEF with boost fields summed, and 0x270F here is the 9999 STAT cap, not a record cap. No relation to duel records. | (pending — name with the monBattle* family) | PENDING |
| F77 | duelist id facts | `gDuel_bOpponentID` (0x8009B361) ids pinned live: Simon Muran = 1, Teana = 2; DarkNite = 37 via the GameShark cross-check. The free-duel grid cursor position is NOT this id (a grid read of 8 belonged to a duel later pinned as id 7). | (facts) | CONFIRMED |
| F78 | drop roll gates on win | `0x80021810` (drop roll) fires only for a WON duel — its one call site is the end-of-duel reward path in F23's starchips branch. Used as the win/loss discriminator by the recomp's rematch mod; 3 duels × win = 3 roll bursts observed, none on the credit-path duels that recorded losses. | (fact) | CONFIRMED |

### FREE DUEL scrollbar + cursor pipeline (session 2026-09-02, Linux seat, DPAD only)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F79 | `0x80168004` (free-duel module) | **The scrollbar function.** Per-frame routine, called from the screen tick at 0x80168D04 (and from two other module sites). Reads the cursor widget (ptr at 0x801690A0 -> 0x800F0858) y, keeps the SHARED scroll-follow position `D_8009B148` (already known to matched C as the generic scroll-follow y, see scroll_follow.c; NOT renamed) so the cursor stays inside [40,145) px of the viewport (scrolls up when above 40, down when past 144), then writes the thumb widget (ptr at 0x80169058 -> 0x800F07E8) y = 7 + (cursor_y - 40) * 72 / 364 via a magic-number divide (mult 0xB40B40B5). 72 px of thumb travel over 7 rows of 52 px. Caught by wtrace: the store at 0x8016808C is the ONLY writer of 0x800F081A, 60x/s. Thumb glides because it follows the tweened cursor y, not the row index. | `FreeDuel_UpdateScrollbar` | APPLIED |
| F80 | `0x80168090` (module) | Cursor placement: x = col*56+20, y = row*52+40 from `gFreeDuel_bCursorColumn/Row` (0x8009B366/67) into the cursor widget, calls func_80035B7C(0x800EB0F8, ...). With a1!=0 also refreshes the selected duelist's W/L pair from `duelistRecords[row*5+col]` into 0x801D5608 and sets 0x8009B32E to the name-string id, guarded by the grid availability table. | `FreeDuel_PlaceCursor` | APPLIED |
| F81 | `0x8009B36C/6D` (2 bytes) | **Pending** grid col/row. The input handler writes these (clamped 0..4 / 0..7, no wrap) and the tween tick at 0x80168A9C animates the cursor widget toward col*56+20 / row*52+40 over 8 frames (vel = delta*32/8 in the widget's 1/256 fixed-point 0x36/0x38 fields), then copies them into `gFreeDuel_bCursorColumn/Row` (0x8009B366/67), re-places the cursor and plays SE 0x2F. So 366/367 are the COMMITTED cell, 36C/36D the target -- resolves the F66 'prev-pair suspect'. | `gFreeDuel_bTargetColumn` / `gFreeDuel_bTargetRow` | APPLIED |
| F82 | `0x80168C7C` (module) | Free-duel screen tick: dialog-open path (flag 0x20 of 0x801690A4) else cursor tween tick (0x80168A9C), scrollbar update (0x80168004), then input: pad-edge word `0x8009B3A4` bits 0x2000 RIGHT / 0x8000 LEFT / 0x4000 DOWN / 0x1000 UP move the pending cell; button word `0x8009B398` 0x20 = cancel (SE 8), 0xC0 = confirm: cell (0,0) opens Build Deck (mode 6), any other available cell checks the 40-slot deck at 0x801D0200 (empty slot -> SE 9 + dialog), else SE 0x30, init_800eb26c_fields_from_args(-1, row*5+col, 0x6000, 0x6000), mode byte 0x8009B26C = 3 (duel start). | `FreeDuel_UpdateScreen` | APPLIED |
| F83 | `0x80168FB4` (module) | Module entry the EXE calls by fixed address from `Main_RunFreeDuelMenu` (src/mask_test_800eb26c_dual_calls_conditional.c, extern func_80168FB4): calls rand() then the screen tick. The free-duel module lives at 0x80160000..0x8017B000 (code ends at a `break` at 0x8017B000); its file is not yet identified (cd_read_log window too short). | `FreeDuel_Entry` | APPLIED |
| F84 | `0x80169030` (module data, 40 bytes) | Grid availability table, 8 rows x 5 cols, indexed row*5+col by both the placement and confirm paths. On this save: (0,0)=1 Build Deck tile, (0,1)=1 Simon, (7,4)=1; everything else 0 (matches gFreeDuel_dwUnlockedDuelists = one bit). | `gFreeDuel_abGridAvailable` | APPLIED |
| F85 | `0x801690A4` (module byte) | Screen flags: 0x40 set while the cursor tween is in flight (input ignored until it lands), 0x20 while the refusal dialog is up. | `gFreeDuel_bScreenFlags` | APPLIED |
| F86 | `0x801681B4` / `0x8016899C` / `0x801689D4` (module) | Sparkle pool: on every cursor move 7 objects are spawned (find_free_slot_0x10_0x60 + get_or_init_D_800EFE48_slot + call_800404CC_store54 with tex 0x801AF000, flags 0x180018) into a 16-entry ptr pool at 0x80169060..0x8016909C; 0x8016899C finds a free pool slot, 0x801689D4 ticks/fades them (counter at +0x60, __builtin_delete on expiry). | `fd_sparkle_spawn` / `fd_sparkle_slot` / `fd_sparkle_tick` | HOLD |

### Screen fade to black (circle out of Free Duel, X back in) -- session 2026-09-02

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F87 | `0x800E9EC8` (0x28 bytes) | **The screen-fade state block.** Layout proven from matched C (func_800151D8, func_800158B8, reset_9ec8_state_if_flag145) plus live sampling through both transitions: +0 u32 fade colour (0 black, 0x00FFFFFF white via flag 0x8009B145), +4 u8 current level, +5 u8 target level, +6 u8 flags (0x80 fade in flight, 0x01 strip-wipe mode, 0x10/0x20 colour-latch), +7 u8 step per frame (8 for menu transitions, 12 elsewhere), +8 s16 head, +0xA u8[30] per-strip brightness. Live: fade-out head 384 -> -129 at 8/frame, ~48 frames; fade-in mirrors it. | `gFade_State` (struct ScreenFade) | APPLIED |
| F88 | `0x800151D8` `func_800151D8` | Per-frame strip walker (matched C). Walks 15 strips from the head value, each 8 less than the previous, clamped to [current,target], and writes each into strips[i] AND strips[29-i] -- so the wipe is mirrored: fading OUT darkens the top and bottom edges first and the middle last; fading IN lights the middle first. Head moves by step * D_8009B0D8 per frame. | `Fade_StepBands` | APPLIED |
| F89 | `0x80015310` `func_80015310` | Per-frame fade update, called from Fade_DrawOverlay. Runs the strip walker while flag 0x01, or a flat level ramp otherwise; when current reaches target it clears 0x80, latches the RGB (0x8009B142..44 <- 0x8009B14A..4C) and clears/sets 0x8009B141 (overlay-enable) via func_80015CFC / func_80015D0C. | `Fade_Update` | APPLIED |
| F90 | `0x800154E4` `Fade_DrawOverlay` | Draws the fade: 30 semi-transparent black box fills (GsSortBoxFill, prim code 0x60, 320x8 each, y += 8) with colour = 255 - strips[i]; in flat mode one full-screen box at 255 - level. Skips drawing when no fade is in flight and 0x8009B141 is clear or level is 255. 1440 box fills in the 48-frame fade = 30/frame. | (keep name) | CONFIRMED |
| F91 | `0x800158B8` / `0x80015904` / `0x80015B00` | The fade-OUT chain both transitions use. 158B8: head=255, target=0, flags=0x80, fill all 30 strips with the current level (func_800156B8, 3-insn loop), step=12. 15904: calls 158B8 then step=8 and flags|=1 (strip mode). 15B00: calls 15904 then loop_call_four_update_funcs_while_flag80 -- i.e. it BLOCKS, pumping frames until flag 0x80 drops. Circle in Free Duel: Main_RunFreeDuelMenu -> 15B00 at +2f, done at +50f, teardown (func_8002CD8C) at +52f, disc load (func_80013C28/LoadImage2) at +56f. X on the sub menu: Main_RunMenu (ra 0x8002D5FC) -> 15B00 at +19f after the module's 17-frame highlight animation; done +67f; load +73f. | `Fade_InitOut` / `Fade_StartOut` / `Fade_WaitOut` | APPLIED |
| F92 | fade-IN (cross, +110f..+163f) | After the free-duel module builds its widgets, call_8004733C_masked_flag(0x72C0) fires and the block goes lo=0 hi=0xFF flags=0x81, head climbing 8/frame from 32 to 384 over ~50 frames. The fade-in is started by the screen's own init path, not by the mode loop. | (caller not yet pinned; hold) | HOLD |
| F93 | `call_80047430_neg8_0` (0x8003FF34) | Fires in the same frame as every screen fade-out, from the mode loop: call_set_1582_1584_masked(-8, 0) on the sound state (D_8009B45C+0x1582/0x1584). Paired music fade-out at -8/frame. | `SD_FadeOutMusic`-class (needs the SD_ vocabulary check) | HOLD |
| F94 | `0x8009B141` | Overlay-enable byte: set to 1 by func_80015CFC when a fade starts / the screen is black, cleared by func_80015D0C when a fade-in completes. Fade_DrawOverlay keeps painting the black overlay while it is set even with no fade in flight -- that is what keeps the screen black during the disc load between screens. | `gFade_bOverlayOn` | APPLIED |

### Name entry + the text-box/dialog machine (session 2026-09-02, Linux seat)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F95 | `0x800EB0F8` `gTextBox_aRecords` | **The text-box/dialog machine's records**, 0x64 bytes each, index = box slot. +0 current text ptr (advances as the typewriter consumes glyphs), +0x24 glyph-entry list (0x1C each: +0 SJIS code, +0xC/+0xE x/y), +0x28/2C/30 box/highlight/shadow widgets, +0x34 flags (0x800/0xA00 build requested, 0x4000 started, 0x2000 done, 0x20 shadow, 0x100 small font), +0x36 string id, +0x3C.. rect, +0x57 slot, +0x5A/5B glyph cell w/h, +0x5C/5E glyph-sprite slot base/count from `gTextBox_awSpriteSlotRange` (0,255,415,575,620). Name entry: slot 0 = arrows/END (id 243), 1 = the letter grid (id 242, 20x18 cells), 2 = 'Input your NAME!' (id 244, 8x12), 3 = the typed name (id 254, 16x16). | `gTextBox_aRecords` | APPLIED |
| F96 | `0x801B0000` / `0x801C0000` | **Strings.** A per-screen bank of 0xFF-terminated glyph strings at 0x801B0000 and a u16 offset table at 0x801C0000 indexed by string id (512 entries live here). `Text_LookupString` (matched C, was bank_table_lookup) returns base+offsets[id]; ids >= 0xD000 index the table relative to 0xD000. Control bytes seen: 00 = space, F8 xx yy = set position/colour, FA/FB/FC/FD xx.. = other ops, FE = newline, FF = end. 'Input your NAME!' is id 244 at 0x801B1152 (glyphs 1a 06 14 0d 02 00 11 04 0d 08 00 2c 18 1e 25 17). Id 254 (0x801B125A) is a blank slot the name-entry module writes the typed name into. | `gText_aBank` / `gText_aBankOffsets` | APPLIED |
| F97 | `0x801D9000` `gText_adwGlyphCodeTable` | u32 per glyph index, low half = the Shift-JIS code ('A' 0x8260 -> glyph 24, 'B' 0x8261 -> 45, 'I' -> 26, 'a' 0x8281 -> 3, '0' 0x824F -> 56, '!' 0x8149 -> 23; 91 entries). `Text_SjisToGlyphCodes` (0x8003BC40, matched) scans it to turn an SJIS string into glyph codes, 0xF0xx for indices >= 0xF0, 0xFF terminated. | `gText_adwGlyphCodeTable` | APPLIED |
| F98 | `0x800393B0` `TextBox_BuildStep` | The typewriter. First call (flag 0x4000 clear): resolves the string ptr, resets counters, allocates the box widgets (`init_highlight_and_shadow_slots`), then each call emits one glyph sprite; sets 0x2000 when the string is done. `poll_call_393b0_until_bit13(_v2)` spin it to completion synchronously. Dismissing 'Input your NAME!' = `TextBox_Destroy` (0x80035B7C) on slot 2 from the module at 0x80169910, after `Widget_SlideSine` (0x80043230) slides the box off (y 248 <-> 176 over an 85-step sine). | `TextBox_BuildStep` | APPLIED |
| F99 | `0x80035BE4` / `0x80035C38` / `0x80035AB8` / `0x80035AF0` / `0x80039934` | Create (rect + record init, optional flag OR), set rect, init record (id, 8x12 cell, sprite partition), move (record + its widgets). Free Duel's refusal dialog = TextBox_CreateFlagged(1, 8, 48,108,224,16, 0x1028); name entry init = TextBox_Create(3, 254, 112,204,96,16) for the name and TextBox_Create(0, 243, 262,60,100,100) for arrows/END. | `TextBox_Create` / `TextBox_CreateFlagged` / `TextBox_SetRect` / `TextBox_InitRecord` / `TextBox_SetPos` | APPLIED |
| F100 | name entry module (loaded at 0x80160000, data 0x8016D400) | NOT applied to symbol_addrs -- this module shares its load address with the free-duel module, so per-address names collide across screens (see notes/research/modules.md). Proven: cursor col 0x8016D401 (0..14, wraps), row 0x8016D402 (0..8, wraps), saved row 0x8016D426, name length 0x8016D42C (cap 5), byte length 0x8016D408, flags 0x8016D400 (0x20 prompt up, 0x80 END, 0x02 prompt sliding, 0x10 leaving), cursor widget ptr 0x8016D404 (-> 0x800F0548), name ptr 0x8016D418 (-> 0x801D060C in the save block, SJIS u16 per char), caret widget 0x8016D43C (x = len*16+107). Pixel targets 0x8016D434/36: x = col*20+22 (+20 more for cols >= 11), y = row*18+24. Cell table 0x8016AB38 (9x15: 0 letter, 4/0x46 arrows/END with cursor size, negative = skip N cells left) and a row-jump table 0x8016ABC0 for the right-hand column. | (module symbols, in modules.md) | CONFIRMED |
| F101 | name entry: how a letter becomes a character | On X: `0x8016868C(1, x, y)` finds the glyph entry of the letter-grid text record whose position equals the cursor's, and its +0 halfword IS the Shift-JIS code; `0x80169690` stores it at name[len] in the save block; `Text_SjisToGlyphCodes(0x801B125A, name, 6)` rebuilds string 254 and the name text box is destroyed and recreated to retype it; SE 0x0C. No character table in the module at all -- the on-screen grid string is the source of truth. | (module) | CONFIRMED |
| F102 | `0x8009B3A4` / `0x8009B398` (pad words) | Second screen with the same layout (free duel was the first): 0x8009B3A4 is the PRESSED-EDGE word with DPAD in the high nibble (0x1000 UP, 0x2000 RIGHT, 0x4000 DOWN, 0x8000 LEFT); 0x8009B398 is the button word (0x20 cancel/Circle, 0x40/0x80 confirm/X, 0xC0 tested together, 0xE0 any face button). Both are allow_duplicated symbols used across matched C -- bulk rename candidate. | `g_PadPressed` / `g_PadButtons` | PENDING |

### Name confirmation dialog + the YES/NO choice machine (session 2026-09-02, Linux seat)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F103 | the 'Your NAME is BBU-' box | Text record slot 2 built from bank string 245 (0x801B1167): `fb 53` box style, 'Your NAME is ' (glyphs 24 04 0d 08 00 2c 18 1e 25 00 05 07 00), `f8 0a 01` style on, `fc 5a 12` = insert the dynamic name string, `f8 0a 00` style off, '-' (0x30), then FE 'YES' FE 'NO' FE `fb 80 91 11 8e 11` (choice-layout attribute) `f8 16` FF. Opened by the module's dialog tick (0x80169734) from the pending id at 0x8016D4D2: func_8003B6AC(2,2) then TextBox_Create(2, id & 0xFFF, 16, 248, 288, 48), record +0x59 = 20, built synchronously with poll_call_393b0_until_bit13 (0x80039A14), then gDialog_bChoice = 0 and Dialog_OpenChoice(rec) stored in rec+0x30. | (string 245 / slot 2) | CONFIRMED |
| F104 | `0x8009B34D` `gDialog_bChoice` | The YES/NO selection: 0 = first line (YES), 1 = second (NO). Written live by Dialog_ReadChoiceInput on every DOWN/UP (wtrace: pc 0x80037098, ra in Dialog_UpdateChoice) and finalised from the low 3 bits of gDialog_bInputState on confirm. Also the choice cursor moves 12 px per line in the prim lists (one 8x12 text line). | `gDialog_bChoice` | APPLIED |
| F105 | `0x800371A8` `Dialog_UpdateChoice` | Per-frame tick of the choice cursor object (matched C): first call spawns its sprite (get_or_init_D_800EFE48_slot(id,4), draw callback Widget_UpdatePulseColour for the blink); then if gDialog_bInputState bit 0x40 (confirmed) -> gDialog_bChoice = state & 7, re-highlight, done; bit 0x80 -> cancelled; otherwise feeds the DPAD to Dialog_ReadChoiceInput. | `Dialog_UpdateChoice` | APPLIED |
| F106 | `0x8003700C` `Dialog_ReadChoiceInput` | DOWN (D_8009B394 bit 0x4000) increments the selection while below gDialog_bChoiceCount, UP decrements while >= 0, bit 0x8 cycles with wrap; each move plays SE 6 and calls Dialog_HighlightChoice. Returns 0 when no relevant input. | `Dialog_ReadChoiceInput` | APPLIED |
| F107 | `0x800374F4` / `0x80036F80` / `0x80037110` | open: creates the cursor object at (box.x + box.w - 16, box.y + box.h - 16) with sprite 0x20C; highlight: colour 0xC0 vs 0xC0C0 depending on bit gDialog_bChoice of gDialog_bChoiceEnabled; pulse: triangle wave of the frame counter into the colour lanes. | `Dialog_OpenChoice` / `Dialog_HighlightChoice` / `Widget_UpdatePulseColour` | APPLIED |
| F108 | module poll (0x801699A0..) | After the box reports done (flags & 0x2008 == 0x2000) the module reads gDialog_bChoice: nonzero (NO) clears its own 0x20 dialog flag and returns to typing; zero (YES) clears the pending id, sets a 1024-frame counter on the cursor widget and flag 0x02, i.e. proceeds to leave the screen. | (module) | CONFIRMED |
| F109 | `0x8009B394` (pad word for the text system) | DPAD edge bits match the module words (0x1000 UP, 0x4000 DOWN); 0x8 = repeat/auto. Same family as 0x8009B3A4 / 0x8009B398 -- rename together. | `g_PadPressedText`? | PENDING |

### Campaign intro cutscene = the story script engine (session 2026-09-02, Linux seat)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F110 | `0x8002CE64` `Main_RunCampaign` | Live: the only mode-loop-family function ambient after YES on the name (mode byte 0xC8 -> 0xC2), running the intro cutscene. IDB name adopted; the CONFLICT row (ours `mask_test_800eb26c_notify_and_recheck`) is adjudicated by trace. | `Main_RunCampaign` | APPLIED |
| F111 | the cutscene = a bytecode script | No screen module is loaded (no module-range callers at all). `Script_RunTick` (0x8002FA54, matched C) picks a script through a table of self-relative u16 offsets (the table lives at `gAiScript_abHandScript` 0x801A8000 -- that IDB name is too narrow, the same bank serves story scripts), latches the byte-stream pointer in `gScript_pStream` (0x8009B290) and dispatches opcodes through `gScript_apfnCommand` (0x80090C50, 24 handlers) via the index in `gScript_wCommand` (0x8009B27C, bit 0x8000 busy). `gScript_wState` (0x8009B2A4): 0x8000 running, 0x4000 waiting on a text box (serviced by func_80039794), low bits = script index. The text itself is string 1350 of the bank (0x801B127A), typed by `TextBox_BuildStep` into slot 0 at one glyph every ~30 frames; the string embeds the script's own control bytes (F7-FD) and choices ('Run away' / 'Keep listening' in the same 【】 + FB 80 syntax as YES/NO). | `Script_RunTick` / `gScript_wCommand*` / `gScript_pStream` / `gScript_wState` | APPLIED |
| F112 | `0x8002E730` `Script_OpShowImage` | Op 5 of the table: on the first call MoveImage copies the staged picture (320x160 at VRAM 448,256) onto the display; otherwise, when no fade/load is busy, frees the previous image records, `Fade_InitOut` for the cross-fade, `func_8002DF2C` + `reinit_d800e9e60_obj` + `func_800137E4` to pull the next picture from disc. Fired at +7f after the first X: that is the picture change. | `Script_OpShowImage` | APPLIED |
| F113 | advancing a page (X) | `func_800375A4` (0x80037658 site): SE 0x0B, deletes the 'press X' arrow object (0x800EFF28) -- which is the SAME choice-cursor object `Dialog_OpenChoice` spawns (called at +69f from func_80038D14 when a page fills), then func_8003741C/func_800373C8 reset the box and the glyph sprites are cleared (func_80039BE0/AAC x496) before typing resumes. Record 0's text pointer moved 0x127E -> 0x129D -> 0x12B6 across two pages. | (pending: `script_page_advance`, `script_page_wait`) | HOLD |
| F114 | script control bytes (observed in string 1350) | F7 xx, F8 xx yy (position/style), F9 xx yy, FA (page wait, no args), FB xx yy (box/choice layout: FB 80 xx after choice lines), FC xx yy (insert dynamic string), FD xx yy, FE newline, FF end. Arg counts for F7/F9/FD are inferred from the decode aligning with readable text; verify against the builder's parser before naming ops. | (table in symbols_guide) | PENDING |

### Simon's choice in the campaign intro (session 2026-09-02, Linux seat)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F115 | Simon's 'Run away / Keep listening' | Live: gDialog_bChoice 0 (Run away highlighted), gDialog_bChoiceCount 2, gDialog_bChoiceEnabled 0xF, gScript_wState 0xC030 (running + waiting on a text box, script 0x30), gScript_wCommand 0x8002 (busy in opcode 2 = the text-box wait), gScript_pStream 0x801A82EE (the story bytecode lives in the 0x801A8000 bank the IDB calls gAiScript_abHandScript). The box is string 1281 ('My dear prince!...'), not 1350. | (state) | CONFIRMED |
| F116 | choice = a branch inside the TEXT stream | Each of the four choice points in string 1281 is `FA` `FB 02` <Run away> FE <Keep listening> FE `FB 80` u16 u16. The two halfwords are bank offsets indexed by the answer: option 0 -> 0x15DE for all four (the shared exit: 'Wait! Stop, my prince!' / 'Drat! He's gone...'), option 1 -> the byte right after the block (0x140F, 0x147F, 0x14FC, 0x1571: the next paragraph). func_80038BF0 (matched C) handles the FB byte: flag 0x80 -> advance the cursor by gDialog_bChoice*2 and hand to func_80038BA8, which reads the u16 and replaces the LOW 16 bits of the text cursor (bank base kept). No script opcode or game flag is touched by the answer here; the story bytecode resumes at the same opcode when the box closes, so on this screen the choice only decides how much of Simon's lecture you hear. | `text_op_layout_or_branch` (0x80038BF0) / `text_cursor_set_offset` (0x80038BA8) | PENDING |

### Campaign overworld (session 2026-09-02, Linux seat)

| # | address | what we proved | proposed name | status |
|---|---|---|---|---|
| F117 | `0x8002D2D8` `Main_RunCampaignMap` | Live: after the intro's last box, Fade_WaitOut from Main_RunCampaign, teardown, mode 0xC2 -> 0xC5, then Main_RunCampaignMap is the only ambient loop; a screen module loads at 0x80168000 (callers 0x80168xxx), the scene is 3D (GsSetRefView2, SetFogNearFar every frame) and text slot 0 shows the location name. | (IDB name, live-confirmed) | CONFIRMED |
| F118 | location names = global bank ids 0x8350+idx | func_800383DC / Text_LookupString: ids >= 0x8000 resolve through `gText_aGlobalOffsets` (0x801D5800) off base 0x801D0000. 0x8350..0x835F decode to Metropolis, Forest Shrine, Desert Shrine, King's Valley, Sea Shrine, Shrine of Glory, Mountain Shrine, Forbidden Ruins, Meadow Shrine, Vast Shrine, Town Plaza, Shrine, Duel Ground, Card Shop, Pharaoh's Palace, Hiding. Indices 0-9 are world-map sites (own camera per row), 10-15 the town (shared camera row). | `gText_aGlobalOffsets` | APPLIED |
| F119 | overworld module state + table | `gCampaignMap_Location` 0x8016960C (14 = Pharaoh's Palace at start; RIGHT -> 11 Shrine, DOWN -> 12 Duel Ground, LEFT/UP back), `gCampaignMap_LocationPrev` 0x80169618 (lerp source), `gCampaignMap_MoveState` 0x80169608 (6 while travelling, 0x18 on a refused exit). `gCampaignMap_aLocationTable` 0x801691A8: 16 x 0x42 bytes = {u16 lock flag, camera x/y/z/pitch/yaw (5 x s16), marker x/y, u8 refuse-message flag, u8, then 4 exits x 12 bytes: u16 story flag (0 = always; bit 0x8000 = must be CLEAR), s16 x, s16 y, u16 DPAD mask (0x1000 UP 0x2000 RIGHT 0x4000 DOWN 0x8000 LEFT), u8, u8 destination (16 = none), u8 move type, u8}. `CampaignMap_PickExit` (0x80168E0C) walks the 4 exits: skip dest 16, skip if the flag test fails, take the first whose mask & pad-edge word is set; X on a site runs Campaign_TestStoryFlag(71) gating. Pharaoh's Palace: DOWN -> Duel Ground (always), RIGHT -> Shrine unless flag 71, RIGHT -> Hiding if flag 90. | (config/modules/overworld.txt) | APPLIED |
| F120 | `0x8002CCA8` `Campaign_TestStoryFlag` / `gCampaign_abStoryFlags` 0x801D0618 | Matched C: bit (0x80 >> (id & 7)) of byte (id & 0x7FF) >> 3 in the save block at 0x801D0618; bit 0x8000 of the id inverts the result. The overworld exits, the campaign scripts and the module gates all go through it. | `Campaign_TestStoryFlag` / `gCampaign_abStoryFlags` | APPLIED |
| F121 | `0x8016824C` (free_duel module) | **Screen init.** Called once per entry by `Main_InitFreeDuelMenu` with a0 = arena[0] (0x80100000, F123). Bit 0x80 of `D_8009B365` -> bump `duelistRecords[row*5+col]` (post-duel return). Zeroes the 16-slot sparkle pool, sets `gFreeDuel_abGridAvailable[0..39]` = 1 then clears ids 1..38 whose met-flag (`Campaign_TestStoryFlag(0x6E0+id)`) is off. Uploads 40 portrait tiles from the buffer, 2432 B each = 48x48 8bpp (2304) + 64-entry CLUT (128): tiles 0..24 to VRAM page 18 (x=128+24*col, y=256+48*row), 25..39 to page 20 (x=256+24*col), CLUTs at y=496+(i&15), x=128+64*(i>>4). Spawns one sprite per available cell (two loops, 25 then 15) at (col*56+20,row*52+40) with u=(i%5)*48, v=(i/5)*48. Then cursor widget, scrollbar, `FreeDuel_PlaceCursor`, `FreeDuel_UpdateScrollbar`, SE 0x72C0. Verified by the recomp's nine-row mod: every loop bound and the six availability-table sites patched, live. | `FreeDuel_Init` | APPLIED |
| F122 | `0x8003B9BC` | The one-shot init `Main_RunFreeDuelMenu` runs when bit 0x40 of the mode byte is clear: `func_80014E1C(0, 0, 0x1E88, 0x57, func_8003B808, 0, 0)` (a CD request: 0x57 sectors, callback), `func_800137E4()`, then `FreeDuel_Init(*(u32 *)0x80010000)` — the portrait buffer comes from the arena pointer table, not a literal. | `Main_InitFreeDuelMenu` | APPLIED |
| F123 | `0x80010000` (8 words) | Load-arena pointer table: 0x80100000 0x80140000 0x80180000 0x8013A000 0x8017A000 0x8013B000 0x8017B000 0x80135000. [0] is where the free-duel portrait file (40 x 2432 B) and the library's 276-sector card block land (block sectors 0..95 -> 0x80100000, 180..199 -> 0x8013B000, 224.. -> 0x801A8000). Proposed `gMain_apLoadArena`; medium until the other seven bases are seen in use. | (proposed `gMain_apLoadArena`) | HOLD |
| F124 | `FreeDuel_UpdateScreen` constants | Row clamp `slti v0,v0,8` / `li v0,7` at 0x80168DDC / 0x80168DE8, column clamp `slti 5` / `li 4` at 0x80168D60 / 0x80168D6C. `FreeDuel_UpdateScrollbar`'s divide is `lui 0xB40B; ori 0x40B5` (0x80168038 / 0x80168050) = /364 = 7 rows x 52 px. `FreeDuel_PlaceCursor` sets the name string id to 0x8328 + row*5+col at 0x80168134 (string 0x8329 = Simon). Confirm passes row*5+col as the opponent id to `func_80024DC8(-1, id, 0x6000, 0x6000)` at 0x80168F84. The availability table is read through six lui/addiu pairs (0x80168118, 0x801683A4, 0x801683C4, 0x80168588, 0x801686A0, 0x80168E68) and is followed at +40 by `gFreeDuel_pThumbWidget`, so a longer table must move. | (facts) | CONFIRMED |
| F125 | card 2D art record on disc | Per card, 7 sectors at LBA 10817 + 7*id (12384 B used): +0 art 102x96 8bpp (102-byte rows), +9792 a 256-entry CLUT, +10304 the baked TITLE 96x14 4bpp, +10976 a 16x88 8bpp strip. Read by CD DMA straight into `gLibrary_aCardArtRecord` (0x801DC000; writer PC 0x8007E49C inside `CD_getsector`, seek issued from 0x80079DEC), then `LoadImage` (0x8007F978) uploads rects (256,256,51,96), (512,240,256,1), (256,352,24,14) and (312,256,8,88). The page draws the art as an 8bpp sprite off page (256,256) through CLUT (512,240) and the title as 4bpp through the 8-grey ramp CLUT at (480,248) (index 1 lightest .. 7 darkest). The chest's TRIANGLE viewer reads only this record; the library page additionally streams the 276-sector model block at LBA 28534 + 276*(id-1) (F45). The recomp's card-skin mod swaps all three at the LoadImage entry. | `gLibrary_aCardArtRecord` | APPLIED |
| F126 | `0x8009B246` (u16) | The card id the chest / library card viewer is showing: 50 while TRIANGLE-viewing card 50, 5 for Ryu-kishin; written by the two chest-side `sh s0,B246` sites the recomp clamps for clone ids (psx_card_chest.c). | `gLibrary_wViewerCardID` | APPLIED |
| F127 | card titles | The 722 baked titles were rasterised from a scalable serif face, not a bitmap font: the same letter advances 5 or 6 px and its pixels differ by sub-pixel phase across names (e.g. two distinct `e` and `o` bitmaps at the same advance), and names longer than ~16 characters are squeezed horizontally to <= 93 px (ink width saturates at 90..93 from length 17 on). Short names reuse pixel-identical first glyphs (all 23 short `B...` names share one). Consequence: an exact title renderer must model quarter-pixel pens and the squeeze, or reproduce the original tool. | (facts) | CONFIRMED-OBSERVED |
| F128 | `D_8009B26C` mode byte values | 0xC2 campaign cutscene, 0xC3 duel, 0xC4 library, 0xC5 campaign map (F117), 0xC6 free-duel grid, 0xC7 build deck / chest (also with TRIANGLE viewer open); bit 0x40 = one-shot init done. While a library page is open `D_801D5608` (u16) also holds the page's card id (it doubles as the free-duel W/L copy target, F80). Library grid cursor is `gCardGrid_bCursorColumn/Row` (0x8009B258/59). | (facts) | CONFIRMED-OBSERVED |
