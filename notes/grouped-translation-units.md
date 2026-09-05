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
| `src/game/duel_card_checks.c` | `gcc_2_8_1_g0_split` | `Duel_CheckEquip` (`0x80019A08`), `Duel_CheckFusion` (`0x80019A60`) |
| `src/game/duel_battle_stats.c` | `gcc_2_8_1_g8` | `Duel_CalcBattleAttack` (`0x8001EF1C`), `Duel_CalcBattleDefense` (`0x8001EF78`) |
| `src/game/ai_fusion.c` | `gcc_2_8_1_g0_split` | `AiScript_FindEquipTarget` (`0x8007249C`) through `Ai_CompleteFusion` (`0x800727C0`) |
| `src/game/ai_script_vm.c` | `gcc_2_8_1_g0_split` | `AiScript_Init` (`0x800705D8`), `AiScript_Run` (`0x80070650`) |
| `src/game/ai_set_queries.c` | `gcc_2_8_1_g0_split` | `Ai_IsCardInSet` (`0x80070870`), `Ai_IsTypeInSet` (`0x800708C4`) |
| `src/game/ai_script_card_info.c` | `gcc_2_8_1_g0_split` | `AiScript_TestHighStat` (`0x80071194`) through `AiScript_LoadCardID` (`0x800712B4`) |
| `src/game/ai_script_jumps.c` | `gcc_2_8_1_g0_split` | `AiScript_JumpNotEqual` (`0x80070B3C`), `AiScript_JumpBetween` (`0x80070BB8`), `AiScript_JumpRandom` (`0x80070C60`) |
| `src/game/ai_script_end.c` | `gcc_2_8_1_g0` | `AiScript_EndHand` (`0x80070FF8`), `AiScript_EndField` (`0x80071000`) |
| `src/game/ai_script_skip.c` | `gcc_2_8_1_g0` | `AiScript_SkipHand` (`0x80072F1C`), `AiScript_SkipField` (`0x80072F54`) |
| `src/game/ai_script_nop.c` | `gcc_2_8_1_g0` | `AiScript_HandNop` (`0x80073300`), `AiScript_FieldNop` (`0x80073308`) |
| `src/game/ai_script_combo.c` | `gcc_2_8_1_g0_split` | `AiScript_TestPinned` (`0x8007154C`), `AiScript_StartCombo` (`0x800715C4`) |
| `src/game/ai_script_actions.c` | `gcc_2_8_1_g0_split` | `AiScript_PlayFaceUp` (`0x80072F8C`), `AiScript_SetPosition` (`0x80073050`) |
| `src/game/ai_script_state_ops.c` | `gcc_2_8_1_g0_split` | Four state-flag handlers at `0x80073448`-`0x80073474`, then `AiScript_MoveCard` (`0x80073480`) |
| `src/game/ai_script_support.c` | `gcc_2_8_1_g0` | Combined card/type set query (`Ai_IsCardInSets`, `0x80070920`) and direct jump handler (`AiScript_Jump`, `0x80070988`) |
| `src/game/file_stream.c` | `gcc_2_8_1_g8` | File-state initialization (`0x80013898`), `File_GetPosition`, and three transfer setup helpers through `0x80013A94` |
| `src/game/file_transfer_setup.c` | `gcc_2_8_1_g8_split` | Two contiguous transfer-descriptor setup helpers at `0x80013B04` and `0x80013B68` |
| `src/game/file_cd_transfer.c` | `gcc_2_8_1_g8_split` | Eight asynchronous disc-transfer callbacks and helpers from `0x800140A0` through `File_ActivateTransfer` (`0x800143DC`) |
| `src/game/file_transfer_control.c` | `gcc_2_8_1_g8_split` | Contiguous transfer-service and object-state callbacks at `0x80014A5C` and `0x80014B30` |
| `src/game/build_deck_compare.c` | `gcc_2_8_1_g0_split` | `BuildDeck_CompareCard` (`0x80032B60`) and its reverse-primary comparator at `0x80032BD4` |
| `src/game/duel_effect_entry_occupancy.c` | `gcc_2_8_1_g8_split` | Five entry-allocation and marker helpers from `0x80035CA8` through `DuelEffect_ResetEntryMarkers` (`0x80035DF4`) |
| `src/game/duel_effect_entry_scan.c` | `gcc_2_8_1_g8_split` | `DuelEffect_HasActiveEntry` (`0x8003735C`) and the following entry marker writer at `0x800373C8` |
| `src/game/util_memory.c` | `gcc_2_8_1_g8` | `Util_CopyWords` (`0x800356A0`) and contiguous repeated-byte fill counterpart `Util_FillMemory` (`0x80035748`) |
| `src/game/duel_rewards.c` | `gcc_2_8_1_g8_split` | `Duel_SelectCardDrop` (`0x80021810`), `Duel_AwardCard` (`0x80021894`) |
| `src/game/main_debug.c` | `gcc_2_8_1_g8` | Debug-mode setup wrapper (`0x8002CDE8`), `Main_RunDebugMenu` (`0x8002CE08`) |
| `src/game/model_handler_registry.c` | `gcc_2_8_1_g8_split` | `Model_RegisterHandlerKey` (`0x80060170`), `Model_FindHandlerKey` (`0x800601D0`), and the following model setup helper at `0x80060220` |
| `src/game/model_primitive_handler.c` | `gcc_2_8_1_g0_split` | Primitive-family selector (`0x800603DC`) and `Model_GetPrimitiveHandler` (`0x8006041C`) |
| `src/game/ai_script_comparison_jumps.c` | `gcc_2_8_1_g0_split` | `AiScript_JumpGreaterEqual` (`0x800709C0`), `AiScript_JumpGreater` (`0x80070A40`) |
| `src/game/ai_script_call_control.c` | `gcc_2_8_1_g8_split` | Three call-stack and control helpers from `AiScript_Call` (`0x80070D00`) through `AiScript_SetRandom` (`0x80070E20`) |
| `src/game/sound_output.c` | `gcc_2_8_1_g8` | Sound output initialization/control helpers from `0x80046F58` through `0x80047278`, including `SD_SetOutputType` |
| `src/game/sound_frontend.c` | `gcc_2_8_1_g8` | Nine game-facing sound initialization and command helpers from `func_8003FE80` through `0x8003FFFC`, including `SD_SEPlayFull` |
| `src/game/sound_init.c` | `gcc_2_8_1_g0` | Thirteen music/sequence and secondary sound-state initialization helpers from `0x80049200` through `0x800495EC`, including `SD_Init` |
| `src/game/sound_voice_selection.c` | `gcc_2_8_1_g0` | Nine voice update, lifetime, selection, normalization, release, and slot-removal helpers from `0x80047864` through `0x80047CC4` |
| `src/game/sound_voice_status.c` | `gcc_2_8_1_g0` | Late voice cleanup (`0x80047EC4`) and voice-group mask selection (`0x80047F38`) |
| `src/game/sound_sequence_state.c` | `gcc_2_8_1_g0` | Two sequence-state setters (`0x800490F0`, `0x80049108`) and the active-state test at `0x80049120` |
| `src/game/sound_term.c` | `gcc_2_8_1_g0` | Pre-termination cleanup (`0x80049640`) and `SD_Term` (`0x80049694`) |
| `src/game/sound_buffer_init.c` | `gcc_2_8_1_g0` | Sound work-buffer pointer setup (`0x80044D48`) and channel-volume defaults (`0x80044DA0`) |
| `src/game/sound_mix.c` | `gcc_2_8_1_g0` | Three CD volume and mix helpers from `0x80044E90` through the current-volume query (`0x80044FE4`), including CD mix packet setup at `0x80044F58` |
| `src/game/sound_output_state.c` | `gcc_2_8_1_g0` | Four output-state selection and command-queue helpers from `0x8004503C` through `0x800451E0` |
| `src/game/sound_state_control.c` | `gcc_2_8_1_g8` | Secondary-state activation (`0x8004695C`) and main sound-state flag setup (`0x80046990`) |
| `src/game/sound_command_wrappers.c` | `gcc_2_8_1_g8` | Four command/sequence wrappers at `0x800473CC-0x80047458` |
| `src/game/sound_voice_data.c` | `gcc_2_8_1_g0` | Voice-step assignment (`0x80048C0C`) and a 512-word transfer helper (`0x80048C70`) |
| `src/game/sound_secondary_reset.c` | `gcc_2_8_1_g0` | Low-level state query (`0x800498BC`) and secondary-state reset (`0x800498F8`) |
| `src/game/sound_secondary_playback.c` | `gcc_2_8_1_g0` | Eight secondary playback lifecycle, object-upload, parameter, and status helpers from `0x80049BAC` through `0x80049F50` |

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
