# Grouped Translation Units

## Contract

`config/slus_01411/matching_c.json` continues to record one row per matched
function, but multiple rows may now share one C source when all of these
conditions hold:

1. The functions are contiguous in executable address order.
2. Every member uses the same compiler profile.
3. The source defines every member in that same order.
4. The sum of the member sizes exactly covers the C subsegment.
5. A clean full build remains byte-identical to `game/SLUS_014.11`.

`make check-notes` verifies the index below against
`config/slus_01411/matching_c.json`: every row's source has to exist and be a
matching_c source, its profile has to be one the build actually uses for that
source, and every address the row names has to belong to it. The table drifts
silently otherwise, because nothing compiles a note. Two rows had already
survived the translation units they described being merged away.

`tools/project/generate_build_config.py` enforces the first four conditions and
emits one `text_sources.json` object with a `members` list. The full executable
match enforces the fifth.

Single-function refinement integration refuses to replace one member of a
grouped source. Semantic renaming may update symbols inside a grouped source,
but it does not automatically rename a source file owned by multiple
functions.

Historical external-candidate hashes remain provenance for the source tested
at integration time. The repository audit checks that the current pure-C
translation unit defines the expected symbol with the accepted profile; the
full executable hash remains the authority after later semantic renames or
source grouping.

## Initial groups

