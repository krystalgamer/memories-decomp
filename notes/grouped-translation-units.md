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
| `src/game/duel_card_display_state.c` | `gcc_2_8_1_g8_split` | Display-marker resource selection (`0x80017DB4`), contiguous face/position/used-state visual updates (`0x80017E3C`), and duel-card display-object creation (`0x80017F04`) |
| `src/game/duel_card_object_helpers.c` | `gcc_2_8_1_g8` | Screen-space duel display-object constructor (`0x80018150`) and contiguous card-category encoder (`0x800181EC`), mapping magic/equip, trap, and ritual types to `2`, `3`, and `4` with optional flag `0x80` |
| `src/game/duel_draw_resolution.c` | `gcc_2_8_1_g8_split` | Five-piece Exodia hand predicate (`0x80018CF8`) and the contiguous draw-animation state machine (`0x80018DB4`) that invokes it before resolving victory |
| `src/game/display_object_property_transitions.c` | `gcc_2_8_1_g8` | Three-channel byte convergence to per-channel targets (`0x8001D344`) and the contiguous timed position/interpolation transition with clip-flag lifecycle (`0x8001D3C4`) |
| `src/game/display_object_motion.c` | `gcc_2_8_1_g8` | Timed position interpolation with speed-to-`0x800` completion (`0x8001EC70`) and the contiguous mode-progress variant that stores the final position and clears the clip flag when appropriate (`0x8001ED20`) |
| `src/game/duel_battle_stats.c` | `gcc_2_8_1_g8` | `Duel_CalcBattleAttack` (`0x8001EF1C`), `Duel_CalcBattleDefense` (`0x8001EF78`) |
| `src/game/duel_trap_resolution.c` | `gcc_2_8_1_g8_split` | Contiguous attack-trap selector (`0x8001F0D0`) and its presentation state machine (`0x8001F364`), linked through the selected card-object index in `D_8009B1B8` |
| `src/game/duel_card_turn_animations.c` | `gcc_2_8_1_g8` | Mirrored three-mode card turn-back callback (`0x80022674`, sets record flag `0x400`) and contiguous flip/turn callback (`0x800229F4`, clears `0x400`), both using side-dependent two-phase rotations |
| `src/game/duel_field_display_objects.c` | `gcc_2_8_1_g8_split` | Active-side field-grid index dispatcher (`0x8002348C`) and contiguous display-object constructor (`0x800234E4`) that selects a side-specific resource, positions it from `D_80090800`, installs the projection callback, and stores it on the source record |
| `src/game/duel_cursor_status.c` | `gcc_2_8_1_g8` | Status-returning cursor wrappers: held-pad directional update (`0x80024060`) through `func_80023FBC`, followed by the explicit-direction `func_80023D08` path (`0x80024088`); both return object byte `+0x19` |
| `src/game/duel_scene_update.c` | `gcc_2_8_1_g8` | Per-frame duel-state, effect, and quit-dialog dispatcher (`0x80024200`) followed by its 2P-aware wrapper (`0x80024388`), which temporarily exposes pad 2 as pad 1 for the selected side and restores pad 1 afterward |
| `src/game/duel_card_object_cleanup.c` | `gcc_2_8_1_g8` | Linked display-object release (`0x80024914`), which clears record flag `0x8000` and nulls the released pointer, followed by the contiguous full flag-word reset wrapper (`0x80024954`) |
| `src/game/duel_action_lock.c` | `gcc_2_8_1_g8` | Shared effect-state latch (`0x80024E24`) that sets bit `0x80` and distinguishes first acquisition (`0`) from an already-active update (`1`), followed by the contiguous full-state reset (`0x80024E4C`) |
| `src/game/duel_magic_effect_dispatch.c` | `gcc_2_8_1_g8_split` | Two-stage occupied-row clear effect (`0x80026A3C`) that spawns a type-`0x17` marker before releasing each linked object and clearing its flag word, followed by the non-monster effect-group dispatcher (`0x80026B34`) that selects one handler from the active group's pair through flag `0x4000` |
| `src/game/duel_card_selection.c` | `gcc_2_8_1_g8_split` | Active-side monster-row target selectors for all occupied cards (`0x8002778C`) and face-up cards only (`0x800278A0`); both return a positively ranked display-object index, the center grid entry for an empty row, or `-1` when no occupied candidate wins |
| `src/game/duel_projection_axes.c` | `gcc_2_8_1_g0` | Symmetric three-point projection helpers for the X components (`0x80029684`) and Z components (`0x800297DC`) of `SVECTOR` triplets; both retry `RotAverage3` at half span when the depth/control result is negative |
| `src/game/main_run_selection_menus.c` | `gcc_2_8_1_g8` | Contiguous main-mode runners for Campaign Map (`0x8002D2D8`, package/location setup and per-frame transition), Build Deck (`0x8002D370`, deck-backed setup and completion return), and Free Duel (`0x8002D3F8`, module setup, per-frame dispatch, and exit cleanup) |
| `src/game/main_run_frontend_menus.c` | `gcc_2_8_1_g8` | Six contiguous frontend mode runners: main menu (`0x8002D588`), name entry (`0x8002D62C`), password (`0x8002D684`), Options (`0x8002D6C8`), game over (`0x8002D730`), and the empty `Main_RunHirata` stub (`0x8002D7C4`) |
| `src/game/script_flag_commands.c` | `gcc_2_8_1_g8_split` | Combined library-used/story-flag command (`0x8002E918`) that updates a card flag or conditionally jumps the script cursor, followed by `Script_OpViewportTween` (`0x8002E9A0`) reading target X, target Y, and duration |
| `src/game/script_stream_commands.c` | `gcc_2_8_1_g8` | `Script_OpSound` (`0x8002EC74`), a contiguous one-byte sound-control dispatcher (`0x8002EDB0`), and a 16-bit base-relative script-cursor jump (`0x8002EE20`) |
| `src/game/script_readers.c` | `gcc_2_8_1_g8` | Contiguous script operand readers for one byte (`0x80030050`) and one little-endian 16-bit value (`0x8003006C`), advancing `gScript_pStream` by one or two bytes respectively |
| `src/game/ai_fusion.c` | `gcc_2_8_1_g0_split` | `AiScript_FindEquipTarget` (`0x8007249C`) through `Ai_CompleteFusion` (`0x800727C0`) |
| `src/game/ai_script_vm.c` | `gcc_2_8_1_g0_split` | `AiScript_Init` (`0x800705D8`), `AiScript_Run` (`0x80070650`) |
| `src/game/ai_card_ranges.c` | `gcc_2_8_1_g0_split` | Four contiguous AI card-selection helpers: winning-card (`0x80070738`) and general card (`0x800707C4`) range decoders with their compiler-owned jump tables, followed by card-ID (`0x80070870`) and card-type (`0x800708C4`) set predicates |
| `src/game/ai_script_card_info.c` | `gcc_2_8_1_g0_split` | `AiScript_TestHighStat` (`0x80071194`) through `AiScript_LoadCardID` (`0x800712B4`) |
| `src/game/ai_script_jumps.c` | `gcc_2_8_1_g0_split` | `AiScript_JumpNotEqual` (`0x80070B3C`), `AiScript_JumpBetween` (`0x80070BB8`), `AiScript_JumpRandom` (`0x80070C60`) |
| `src/game/ai_script_end.c` | `gcc_2_8_1_g0` | `AiScript_EndHand` (`0x80070FF8`), `AiScript_EndField` (`0x80071000`) |
| `src/game/ai_script_skip.c` | `gcc_2_8_1_g0` | `AiScript_SkipHand` (`0x80072F1C`), `AiScript_SkipField` (`0x80072F54`) |
| `src/game/ai_script_nop.c` | `gcc_2_8_1_g0` | `AiScript_HandNop` (`0x80073300`), `AiScript_FieldNop` (`0x80073308`) |
| `src/game/ai_script_combo.c` | `gcc_2_8_1_g0_split` | `AiScript_TestPinned` (`0x8007154C`), `AiScript_StartCombo` (`0x800715C4`) |
| `src/game/ai_script_load_best_values.c` | `gcc_2_8_1_g0` | Three contiguous result loaders for best difference (`0x8007164C`), attacker (`0x80071688`), and target (`0x800716C4`), each copying search state into `gAiScript_aMemory` |
| `src/game/ai_script_actions.c` | `gcc_2_8_1_g0_split` | `AiScript_PlayFaceUp` (`0x80072F8C`), `AiScript_SetPosition` (`0x80073050`) |
| `src/game/ai_script_state_ops.c` | `gcc_2_8_1_g0_split` | Four state-flag handlers at `0x80073448`-`0x80073474`, then `AiScript_MoveCard` (`0x80073480`) |
| `src/game/ai_script_support.c` | `gcc_2_8_1_g0` | Combined card/type set query (`Ai_IsCardInSets`, `0x80070920`) and direct jump handler (`AiScript_Jump`, `0x80070988`) |
| `src/game/file_stream.c` | `gcc_2_8_1_g8` | File-state initialization (`0x80013898`), `File_GetPosition`, and three transfer setup helpers through `0x80013A94` |
| `src/game/file_transfer_setup.c` | `gcc_2_8_1_g8_split` | Two contiguous transfer-descriptor setup helpers at `0x80013B04` and `0x80013B68` |
| `src/game/file_cd_transfer.c` | `gcc_2_8_1_g8_split` | Eight asynchronous disc-transfer callbacks and helpers from `0x800140A0` through `File_ActivateTransfer` (`0x800143DC`) |
| `src/game/file_transfer_control.c` | `gcc_2_8_1_g8_split` | Three contiguous transfer-service, object-state, and descriptor-dispatch helpers from `0x80014A5C` through `func_80014C40` |
| `src/game/file_transfer_flags.c` | `gcc_2_8_1_g8` | Six contiguous active/secondary transfer-request and cancellation helpers from `0x80014E1C` through `0x80015078` |
| `src/game/fade_in.c` | `gcc_2_8_1_g8` | Target-`0xFF` transition initializer (`0x80015780`, flag `0x80`, step `0x0C`) and the contiguous flag-`0x01` variant using step `8` (`0x800157DC`) |
| `src/game/fade_color.c` | `gcc_2_8_1_g8_split` | Colour-selecting transition setup (`0x8001581C`, flags `0x30`) and the contiguous conditional white-state reset (`0x80015870`, flags `0xB0`, step `0x0C`) |
| `src/game/fade_out.c` | `gcc_2_8_1_g8` | `Fade_InitOut` (`0x800158B8`, head `0xFF`, target `0`, flag `0x80`, step `0x0C`) and contiguous strip-mode `Fade_StartOut` (`0x80015904`, flag `0x01`, step `8`) |
| `src/game/fade_transition_wait.c` | `gcc_2_8_1_g8_split` | Colour-selecting fade-out setup (`0x80015944`, flags `0x30`) and the contiguous blocking frame-pump loop that waits for flag `0x80` to clear (`0x80015998`) |
| `src/game/fade_control.c` | `gcc_2_8_1_g8` | Eighteen contiguous blocking/nonblocking fade wrappers, direct level/target setters, flag-`0x02`/`0x06` variants, and overlay-latch toggles from `0x800159D8` through `0x80015D0C`, including `Fade_WaitOut` |
| `src/game/display_projection.c` | `gcc_2_8_1_g8_split` | Scratchpad RTPS screen projection with coordinate bias (`0x80015D18`), the contiguous display-slot position wrapper (`0x80015DB8`), and tracked-record projection with side-dependent Y bias (`0x80015DFC`) |
| `src/game/movie_stream_requests.c` | `gcc_2_8_1_g0_split` | Indexed `MOVIE.STR` range setup (`0x8005C388`) and named-file stream setup (`0x8005C464`) |
| `src/game/file_cd_helpers.c` | `gcc_2_8_1_g0` | `File_Exists` (`0x8005C4F0`) and two contiguous low-level CD state/wait helpers through `0x8005C568` |
| `src/game/mdec_sync.c` | `gcc_2_8_1_g8` | MDEC completion-latch setter (`0x8005C5C4`) and contiguous bounded wait/reset helper (`0x8005C5D4`) |
| `src/game/build_deck_card_counts.c` | `gcc_2_8_1_g8` | Card-reference release (`0x80031F7C`) and full Build Deck count reconstruction (`0x8003201C`) |
| `src/game/build_deck_compare.c` | `gcc_2_8_1_g0_split` | `BuildDeck_CompareCard` (`0x80032B60`) and its reverse-primary comparator at `0x80032BD4` |
| `src/game/text_box_lifecycle.c` | `gcc_2_8_1_g0` | `TextBox_Destroy` (`0x80035B7C`), `TextBox_Create` (`0x80035BE4`), and contiguous flagged creator `TextBox_CreateFlagged` (`0x80035C38`) |
| `src/game/duel_effect_entry_occupancy.c` | `gcc_2_8_1_g8_split` | Five entry-allocation and marker helpers from `0x80035CA8` through `DuelEffect_ResetEntryMarkers` (`0x80035DF4`) |
| `src/game/duel_effect_object_pool.c` | `gcc_2_8_1_g8_split` | External duel-object readiness check (`0x8002C570`) followed by the contiguous eight-entry effect-pool reset and free-entry search helpers through `0x8002C5CC` |
| `src/game/duel_effect_entry_control.c` | `gcc_2_8_1_g8_split` | `DuelEffect_HasActiveEntry` (`0x8003735C`), the following entry marker writer (`0x800373C8`), and contiguous entry-wait reset callback (`0x8003741C`) |
| `src/game/dialog_choice_state.c` | `gcc_2_8_1_g0` | Choice-object state initializer (`0x800374A8`) and contiguous `Dialog_OpenChoice` cursor creator (`0x800374F4`) |
| `src/game/duel_effect_interaction_states.c` | `gcc_2_8_1_g8` | Five contiguous choice, effect-script operand/trigger setup, wait, and display-state callbacks from `0x800375A4` through `0x800377C8` |
| `src/game/text_stream_commands.c` | `gcc_2_8_1_g8` | Choice-completion callback (`0x80037CE0`) followed by two contiguous text-stream command readers at `0x80037D2C` and `0x80037D6C` |
| `src/game/text_box_runtime.c` | `gcc_2_8_1_g8` | `TextBox_SetPos` (`0x80039934`) followed by two contiguous blocking text-box build-to-completion helpers at `0x80039A14` and `0x80039A60` |
| `src/game/duel_field_effect_transition.c` | `gcc_2_8_1_g8_split` | Card-object transition callback (`0x80025B28`) and the contiguous prompt/controller that installs it (`0x80025BEC`) |
| `src/game/duel_card_icon_setup.c` | `gcc_2_8_1_g8_split` | Card-type icon object creation (`0x80024C1C`) and the contiguous duel-card slot setup helper (`0x80024D34`) |
| `src/game/duel_deck_card_data.c` | `gcc_2_8_1_cc_g8_as_g0_split` | `Duel_RequestCombinedDeckData` (`0x80024734`) sort/deduplication and asynchronous request setup, followed by contiguous `Duel_PopulateCombinedDeckData` (`0x80024824`) record and asset-block population |
| `src/game/duel_life_point_effects.c` | `gcc_2_8_1_g8_split` | Contiguous table-driven LP recovery (`0x800250C8`) and direct-damage (`0x8002525C`) effect handlers |
| `src/game/duel_field_effect_updates.c` | `gcc_2_8_1_g8_split` | Contiguous duel-field marker (`0x800255FC`) and field-card effect completion (`0x800257A0`) state handlers sharing `D_8009B220` flags |
| `src/game/duel_field_equip_search.c` | `gcc_2_8_1_g8_split` | Two contiguous field-card filters (`0x80026C6C`, `0x80026D18`) and their following equip-pair search (`0x80026DC8`) |
| `src/game/util_memory.c` | `gcc_2_8_1_g8` | `Util_CopyWords` (`0x800356A0`) and contiguous repeated-byte fill counterpart `Util_FillMemory` (`0x80035748`) |
| `src/game/display_object_brightness.c` | `gcc_2_8_1_g0_split` | Paired display-object RGB setters at `0x80030090` and `0x800300AC`, writing uniform brightness values `0x40` and `0x80` |
| `src/game/display_parent_links.c` | `gcc_2_8_1_g8` | Relative-position parent attachment (`0x80022F98`) and the contiguous one-plus-two-by-five object-row traversal with optional pointer clearing (`0x80022FF0`) |
| `src/game/display_object_position.c` | `gcc_2_8_1_g8` | Three-child position propagation (`0x8003A920`) and the contiguous group-position setter that also invokes it (`0x8003A95C`) |
| `src/game/display_slot_lifecycle.c` | `gcc_2_8_1_g8_split` | Reserved/full-pool free-slot searches (`0x8004002C`, `0x8004006C`), linked-list slot initialization (`0x800400AC`), and contiguous unlink/free teardown (`0x8004020C`) |
| `src/game/display_object_config.c` | `gcc_2_8_1_g8` | Seven pool-reset, state-byte, resource/color/texture, position, and dimension configuration helpers from `0x800403F0` through `0x80040510` |
| `src/game/display_object_updates.c` | `gcc_2_8_1_g0_split` | Three display-list walkers from `0x80040BF8` through `0x80040D14`, invoking object updates and the list-specific render or secondary callbacks |
| `src/game/display_object_stream_state.c` | `gcc_2_8_1_g8` | Five compact stream-state commands from `0x8004141C` through `0x80041464`, including counter resets, a constant-success handler, base-relative cursor jumps, and a flag-`0x800000` toggle |
| `src/game/display_object_helpers.c` | `gcc_2_8_1_g8` | Eighteen contiguous display-object initialization, resource, animation, stream-offset, 8.8-velocity, and scalar-step helpers from `0x800427DC` through `func_80042B08` |
| `src/game/display_object_lifecycle.c` | `gcc_2_8_1_g8` | One-shot flag-`0x80` activation test (`0x80042B98`) and contiguous brightness-step callback that destroys the object at zero (`0x80042BC0`) |
| `src/game/display_object_interpolation.c` | `gcc_2_8_1_g0` | Cosine midpoint interpolation (`0x8004318C`) and signed-phase sine interpolation toward a target position (`0x80043230`) |
| `src/game/display_object_transition.c` | `gcc_2_8_1_g0` | Blocking clone/brightness transition (`0x8004365C`) and the contiguous wait helper that permits Start/confirm skip only after the transfer state is idle (`0x800438B8`) |
| `src/game/display_object_list_renderers.c` | `gcc_2_8_1_cc_g8_as_g0_split` | Sibling visible-object list renderers at `0x80040DD8` and `0x80041068` for the `0x38` and `0x3C` packet forms |
| `src/game/display_object_projection.c` | `gcc_2_8_1_g8` | Contiguous packed-angle projection helper (`0x80041E7C`) and display-object transform/projection helper (`0x80041F90`) sharing the scratchpad GTE workspace |
| `src/game/two_player_save_setup.c` | `gcc_2_8_1_g8_split` | Two contiguous two-player save/deck setup helpers at `0x8003FD14` and `0x8003FE14` |
| `src/game/duel_rewards.c` | `gcc_2_8_1_g8_split` | `Duel_SelectCardDrop` (`0x80021810`), `Duel_AwardCard` (`0x80021894`) |
| `src/game/duel_card_state_helpers.c` | `gcc_2_8_1_g8` | Duel-card state export (`0x80028220`) and encoded slot normalization (`0x80028260`) |
| `src/game/main_debug.c` | `gcc_2_8_1_g8` | Debug-mode setup wrapper (`0x8002CDE8`), `Main_RunDebugMenu` (`0x8002CE08`) |
| `src/game/script_control_commands.c` | `gcc_2_8_1_g8` | Two script mode setters at `0x8002F930` and `0x8002F94C`, followed by the contiguous script-delay updater at `0x8002F968` |
| `src/game/frontend_mode_states.c` | `gcc_2_8_1_g8` | Six contiguous debug/frontend, duel-effect mode, memory-card transition, duel setup, and effect-start handlers from `0x80030D5C` through `0x80030F80` |
| `src/game/display_object_fade_callbacks.c` | `gcc_2_8_1_g0` | Three contiguous display-object fade callbacks from `0x80039AFC` through `0x80039C94`, sharing initialization flags and frame-step state |
| `src/game/options_update.c` | `gcc_2_8_1_g8` | Contiguous options input handler (`0x8003C7A0`) and per-frame state dispatcher (`0x8003C8CC`) |
| `src/game/game_over.c` | `gcc_2_8_1_g8_split` | Contiguous Game Over setup (`0x8003C950`) and per-frame update (`0x8003CA5C`) |
| `src/game/input_update_pads.c` | `gcc_2_8_1_g8_split` | Contiguous raw controller-packet decoder (`0x8003CC38`) and held/pressed/repeat publisher (`0x8003CCD8`) |
| `src/game/input_state_backup.c` | `gcc_2_8_1_g8` | `Input_BackupPad1AndUsePad2` (`0x8003CDF8`) and the contiguous `Input_RestorePad1FromBackup` (`0x8003CE48`) counterpart |
| `src/game/save_data_checksum.c` | `gcc_2_8_1_g8` | `SaveData_NextMaskWord` (`0x8003CE74`), the contiguous CRC-16/XMODEM calculator (`0x8003CEB8`), and primary/secondary checksum-mask writer (`0x8003CF14`) |
| `src/game/save_data_validation.c` | `gcc_2_8_1_g8` | `SaveData_HasSameDuelistCode` (`0x8003D288`) and `SaveData_MatchesDuelistAndCurrentSequence` (`0x8003D2B8`) |
| `src/game/dialog_transition.c` | `gcc_2_8_1_g8` | Three contiguous dialog/card-reveal transition handlers from `0x8003D518` through `0x8003D74C`, sharing display objects and `D_8009B3C1` state bits |
| `src/game/mem_card_dialog_runtime.c` | `gcc_2_8_1_g8_split` | Three contiguous memory-card modal helpers from object motion (`0x8003F2B0`) through channel setup (`0x8003F388`) and the request/dialog state machine (`0x8003F454`) |
| `src/game/io_event_helpers.c` | `gcc_2_8_1_g8` | Four-handle event reset/poll helpers (`0x80043D48`, `0x80043DA0`) followed by the contiguous LIBCARD startup wrapper (`0x80043E30`) |
| `src/game/mem_card_init_io_events.c` | `gcc_2_8_1_g8_split` | `MemCard_InitIOEvents` (`0x80043EBC`) and the contiguous bounded `_card_clear` timeout retry helper (`0x80044038`) |
| `src/game/mem_card_requests.c` | `gcc_2_8_1_g8_split` | Six contiguous memory-card request helpers from `0x8004413C` through `0x800443EC`, followed by the directory enumeration helper at `0x80044470` |
| `src/game/mem_card_directory.c` | `gcc_2_8_1_g8` | Memory-card free-block calculation (`0x80044544`) and contiguous directory-name search (`0x80044598`) over 40-byte `DIRENTRY` records |
| `src/game/mem_card_io_result_callbacks.c` | `gcc_2_8_1_g0` | Four contiguous callbacks from `MemCard_SetIOResultCompleteCB` (`0x80044CFC`) through `MemCard_SetIOResultNewCardCB` (`0x80044D34`) |
| `src/game/model_handler_registry.c` | `gcc_2_8_1_g8_split` | `Model_RegisterHandlerKey` (`0x80060170`), `Model_FindHandlerKey` (`0x800601D0`), and the following model setup helper at `0x80060220` |
| `src/game/model_slot_state_updates.c` | `gcc_2_8_1_g0_split` | Contiguous model-slot state/velocity update (`0x80059700`) and child-parameter propagation (`0x800597C8`) |
| `src/game/model_view_adjustments.c` | `gcc_2_8_1_g8_split` | Contiguous model-view angle adjustment (`0x80052528`) and range/position controller (`0x80052694`) |
| `src/game/model_state_getters.c` | `gcc_2_8_1_g8` | Two leaf getters at `0x80058DC0` and `0x80058DCC` returning the model-state bytes at `D_8009AFA6` and `D_8009AFA0` |
| `src/game/model_buffer_getters.c` | `gcc_2_8_1_g0_split` | Two leaf pointer getters at `0x80059214` and `0x80059220` returning buffer bases `D_800F56A0` and `D_800FE148` |
| `src/game/model_state_setters.c` | `gcc_2_8_1_g8` | Paired leaf setters at `0x80059AE0` and `0x80059AEC` for the halfword at `D_8009AF92` and byte at `D_8009AFA4` |
| `src/game/model_handler_dispatch.c` | `gcc_2_8_1_g8_split` | Two contiguous packed-id dispatch maps at `0x8005FC1C` and `0x8005FE44` over the shared model-handler registry |
| `src/game/model_primitive_handler.c` | `gcc_2_8_1_g0_split` | Primitive-family selector (`0x800603DC`) and `Model_GetPrimitiveHandler` (`0x8006041C`) |
| `src/game/ai_script_comparison_jumps.c` | `gcc_2_8_1_g0_split` | `AiScript_JumpGreaterEqual` (`0x800709C0`), `AiScript_JumpGreater` (`0x80070A40`) |
| `src/game/ai_script_call_control.c` | `gcc_2_8_1_g8_split` | Three call-stack and control helpers from `AiScript_Call` (`0x80070D00`) through `AiScript_SetRandom` (`0x80070E20`) |
| `src/game/sound_output.c` | `gcc_2_8_1_g8` | Seventeen sound output initialization, control, command dispatch, default-argument, and sequence-state helpers from `0x80046F58` through `0x80047458`, including `SD_SetOutputType` |
| `src/game/sound_frontend.c` | `gcc_2_8_1_g8` | Nine game-facing sound initialization and command helpers from `Sound_InitFrontend` (`0x8003FE80`) through `SD_StopAll` (`0x8003FFFC`), including `SD_SEPlayFull` |
| `src/game/sound_init.c` | `gcc_2_8_1_g0` | Thirteen music/sequence and secondary sound-state initialization helpers from `0x80049200` through `0x800495EC`, including `SD_Init` |
| `src/game/sound_voice_selection.c` | `gcc_2_8_1_g0` | Nine voice update, lifetime, selection, normalization, release, and slot-removal helpers from `0x80047864` through `0x80047CC4` |
| `src/game/sound_voice_status.c` | `gcc_2_8_1_g0` | `SD_KeyOffVoiceSlots` (`0x80047EC4`) and the adjacent voice-group mask selection helper (`0x80047F38`) |
| `src/game/sound_sequence_state.c` | `gcc_2_8_1_g0` | Two sequence-state setters (`0x800490F0`, `0x80049108`) and the active-state test at `0x80049120` |
| `src/game/sound_sequence_reader.c` | `gcc_2_8_1_g0` | Byte comparison primitive (`0x8004BAA0`), bounded `SD_ReadSequenceByte` (`0x8004BAE4`), and the contiguous MIDI-style variable-length decoder (`0x8004BB34`) |
| `src/game/sound_sequence_values.c` | `gcc_2_8_1_g0` | Fixed-width `SD_ReadSequenceU32BE` (`0x8004BC2C`) and `SD_ReadSequenceU16BE` (`0x8004BCA8`) readers built from the sequence-byte helper |
| `src/game/sound_transfer_lifecycle.c` | `gcc_2_8_1_g0` | Four contiguous helpers from pre-termination cleanup (`0x80049640`) and `SD_Term` through the two transfer-setup functions ending at `0x8004975C` |
| `src/game/sound_buffer_init.c` | `gcc_2_8_1_g0` | Sound work-buffer pointer setup (`0x80044D48`) and channel-volume defaults (`0x80044DA0`) |
| `src/game/sound_mix.c` | `gcc_2_8_1_g0` | Three CD volume and mix helpers from `0x80044E90` through the current-volume query (`0x80044FE4`), including CD mix packet setup at `0x80044F58` |
| `src/game/sound_output_state.c` | `gcc_2_8_1_g0` | Four output-state selection and command-queue helpers from `0x8004503C` through `0x800451E0` |
| `src/game/sound_state_control.c` | `gcc_2_8_1_g8` | Secondary-state activation (`0x8004695C`) and main sound-state flag setup (`0x80046990`) |
| `src/game/sound_voice_data.c` | `gcc_2_8_1_g0` | Voice-step assignment (`0x80048C0C`) and a 512-word transfer helper (`0x80048C70`) |
| `src/game/sound_secondary_reset.c` | `gcc_2_8_1_g0` | Low-level state query (`0x800498BC`) and secondary-state reset (`0x800498F8`) |
| `src/game/sound_secondary_playback.c` | `gcc_2_8_1_g0` | Ten secondary sequence attachment, playback lifecycle, object-upload, parameter, and status helpers from `0x80049A64` through `0x80049F50` |
| `src/game/sound_voice_setup.c` | `gcc_2_8_1_cc_g8_as_g0_split` | Per-record voice-parameter refresh (`0x8004A43C`) and the contiguous driver voice/key initialization routine (`0x8004A518`) |
| `src/game/sound_secondary_object_selection.c` | `gcc_2_8_1_g0` | Secondary-object best-candidate selection (`0x8004A854`) and referenced-record counter update (`0x8004A8E4`) |
| `src/game/sound_secondary_commands.c` | `gcc_2_8_1_g0` | Three secondary-record command setters from `0x8004B49C` through `0x8004B70C`, followed by contiguous `SD_SequenceTimerCallback` (`0x8004B734`) |

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
so `AiScript_Call`, `AiScript_Return`, and `AiScript_SetRandom` build together
in `ai_script_call_control.c`.

## Expansion policy

Expand grouping only after names and behavior are stable. Prefer small
subsystem runs with shared declarations. Do not bridge an assembly function,
mix compiler profiles, reorder functions, or merge a function whose remaining
inline assembly would make later refinement unsafe.
