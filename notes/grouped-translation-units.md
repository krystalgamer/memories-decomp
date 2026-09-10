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
| `src/game/duel_state_init.c` | `gcc_2_8_1_g8_split` | Duel-side and life-point initialization (`func_800175A0`, `0x800175A0`), followed by contiguous `Duel_ClearHandSlots` (`0x800176D0`) clearing five hand-state entries |
| `src/game/func_800179F4.c` | `gcc_2_8_1_g8_split` | Four contiguous duel scene/display functions: scene entry and active-side setup (`0x800179F4`), display-marker resource selection (`0x80017DB4`), face/position/used-state visual updates (`0x80017E3C`), and duel-card display-object creation (`0x80017F04`). The entry initializes the card, side, resource, and display state the helpers consume; the immediately following `func_80018004` calls the factory, which invokes both state helpers before returning. `gcc_2_8_1_g8` functions bound the run on both sides |
| `src/game/duel_card_object_helpers.c` | `gcc_2_8_1_g8` | Screen-space duel display-object constructor (`0x80018150`) and contiguous card-category encoder (`0x800181EC`), mapping magic/equip, trap, and ritual types to `2`, `3`, and `4` with optional flag `0x80` |
| `src/game/duel_phase_entry.c` | `gcc_2_8_1_g8_split` | Three contiguous entries from the `D_80090998` duel-phase callback table: resume/replay reconstruction (`0x8001825C`), initial combined-deck and selection setup (`0x80018608`), and draw-phase hand reconstruction (`0x8001898C`). All three use `D_8009B23A` as the first-call latch, rebuild card/side state, and coordinate fade or file-transfer gates. The complete run is bounded by `gcc_2_8_1_g8` functions on both sides |
| `src/game/duel_draw_resolution.c` | `gcc_2_8_1_g8_split` | Five-piece Exodia hand predicate (`0x80018CF8`) and the contiguous draw-animation state machine (`0x80018DB4`) that invokes it before resolving victory |
| `src/game/display_object_property_transitions.c` | `gcc_2_8_1_g8` | Three-channel byte convergence to per-channel targets (`0x8001D344`) and the contiguous timed position/interpolation transition with clip-flag lifecycle (`0x8001D3C4`) |
| `src/game/display_object_motion.c` | `gcc_2_8_1_g8` | Timed position interpolation with speed-to-`0x800` completion (`0x8001EC70`) and the contiguous mode-progress variant that stores the final position and clears the clip flag when appropriate (`0x8001ED20`) |
| `src/game/display_effect_update_callbacks.c` | `gcc_2_8_1_g0` | Two display-effect update callbacks, `func_8003AD6C` (`0x8003AD6C`) and the contiguous `func_8003B054` (`0x8003B054`). Contiguous -- 0x8003AD6C is 0x2E8 bytes and ends exactly at 0x8003B054 -- and bounded on both sides by a profile change, `func_8003AC48.c` (`gcc_2_8_1_cc_g8_as_g0_split`) below and `func_8003B378.c` (`gcc_2_8_1_g8`) above. Both take the same `DisplayEffectState` record and open with the same first-frame latch `func_80039F1C`, and both scale by the per-frame step multiplier `D_8009B0D8`. They read that multiplier at different widths, so the unit keeps the plain `s32` arm and the second function reaches the same symbol through a `D_8009B0D8_halfword asm("D_8009B0D8")` alias, the device graphics_frame.h documents and display_object_fade_callbacks.c already uses |
| `src/game/duel_battle_stats.c` | `gcc_2_8_1_g8_split` | Battle arithmetic between two duel cards: `Duel_CalcGuardianStarBonus` (`0x8001EE44`), the attack and defence calculators `Duel_CalcBattleAttack` (`0x8001EF1C`) and `Duel_CalcBattleDefense` (`0x8001EF78`) that add that bonus, and the AI's battle comparison `func_8001EFD4` (`0x8001EFD4`) that calls them. The two calculators were recorded at `gcc_2_8_1_g8` but compile to identical objects at `gcc_2_8_1_g8_split`. Bounded below by `display_object_motion.c` and above by the attack-trap selector in `duel_trap_resolution.c` |
| `src/game/duel_trap_resolution.c` | `gcc_2_8_1_g8_split` | Contiguous attack-trap selector (`0x8001F0D0`) and its presentation state machine (`0x8001F364`), linked through the selected card-object index in `D_8009B1B8` |
| `src/game/duel_field_effect_steps.c` | `gcc_2_8_1_g8_split` | Two steps of the same field-wide duel effect: the terrain-modifier pass (`0x80025F3C`) and the contiguous staggered row sweep (`0x800260D0`). Contiguous -- 0x80025F3C is 0x194 bytes and ends exactly at 0x800260D0 -- and bounded on both sides by a profile change, `func_80025EE0.c` below and `func_8002622C.c` above, both `gcc_2_8_1_g8`, so the run is exactly these two. Four shared globals: both latch their first frame through `DuelEffect_MarkInitialized`, drive their phase through `D_8009B220`, and index the acting side's grid `D_800907D8` by `D_8009B1D5` into the card records at `D_801A7AD8`. The unit takes both files' translation-unit switches, `D_8009B1D5_IS_VOLATILE` and `DUEL_FIELD_GRID_2D`; the sweep keeps its flat grid walk behind an explicit cast |
| `src/game/duel_card_turn_animations.c` | `gcc_2_8_1_g8` | Mirrored three-mode card turn-back callback (`0x80022674`, sets record flag `0x400`) and contiguous flip/turn callback (`0x800229F4`, clears `0x400`), both using side-dependent two-phase rotations |
| `src/game/func_8001B938.c` | `gcc_2_8_1_g8_split` | Two contiguous private helpers of the unmatched duel action controller `func_8001BD88`: selection-side setup (`0x8001B938`) in the controller's state-3 branches, followed by AI-script hand/field selection execution (`0x8001BAF0`) in state 0. Both share the active side, hand slots, duel card records, selection state, and display-object recreation. Different compiler profiles bound the pair on both sides |
| `src/game/duel_field_display_objects.c` | `gcc_2_8_1_g8_split` | Four contiguous field-display functions: guardian-star comparison for two cursor slots (`0x80023090`), the card info-box builder (`0x80023144`) that calls it, the active-side field-grid index dispatcher (`0x8002348C`), and the display-object constructor (`0x800234E4`). The comparator shares `D_800907D8`, `D_801A7AD8`, the active side, and the source record's cursor-byte view with its caller, and is byte-identical under its historical `gcc_2_8_1_g8` profile and this unit's split-address profile. The unit also preserves the source record's two DisplayObject pointers, shared `+0x17` selector, and `s32` builder index contract |
| `src/game/duel_cursor_status.c` | `gcc_2_8_1_g8` | Status-returning cursor wrappers: held-pad directional update (`0x80024060`) through `func_80023FBC`, followed by the explicit-direction `func_80023D08` path (`0x80024088`); both return object byte `+0x19` |
| `src/game/duel_scene_update.c` | `gcc_2_8_1_g8` | Per-frame duel-state, effect, and quit-dialog dispatcher (`0x80024200`) followed by its 2P-aware wrapper (`0x80024388`), which temporarily exposes pad 2 as pad 1 for the selected side and restores pad 1 afterward |
| `src/game/duel_card_record_lifecycle.c` | `gcc_2_8_1_g8_split` | One duel card record's lifecycle, six contiguous functions: the release (`0x80024914`), which drops the display object and `DUEL_CARD_FLAG_OCCUPIED`, and the full flag reset built on it (`0x80024954`), `Duel_GetTerrainBoost` (`0x8002497C`), `Duel_SetupCardRecord` (`0x800249E0`), which calls it while filling the record from the deck card data, the card-type icon object (`0x80024C1C`), and the slot setup (`0x80024D34`) that sets up a record and attaches that icon to it. The former sources were recorded at `gcc_2_8_1_g8`, `gcc_2_8_1_g8_split` and `gcc_2_8_1_g0_split`, and every member compiles to an identical object at `gcc_2_8_1_g8_split`. Bounded below by `Duel_PopulateCombinedDeckData` (`gcc_2_8_1_cc_g8_as_g0_split`, which does not build at `g8_split`) and above by `func_80024DC8`, which needs `gcc_2_8_1_g0` |
| `src/game/duel_action_lock.c` | `gcc_2_8_1_g8` | `DuelEffect_MarkInitialized` (`0x80024E24`), which sets bit `0x80` and distinguishes first entry (`0`) from an already-active update (`1`), followed by the contiguous full-state reset (`0x80024E4C`) |
| `src/game/duel_magic_effect_dispatch.c` | `gcc_2_8_1_g8_split` | Three contiguous magic-effect functions: the two-stage occupied-row clear handler (`0x80026A3C`), the effect-group dispatcher (`0x80026B34`), and the card-ID activation helper (`0x80026BA4`) that writes the exact effect ID, original card ID, and active/second-handler flags the dispatcher consumes. The activation helper is byte-identical under its historical `gcc_2_8_1_g8` profile and this unit's split-address profile; the following free-field-slot search starts a separate AI/field-selection subsystem |
| `src/game/func_800283F4.c` | `gcc_2_8_1_g8_split` | The state-2 duel display handler (`0x800283F4`) followed by the contiguous generic `DuelEffect_UpdateState` dispatcher (`0x8002892C`). The state-handler table selects the first function at index 2, while both functions share `D_8009B248` and `D_8009B254` as the handler-local and dispatcher lifecycle bytes |
| `src/game/duel_projection_axes.c` | `gcc_2_8_1_g0` | Symmetric three-point projection helpers for the X components (`0x80029684`) and Z components (`0x800297DC`) of `SVECTOR` triplets; both retry `RotAverage3` at half span when the depth/control result is negative |
| `src/game/library_runtime.c` | `gcc_2_8_1_g8_split` | The Library screen, six contiguous functions: the dispatcher's trivial handlers for states 0 and 3 (`0x8002BAA0`, `0x8002BAAC`), the per-frame state dispatcher (`0x8002BAB4`), its package-transfer phase callback (`0x8002BD0C`), `Library_MarkOwnedCards` (`0x8002BF3C`), and the screen entry `func_8002BFCC` (`0x8002BFCC`), which installs that callback, waits for all package phases and initializes the `D_800EA1E8` screen state before `Main_RunLibraryMenu` drives the dispatcher. The state handlers were recorded at `gcc_2_8_1_g8` and the owned-card pass at `gcc_2_8_1_g0_split`, and both compile to identical objects at `gcc_2_8_1_g8_split`. Bounded above by the duel grid offset helpers in `duel_grid_offsets.c`; the unit owns the callback's jump table at ROM `0xA08` |
| `src/game/main_run_selection_menus.c` | `gcc_2_8_1_g8` | Contiguous main-mode runners for Campaign Map (`0x8002D2D8`, package/location setup and per-frame transition), Build Deck (`0x8002D370`, deck-backed setup and completion return), and Free Duel (`0x8002D3F8`, module setup, per-frame dispatch, and exit cleanup) |
| `src/game/main_run_frontend_menus.c` | `gcc_2_8_1_g8` | Six contiguous frontend mode runners: main menu (`0x8002D588`), name entry (`0x8002D62C`), password (`0x8002D684`), Options (`0x8002D6C8`), game over (`0x8002D730`), and the empty `Main_RunHirata` stub (`0x8002D7C4`) |
| `src/game/script_flag_commands.c` | `gcc_2_8_1_g8_split` | Combined library-used/story-flag command (`0x8002E918`) that updates a card flag or conditionally jumps the script cursor, followed by `Script_OpViewportTween` (`0x8002E9A0`) reading target X, target Y, and duration |
| `src/game/script_stream_commands.c` | `gcc_2_8_1_g8` | `Script_OpSound` (`0x8002EC74`), a contiguous one-byte sound-control dispatcher (`0x8002EDB0`), and a 16-bit base-relative script-cursor jump (`0x8002EE20`) |
| `src/game/script_readers.c` | `gcc_2_8_1_g8` | Contiguous script operand readers for one byte (`0x80030050`) and one little-endian 16-bit value (`0x8003006C`), advancing `gScript_pStream` by one or two bytes respectively |
| `src/game/ai_fusion.c` | `gcc_2_8_1_g0_split` | `AiScript_FindEquipTarget` (`0x8007249C`) through `Ai_CompleteFusion` (`0x800727C0`), followed by the two fusion-search opcodes that seed the fusion scratch and recurse through it, `AiScript_FindBestCombo` (`0x80072A48`) and `AiScript_EvaluateFusion` (`0x80072DC0`). Those two were recorded at `gcc_2_8_1_cc_g0_as_g8_split` and `gcc_2_8_1_g8_split`, and each compiles to an identical object at `gcc_2_8_1_g0_split`. Bounded above by the skip opcodes |
| `src/game/ai_script_vm.c` | `gcc_2_8_1_g0_split` | `AiScript_Init` (`0x800705D8`), `AiScript_Run` (`0x80070650`) |
| `src/game/ai_card_ranges.c` | `gcc_2_8_1_g0_split` | Four contiguous AI card-selection helpers: winning-card (`0x80070738`) and general card (`0x800707C4`) range decoders with their compiler-owned jump tables, followed by card-ID (`0x80070870`) and card-type (`0x800708C4`) set predicates |
| `src/game/ai_script_card_info.c` | `gcc_2_8_1_g0_split` | `AiScript_TestHighStat` (`0x80071194`) through `AiScript_LoadCardID` (`0x800712B4`) |
| `src/game/ai_script_control_flow.c` | `gcc_2_8_1_g0_split` | The AI script VM's control-flow opcodes, nine contiguous functions: the six conditional jumps from `AiScript_JumpGreaterEqual` (`0x800709C0`) through `AiScript_JumpRandom` (`0x80070C60`), `AiScript_Call` (`0x80070D00`) and `AiScript_Return` (`0x80070DA8`), and `AiScript_SetRandom` (`0x80070E20`), which was already grouped with the call pair. The former sources alternated between `gcc_2_8_1_g0_split` and `gcc_2_8_1_g8_split`, and every member compiles to an identical object at `gcc_2_8_1_g0_split`. Bounded below by `ai_script_support.c` and above by `AiScript_Subtract`, an arithmetic opcode |
| `src/game/ai_script_end.c` | `gcc_2_8_1_g0` | `AiScript_EndHand` (`0x80070FF8`), `AiScript_EndField` (`0x80071000`) |
| `src/game/ai_script_skip.c` | `gcc_2_8_1_g0` | `AiScript_SkipHand` (`0x80072F1C`), `AiScript_SkipField` (`0x80072F54`) |
| `src/game/ai_script_combo.c` | `gcc_2_8_1_g0_split` | `AiScript_TestPinned` (`0x8007154C`), `AiScript_StartCombo` (`0x800715C4`) |
| `src/game/ai_script_load_best_values.c` | `gcc_2_8_1_g0` | Three contiguous result loaders for best difference (`0x8007164C`), attacker (`0x80071688`), and target (`0x800716C4`), each copying search state into `gAiScript_aMemory` |
| `src/game/ai_script_power_search.c` | `gcc_2_8_1_g0_split` | `AiScript_FindStrongest` (`0x80071700`) and `AiScript_FindWeakest` (`0x80071924`), sharing the same active-card view and operand contract while retaining their separate ranking loops, initial bounds, and tie behavior |
| `src/game/ai_script_find_card.c` | `gcc_2_8_1_g0_split` | Four contiguous AI script opcodes: the strongest-against-strongest field pairing `AiScript_FindDefenseStopper` (`0x80071CB0`), `AiScript_CountCards` (`0x80071EB8`), state-based `AiScript_FindFirstCard` (`0x80071FC8`), and card-ID-based `AiScript_FindCard` (`0x8007214C`). They sit directly above `func_80071B64`, which compiles at a different profile; all four read operands with `AiScript_ReadByte`, scan `gDuel_aActiveCards` and answer into `gAiScript_aMemory` |
| `src/game/ai_script_actions.c` | `gcc_2_8_1_g0_split` | `AiScript_PlayFaceUp` (`0x80072F8C`), `AiScript_SetPosition` (`0x80073050`) |
| `src/game/ai_script_state_ops.c` | `gcc_2_8_1_g0_split` | The AI script VM's state opcodes, eighteen contiguous functions from `AiScript_PushComboCard` (`0x80073220`) through `func_8007368C` (`0x8007368C`): the combo-stack pushes, the hand and field no-ops, the card and type set edits, the four state-flag handlers at `0x80073448`-`0x80073474` and `AiScript_MoveCard`, `AiScript_LoadOpponentData`, the register store, copy and add, and a flag load. The eleven former sources were recorded at five profiles, and every member compiles to an identical object at `gcc_2_8_1_g0_split`. Bounded below by `AiScript_FindBestAttack` (`gcc_2_8_1_g8_split_no_strength_reduce`) and above by `AiScript_Print` (`gcc_2_8_1_g8`) |
| `src/game/ai_script_support.c` | `gcc_2_8_1_g0` | Combined card/type set query (`Ai_IsCardInSets`, `0x80070920`) and direct jump handler (`AiScript_Jump`, `0x80070988`) |
| `src/game/graphics_frame.c` | `gcc_2_8_1_g8_split_comm` | Contiguous frame-boundary synchronization (`Graphics_SyncFrame`, `0x80012DB4`) and next-buffer/ordering-table setup (`Graphics_BeginFrame`, `0x80012E5C`) used in sequence by the main frame pump |
| `src/game/file_stream.c` | `gcc_2_8_1_g8` | File-state initialization (`0x80013898`), `File_GetPosition`, and three transfer setup helpers through `0x80013A94` |
| `src/game/file_transfer_setup.c` | `gcc_2_8_1_g8_split` | Two contiguous transfer-descriptor setup helpers at `0x80013B04` and `0x80013B68` |
| `src/game/file_transfer_runtime.c` | `gcc_2_8_1_g8_split` | Thirteen contiguous asynchronous file-transfer functions from the first disc-command callback (`0x800140A0`) through request dispatcher `func_80014C40` (`0x80014C40`): seven DS completion callbacks, secondary-to-primary activation, the bridge helper, the primary transfer step, frame service, descriptor callback and request staging. The callbacks clear and advance the exact `D_8009B0F4` states consumed by `func_8001455C`; `File_ActivateTransfer` and `func_800144B8` promote the shared `D_801D4200` request slots and feed that step; `func_80014A5C` drives it; and `func_80014C40` constructs the secondary request the activation path later promotes. The bridge object's `gcc_2_8_1_g8` and `gcc_2_8_1_g8_split` builds are byte-identical. Same-symbol byte and typed views preserve the codegen-sensitive `D_8009B11C` and `D_801D4200` accesses. The unit owns `jtbl_80010044` through the `.rodata` line at ROM `0x844`; `file_transfer_setup.c` is separated below by unmatched `func_80013C28`, and `file_transfer_flags.c` changes profile above |
| `src/game/file_transfer_flags.c` | `gcc_2_8_1_g8` | Seven contiguous helpers over one `FileTransferDescriptor`: the active/secondary transfer-request and cancellation helpers from `0x80014E1C` through `0x80015078`, and the phase stepper `func_8001513C` (`0x8001513C`) that counts a descriptor's phase down and fires its `phase_callback`. The run this sits in is bounded by a profile change on both sides, `file_transfer_runtime.c` (`gcc_2_8_1_g8_split`) below and `fade_step_bands.c` (`gcc_2_8_1_cc_g8_as_g0_split`) above, but it is not taken whole: the fade-state reset `func_800151B0` sits between this unit and that upper bound and resets `gFade_State`, so it is left in its own source. It cannot join the fade units either, because they use a different profile |
| `src/game/fade_overlay.c` | `gcc_2_8_1_g8_split` | The fade overlay and the fade-in side of its setup, eight contiguous functions: `Fade_DrawOverlay` (`0x800154E4`), the band fill `func_800156B8` (`0x800156B8`), the white-mode reset (`0x800156DC`) and white fade-in helper (`0x8001572C`), uniform `Fade_InitIn` (`0x80015780`, target `0xFF`, flag `0x80`, step `0x0C`) and banded `Fade_StartIn` (`0x800157DC`, flag `0x01`, step `8`), and colour-selecting `Fade_InitInColor` (`0x8001581C`, flags `0x30`) with the conditional white-state reset (`0x80015870`, flags `0xB0`, step `0x0C`). The former sources were recorded at `gcc_2_8_1_g8_split`, `gcc_2_8_1_g0_split` and `gcc_2_8_1_g8`, and every member compiles to an identical object at `gcc_2_8_1_g8_split`. Bounded below by `Fade_Update` (`gcc_2_8_1_cc_g8_as_g1_split_comm`) and above by `fade_out.c`, which changes without split addresses |
| `src/game/fade_out.c` | `gcc_2_8_1_g8` | `Fade_InitOut` (`0x800158B8`, head `0xFF`, target `0`, flag `0x80`, step `0x0C`) and contiguous strip-mode `Fade_StartOut` (`0x80015904`, flag `0x01`, step `8`) |
| `src/game/fade_control.c` | `gcc_2_8_1_g8_split` | Colour-selecting `Fade_InitOutColor` (`0x80015944`, flags `0x30`), the blocking frame-pump loop `Fade_Wait` (`0x80015998`) that waits for flag `0x80` to clear, and the eighteen contiguous blocking/nonblocking fade wrappers built on them and the other setup paths, direct level/target setters, flag-`0x02`/`0x06` variants, and overlay-latch toggles from `0x800159D8` through `0x80015D0C`, including `Fade_WaitOut`. The wrappers were recorded at `gcc_2_8_1_g8` but compile to identical objects at `gcc_2_8_1_g8_split`. Bounded below by `fade_out.c`, which changes without split addresses, and above by `display_projection.c` |
| `src/game/display_projection.c` | `gcc_2_8_1_g8_split` | Scratchpad RTPS screen projection with coordinate bias (`0x80015D18`), the contiguous display-slot position wrapper (`0x80015DB8`), and tracked-record projection with side-dependent Y bias (`0x80015DFC`) |
| `src/game/movie_frame_pipeline.c` | `gcc_2_8_1_g8` | The movie player in image order: the stop path that tears the stream down and repaints the screen (`0x8005BB7C`), frame presentation (`0x8005BE3C`), CD-ring/VLC fetch (`0x8005BFC8`), the MDEC strip-output callback (`0x8005C1F4`), and the setter for the three bytes at `D_8009B4A0` (`0x8005C374`). The five share the frame work area and stream state and are the whole `gcc_2_8_1_g8` run in that region |
| `src/game/movie_stream_requests.c` | `gcc_2_8_1_g0_split` | Indexed `MOVIE.STR` range setup (`0x8005C388`) and named-file stream setup (`0x8005C464`) |
| `src/game/file_cd_helpers.c` | `gcc_2_8_1_g0` | `File_Exists` (`0x8005C4F0`) and two contiguous low-level CD state/wait helpers through `0x8005C568` |
| `src/game/mdec_sync.c` | `gcc_2_8_1_g8` | MDEC completion-latch setter (`0x8005C5C4`) and contiguous bounded wait/reset helper (`0x8005C5D4`) |
| `src/game/main_services.c` | `gcc_2_8_1_g8_split` | The resident system layer: per-frame service pump (`0x8001306C`), boot-time graphics/input start-up (`0x80013154`), the pad-driven screen-offset adjustment loop (`0x80013360`), and the reset of the callback registry the pump walks (`0x800134B4`). The four are the whole `gcc_2_8_1_g8_split` run between `graphics_frame.c` and `func_800134E0`, and the pump and the reset share the `D_800E9DB0` slots and `D_8009B0B8` |
| `src/game/view_state_orbit.c` | `gcc_2_8_1_g8_split` | Placing the `D_800F2848` view: `func_800134E0` (`0x800134E0`) sets the viewpoint at an offset from the reference point and installs the `GsRVIEW2`, and the two orbit computations after it, `func_8001352C` (`0x8001352C`) and `func_800135FC` (`0x800135FC`), derive that offset with `rsin`/`rcos` and call it. Recorded at `gcc_2_8_1_g8`, `gcc_2_8_1_g0_split` and `gcc_2_8_1_g8_split`; each compiles to an identical object at `gcc_2_8_1_g8_split`. Bounded below by `main_services.c` and above by the empty `func_800136D4` |
| `src/game/debug_effect_screen.c` | `gcc_2_8_1_g8_split` | The developer effect-preview screen, in call order: the pad-driven camera and viewport nudge (`0x800220B8`), the controller that builds one of four preview pages and spawns an effect on CROSS (`0x800222F4`), and the HUD line that prints the tuned pair (`0x80022618`). Each calls the one before it, and the three are the whole `gcc_2_8_1_g8_split` run in that region |
| `src/game/ai_turn_action.c` | `gcc_2_8_1_g8_split` | The five-function AI action and target-selection run from `0x8002712C` through `0x800278A0`: spell and fusion searches feed the turn-action pick through the pending selection at `D_800EAE88`, then the two active-side monster-row selectors rank all occupied or only face-up candidates for the immediately following AI routine. The whole run shares duel-card, grid, side, and display-object state; the selection header preserves the two private caller views |
| `src/game/build_deck_card_counts.c` | `gcc_2_8_1_g0_split` | The Build Deck screen's card counts, five contiguous functions over one screen record: the count box refresh (`0x80031E5C`), returning a copy to the chest (`0x80031EE4`), card-reference release (`0x80031F7C`), full count reconstruction (`0x8003201C`), and `BuildDeck_AddCard` (`0x800320BC`), which calls the reconstruction. The former sources were recorded at `gcc_2_8_1_g0_split`, `gcc_2_8_1_g0` and `gcc_2_8_1_g8`, and every member compiles to an identical object at `gcc_2_8_1_g0_split`. Bounded below by `card_list_text_boxes.c` (`gcc_2_8_1_g0` only) and above by `func_80032184` (`gcc_2_8_1_cc_g8_as_g0_split`) |
| `src/game/build_deck_compare.c` | `gcc_2_8_1_g0_split` | `BuildDeck_CompareCard` (`0x80032B60`) and its reverse-primary comparator at `0x80032BD4` |
| `src/game/func_800339D0.c` | `gcc_2_8_1_g8_split` | Build Deck transition exit step (`0x800339D0`) followed by its per-frame colour/step driver (`0x80033BE8`). The driver pulses the two pane objects, then dispatches `D_80090DF8[state & 0x3F]`; the table contains the preceding function as its exit handler. Both use `BuildDeckTransitionState`, and `gcc_2_8_1_g8` functions bound the pair on both sides |
| `src/game/text_box_lifecycle.c` | `gcc_2_8_1_g0` | `TextBox_Destroy` (`0x80035B7C`), `TextBox_Create` (`0x80035BE4`), and contiguous flagged creator `TextBox_CreateFlagged` (`0x80035C38`) |
| `src/game/duel_effect_entry_occupancy.c` | `gcc_2_8_1_g8_split` | Five entry-allocation and marker helpers from `0x80035CA8` through `DuelEffect_ResetEntryMarkers` (`0x80035DF4`) |
| `src/game/func_80036C14.c` | `gcc_2_8_1_g8_split` | Big-endian lookup over the 30-byte records at `D_801D9174` (`0x80036BCC`) followed by the contiguous tagged 28-byte packet append at `0x80036C14`. Both access adjacent text-rendering data symbols; the address-based names remain because that supports the shared subsystem and placement, not stronger semantics |
| `src/game/duel_effect_object_pool.c` | `gcc_2_8_1_g8_split` | External duel-object readiness check (`0x8002C570`) followed by the contiguous eight-entry effect-pool reset and free-entry search helpers through `0x8002C5CC` |
| `src/game/duel_effect_entry_control.c` | `gcc_2_8_1_g8_split` | `DuelEffect_HasActiveEntry` (`0x8003735C`), the following entry marker writer (`0x800373C8`), and contiguous entry-wait reset callback (`0x8003741C`) |
| `src/game/duel_effect_state_callbacks.c` | `gcc_2_8_1_g8` | Sixteen contiguous choice/text-box state functions from `func_800374A8` (`0x800374A8`) through the transfer-wait callback `func_80037B40` (`0x80037B40`): fifteen entries of `D_80090E64` plus `Dialog_OpenChoice`, which the first interaction callback calls and stores on the same `DuelEffectChannel`. The callbacks all dispatch through and update state byte `0x51`; the middle interaction states additionally share the choice object, script-state gate `D_8009B357`, and operand globals, while the final wait callbacks share the display-effect record and completion convention. The former two-function choice object is byte-identical under `gcc_2_8_1_g0` and `gcc_2_8_1_g8`, and `func_8003787C` is byte-identical under `gcc_2_8_1_g8_split` and `gcc_2_8_1_g8`. The lower `func_8003741C` callback remains separate because its text changes size under `gcc_2_8_1_g8`; the upper `func_80037C74` uses `gcc_2_8_1_g0` |
| `src/game/text_stream_commands.c` | `gcc_2_8_1_g8` | Choice-completion callback (`0x80037CE0`) followed by two contiguous text-stream command readers at `0x80037D2C` and `0x80037D6C` |
| `src/game/text_control_commands.c` | `gcc_2_8_1_g8_split` | Nine of the text stream's eleven primary control-byte handlers, contiguous from `Text_DispatchSecondaryCommand` (`0x80038B4C`, F8) through `Text_HandleDisplayEffectCommand` (`0x80038EB0`, F6): the cursor set, choice command, page wait, campaign-flag command, stream push, new line and end of stream between them. The seven former sources were recorded at four profiles, and every member compiles to an identical object at `gcc_2_8_1_g8_split`. Bounded below by the secondary-table object commands in `duel_effect_object_commands.c` and above by the text-box layout helpers `func_80039140` and `func_800391E4` |
| `src/game/text_box_runtime.c` | `gcc_2_8_1_g8` | `TextBox_SetPos` (`0x80039934`) followed by two contiguous blocking text-box build-to-completion helpers at `0x80039A14` and `0x80039A60` |
| `src/game/duel_field_effect_transition.c` | `gcc_2_8_1_g8_split` | Three contiguous field-effect functions: card-object transition callback (`0x80025B28`), the prompt/controller that installs it (`0x80025BEC`), and the companion stat-penalty row sweep (`0x80025D30`). The latter two share the acting-side grid, `D_8009B220` phase flag, `D_8009B260` completion gate, card records, and effect-object creation; the unit keeps 2D and flat same-symbol views of `D_800907D8` for their original addressing forms. Different compiler profiles bound the run on both sides |
| `src/game/duel_deck_card_data.c` | `gcc_2_8_1_cc_g8_as_g0_split` | `Duel_RequestCombinedDeckData` (`0x80024734`) sort/deduplication and asynchronous request setup, followed by contiguous `Duel_PopulateCombinedDeckData` (`0x80024824`) record and asset-block population |
| `src/game/duel_card_effects.c` | `gcc_2_8_1_g8_split` | Five contiguous card-effect handlers from `0x800250C8` through `0x800257A0`: table-driven LP recovery (`0x800250C8`) and direct damage (`0x8002525C`), the effect dispatcher at `0x8002538C`, and the `DuelEffect_UpdateFieldMarker` (`0x800255FC`) and field-card effect completion (`0x800257A0`) state handlers sharing `D_8009B220` flags |
| `src/game/duel_field_equip_search.c` | `gcc_2_8_1_g8_split` | Four contiguous active-side field search helpers: first-free-slot scan (`0x80026C0C`), two card-type collectors (`0x80026C6C`, `0x80026D18`), and their following equip-pair search (`0x80026DC8`), which directly calls the slot scan before constructing a moved equip pairing. All share `D_8009B1D5`, `D_801A7AD8`, and five-slot row conventions. The slot helper is byte-identical under its historical `gcc_2_8_1_g8` profile and this unit's split-address profile |
| `src/game/util_memory.c` | `gcc_2_8_1_g8` | `Util_CopyWords` (`0x800356A0`) and contiguous repeated-byte fill counterpart `Util_FillMemory` (`0x80035748`) |
| `src/game/display_object_brightness.c` | `gcc_2_8_1_g0_split` | Paired display-object RGB setters at `0x80030090` and `0x800300AC`, writing uniform brightness values `0x40` and `0x80` |
| `src/game/display_parent_links.c` | `gcc_2_8_1_g8` | Relative-position parent attachment (`0x80022F98`) and the contiguous one-plus-two-by-five object-row traversal with optional pointer clearing (`0x80022FF0`) |
| `src/game/display_object_position.c` | `gcc_2_8_1_g8` | Three-child position propagation (`0x8003A920`) and the contiguous group-position setter that also invokes it (`0x8003A95C`) |
| `src/game/display_slot_lifecycle.c` | `gcc_2_8_1_g8_split` | Eight contiguous display-slot lifecycle functions: reserved/full-pool free-slot searches (`0x8004002C`, `0x8004006C`), linked-list slot allocation (`0x800400AC`), unlink/free teardown (`0x8004020C`), relinking to a new list (`0x800402A0`), null-safe release (`0x8004036C`), full pool reset (`0x80040390`), and its no-argument wrapper (`0x800403F0`). The relinker directly calls the unlinker while preserving the flags word; the release wrapper calls the same unlinker; the reset clears the same list heads, allocation flags and counters the allocation/free paths maintain. The former singleton relinker and release objects are byte-identical at `gcc_2_8_1_g8_split`, as is the former configuration object that held the reset wrapper. The first selector-writing configuration helper bounds the unit above |
| `src/game/display_object_config.c` | `gcc_2_8_1_g8` | Six contiguous state-byte, resource/color/texture, position, and dimension configuration helpers from `0x80040410` through `0x80040510`; the preceding pool-reset wrapper belongs to the slot lifecycle it invokes |
| `src/game/display_object_updates.c` | `gcc_2_8_1_g0_split` | Three display-list walkers from `0x80040BF8` through `0x80040D14`, invoking object updates and the list-specific render or secondary callbacks |
| `src/game/display_object_stream_state.c` | `gcc_2_8_1_g8_split` | The seven display-object stream opcode handlers, entries 0 through 6 of `D_80090FEC`: five compact stream-state commands from `0x8004141C` through `0x80041464`, including counter resets, a constant-success handler, base-relative cursor jumps, and a flag-`0x800000` toggle, then the random jump (`0x8004149C`) and the four-operand attribute command (`0x80041534`). The five were recorded at `gcc_2_8_1_g8`, and every member compiles to an identical object at `gcc_2_8_1_g8_split`. Bounded below by the light and geometry setup `func_80041340` and above by unmatched assembly |
| `src/game/display_object_helpers.c` | `gcc_2_8_1_g8` | Eighteen contiguous display-object initialization, resource, animation, stream-offset, 8.8-velocity, and scalar-step helpers from `0x800427DC` through `DisplayObject_StepTowardZero` |
| `src/game/display_object_lifecycle.c` | `gcc_2_8_1_g8` | One-shot flag-`0x80` activation test (`0x80042B98`) and contiguous brightness-step callback that destroys the object at zero (`0x80042BC0`) |
| `src/game/display_object_interpolation.c` | `gcc_2_8_1_g0` | Cosine midpoint interpolation (`0x8004318C`) and signed-phase sine interpolation toward a target position (`0x80043230`) |
| `src/game/display_object_transition.c` | `gcc_2_8_1_g0` | Blocking clone/brightness transition (`0x8004365C`) and the contiguous wait helper that permits Start/confirm skip only after the transfer state is idle (`0x800438B8`) |
| `src/game/display_object_list_renderers.c` | `gcc_2_8_1_cc_g8_as_g0_split` | Sibling visible-object list renderers at `0x80040DD8` and `0x80041068` for the `0x38` and `0x3C` packet forms |
| `src/game/display_object_projection.c` | `gcc_2_8_1_g8` | Contiguous packed-angle projection helper (`0x80041E7C`) and display-object transform/projection helper (`0x80041F90`) sharing the scratchpad GTE workspace |
| `src/game/two_player_save_runtime.c` | `gcc_2_8_1_g8_split` | Four contiguous two-player save functions from `SaveData_UpdateLoadPair` (`0x8003F8D4`) through `SaveData_RequestTradeWrite` (`0x8003FE14`). The first state machine loads both memory-card slots and compares their duelist codes; `SaveData_UpdateTradeLoad` seeds its trade message and directly calls it; `SaveData_UpdateDuelLoad` directly calls that wrapper before checking both decks and converting both names; the final request stamps and writes the paired trade results through dialog step 4. All four share `D_8009B3EA`, `D_8009B3ED`, the staged two-slot buffers and dialog state. The former trade-load singleton is byte-identical under `gcc_2_8_1_g8` and `gcc_2_8_1_g8_split`. The unit retains the load-pair jump table at rodata offset `0xCF0`; the preceding single-player write request and following mask-seed setter are separate lifecycles |
| `src/game/duel_result_outro.c` | `gcc_2_8_1_g8_split` | The duel-result outro: the confetti orbit callback `func_80020D4C` (`0x80020D4C`), the fly-off callback `func_80020EE8` (`0x80020EE8`), and the outro sequence `func_80020F4C` (`0x80020F4C`) that spawns the sprites on the first and retargets them at the second. Recorded at `gcc_2_8_1_g8_no_split`, `gcc_2_8_1_g8` and `gcc_2_8_1_g8_split`; each compiles to an identical object at `gcc_2_8_1_g8_split`. Bounded below by `func_80020BE4` (`gcc_2_8_1_g0`) and above by the reward step that begins at `func_80021480` |
| `src/game/duel_rewards.c` | `gcc_2_8_1_g8_split` | `Duel_SelectCardDrop` (`0x80021810`), `Duel_AwardCard` (`0x80021894`) |
| `src/game/duel_card_state_helpers.c` | `gcc_2_8_1_g8` | Duel-card state export (`0x80028220`) and encoded slot normalization (`0x80028260`) |
| `src/game/main_debug.c` | `gcc_2_8_1_g8` | Debug-mode setup wrapper (`0x8002CDE8`), `Main_RunDebugMenu` (`0x8002CE08`) |
| `src/game/script_control_commands.c` | `gcc_2_8_1_g8` | Two script mode setters at `0x8002F930` and `0x8002F94C`, followed by the contiguous script-delay updater at `0x8002F968` |
| `src/game/frontend_scene_states.c` | `gcc_2_8_1_g8` | Eight contiguous scene states, each a step of the same `D_8009B2EB` state machine: the two list-selection states (`0x80030C10`, `0x80030CB0`) and the six debug/frontend, duel-effect mode, memory-card transition, duel setup and effect-start handlers from `0x80030D5C` through `0x80030F80`. They are the whole `gcc_2_8_1_g8` run between `func_80030998` and `func_80030FA0` |
| `src/game/display_object_fade_callbacks.c` | `gcc_2_8_1_g0` | Three contiguous display-object fade callbacks from `0x80039AFC` through `0x80039C94`, sharing initialization flags and frame-step state |
| `src/game/options_screen.c` | `gcc_2_8_1_g8_split` | The options screen, five contiguous functions: its text-colour and text-box setup `func_8003C4E0` (`0x8003C4E0`), the cursor layout pass `Options_UpdateLayout` (`0x8003C568`), `Options_Init` (`0x8003C628`), which calls both, and the input handler (`0x8003C7A0`) and per-frame dispatcher (`0x8003C8CC`). The former sources were recorded at `gcc_2_8_1_g0_split`, `gcc_2_8_1_g8_split`, `gcc_2_8_1_cc_g8_as_g4_no_split` and `gcc_2_8_1_g8`, and every member compiles to an identical object at `gcc_2_8_1_g8_split`. Bounded below by `func_8003C498`, a package-transfer request, and above by `game_over.c` |
| `src/game/game_over.c` | `gcc_2_8_1_g8_split` | Contiguous Game Over setup (`0x8003C950`) and per-frame update (`0x8003CA5C`) |
| `src/game/input_pads.c` | `gcc_2_8_1_g8_split` | The controller runtime, six contiguous functions: `Input_ResetPads` (`0x8003CB7C`), `Input_InitPads` (`0x8003CBE8`), which ends by calling it, the raw controller-packet decoder (`0x8003CC38`) and held/pressed/repeat publisher (`0x8003CCD8`), and the pad-1/pad-2 swap pair `Input_BackupPad1AndUsePad2` (`0x8003CDF8`) and `Input_RestorePad1FromBackup` (`0x8003CE48`). `Input_InitPads` and the swap pair were recorded at `gcc_2_8_1_g8`, but each compiles to an identical object at `gcc_2_8_1_g8_split`. Bounded below by `game_over.c`, whose object does change without split addresses, and above by the `gcc_2_8_1_g8` save-data checksum unit |
| `src/game/save_data_payload.c` | `gcc_2_8_1_g8` | The save payload, nine contiguous functions: the mask generator `SaveData_NextMaskWord` (`0x8003CE74`) and CRC-16/XMODEM calculator (`0x8003CEB8`), the primary/secondary and tertiary seal writers (`0x8003CF14`, `0x8003CFC8`), `SaveData_BuildPayload` (`0x8003D03C`), which calls both, its inverse `SaveData_ApplyRuntimeState` (`0x8003D0F4`), `SaveData_ValidateIntegrity` (`0x8003D174`), which replays the three seals, and the duelist-code and save-sequence comparisons (`0x8003D288`, `0x8003D2B8`). Three members were recorded at `gcc_2_8_1_g8_split` or `gcc_2_8_1_g8_no_split`, but each compiles to an identical object at `gcc_2_8_1_g8`. Bounded below by `input_pads.c`, the controller runtime, and above by `func_8003D300`, a duel-effect state reset whose object does change at `gcc_2_8_1_g8` |
| `src/game/dialog_transition.c` | `gcc_2_8_1_g8` | Three contiguous dialog/card-reveal transition handlers from `0x8003D518` through `0x8003D74C`, sharing display objects and `D_8009B3C1` state bits |
| `src/game/mem_card_dialog_load_save.c` | `gcc_2_8_1_g8_split` | Seven contiguous load/save dialog operations from `MemCardDialog_SetMessage` (`0x8003E46C`) through the empty step-table callback at `0x8003EEC8`: the common message/flag publisher, the load state machine and its prompted/unprompted wrappers, and the save state machine with its wrapper and empty slot. Both machines switch on the low nibble of `D_8009B3EB`, publish through the leading helper, use `D_8009B3C1` as their one-shot entry latch and are adjacent entries in `D_80090F9C`. The three former `gcc_2_8_1_g8` objects are byte-identical under `gcc_2_8_1_g8_split`. Their rodata is one exact run from `0xC10` through `0xCD4`: the load table followed by the save machine's three tables. The zero word at `0xCD4` aligns the next object's table to `0xCD8`, so it remains the upper object boundary |
| `src/game/mem_card_dialog_runtime.c` | `gcc_2_8_1_g8_split` | Seven contiguous memory-card runtime/request functions: the trade save operation (`0x8003EED0`), modal object motion (`0x8003F2B0`), channel setup (`0x8003F388`), the request/dialog dispatcher (`0x8003F454`), then `MemCardDialog_Poll`, `MemCardDialog_Start` and `MemCardDialog_Request` (`0x8003F758`). The public API seeds `D_8009B3DE`, buffer/size/file state and `gMemCard_wDialogFlags`; polling directly pumps the dispatcher, which invokes the selected operation through `D_80090F9C`. The former request API object is byte-identical under `gcc_2_8_1_g8` and `gcc_2_8_1_g8_split`. The unit retains the trade-save switch table at rodata offset `0xCD8`; the preceding aligned zero word and the following `SaveData_RequestLoad` at `gcc_2_8_1_g0_split` bound it |
| `src/game/io_event_helpers.c` | `gcc_2_8_1_g8` | Four-handle event reset/poll helpers (`0x80043D48`, `0x80043DA0`) followed by the contiguous LIBCARD startup wrapper (`0x80043E30`) |
| `src/game/mem_card_init_io_events.c` | `gcc_2_8_1_g8_split` | `MemCard_InitIOEvents` (`0x80043EBC`) and the contiguous bounded `_card_clear` timeout retry helper (`0x80044038`) |
| `src/game/mem_card_requests.c` | `gcc_2_8_1_g8_split` | Six contiguous memory-card request helpers from `0x8004413C` through `0x800443EC`, followed by the directory enumeration helper at `0x80044470` |
| `src/game/mem_card_directory.c` | `gcc_2_8_1_g8` | Memory-card free-block calculation (`0x80044544`) and contiguous directory-name search (`0x80044598`) over 40-byte `DIRENTRY` records |
| `src/game/mem_card_io_result_callbacks.c` | `gcc_2_8_1_g0` | Four contiguous callbacks from `MemCard_SetIOResultCompleteCB` (`0x80044CFC`) through `MemCard_SetIOResultNewCardCB` (`0x80044D34`) |
| `src/game/model_handler_registry.c` | `gcc_2_8_1_g8_split` | The model handler registry and the two maps that search it: the packed-id dispatch maps at `0x8005FC1C` and `0x8005FE44`, `Model_RegisterHandlerKey` (`0x80060170`), `Model_FindHandlerKey` (`0x800601D0`) and the model setup helper at `0x80060220`. Contiguous across all five and bounded on both sides by a profile change, `func_8005FBC4.c` (`gcc_2_8_1_g0`) below and `model_primitive_handler.c` (`gcc_2_8_1_g0_split`) above. The two maps open-code `Model_FindHandlerKey` over the shared registry `D_800F5918` -- same `GsU_00000000` sentinel, same eighty-entry scan, same `-1` on miss -- before dispatching on the id it returns |
| `src/game/model_effect_state.c` | `gcc_2_8_1_g8_split` | Nine contiguous model-effect setup, state and endpoint-queue functions from `func_8005F27C` (`0x8005F27C`) through `func_8005F91C` (`0x8005F91C`). The first function derives a coefficient and directly enters `func_8005F3B8`; `func_8005F714`, `func_8005F7B0` and `func_8005F828` all normalize first/continuation state through `D_8009B07A` before sending endpoint pairs to the final function, which owns the ten-entry `D_800F5788` queue and its start/append/flush modes. Every member shares the `D_8009B07B`/`D_8009B07C` transfer gate. The original seven-function middle object and the former `func_8005F27C.c` object are byte-identical under `gcc_2_8_1_g8` and `gcc_2_8_1_g8_split`; the final dispatcher requires the split profile. The run is bounded below by `model_effect_coefficients.c` (`gcc_2_8_1_g0`) and above by `model_transfer_state.c` (`gcc_2_8_1_g8`) |
| `src/game/model_slot_state_updates.c` | `gcc_2_8_1_g0_split` | Three contiguous model-slot state updates: bounded type selection (`0x8005969C`), which directly starts the positive state when the slot is idle; the state/velocity update (`0x80059700`); and child-parameter propagation (`0x800597C8`). All three index `D_800F2C40`, and the first two share fields `field_E0D` and `field_E16`. The leading function is byte-identical under its historical `gcc_2_8_1_g0` profile and this unit's split profile. The preceding clamped vector/property setter changes under `gcc_2_8_1_g0_split`, while the following file-query wrappers return to non-split `gcc_2_8_1_g0` |
| `src/game/model_scene_setup.c` | `gcc_2_8_1_g8_split` | Seven contiguous model-scene functions from view-angle correction (`0x80052528`) and its range/position controller (`0x80052694`), through timed tint processing (`0x800528AC`) and camera-move setup (`0x80052D2C`), to scene reset (`0x800530C4`), slot properties (`0x80053248`), and cleanup (`0x800533D8`). The controller directly calls the angle helper, scene reset calls the camera-move routine, and cleanup clears the tint state consumed by the tint pass; all share camera, model-slot, request, and `D_8009AFxx` scene state. The run is isolated by unmatched assembly gaps on both sides. `func_800528AC` keeps its `$0` binding function-local because file scope grows the later varargs function by `0xC` |
| `src/game/model_state_getters.c` | `gcc_2_8_1_g8` | Two leaf getters at `0x80058DC0` and `0x80058DCC` returning the model-state bytes at `D_8009AFA6` and `D_8009AFA0` |
| `src/game/model_slot_queries.c` | `gcc_2_8_1_g0_split` | Four by-index reads of one model slot: `field_E0D` (`0x80058E3C`), `field_BF5` (`0x80058E68`), `field_E06 >> 4` (`0x80058E94`), and contiguous `func_80058EC0` (`0x80058EC0`), which reads that same `field_BF5` and returns the `max` of the `field_750` row it selects. `func_80058EC0` was recorded at `gcc_2_8_1_g8_split` and compiles to an identical object at `gcc_2_8_1_g0_split`. The four sizes sum to `0xD4` and the run ends exactly at `func_80058F10`, `0x10` bytes of SDK assembly that bound it above; `func_80058E1C.c` at `gcc_2_8_1_g8` bounds it below |
| `src/game/model_buffer_getters.c` | `gcc_2_8_1_g0_split` | Two leaf pointer getters at `0x80059214` and `0x80059220` returning buffer bases `D_800F56A0` and `D_800FE148` |
| `src/game/model_state_setters.c` | `gcc_2_8_1_g8` | Paired leaf setters at `0x80059AE0` and `0x80059AEC` for the halfword at `D_8009AF92` and byte at `D_8009AFA4` |
| `src/game/model_primitive_handler.c` | `gcc_2_8_1_g0_split` | Primitive-family selector (`0x800603DC`) and `Model_GetPrimitiveHandler` (`0x8006041C`) |
| `src/game/sound_output.c` | `gcc_2_8_1_g8` | Eighteen contiguous sound output reset, initialization, control, command dispatch, default-argument, and sequence-state helpers from `0x80046DE8` through `0x80047458`, including `SD_SetOutputType` |
| `src/game/sound_frontend.c` | `gcc_2_8_1_g8` | Nine game-facing sound initialization and command helpers from `Sound_InitFrontend` (`0x8003FE80`) through `SD_StopAll` (`0x8003FFFC`), including `SD_SEPlayFull` |
| `src/game/sound_init.c` | `gcc_2_8_1_g0` | Thirteen music/sequence and secondary sound-state initialization helpers from `0x80049200` through `0x800495EC`, including `SD_Init` |
| `src/game/sound_effect_voices.c` | `gcc_2_8_1_g0_no_split` | The sound-effect voice slots: the per-id active-voice count (`0x80047FAC`) and the voice start that plays one (`0x8004803C`). The two are contiguous -- 0x80047FAC is 0x90 bytes and ends exactly at 0x8004803C -- and sit between `sound_voice_selection.c` and `func_80048768`. They read and write the same four voice slots: the count walks `g_SDValue->voice_ids` and the start assigns it alongside the flags, volumes and timer |
| `src/game/model_slot_row_tables.c` | `gcc_2_8_1_g0` | Four contiguous model-loader helpers: animation-channel decoding and record-size accumulation (`0x8004D134`), row-table reset and command-list import (`0x8004D58C`), the walk that claims keys and accumulates row maxima (`0x8004D75C`), and the command-list relink pass `func_8004D914` (`0x8004D914`) that rewrites entries using those discovered rows. The tracked `func_8004CB0C` candidate resets the tables before sending each scanned event through the decoder, and matching model-slot setup runs discovery then relinking. The finalizer is byte-identical under its historical `gcc_2_8_1_g0_split` profile and this unit's `gcc_2_8_1_g0`; the following model part update changes to `gcc_2_8_1_g8`. The decoder's `$15` binding must be function-local and non-volatile: file scope changes the reset, while local volatility grows the decoder by one instruction |
| `src/game/model_slot_setup.c` | `gcc_2_8_1_g8_split` | One model slot's setup: the reset that gives it its defaults (`0x8005611C`) and the per-frame duel-side layout pass that reads them (`0x80056250`). Contiguous -- 0x8005611C is 0x134 bytes and ends exactly at 0x80056250 -- and bounded above by `model_load_monster_merge.c` at a different profile. Initializer and consumer: the reset writes the mode byte at +0xE16 as 0x3E, +0xE0C/+0xE0D as 7 and 8 and +0xE0A as 0x1000; the layout pass switches on that same +0xE16 and reads the three back |
| `src/game/model_slot_properties.c` | `gcc_2_8_1_g0` | Six by-index accessors over one model slot, five of them on the three adjacent property blocks the record declares at `0xDA0`, `0xDB0` and `0xDC0`: the part-vertex projection (`0x800593D0`), the `field_DB0` pointer and its writer (`0x8005949C`, `0x800594C0`), the `field_DC0` descriptor and its writer (`0x80059520`, `0x80059590`), and contiguous `func_800595C8` (`0x800595C8`), which clamps three values into `field_DA0` with the same `MODEL_FIXED_*` constants and then sets the byte at `0xE11`. The six sizes sum to `0x2CC`, so the run covers `0x800593D0` up to `func_8005969C`. `func_800595C8` was recorded at `gcc_2_8_1_g8` and compiles to an identical object at `gcc_2_8_1_g0`, which the other five already use. `func_800592AC.c` at `gcc_2_8_1_g8_split` bounds it below; above, `func_8005969C` is at the same `gcc_2_8_1_g0`, so what bounds the run there is meaning rather than the profile -- it sets the byte at `0xE0D` and then calls into `model_slot_state_updates.c`, and it is left as is |
| `src/game/model_update_view_metrics.c` | `gcc_2_8_1_g8_split` | The camera-relative model transform: `Model_UpdateViewMetrics` (`0x80057F38`) and the contiguous `func_800580D4` (`0x800580D4`). Contiguous -- 0x80057F38 is 0x19C bytes and ends exactly at 0x800580D4 -- and bounded on *both* sides by a profile change, `func_80057E20.c` below and `func_800582C0.c` above, both `gcc_2_8_1_g8`, so the run is exactly these two. Producer and sole consumer: `Model_UpdateViewMetrics` writes the cached yaw and pitch `D_8009B47A`/`D_8009B47C` and `func_800580D4` is the only function that reads the pair, as `MODEL_ANGLE_FULL_TURN - D_8009B47A` and `D_8009B47C`; the consumer already included the producer's header |
| `src/game/sound_voice_selection.c` | `gcc_2_8_1_g0` | Twelve contiguous voice update, lifetime, selection, normalization, release, slot-removal, key-off, status, and group-mask helpers from `0x80047864` through `0x80047F38`, including `SD_KeyOffVoiceSlots` |
| `src/game/sound_sequence_state.c` | `gcc_2_8_1_g0` | Two sequence-state setters (`0x800490F0`, `0x80049108`) and the active-state test at `0x80049120` |
| `src/game/sound_sequence_runtime.c` | `gcc_2_8_1_g0` | Six contiguous sequence event-stop/update, byte comparison, bounded read, and MIDI-style variable-length decoding helpers from `0x8004B910` through `0x8004BB34`, ending before the different-profile marker scanner |
| `src/game/sound_sequence_parser.c` | `gcc_2_8_1_g0` | The sequence parser, eight contiguous functions: the fixed-width readers `SD_ReadSequenceU32BE` (`0x8004BC2C`) and `SD_ReadSequenceU16BE` (`0x8004BCA8`), the track and tempo header reader `SD_ReadSequenceHeader` (`0x8004BCE8`) built on them, and the event handlers from `SD_AdvanceSequencePosition` (`0x8004BE6C`) through `SD_DispatchSequenceChannelEvent` (`0x8004C114`). `SD_ReadSequenceHeader` was recorded at `gcc_2_8_1_cc_g8_as_g0_split` but compiles to an identical object at `gcc_2_8_1_g0`. Bounded below by `SD_FindMidiTrackChunk`, which does need `gcc_2_8_1_cc_g8_as_g0_split`, and above by the running-status reader `SD_ReadSequenceEvent` at `gcc_2_8_1_g8_split` |
| `src/game/sound_transfer_lifecycle.c` | `gcc_2_8_1_g0` | Four contiguous helpers from pre-termination cleanup (`0x80049640`) and `SD_Term` through the two transfer-setup functions ending at `0x8004975C` |
| `src/game/sound_buffer_init.c` | `gcc_2_8_1_g0` | Sound work-buffer pointer setup (`0x80044D48`) and channel-volume defaults (`0x80044DA0`) |
| `src/game/sound_mix.c` | `gcc_2_8_1_g0` | Three CD volume and mix helpers from `0x80044E90` through the current-volume query (`0x80044FE4`), including CD mix packet setup at `0x80044F58` |
| `src/game/sound_output_state.c` | `gcc_2_8_1_g0` | Seven contiguous output-state and tagged command-request helpers from `0x8004503C` through `SD_ClearBusyFlag` at `0x8004544C`, retaining the request builders' distinct signatures, tags, callback sequence, inner scopes, and register pins |
| `src/game/sound_runtime.c` | `gcc_2_8_1_g0` | Contiguous command enqueue (`SD_EnqueueCommand`), three-ramp fade update (`SD_UpdateFades`), and per-frame key-status/queue processing (`SD_UpdateRuntime`) from `0x80045BE8` through `0x80045F3C`, all using the shared `SDValue` layout |
| `src/game/sound_state_control.c` | `gcc_2_8_1_g8` | Secondary-state activation (`0x8004695C`) and main sound-state flag setup (`0x80046990`) |
| `src/game/sound_voice_data.c` | `gcc_2_8_1_g0` | Five contiguous voice value/pan update, step assignment, pending-input block copy, lookup rebuild, and reverb/music-state initialization helpers from `0x80048A28` through `0x80048F14`, using canonical `SDValue`, `SDNote`, and `SpuReverbAttr` layouts |
| `src/game/sound_secondary_reset.c` | `gcc_2_8_1_g0` | Low-level state query (`0x800498BC`) and secondary-state reset (`0x800498F8`) |
| `src/game/sound_secondary_playback.c` | `gcc_2_8_1_g0` | Ten secondary sequence attachment, playback lifecycle, object-upload, parameter, and status helpers from `0x80049A64` through `0x80049F50` |
| `src/game/sound_voice_setup.c` | `gcc_2_8_1_cc_g8_as_g0_split` | Per-record voice-parameter refresh (`0x8004A43C`) and the contiguous driver voice/key initialization routine (`0x8004A518`) |
| `src/game/sound_secondary_object_selection.c` | `gcc_2_8_1_g0` | Three contiguous secondary-object best-candidate, referenced-record update, free/reusable-slot, owner/variant, and oldest-entry selection helpers from `0x8004A854` through `0x8004A940`; the shared TU preserves `func_8004A8E4`'s unused second argument from every caller |
| `src/game/sound_secondary_commands.c` | `gcc_2_8_1_g0` | Three secondary-record command setters from `0x8004B49C` through `0x8004B70C`, followed by contiguous `SD_SequenceTimerCallback` (`0x8004B734`) |
| `src/game/color_transform.c` | `gcc_2_8_1_g8` | The complete five-function fixed-point colour family from RGB-to-HSL conversion (`0x8005A98C`) through unpacked RGB tint (`0x8005B0B4`): inverse HSL-to-RGB conversion, packed BGR555 tint, direct HSL-to-BGR555 packing, and the unpacked counterpart. `func_8005B054` is byte-identical under its historical `gcc_2_8_1_g0` profile and this unit's `gcc_2_8_1_g8`; private same-symbol aliases preserve the final two callers' codegen-sensitive discarded-return and wide-argument contracts. The following GPU packet writers form the hard upper boundary |
| `src/game/gpu_packets.c` | `gcc_2_8_1_g8` | The three ordering-table packet writers that share the `D_800FE240` buffer cursor: draw-mode (`0x8005B260`), texture-window (`0x8005B36C`) and mask-write (`0x8005B4D8`) |
| `src/game/triangle_subdivision.c` | `gcc_2_8_1_g0` | Recursive triangle subdivision written twice, over s16 vertices (`0x8006BCA4`) and over three-byte triplets (`0x8006C120`), followed by the triplet version's three primitives: set-from-three (`0x8006C2FC`), copy (`0x8006C30C`) and per-byte mean (`0x8006C330`). Five contiguous functions bounded by unmatched asm on both sides. `func_8006BCA4` was recorded at `gcc_2_8_1_g8`, but its object is identical at `gcc_2_8_1_g0`, so the apparent profile boundary at `0x8006C120` was not one. The triplet subdivider is the only caller of the copy and mean primitives, and `func_8006C2FC` has no caller at all |
| `src/game/duel_shuffle_deck.c` | `gcc_2_8_1_g8_split` | The deck shuffle and its two-sided caller: `Duel_ShuffleDeck` (`0x800243F4`), which builds one side's halfword id list and byte order list and then swaps pairs, followed by contiguous `Duel_ShuffleBothDecks` (`0x800245A0`), which calls it once per side at `gDuel_awPlayerShuffledDeck` and at that base less `0x54` and `0x2C`. `Duel_ShuffleBothDecks` was recorded at `gcc_2_8_1_g8` and compiles to an identical object at `gcc_2_8_1_g8_split`. Bounded below by `duel_scene_update.c` at `gcc_2_8_1_g8` and above by `Duel_StepCardDataTransfer` at `gcc_2_8_1_cc_g0_as_g8_no_split` |

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

Five resident sources are an `__asm__` block of `.word` literals with explicit
`.reloc` directives and no C statements outside it:

- `src/game/func_800291E0.c`
- `src/game/func_80030998.c`
- `src/game/func_8002A788.c`
- `src/game/func_8002A4A8.c`
- `src/game/main_run_credits.c`

They are registered in `matching_c.json` with a compiler profile, but no C is
compiled for them, so the profile is inert and the recorded match reflects
literal bytes rather than codegen. There is nothing to merge, and they cannot
satisfy the per-TU header, single-definition-site, or usage-derived-type work
either. Any candidate scan over matching C should exclude them explicitly;
three otherwise-plausible contiguous same-profile runs are blocked by nothing
except one of these sitting inside them.

This is a different thing from a source carrying a single inline opcode.
`display_projection.c`, `func_800177C4.c`, `func_800178BC.c` and
`func_8001B0CC.c` each contain `.word 0x4A180001`, the GTE `rtps` encoding the
period assembler could not spell, inside ordinary C with real operand
constraints. Those are normal C sources and group normally.

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