| Translation unit | Profile | Members |
|---|---|---|
| `src/game/main_frame.c` | `gcc_2_8_1_g8` | `Main_VBlankCB` (`0x80012CD4`), the contiguous four-stage frame update pump (`0x80012D4C`), and its count-controlled repeat wrapper (`0x80012D84`) |
| `src/game/duel_card_checks.c` | `gcc_2_8_1_g0_split` | `Duel_CheckEquip` (`0x80019A08`), `Duel_CheckFusion` (`0x80019A60`) |
| `src/game/duel_state_init.c` | `gcc_2_8_1_g8_split` | The duel reset phase, four contiguous functions starting at `0x800175A0` and spanning 0x224 bytes: duel-side and life-point initialization (`func_800175A0`, `0x800175A0`), `Duel_ClearHandSlots` (`0x800176D0`) clearing five hand-state entries, `func_80017708` (`0x80017708`) resetting the four selection records of each side, and `func_8001778C` (`0x8001778C`) clearing three fields of every card record. `func_800179F4.c` calls all four, the first three consecutively. Bounded below by `src/game/func_8001755C.c` at `gcc_2_8_1_g8` and above by `src/game/func_800177C4.c` at `gcc_2_8_1_g8_split_no_strength_reduce` |
| `src/game/func_80017DB4.c` | `gcc_2_8_1_g8_split` | 3 contiguous functions: `0x80017DB4`, `0x80017E3C`, `0x80017F04`. Split out of `src/game/func_800179F4.c` after #3859 moved `func_800179F4`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/duel_card_object_helpers.c` | `gcc_2_8_1_g8` | Screen-space duel display-object constructor (`0x80018150`) and contiguous card-category encoder (`0x800181EC`), mapping magic/equip, trap, and ritual types to `2`, `3`, and `4` with optional flag `0x80` |
| `src/game/duel_phase_entry.c` | `gcc_2_8_1_g8_split` | Three contiguous entries from the `D_80090998` duel-phase callback table: resume/replay reconstruction (`0x8001825C`), initial combined-deck and selection setup (`0x80018608`), and draw-phase hand reconstruction (`0x8001898C`). All three use `D_8009B23A` as the first-call latch, rebuild card/side state, and coordinate fade or file-transfer gates. The complete run is bounded by `gcc_2_8_1_g8` functions on both sides |
| `src/game/duel_draw_resolution.c` | `gcc_2_8_1_g8_split` | Five-piece Exodia hand predicate (`0x80018CF8`) and the contiguous draw-animation state machine (`0x80018DB4`) that invokes it before resolving victory |
| `src/game/display_object_property_transitions.c` | `gcc_2_8_1_g8` | Three-channel byte convergence to per-channel targets (`0x8001D344`) and the contiguous timed position/interpolation transition with clip-flag lifecycle (`0x8001D3C4`) |
| `src/game/display_object_motion.c` | `gcc_2_8_1_g8` | Timed position interpolation with speed-to-`0x800` completion (`0x8001EC70`) and the contiguous mode-progress variant that stores the final position and clears the clip flag when appropriate (`0x8001ED20`) |
| `src/game/display_effect_resource_setup.c` | `gcc_2_8_1_g8_split` | Four contiguous display-effect resource functions from `func_8003A198` (`0x8003A198`) through `func_8003A560` (`0x8003A560`): the three-level resource-table probe, the up-to-three-object builder that calls it, child layer/attribute setup, and the VRAM page swap state callback that calls the builder after loading or restoring a page. The first and third functions are byte-identical under all four standard g0/g8 profiles; the builder is byte-identical at g0_split and g8_split; the final state callback requires g8_split and fixes the unit. The unrelated file-transfer callback `func_8003A01C` bounds it below, and the g0 display-effect position/update family bounds it above |
| `src/game/display_effect_update_callbacks.c` | `gcc_2_8_1_g0` | Seven contiguous display-effect position and update functions from `func_8003A920` (`0x8003A920`) through `func_8003B054` (`0x8003B054`): three-child position propagation and its group setter, cosine position easing, paired fade-in/fade-out resource transitions, the four-row transition, and the portrait transition. Every callback operates on the same `MenuRecord`/`DisplayEffectState` storage, uses the `func_80039F1C` first-frame latch, and either creates, positions, fades, or releases the record's display-object groups. The first three functions are byte-identical at g0, g0_split, g8, and g8_split; the four trailing callbacks require g0 and fix the common profile. The unit retains the halfword alias for `D_8009B0D8` used by `func_8003B054`; the g8_split resource callback below and g0_split frame runtime above fix both boundaries |
| `src/game/display_effect_process_menu_records.c` | `gcc_2_8_1_g0_split` | Per-record animation helper `func_8003B378` (`0x8003B378`) and its contiguous sole frame dispatcher `DisplayEffect_ProcessMenuRecords` (`0x8003B50C`). The dispatcher walks the three live `D_800EB010` records, calls the helper for flag-0x40 records, then dispatches `D_80090F68`; the helper's object is byte-identical under all four standard g0/g8 profiles and the dispatcher is byte-identical under the two split profiles. The following `func_8003B5C8` is an unrelated Shift-JIS decimal-key lookup, fixing the semantic upper boundary |
| `src/game/duel_draw_status_numbers.c` | `gcc_2_8_1_g8_split` | 2 contiguous functions: `0x80016D2C`, `Duel_UpdateLifePointDisplay` (`0x80016DDC`). What remains after #3859 moved `Duel_DrawLifePointsAndDeckCounts`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/duel_battle_stats.c` | `gcc_2_8_1_g8_split` | Battle arithmetic between two duel cards: `Duel_CalcGuardianStarBonus` (`0x8001EE44`), the attack and defence calculators `Duel_CalcBattleAttack` (`0x8001EF1C`) and `Duel_CalcBattleDefense` (`0x8001EF78`) that add that bonus, and the AI's battle comparison `func_8001EFD4` (`0x8001EFD4`) that calls them. The two calculators were recorded at `gcc_2_8_1_g8` but compile to identical objects at `gcc_2_8_1_g8_split`. Bounded below by `display_object_motion.c` and above by the attack-trap selector in `duel_trap_resolution.c` |
| `src/game/duel_trap_resolution.c` | `gcc_2_8_1_g8_split` | Contiguous attack-trap selector (`0x8001F0D0`) and its presentation state machine (`0x8001F364`), linked through the selected card-object index in `D_8009B1B8` |
| `src/game/duel_card_turn_animations.c` | `gcc_2_8_1_g8` | Mirrored three-mode card turn-back callback (`0x80022674`, sets record flag `0x400`) and contiguous flip/turn callback (`0x800229F4`, clears `0x400`), both using side-dependent two-phase rotations |
| `src/game/duel_field_display_objects.c` | `gcc_2_8_1_g8_split` | 2 contiguous functions: `0x8002348C`, `0x800234E4`. What remains after #3859 moved `func_80023090` and `func_80023144`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/duel_cursor_status.c` | `gcc_2_8_1_g8` | Status-returning cursor wrappers: held-pad directional update (`0x80024060`) through `func_80023FBC`, followed by the explicit-direction `func_80023D08` path (`0x80024088`); both return object byte `+0x19` |
| `src/game/duel_card_record_lifecycle.c` | `gcc_2_8_1_g8_split` | Seven contiguous combined-deck/card-record functions from `Duel_PopulateCombinedDeckData` (`0x80024824`) through `func_80024D34` (`0x80024D34`). The producer builds all eighty `gDuel_aDeckCardRecords` entries and copies their selected `D_8018C2D8` image blocks; `Duel_SetupCardRecord` later consumes those exact records/blocks while filling one live card record and uploading its art/name strip. Between them sit record release/full reset and terrain boost; the final two functions build and attach the card-type icon. Every member compiles exactly at g8_split. The demoted `Duel_RequestCombinedDeckData` candidate fixes the lower boundary, while `func_80024DC8` above requires non-split g0 |
| `src/game/duel_action_lock.c` | `gcc_2_8_1_g8` | `DuelEffect_MarkInitialized` (`0x80024E24`), which sets bit `0x80` and distinguishes first entry (`0`) from an already-active update (`1`), followed by the contiguous full-state reset (`0x80024E4C`) |
| `src/game/duel_magic_effect_dispatch.c` | `gcc_2_8_1_g8_split` | Three contiguous magic-effect functions: the two-stage occupied-row clear handler (`0x80026A3C`), the effect-group dispatcher (`0x80026B34`), and the card-ID activation helper (`0x80026BA4`) that writes the exact effect ID, original card ID, and active/second-handler flags the dispatcher consumes. The activation helper is byte-identical under its historical `gcc_2_8_1_g8` profile and this unit's split-address profile; the following free-field-slot search starts a separate AI/field-selection subsystem |
| `src/game/duel_projection_axes.c` | `gcc_2_8_1_g0` | Symmetric three-point projection helpers for the X components (`0x80029684`) and Z components (`0x800297DC`) of `SVECTOR` triplets; both retry `RotAverage3` at half span when the depth/control result is negative |
| `src/game/library_runtime.c` | `gcc_2_8_1_g8_split` | 2 contiguous functions: `0x8002BAA0`, `0x8002BAAC`. What remains after #3859 moved `func_8002BAB4`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/func_8002BD0C.c` | `gcc_2_8_1_g8_split` | 3 contiguous functions: `0x8002BD0C`, `Library_MarkOwnedCards` (`0x8002BF3C`), `0x8002BFCC`. Split out of `src/game/library_runtime.c` after #3859 moved `func_8002BAB4`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/script_stream_commands.c` | `gcc_2_8_1_g8` | `Script_OpSound` (`0x8002EC74`), a contiguous one-byte sound-control dispatcher (`0x8002EDB0`), and a 16-bit base-relative script-cursor jump (`0x8002EE20`) |
| `src/game/script_readers.c` | `gcc_2_8_1_g8` | Contiguous script operand readers for one byte (`0x80030050`) and one little-endian 16-bit value (`0x8003006C`), advancing `gScript_pStream` by one or two bytes respectively |
| `src/game/ai_fusion.c` | `gcc_2_8_1_g0_split` | `AiScript_FindEquipTarget` (`0x8007249C`) through `Ai_CompleteFusion` (`0x800727C0`), followed by the two fusion-search opcodes that seed the fusion scratch and recurse through it, `AiScript_FindBestCombo` (`0x80072A48`) and `AiScript_EvaluateFusion` (`0x80072DC0`). Those two were recorded at `gcc_2_8_1_cc_g0_as_g8_split` and `gcc_2_8_1_g8_split`, and each compiles to an identical object at `gcc_2_8_1_g0_split`. Bounded above by the skip opcodes |
| `src/game/ai_script_vm.c` | `gcc_2_8_1_g0_split` | `AiScript_Init` (`0x800705D8`), `AiScript_Run` (`0x80070650`) |
| `src/game/ai_card_ranges.c` | `gcc_2_8_1_g0_split` | Four contiguous AI card-selection helpers: winning-card (`0x80070738`) and general card (`0x800707C4`) range decoders with their compiler-owned jump tables, followed by card-ID (`0x80070870`) and card-type (`0x800708C4`) set predicates |
| `src/game/ai_script_card_info.c` | `gcc_2_8_1_g0_split` | `AiScript_TestHighStat` (`0x80071194`) through `AiScript_LoadCardID` (`0x800712B4`) |
| `src/game/ai_script_control_flow.c` | `gcc_2_8_1_g0_split` | Twelve contiguous AI VM primitive operations from combined set-query helper `Ai_IsCardInSets` (`0x80070920`) through arithmetic opcode `AiScript_Subtract` (`0x80070EB4`): the direct jump, six conditional jumps, call/return stack pair, random assignment, and three-register subtraction all share the script readers and `gAiScript_State`/`gAiScript_aMemory` contracts. The former support and subtract objects recorded at g0 are byte-identical at g0_split, while the nine-function control object is byte-identical at g0_split and g8_split. `ai_card_ranges.c` below remains the reusable range/set predicate utility used by later search and fusion code; `AiScript_PlayFieldCard` above begins the action-opcode unit |
| `src/game/ai_script_end.c` | `gcc_2_8_1_g0` | `AiScript_EndHand` (`0x80070FF8`), `AiScript_EndField` (`0x80071000`) |
| `src/game/ai_script_skip.c` | `gcc_2_8_1_g0` | `AiScript_SkipHand` (`0x80072F1C`), `AiScript_SkipField` (`0x80072F54`) |
| `src/game/ai_script_combo.c` | `gcc_2_8_1_g0_split` | `AiScript_TestPinned` (`0x8007154C`), `AiScript_StartCombo` (`0x800715C4`) |
| `src/game/ai_script_power_search.c` | `gcc_2_8_1_g0_split` | `AiScript_FindStrongest` (`0x80071700`) and `AiScript_FindWeakest` (`0x80071924`), sharing the same active-card view and operand contract while retaining their separate ranking loops, initial bounds, and tie behavior |
| `src/game/ai_script_find_card.c` | `gcc_2_8_1_g0_split` | Two contiguous AI script opcodes that scan a zone for its first match: state-based `AiScript_FindFirstCard` (`0x80071FC8`) and card-ID-based `AiScript_FindCard` (`0x8007214C`). Both read operands with `AiScript_ReadByte`, scan `gDuel_aActiveCards` and answer into `gAiScript_aMemory`. The unit also held `AiScript_FindDefenseStopper`, now `ai_script_find_defense_stopper.c`, until #3859 moved the `AiScript_CountCards` between them to a candidate |
| `src/game/ai_script_actions.c` | `gcc_2_8_1_g0_split` | `AiScript_PlayFaceUp` (`0x80072F8C`), `AiScript_SetPosition` (`0x80073050`) |
| `src/game/ai_script_state_ops.c` | `gcc_2_8_1_g0_split` | The AI script VM's state opcodes, eighteen contiguous functions from `AiScript_PushComboCard` (`0x80073220`) through `func_8007368C` (`0x8007368C`): the combo-stack pushes, the hand and field no-ops, the card and type set edits, the four state-flag handlers at `0x80073448`-`0x80073474` and `AiScript_MoveCard`, `AiScript_LoadOpponentData`, the register store, copy and add, and a flag load. The eleven former sources were recorded at five profiles, and every member compiles to an identical object at `gcc_2_8_1_g0_split`. Bounded below by `AiScript_FindBestAttack` (`gcc_2_8_1_g8_split_no_strength_reduce`) and above by `AiScript_Print` (`gcc_2_8_1_g8`) |
| `src/game/graphics_frame.c` | `gcc_2_8_1_g8_split_comm` | Contiguous frame-boundary synchronization (`Graphics_SyncFrame`, `0x80012DB4`) and next-buffer/ordering-table setup (`Graphics_BeginFrame`, `0x80012E5C`) used in sequence by the main frame pump |
| `src/game/file_stream.c` | `gcc_2_8_1_g8` | File-state initialization (`0x80013898`), `File_GetPosition`, and three transfer setup helpers through `0x80013A94` |
| `src/game/file_transfer_setup.c` | `gcc_2_8_1_g8_split` | Two contiguous transfer-descriptor setup helpers at `0x80013B04` and `0x80013B68` |
| `src/game/file_transfer_runtime.c` | `gcc_2_8_1_g8_split` | 3 contiguous functions: `0x800140A0`, `0x80014134`, `0x800141A8`. What remains after #3859 moved `func_80014220`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/func_80014294.c` | `gcc_2_8_1_g8_split` | 9 contiguous functions: `0x80014294`, `0x80014308`, `0x80014390`, `File_ActivateTransfer` (`0x800143DC`), `0x800144B8`, `0x8001455C`, `0x80014A5C`, `0x80014B30`, `0x80014C40`. Split out of `src/game/file_transfer_runtime.c` after #3859 moved `func_80014220`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/file_transfer_flags.c` | `gcc_2_8_1_g8` | Seven contiguous helpers over one `FileTransferDescriptor`: the active/secondary transfer-request and cancellation helpers from `0x80014E1C` through `0x80015078`, and the phase stepper `func_8001513C` (`0x8001513C`) that counts a descriptor's phase down and fires its `phase_callback`. The run this sits in is bounded by a profile change on both sides, `file_transfer_runtime.c` (`gcc_2_8_1_g8_split`) below and `Fade_StepBands` above (assembly since #3859; it was `fade_step_bands.c` at `gcc_2_8_1_cc_g8_as_g0_split`), but it is not taken whole: the fade-state reset `func_800151B0` sits between this unit and that upper bound and resets `gFade_State`, so it is left in its own source. It cannot join the fade units either, because they use a different profile |
| `src/game/fade_runtime.c` | `gcc_2_8_1_g8_split` | The complete thirty-function fade runtime from `Fade_DrawOverlay` (`0x800154E4`) through the overlay-latch clear `func_80015D0C` (`0x80015D0C`): frame update and box submission, band fill, white-mode resets, uniform/banded/colour fade-in and fade-out setup, the blocking frame-pump wait, every blocking and nonblocking flag-`0x02`/`0x06` wrapper, direct level/target setters, and both overlay-latch toggles. All members share `gFade_State`, the `D_8009B14x` colour controls, and direct setup/wait call chains; the former overlay and wrapper objects already used `gcc_2_8_1_g8_split`, and the clean merged executable proves translation-unit scope preserves every member. Bounded below by `Fade_Update` (`gcc_2_8_1_cc_g8_as_g1_split_comm`), which is pending reclassification in #3878, and above by `display_projection.c`, whose edge members are pending reclassification in #3904 |
| `src/game/movie_frame_pipeline.c` | `gcc_2_8_1_g8` | 3 contiguous functions: `0x8005BB7C`, `0x8005BE3C`, `0x8005BFC8`. What remains after #3859 moved `func_8005C1F4`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/file_cd_helpers.c` | `gcc_2_8_1_g0` | 2 contiguous functions: `0x8005C530`, `0x8005C568`. What remains after #3859 moved `File_Exists`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/file_query_wrappers.c` | `gcc_2_8_1_g0` | Host-file access through the SN fileserver, three contiguous functions spanning `0x170` bytes: the size query `func_8005988C` (`0x8005988C`), the whole-file read `func_800598E4` (`0x800598E4`), and the chunked reader `func_80059908` (`0x80059908`) that the second one is a single call to. `func_800598E4` is the only function in the executable that calls the reader -- one `jal` site in a complete disassembly -- and no overlay C source names it. The reader was recorded at `gcc_2_8_1_g8` and compiles to an identical object at this unit's `gcc_2_8_1_g0`. The unit was named for the two wrappers and now also holds the primitive they wrap; the name stays because it is the existing one and no other header covers the three. Below `0x8005988C` the preceding unit is identical only under split profiles and holds pinned register variables, so that boundary is the profile; above the span, `func_800599FC.c` is a `GsIMAGE` upload wrapper over `model_texture_upload.h` and is also identical at `gcc_2_8_1_g0`, so that one is meaning rather than profile |
| `src/game/mdec_sync.c` | `gcc_2_8_1_g8` | MDEC completion-latch setter (`0x8005C5C4`) and contiguous bounded wait/reset helper (`0x8005C5D4`) |
| `src/game/view_state_orbit.c` | `gcc_2_8_1_g8_split` | Placing the `D_800F2848` view: `func_800134E0` (`0x800134E0`) sets the viewpoint at an offset from the reference point and installs the `GsRVIEW2`, and the two orbit computations after it, `func_8001352C` (`0x8001352C`) and `func_800135FC` (`0x800135FC`), derive that offset with `rsin`/`rcos` and call it. Recorded at `gcc_2_8_1_g8`, `gcc_2_8_1_g0_split` and `gcc_2_8_1_g8_split`; each compiles to an identical object at `gcc_2_8_1_g8_split`. Bounded below by `main_services.c` and above by the empty `func_800136D4` |
| `src/game/debug_effect_screen.c` | `gcc_2_8_1_g8_split` | The developer effect-preview screen, in call order: the pad-driven camera and viewport nudge (`0x800220B8`), the controller that builds one of four preview pages and spawns an effect on CROSS (`0x800222F4`), and the HUD line that prints the tuned pair (`0x80022618`). Each calls the one before it, and the three are the whole `gcc_2_8_1_g8_split` run in that region |
| `src/game/ai_turn_action.c` | `gcc_2_8_1_g8_split` | The five-function AI action and target-selection run from `0x8002712C` through `0x800278A0`: spell and fusion searches feed the turn-action pick through the pending selection at `D_800EAE88`, then the two active-side monster-row selectors rank all occupied or only face-up candidates for the immediately following AI routine. The whole run shares duel-card, grid, side, and display-object state; the selection header preserves the two private caller views |
| `src/game/card_list_sort.c` | `gcc_2_8_1_g8_split` | `BuildDeck_CompareCard` (`0x80032B60`) and its reverse-primary comparator (`0x80032BD4`), followed by the card-list sort `func_80032C48` (`0x80032C48`) that builds each row's key and hands the list to `qsort` with one of them. The comparators were recorded at `gcc_2_8_1_g0_split` and compile to an identical object at `gcc_2_8_1_g8_split`. Bounded below by the transition latch `func_80032B38` and above by `func_800330BC` (`gcc_2_8_1_cc_g8_as_g0_split`) |
| `src/game/func_800339D0.c` | `gcc_2_8_1_g8_split` | Build Deck transition exit step (`0x800339D0`) followed by its per-frame colour/step driver (`0x80033BE8`). The driver pulses the two pane objects, then dispatches `D_80090DF8[state & 0x3F]`; the table contains the preceding function as its exit handler. Both use `BuildDeckTransitionState`, and `gcc_2_8_1_g8` functions bound the pair on both sides |
| `src/game/text_box_lifecycle.c` | `gcc_2_8_1_g0` | `TextBox_Destroy` (`0x80035B7C`), `TextBox_Create` (`0x80035BE4`), and contiguous flagged creator `TextBox_CreateFlagged` (`0x80035C38`) |
| `src/game/duel_effect_entry_occupancy.c` | `gcc_2_8_1_g8_split` | Five entry-allocation and marker helpers from `0x80035CA8` through `DuelEffect_ResetEntryMarkers` (`0x80035DF4`) |
| `src/game/func_80036C14.c` | `gcc_2_8_1_g8_split` | Big-endian lookup over the 30-byte records at `D_801D9174` (`0x80036BCC`) followed by the contiguous tagged 28-byte packet append at `0x80036C14`. Both access adjacent text-rendering data symbols; the address-based names remain because that supports the shared subsystem and placement, not stronger semantics |
| `src/game/duel_effect_object_pool.c` | `gcc_2_8_1_g8_split` | External duel-object readiness check (`0x8002C570`) followed by the contiguous eight-entry effect-pool reset and free-entry search helpers through `0x8002C5CC` |
| `src/game/duel_effect_entry_control.c` | `gcc_2_8_1_g8_split` | `DuelEffect_HasActiveEntry` (`0x8003735C`), the following entry marker writer (`0x800373C8`), and contiguous entry-wait reset callback (`0x8003741C`) |
| `src/game/duel_effect_state_callbacks.c` | `gcc_2_8_1_g8` | Sixteen contiguous choice/text-box state functions from `func_800374A8` (`0x800374A8`) through the transfer-wait callback `func_80037B40` (`0x80037B40`): fifteen entries of `D_80090E64` plus `Dialog_OpenChoice`, which the first interaction callback calls and stores on the same `DuelEffectChannel`. The callbacks all dispatch through and update state byte `0x51`; the middle interaction states additionally share the choice object, script-state gate `D_8009B357`, and operand globals, while the final wait callbacks share the display-effect record and completion convention. The former two-function choice object is byte-identical under `gcc_2_8_1_g0` and `gcc_2_8_1_g8`, and `func_8003787C` is byte-identical under `gcc_2_8_1_g8_split` and `gcc_2_8_1_g8`. The lower `func_8003741C` callback remains separate because its text changes size under `gcc_2_8_1_g8`; the upper `func_80037C74` uses `gcc_2_8_1_g0` |
| `src/game/duel_effect_command.c` | `gcc_2_8_1_g8_split` | 6 contiguous functions: `0x80038024`, `0x80038070`, `0x80038094`, `0x800380D4`, `0x80038110`, `0x80038148`. What remains after #3859 moved `func_80037DA4`, `func_800382A8` and `func_80038334`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/func_80038388.c` | `gcc_2_8_1_g8_split` | 5 contiguous functions: `0x80038388`, `0x800383B0`, `0x800383DC`, `0x80038498`, `0x800384E4`. Split out of `src/game/duel_effect_command.c` after #3859 moved `func_80037DA4`, `func_800382A8` and `func_80038334`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/text_control_commands.c` | `gcc_2_8_1_g8_split` | Nine of the text stream's eleven primary control-byte handlers, contiguous from `Text_DispatchSecondaryCommand` (`0x80038B4C`, F8) through `Text_HandleDisplayEffectCommand` (`0x80038EB0`, F6): the cursor set, choice command, page wait, campaign-flag command, stream push, new line and end of stream between them. The seven former sources were recorded at four profiles, and every member compiles to an identical object at `gcc_2_8_1_g8_split`. Bounded below by the secondary-table object commands in `duel_effect_object_commands.c` and above by the text-box layout helpers `func_80039140` and `func_800391E4` |
| `src/game/text_box_runtime.c` | `gcc_2_8_1_g8` | `TextBox_SetPos` (`0x80039934`) followed by two contiguous blocking text-box build-to-completion helpers at `0x80039A14` and `0x80039A60` |
| `src/game/duel_card_effects.c` | `gcc_2_8_1_g8_split` | Nine contiguous field/card-effect functions from `0x800250C8` through `func_80025D30` (`0x80025D30`): eight adjacent second handlers in `gDuelEffect_apfnGroupHandler` plus the `func_80025B28` display callback installed by `func_80025BEC`. The first five cover LP recovery/damage, monster removal, field-marker movement and field-card completion; `func_8002596C`, `func_80025BEC` and `func_80025D30` sweep the acting-side grid through the same `D_8009B220`, `D_8009B20C`, `D_8009B260`, `D_8009B17C`, `D_800907D8` and `D_801A7AD8` state. The unit owns the LP amount tables in `.sdata` and the shared `DuelFieldEffectObject` contract. Private 2D and flat same-symbol grid views preserve the transition handlers' original addressing while the earlier handlers keep the canonical flat declaration. The preceding `func_80025028` uses `gcc_2_8_1_g8_split_comm`; the following `func_80025EE0` uses `gcc_2_8_1_g8` |
| `src/game/duel_field_equip_search.c` | `gcc_2_8_1_g8_split` | 2 contiguous functions: `Duel_CollectFieldCardsByType` (`0x80026D18`), `0x80026DC8`. What remains after #3859 moved `func_80026C0C` and `Duel_CollectFieldCardsBelowType`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/util_memory.c` | `gcc_2_8_1_g8` | `Util_CopyWords` (`0x800356A0`) and contiguous repeated-byte fill counterpart `Util_FillMemory` (`0x80035748`) |
| `src/game/display_object_brightness.c` | `gcc_2_8_1_g0_split` | Paired display-object RGB setters at `0x80030090` and `0x800300AC`, writing uniform brightness values `0x40` and `0x80` |
| `src/game/display_object_core.c` | `gcc_2_8_1_g8_split` | The complete 15-function display-object core from free-slot search `func_8004002C` (`0x8004002C`) through primary list renderer `func_80040588` (`0x80040588`). Eight pool/list lifecycle functions allocate, link, unlink, release, and reset `D_800EFE48` and its list heads; six configuration helpers then write the selector, colour, texture, position, extent, and transform fields of those objects; the renderer consumes those exact fields while walking the list rooted at `D_800EFE3A`. The six former g8 configuration functions are byte-identical under all four standard g0/g8 profiles, so the entire run takes g8_split with the lifecycle and renderer. `SD_StopAll` below is unrelated and uses g8; `func_80040814` above is a different list renderer whose object is byte-exact only at g0_split |
| `src/game/display_object_updates.c` | `gcc_2_8_1_g8_split` | The sprite-strip submitter (`0x800408D0`) and the three display-list walkers from `0x80040BF8` through `0x80040D14`, spanning 0x508 bytes. Each walker runs every object's update callback and differs in which list it takes and what it does afterwards; the list-3 walker is the submitter's only caller, reaching it for every renderable object with the object's ordering-table entry and the halfword at `+0x14`. The submitter was recorded alone at `gcc_2_8_1_g8_split` and the three walkers at `gcc_2_8_1_g0_split`; all four are byte-identical at the split `-G8` profile, which the merged unit takes. The preceding object-list update entry is identical only under `gcc_2_8_1_g0_split`, so it is not reached by this unit's profile |
| `src/game/display_object_runtime.c` | `gcc_2_8_1_g8_split` | The complete eight-function matching display-object runtime from frame renderer `func_80041340` (`0x80041340`) through stream attribute command `func_80041534` (`0x80041534`). The renderer installs the two flat lights, resets geometry, and dispatches each live object list through the seven-entry `D_80090FB0`; the following seven functions are entries 0 through 6 of the adjacent `D_80090FEC` stream-opcode table, covering counter resets, a constant-success handler, base-relative jumps, the flag-`0x800000` toggle, random jump, and four-operand attribute command. Every member builds at g8_split; the five compact handlers originally recorded at g8 are byte-identical there. Unmatched display-object code bounds the run on both sides: the preceding list-renderer block ends where this unit begins, and func_8004158C begins immediately above |
| `src/game/display_object_helpers.c` | `gcc_2_8_1_g8_split` | Twenty-one contiguous generic display-object utility and lifecycle functions from `0x800427DC` through `func_80042BC0` (`0x80042BC0`): initialization/resource/animation helpers, stream-offset decoding, 8.8 fixed-point velocity and scalar convergence, the pool-wide tag query `func_80042B40`, the one-shot allocation latch, and the brightness-step callback that releases an object at zero. The first eighteen functions and final two-function lifecycle object are each byte-identical under `gcc_2_8_1_g8` and `gcc_2_8_1_g8_split`; the central pool query requires the split profile. All operate on the same `DisplayObject` arena and public helper/API/lifecycle contracts. Unmatched assembly gaps bound the unit on both sides |
| `src/game/display_object_interpolation.c` | `gcc_2_8_1_g0` | Cosine midpoint interpolation (`0x8004318C`) and signed-phase sine interpolation toward a target position (`0x80043230`) |
| `src/game/display_object_transition.c` | `gcc_2_8_1_g0` | Blocking clone/brightness transition (`0x8004365C`) and the contiguous wait helper that permits Start/confirm skip only after the transfer state is idle (`0x800438B8`) |
| `src/game/save_data_transfer_runtime.c` | `gcc_2_8_1_g8_split` | Seven contiguous single-player and two-player save-transfer functions from `SaveData_RequestLoad` (`0x8003F7D4`) through `SaveData_RequestTradeWrite` (`0x8003FE14`). The first three request, poll/apply, build and write the resident save through the memory-card dialog API; `SaveData_UpdateLoadPair` then loads both card slots and compares their duelist codes, the trade and duel wrappers add their setup/validation, and the final request stamps and writes paired trade results through dialog step 4. `SaveData_RequestLoad` is byte-identical at g0_split/g8_split, `SaveData_PollLoad` at g0/g0_split/g8_split, and `SaveData_RequestWrite` has identical text and relocations at g8/g8_split. The unit owns the load-pair jump table at rodata offset `0xCF0`; the four-byte alignment region at `0xCEC` separates it from the dialog object, while the following mask-seed setter begins a distinct serialization helper |
| `src/game/duel_result_runtime.c` | `gcc_2_8_1_g8_split` | The complete eight-function duel-result outro and reward runtime from confetti orbit callback `func_80020D4C` (`0x80020D4C`) through `Duel_AwardCard` (`0x80021894`): orbit and fly-off callbacks, the outro state machine that records the winner and transfers into the result flow, result-page display, per-rule and aggregate rank scoring, weighted card-drop selection, and saturated inventory/recent-drop update. The three former outro sources and the first two reward objects were recorded across g8_no_split, g8, g0_split, and g8_split but all compile exactly at g8_split. The package-transfer callback immediately below is semantically connected but cannot reproduce its object at g8_split, while unmatched assembly bounds the runtime above |
| `src/game/duel_card_state_helpers.c` | `gcc_2_8_1_g8` | Duel-card state export (`0x80028220`) and encoded slot normalization (`0x80028260`) |
| `src/game/main_debug.c` | `gcc_2_8_1_g8` | Debug-mode setup wrapper (`0x8002CDE8`), `Main_RunDebugMenu` (`0x8002CE08`) |
| `src/game/script_control_commands.c` | `gcc_2_8_1_g8` | Two script mode setters at `0x8002F930` and `0x8002F94C`, followed by the contiguous script-delay updater at `0x8002F968` |
| `src/game/frontend_scene_states.c` | `gcc_2_8_1_g8` | 2 contiguous functions: `0x80030C10`, `0x80030CB0`. What remains after #3859 moved `func_80030D5C`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/func_80030E30.c` | `gcc_2_8_1_g8` | 5 contiguous functions: `0x80030E30`, `0x80030E7C`, `0x80030EC8`, `0x80030F40`, `0x80030F80`. Split out of `src/game/frontend_scene_states.c` after #3859 moved `func_80030D5C`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/display_object_fade_callbacks.c` | `gcc_2_8_1_g0` | Three contiguous display-object fade callbacks from `0x80039AFC` through `0x80039C94`, sharing initialization flags and frame-step state |
| `src/game/options_screen.c` | `gcc_2_8_1_g8_split` | The options screen, five contiguous functions: its text-colour and text-box setup `func_8003C4E0` (`0x8003C4E0`), the cursor layout pass `Options_UpdateLayout` (`0x8003C568`), `Options_Init` (`0x8003C628`), which calls both, and the input handler (`0x8003C7A0`) and per-frame dispatcher (`0x8003C8CC`). The former sources were recorded at `gcc_2_8_1_g0_split`, `gcc_2_8_1_g8_split`, `gcc_2_8_1_cc_g8_as_g4_no_split` and `gcc_2_8_1_g8`, and every member compiles to an identical object at `gcc_2_8_1_g8_split`. Bounded below by `func_8003C498`, a package-transfer request, and above by `game_over.c` |
| `src/game/input_pads.c` | `gcc_2_8_1_g8_split` | 4 contiguous functions: `Input_ResetPads` (`0x8003CB7C`), `Input_InitPads` (`0x8003CBE8`), `Input_ReadRawPads` (`0x8003CC38`), `Input_UpdatePads` (`0x8003CCD8`). What remains after #3859 moved `Input_BackupPad1AndUsePad2` and `Input_RestorePad1FromBackup`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/save_data_payload.c` | `gcc_2_8_1_g8` | The save payload, nine contiguous functions: the mask generator `SaveData_NextMaskWord` (`0x8003CE74`) and CRC-16/XMODEM calculator (`0x8003CEB8`), the primary/secondary and tertiary seal writers (`0x8003CF14`, `0x8003CFC8`), `SaveData_BuildPayload` (`0x8003D03C`), which calls both, its inverse `SaveData_ApplyRuntimeState` (`0x8003D0F4`), `SaveData_ValidateIntegrity` (`0x8003D174`), which replays the three seals, and the duelist-code and save-sequence comparisons (`0x8003D288`, `0x8003D2B8`). Three members were recorded at `gcc_2_8_1_g8_split` or `gcc_2_8_1_g8_no_split`, but each compiles to an identical object at `gcc_2_8_1_g8`. Bounded below by `input_pads.c`, the controller runtime, and above by `func_8003D300`, a duel-effect state reset whose object does change at `gcc_2_8_1_g8` |
| `src/game/dialog_transition.c` | `gcc_2_8_1_g8` | Three contiguous dialog/card-reveal transition handlers from `0x8003D518` through `0x8003D74C`, sharing display objects and `D_8009B3C1` state bits |
| `src/game/mem_card_dialog_load_save.c` | `gcc_2_8_1_g8_split` | 4 contiguous functions: `MemCardDialog_SetMessage` (`0x8003E46C`), `MemCardDialog_UpdateLoad` (`0x8003E490`), `MemCardDialog_StepLoad` (`0x8003E7D4`), `MemCardDialog_StepLoadUnprompted` (`0x8003E80C`). What remains after #3859 moved `MemCardDialog_UpdateSave`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/mem_card_dialog_runtime.c` | `gcc_2_8_1_g8_split` | Nine contiguous memory-card dialog functions from save-step callback `MemCardDialog_StepSave` (`0x8003EE90`) through `MemCardDialog_Request` (`0x8003F758`): the save callback and empty table slot, trade save operation, modal object motion, channel setup, request/dialog dispatcher, and poll/start/request API. The callback forwards to the demoted `MemCardDialog_UpdateSave`; the dispatcher invokes it and `MemCardDialog_UpdateTradeSave` through `D_80090F9C`, while the public API seeds the shared buffer/size/file/channel state. The former save/empty object and request API object are byte-identical at g8_split. The demoted save state machine fixes the lower boundary; the unit owns the trade-save switch table at rodata offset `0xCD8`, and the alignment word at `0xCEC` fixes the upper object boundary before the save-transfer runtime's table at `0xCF0` |
| `src/game/mem_card_driver.c` | `gcc_2_8_1_g8_split` | Eighteen contiguous low-level memory-card driver functions from `MemCard_ClearIOEvents` (`0x80043D48`) through `MemCard_DoLoadDirectory` (`0x80044608`): event reset/poll and LIBCARD startup, event close/open and bounded card-clear retry, the common request gate, seven request wrappers, directory enumeration/free-space/name lookup, and the directory-load state machine that consumes them. `MemCard_BeginRequest` seeds the channel/request/retry/load state used by every wrapper; all request paths clear and poll the same two event-handle banks; `MemCard_DoLoadDirectory` drives `_card_info`, `_card_clear`, `_card_load`, `MemCard_FindFiles` and `MemCard_CalcFreeBlocks` over that state. The former event-helper, close-event, begin-request, card-info and directory-helper objects are byte-identical under `gcc_2_8_1_g8_split`. The unrelated frontend loop bounds the run below, while unmatched request dispatcher `func_80044838` bounds it above |
| `src/game/model_handler_registry.c` | `gcc_2_8_1_g8_split` | The model handler registry and the two maps that search it: the packed-id dispatch maps at `0x8005FC1C` and `0x8005FE44`, `Model_RegisterHandlerKey` (`0x80060170`), `Model_FindHandlerKey` (`0x800601D0`) and the model setup helper at `0x80060220`. Contiguous across all five and bounded below by the cubic keyframe coefficient helper that completes `model_effect_state.c`, a separate effect runtime, and above by the profile change to `model_primitive_handler.c` (`gcc_2_8_1_g0_split`). The two maps open-code `Model_FindHandlerKey` over the shared registry `D_800F5918` -- same `GsU_00000000` sentinel, same eighty-entry scan, same `-1` on miss -- before dispatching on the id it returns |
| `src/game/model_effect_state.c` | `gcc_2_8_1_g8_split` | The complete twenty-two-function model effect keyframe/transfer runtime from evaluator `func_8005EBF4` (`0x8005EBF4`) through cubic coefficient builder `func_8005FBC4` (`0x8005FBC4`). It seeds and evaluates the `D_800F5768`/`D_800F5788` keyframe ring, owns the `D_8009B074..D_8009B07C` pointer/count/gate state and its accessors, exposes the `D_80091570` coefficient table and adjustment helper, queues and flushes effect endpoints, resets and queries transfer state, replays staged slot data, and ends with the coefficient helper called by the first function. The former flag and transfer-state objects are byte-identical at g8 and g8_split; the coefficient object matches at g0, g0_split, and g8_split; `func_8005FBC4` matches all four standard profiles; the endpoint dispatcher `func_8005F91C` requires g8_split and fixes the unit. An unmatched assembly gap bounds the runtime below, while the following model-handler registry begins a separate dispatch subsystem |
| `src/game/model_scene_states.c` | `gcc_2_8_1_g8_split` | Seven contiguous animated-battle model scene functions from state setter `func_80059C18` (`0x80059C18`) through slot-handler traversal `func_80059DD8` (`0x80059DD8`): two scene initializers, their paired completion predicates, the per-frame camera/model cleanup and draw pass keyed by the `D_8009AF94` values those initializers install, and the model-slot node traversal. The five former state functions are byte-identical at g8 and g8_split; the cleanup object requires g8_split and fixes the unit. The preceding HMD coordinate conversion uses g0_split, while the following transfer-gated camera request family also uses g0_split |
| `src/game/model_effect_requests.c` | `gcc_2_8_1_g0_split` | Seven contiguous transfer-gated model camera controls from `func_80059EBC` (`0x80059EBC`) through `func_8005A130` (`0x8005A130`): two request wrappers around `func_80052D2C`, eye and target slot selectors, duration reset, doubled halfword parameter, and paired field setter. Every member tests the same `func_8005F174`/`func_8005F18C` gate owned by `model_effect_state.c` before touching the camera request. The two request functions are byte-identical under all four standard g0/g8 profiles; the five parameter functions require g0_split and fix the unit. The preceding scene-state/cleanup unit uses g8_split, and the following model-graphics state helper is not equivalent at g0_split |
| `src/game/model_scene_setup.c` | `gcc_2_8_1_g8_split` | 2 contiguous functions: `0x80052528`, `0x80052694`. What remains after #3859 moved `func_800528AC`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/func_80052D2C.c` | `gcc_2_8_1_g8_split` | 4 contiguous functions: `0x80052D2C`, `0x800530C4`, `Model_SetSlotProperties` (`0x80053248`), `0x800533D8`. Split out of `src/game/model_scene_setup.c` after #3859 moved `func_800528AC`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/model_state_getters.c` | `gcc_2_8_1_g8` | Two leaf getters at `0x80058DC0` and `0x80058DCC` returning the model-state bytes at `D_8009AFA6` and `D_8009AFA0` |
| `src/game/model_slot_queries.c` | `gcc_2_8_1_g0_split` | Four by-index reads of one model slot: `field_E0D` (`0x80058E3C`), `field_BF5` (`0x80058E68`), `field_E06 >> 4` (`0x80058E94`), and contiguous `func_80058EC0` (`0x80058EC0`), which reads that same `field_BF5` and returns the `max` of the `field_750` row it selects. `func_80058EC0` was recorded at `gcc_2_8_1_g8_split` and compiles to an identical object at `gcc_2_8_1_g0_split`. The four sizes sum to `0xD4` and the run ends exactly at `func_80058F10`, `0x10` bytes of SDK assembly that bound it above; `func_80058E1C.c` at `gcc_2_8_1_g8` bounds it below |
| `src/game/model_slot_support.c` | `gcc_2_8_1_g8_split` | Seven contiguous model-slot support and light helpers from clamped entry lookup `func_80058F20` (`0x80058F20`) through light-record lookup `func_800591C0` (`0x800591C0`): selected-entry address helpers, four-halfword slot-value copy, adjusted-value extraction, temporary-enable redraw, initialization of the three `field_D70` light records, and the accessor that returns one of those exact records. Every member operates on `D_800F2C40`; the former g0 objects are byte-identical at g0, g0_split, and g8_split, while the copy/light objects match at g0_split and g8_split. The embedded SDK getter gap immediately below fixes the lower boundary, and the following global buffer-getter family begins a separate unit |
| `src/game/model_buffer_getters.c` | `gcc_2_8_1_g8_split` | Four contiguous leaf pointer getters from `func_800591FC` (`0x800591FC`) through `func_80059220` (`0x80059220`), returning the camera-view, view-metrics, screen-projection, and secondary model buffer bases. The former g0_split objects are byte-identical at g8_split, and `func_80059208` is byte-identical at g8 and g8_split. The preceding light-record accessor completes `model_slot_support.c`; the following matrix/rotation helper returns to non-split g0 |
| `src/game/model_state_setters.c` | `gcc_2_8_1_g8` | Paired leaf setters at `0x80059AE0` and `0x80059AEC` for the halfword at `D_8009AF92` and byte at `D_8009AFA4` |
| `src/game/model_primitive_handler.c` | `gcc_2_8_1_g0_split` | The complete five-function primitive-handler selector/binder family from `func_800603DC` (`0x800603DC`) through `func_80060AEC` (`0x80060AEC`). The first compact selector and `Model_GetPrimitiveHandler` feed `func_8006086C`, their sole binder, which stores and registers the selected handler under the object's key. `func_800608B8` then selects from the second primitive family and `func_80060AEC` performs the identical bind/register operation; that binder is byte-identical at g0, g0_split, g8, and g8_split. `model_handler_registry.c` at g8_split bounds the unit below, while `func_80060B38` above is an unrelated card-list sprite renderer |
| `src/game/sound_output.c` | `gcc_2_8_1_g8` | 11 contiguous functions: `0x80046DE8`, `0x80046F58`, `SD_SetOutputType` (`0x80046FA0`), `0x80047008`, `0x8004701C`, `0x8004703C`, `0x80047050`, `0x800470B0`, `0x80047140`, `0x800471D0`, `0x80047278`. What remains after #3859 moved `func_800472A8` and `func_8004733C`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/func_800473CC.c` | `gcc_2_8_1_g8` | 4 contiguous functions: `0x800473CC`, `0x800473F0`, `0x80047430`, `0x80047458`. Split out of `src/game/sound_output.c` after #3859 moved `func_800472A8` and `func_8004733C`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/sound_frontend.c` | `gcc_2_8_1_g8` | Nine game-facing sound initialization and command helpers from `Sound_InitFrontend` (`0x8003FE80`) through `SD_StopAll` (`0x8003FFFC`), including `SD_SEPlayFull` |
| `src/game/sound_init.c` | `gcc_2_8_1_g0` | Thirteen music/sequence and secondary sound-state initialization helpers from `0x80049200` through `0x800495EC`, including `SD_Init` |
| `src/game/sound_effect_voices.c` | `gcc_2_8_1_g0_no_split` | The sound-effect voice slots: the per-id active-voice count (`0x80047FAC`) and the voice start that plays one (`0x8004803C`). The two are contiguous -- 0x80047FAC is 0x90 bytes and ends exactly at 0x8004803C -- and sit between `sound_voice_selection.c` and `func_80048768`. They read and write the same four voice slots: the count walks `g_SDValue->voice_ids` and the start assigns it alongside the flags, volumes and timer |
| `src/game/model_slot_row_tables.c` | `gcc_2_8_1_g0` | 2 contiguous functions: `0x8004D75C`, `0x8004D914`. What remains after #3859 moved `func_8004D134` and `func_8004D58C`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/model_slot_setup.c` | `gcc_2_8_1_g8_split` | One model slot's setup: the reset that gives it its defaults (`0x8005611C`) and the per-frame duel-side layout pass that reads them (`0x80056250`). Contiguous -- 0x8005611C is 0x134 bytes and ends exactly at 0x80056250 -- and bounded above by `Model_LoadMonsterMerge`, which is assembly again since #3859. Initializer and consumer: the reset writes the mode byte at +0xE16 as 0x3E, +0xE0C/+0xE0D as 7 and 8 and +0xE0A as 0x1000; the layout pass switches on that same +0xE16 and reads the three back |
| `src/game/model_slot_properties.c` | `gcc_2_8_1_g0` | Six by-index accessors over one model slot, five of them on the three adjacent property blocks the record declares at `0xDA0`, `0xDB0` and `0xDC0`: the part-vertex projection (`0x800593D0`), the `field_DB0` pointer and its writer (`0x8005949C`, `0x800594C0`), the `field_DC0` descriptor and its writer (`0x80059520`, `0x80059590`), and contiguous `func_800595C8` (`0x800595C8`), which clamps three values into `field_DA0` with the same `MODEL_FIXED_*` constants and then sets the byte at `0xE11`. The six sizes sum to `0x2CC`, so the run covers `0x800593D0` up to `func_8005969C`. `func_800595C8` was recorded at `gcc_2_8_1_g8` and compiles to an identical object at `gcc_2_8_1_g0`, which the other five already use. `func_800592AC.c` at `gcc_2_8_1_g8_split` bounds it below; above, `func_8005969C` is at the same `gcc_2_8_1_g0`, so what bounds the run there is meaning rather than the profile -- it sets the byte at `0xE0D` and then calls into `model_slot_state_updates.c`, and it is left as is |
| `src/game/sound_voice_selection.c` | `gcc_2_8_1_g0` | 5 contiguous functions: `0x800478EC`, `0x80047A68`, `0x80047AD0`, `0x80047B68`, `0x80047BB4`. What remains after #3859 moved `func_80047864`, `func_80047C50` and `func_80047DB0`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/func_80047C70.c` | `gcc_2_8_1_g0` | 2 contiguous functions: `0x80047C70`, `0x80047CC4`. Split out of `src/game/sound_voice_selection.c` after #3859 moved `func_80047864`, `func_80047C50` and `func_80047DB0`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/sd_key_off_voice_slots.c` | `gcc_2_8_1_g0` | 2 contiguous functions: `SD_KeyOffVoiceSlots` (`0x80047EC4`), `0x80047F38`. Split out of `src/game/sound_voice_selection.c` after #3859 moved `func_80047864`, `func_80047C50` and `func_80047DB0`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/sound_sequence_state.c` | `gcc_2_8_1_g0` | Two sequence-state setters (`0x800490F0`, `0x80049108`) and the active-state test at `0x80049120` |
| `src/game/sound_sequence_runtime.c` | `gcc_2_8_1_g0` | Six contiguous sequence event-stop/update, byte comparison, bounded read, and MIDI-style variable-length decoding helpers from `0x8004B910` through `0x8004BB34`, ending before the different-profile marker scanner |
| `src/game/sound_sequence_parser.c` | `gcc_2_8_1_g0` | The sequence parser, eight contiguous functions: the fixed-width readers `SD_ReadSequenceU32BE` (`0x8004BC2C`) and `SD_ReadSequenceU16BE` (`0x8004BCA8`), the track and tempo header reader `SD_ReadSequenceHeader` (`0x8004BCE8`) built on them, and the event handlers from `SD_AdvanceSequencePosition` (`0x8004BE6C`) through `SD_DispatchSequenceChannelEvent` (`0x8004C114`). `SD_ReadSequenceHeader` was recorded at `gcc_2_8_1_cc_g8_as_g0_split` but compiles to an identical object at `gcc_2_8_1_g0`. Bounded below by `SD_FindMidiTrackChunk`, which does need `gcc_2_8_1_cc_g8_as_g0_split`, and above by the running-status reader `SD_ReadSequenceEvent` at `gcc_2_8_1_g8_split` |
| `src/game/sound_transfer_lifecycle.c` | `gcc_2_8_1_g0` | 2 contiguous functions: `0x80049640`, `SD_Term` (`0x80049694`). What remains after #3859 moved `func_800496C4`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/sound_buffer_init.c` | `gcc_2_8_1_g0` | Sound work-buffer pointer setup (`0x80044D48`) and channel-volume defaults (`0x80044DA0`) |
| `src/game/sound_mix.c` | `gcc_2_8_1_g0` | 2 contiguous functions: `0x80044F58`, `0x80044FE4`. What remains after #3859 moved `func_80044E90`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/func_80045114.c` | `gcc_2_8_1_g0` | 2 contiguous functions: `0x80045114`, `0x800451E0`. Split out of `src/game/sound_output_state.c` after #3859 moved `func_80045054`, `func_80045208` and `func_80045334`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/sound_state_control.c` | `gcc_2_8_1_g8` | Secondary-state activation (`0x8004695C`) and main sound-state flag setup (`0x80046990`) |
| `src/game/sound_voice_data.c` | `gcc_2_8_1_g0` | 3 contiguous functions: `0x80048C0C`, `0x80048C70`, `0x80048D08`. What remains after #3859 moved `func_80048A28` and `func_80048F14`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/sound_secondary_reset.c` | `gcc_2_8_1_g0` | Low-level state query (`0x800498BC`) and secondary-state reset (`0x800498F8`) |
| `src/game/func_80049BAC.c` | `gcc_2_8_1_g0` | 3 contiguous functions: `0x80049BAC`, `0x80049C40`, `0x80049CB0`. Split out of `src/game/sound_secondary_playback.c` after #3859 moved `func_80049AF4`, `func_80049CF8` and `func_80049DD8`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/func_80049EC8.c` | `gcc_2_8_1_g0` | 3 contiguous functions: `0x80049EC8`, `0x80049F10`, `0x80049F50`. Split out of `src/game/sound_secondary_playback.c` after #3859 moved `func_80049AF4`, `func_80049CF8` and `func_80049DD8`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/sound_secondary_commands.c` | `gcc_2_8_1_g0` | 3 contiguous functions: `0x8004B49C`, `0x8004B6E8`, `0x8004B70C`. What remains after #3859 moved `SD_SequenceTimerCallback`, which matched only through pinned registers or inline asm, to `src/candidates/`. |
| `src/game/color_transform.c` | `gcc_2_8_1_g8` | The complete five-function fixed-point colour family from RGB-to-HSL conversion (`0x8005A98C`) through unpacked RGB tint (`0x8005B0B4`): inverse HSL-to-RGB conversion, packed BGR555 tint, direct HSL-to-BGR555 packing, and the unpacked counterpart. `func_8005B054` is byte-identical under its historical `gcc_2_8_1_g0` profile and this unit's `gcc_2_8_1_g8`; private same-symbol aliases preserve the final two callers' codegen-sensitive discarded-return and wide-argument contracts. The following GPU packet writers form the hard upper boundary |
| `src/game/triangle_subdivision.c` | `gcc_2_8_1_g0` | Recursive triangle subdivision written twice, over s16 vertices (`0x8006BCA4`) and over three-byte triplets (`0x8006C120`), followed by the triplet version's three primitives: set-from-three (`0x8006C2FC`), copy (`0x8006C30C`) and per-byte mean (`0x8006C330`). Five contiguous functions bounded by unmatched asm on both sides. `func_8006BCA4` was recorded at `gcc_2_8_1_g8`, but its object is identical at `gcc_2_8_1_g0`, so the apparent profile boundary at `0x8006C120` was not one. The triplet subdivider is the only caller of the copy and mean primitives, and `func_8006C2FC` has no caller at all |
| `src/game/duel_shuffle_deck.c` | `gcc_2_8_1_g8_split` | The deck shuffle and its two-sided caller: `Duel_ShuffleDeck` (`0x800243F4`), which builds one side's halfword id list and byte order list and then swaps pairs, followed by contiguous `Duel_ShuffleBothDecks` (`0x800245A0`), which calls it once per side at `gDuel_awPlayerShuffledDeck` and at that base less `0x54` and `0x2C`. `Duel_ShuffleBothDecks` was recorded at `gcc_2_8_1_g8` and compiles to an identical object at `gcc_2_8_1_g8_split`. Bounded below by `duel_scene_update.c` at `gcc_2_8_1_g8` and above by `Duel_StepCardDataTransfer` at `gcc_2_8_1_g0_no_split` |

The sound-code request group shares only the identical request layout and
external declarations, not either algorithm's body. Its common
`SD_EnqueueCommand` declaration returns `s32`, matching the implemented queue
submission contract; both builders discard that result. Their conditional
loads, local scopes, and store order remain separate for exact code generation.

The original pilots reduced four one-function source files to two coherent
translation units. The later subsystem pass applies the same invariants across
AI, File, Duel, Main, Build Deck, and sound code. Function emission order and
the complete retail executable SHA-256 remain unchanged.

The initial expanded-subsystem snapshot contained 773 matching functions in
681 translation units. Forty-three grouped units contained 135 functions; the
largest was the thirteen-function `sound_init.c` block. The authoritative
current grouping is `config/slus_01411/matching_c.json`.

At that snapshot, an executable-order audit of the established AI, File,
sound-frontend, and sound-driver ranges reported no remaining pair of
single-source pure-C functions that was both contiguous and
profile-compatible. Source splits in that audit were caused by at least one
of:

- a nonmatching assembly function between matching functions;
- different compiler profiles;
- an existing GCC inline-assembly source retained for later pure-C
  refinement;
- noncontiguous executable addresses, which cannot share one object without
  changing layout.

The three-function AI call-control group at `0x80070D00-0x80070EB4` was
initially blocked by incompatible local declarations of `gAiScript_State`.
All three helpers now use the shared `AiScriptState` declaration from `ai.h`,
so `AiScript_Call`, `AiScript_Return`, and `AiScript_SetRandom` build together.
They now sit in `ai_script_control_flow.c` with the six jump opcodes below them.

## Preconditions worth checking before spending a build

Conditions 1 and 2 of the contract are cheap to read out of
`config/slus_01411/functions.csv` and `matching_c.json`. The conditions that
actually reject candidates are less obvious, and all three can be settled from
files already on disk.

### The two halves must agree on every shared declaration

Two sources that each declare the same global differently cannot be merged,
because a grouped unit is compiled once and one spelling has to win. This
already blocked the AI call-control group above until the three helpers moved
to a shared `AiScriptState`.

The sharp case is a guard-selected arm. `func_8003AD6C.c` took the default
`extern s32 D_8009B0D8` arm and its neighbour `func_8003B054.c` opened with
`#define D_8009B0D8_IS_HALFWORD`. They are adjacent and share `gcc_2_8_1_g0`,
so they pass conditions 1 and 2, and the pair looks mergeable.

Disassembling the two objects the build already produced shows the
disagreement without compiling anything:

```
func_8003AD6C.o    lw   v1,0(v1)     R_MIPS_LO16   D_8009B0D8
func_8003B054.o    lhu  v1,0(v1)     R_MIPS_LO16   D_8009B0D8
```

**This was recorded as blocking, and that was wrong.** The reasoning ran:
retail reads one symbol at two widths, neither arm can replace the other, so
merging must change a load. The first clause is right and the conclusion does
not follow, because picking an arm is not the only way to spell a global. A
unit that needs two widths takes an **alias** -- a second name for the same
symbol, `extern u16 D_8009B0D8_halfword asm("D_8009B0D8");` -- which
`graphics_frame.h` documents and `display_object_fade_callbacks.c` already
uses on this very global for the plain and volatile pair. With the alias both
loads survive exactly as above, and the two are now one unit,
`display_effect_update_callbacks.c`, with the executable byte-identical.

So the rule is narrower than it looked. **Two sources that spell a shared
global differently are blocked only when the difference cannot be expressed
twice in one unit.** A guard-selected *width* or qualifier can be, through an
alias. What genuinely blocks is a difference in the object itself -- a
different type, extent or section attribute -- because an alias would then be
a second declaration of a different thing rather than a second name for the
same one.

Note also what the two-width read does *not* prove. It is tempting to read
`lw` beside `lhu` as evidence that retail had two translation units here, but
`display_object_fade_callbacks.c` is a single retail unit reading this same
symbol two ways, so the inference does not hold in either direction.

`objdump -dr tmp/splat/build/src/game/NAME.o` is the general form. It is the
cheapest tier of evidence available here, below even the `-G` table in
`notes/build.md`, and it is the right tool for finding this disagreement --
just not for concluding the merge is impossible.

### A recorded profile is one that matches, not the only one

Condition 2 is checked against the profile `matching_c.json` records, but that
profile only says which flags reproduce the function. It does not say no other
profile does. Where two neighbours differ only in a flag that one of them never
exercises, the "profile boundary" between them is an accident of which profile
was tried first.

The profiles most often split by this are `gcc_2_8_1_g8` and
`gcc_2_8_1_g8_split`, which differ only in `-msplit-addresses`, and
`gcc_2_8_1_g0` and `gcc_2_8_1_g8`, which differ only in the small-data
threshold. A function with no global it could split, or no small data it could
reach, often compiles to the same object under both.

The check is cheap and needs no link: compile the source under both profiles
with `build_baseline.compile_c` and compare `objdump -s -r -t -h` of the two
objects. If they are identical, the function can take its neighbour's profile
and the pair is judged on meaning like any other. `triangle_subdivision.c`,
`input_pads.c` and `save_data_payload.c` were formed this way. The check does
not replace the full build, which still has to match, because the merged unit
is compiled as one.

### A merged unit inherits both halves' `.rodata`

A source that owns a jump table has a `.rodata` subsegment in
`config/slus_01411/split.yaml`. Merging two such sources gives the combined
object one `.rodata` run, so the two subsegment entries must collapse into a
single entry — which is only possible when the two tables are already
adjacent in the image.

`func_80057544` and `func_800577B0` are the worked example. Each owns a jump
table, and the two subsegments are:

```
- [0x1F84, .rodata, game/func_80057544]
- [0x1F9C, .rodata, game/func_800577B0]
```

`0x18` is six entries for the six-case switch and `0x2C` is eleven for the
eleven-case one, and `0x1F84 + 0x18 = 0x1F9C`, so a merged object owns one
`0x44` run at `0x1F84` and the two entries become one. That merge is PR #2959,
which folds the pair into `file_transfer_steps.c`.

This makes condition 3 load bearing beyond `.text`: definition order is what
emits the six-case table ahead of the eleven-case one. Swapping the two
functions would reorder `.rodata` even if `.text` could still be made to fit.

A subsegment size is also a free cross-check on a decompiled switch, since
its byte size divided by four is the number of table entries.

## Sources that cannot be grouped at all

Five resident sources were an `__asm__` block of `.word` literals with
explicit `.reloc` directives and no C statements outside it: `func_800291E0`,
`func_8002A4A8`, `func_8002A788`, `func_80030998` and `Main_RunCredits`. They
were registered in `matching_c.json` with a compiler profile, but no C was
compiled for them, so the profile was inert and the recorded match reflected
literal bytes rather than codegen.

#3859 reclassified all five to `unmatched_asm` and removed the sources, and
each one's `functions.csv` row records that. They are now in neither
`matching_c.json` nor `candidates.json`, so a candidate scan over matching C
no longer reaches them and none of them needs excluding by name.

What they still do is leave a hole. Each sits between two matched functions,
and for three of the five those neighbours share a profile: `func_800291E0`
between two at `gcc_2_8_1_g0`, `func_80030998` between two at
`gcc_2_8_1_g8`, and `Main_RunCredits` between two at
`gcc_2_8_1_g8_split_comm`. Of those three, only in the last is the hole
exactly the one function -- its size is `0x21C` and the gap between its
neighbours is `0x21C` -- so that run is blocked by this alone; the other two
gaps are wider than the function, holding two and three unmatched functions
respectively. `func_8002A4A8`'s gap is exactly its size as well, but its
neighbours carry different profiles, so no run reached it anyway. Either way a
run spanning one of them fails conditions 1 and 4 on the gap, which needs no
special case.

This used to be contrasted with a source carrying a single inline opcode, and
that contrast no longer separates two groupable populations. Seven sources
contain `.word 0x4A180001`, the GTE `rtps` encoding the period assembler
could not spell, inside ordinary C with real operand constraints:
`func_80015D18.c`, `func_80015DFC.c`, `func_80015EF4.c`, `func_800177C4.c`,
`func_800178BC.c`, `func_8001B0CC.c` and `func_80029934.c`. All seven are
build-integrated candidates under `src/candidates/`, so none of them is in
matching C to group, and they are there for two different reasons. Five were
reclassified by #3859, and each of those five rows says the source was
byte-exact only through that inline asm statement -- two of them through a
register pin as well -- so for those the opcode is the reason rather than an
incidental detail. The other two, `func_80015EF4.c` and `func_80029934.c`,
are ordinary unmatched candidates whose rows record a live residual.

`display_projection.c` used to be an example here and no longer carries the
encoding at all. #3859/#3904 moved `func_80015D18` and `func_80015DFC` out of
it -- 97 lines to 19 -- and both `.word` directives went with them.

## Whole-resident candidate survey

The audit recorded above was scoped to the established AI, File, and sound
ranges. A survey across the whole resident finds 46 address-contiguous
same-profile runs that span more than one source. Twenty-one are blocked by a
declaration conflict or a byte-blob member.

The remaining twenty-five are mostly not merge candidates, and the reason is
worth stating because the raw count is misleading: most are simply *file
boundaries*, where the last function of one source happens to abut the first
of the next. The `0x8003FE70` run pairs `SaveData_SetMaskSeed` with nine
sound-frontend functions; grouping those would be merging by adjacency, not by
meaning.

The filter that survives is: every source in the run is a single-function
source, and the bodies pass a meaning test. Applying it left two runs, one of
which was three unrelated routines (a menu tick dispatcher, a Shift-JIS digit
lookup and a duel effect setter) that merely share a profile.

## Expansion policy

Expand grouping only after names and behavior are stable. Prefer small
subsystem runs with shared declarations. Do not bridge an assembly function,
mix compiler profiles, reorder functions, or merge a function whose remaining
inline assembly would make later refinement unsafe.
