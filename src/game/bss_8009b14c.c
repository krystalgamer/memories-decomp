#include "../types.h"
#include "../external_funcs.h"
#include "build_deck_card_counts.h"
#include "build_deck_draw_card_list_page.h"
#include "campaign_scene_package.h"
#include "card_grid.h"
#include "debug_effect_screen.h"
#include "debug_value_editor_config.h"
#include "dialog_choice.h"
#include "dialog_choice_state.h"
#include "dialog_transition.h"
#include "display_effect_update_resource_acquisition.h"
#include "display_parent_links.h"
#include "duel_action_lock.h"
#include "duel_card_and_status_draw.h"
#include "duel_card_pick_cursor.h"
#include "duel_card_viewer.h"
#include "duel_check_quit_input.h"
#include "duel_cursor_status.h"
#include "duel_draw_resolution.h"
#include "duel_effect_dialog_state.h"
#include "duel_effect_entry_occupancy.h"
#include "duel_effect_state_callbacks.h"
#include "duel_init_ground_projection_reference.h"
#include "duel_init_scene.h"
#include "duel_result_display.h"
#include "duel_result_localized_sprites.h"
#include "duel_ritual_controller.h"
#include "duel_scene_card_placement.h"
#include "duel_scene_field_actions.h"
#include "duel_scene_hand_actions.h"
#include "duel_view_tween.h"
#include "fade.h"
#include "frontend_debug_tables.h"
#include "graphics_frame.h"
#include "input.h"
#include "main_mode_state.h"
#include "main_reset_frontend_runtime.h"
#include "main_services.h"
#include "mem_card.h"
#include "mem_card_directory.h"
#include "mem_card_work.h"
#include "model_scene_controllers.h"
#include "model_update_view_metrics.h"
#include "movie_frame_pipeline.h"
#include "options.h"
#include "resident_bss.h"
#include "script_op_save_prompt.h"
#include "script_op_show_dialog.h"
#include "script_state.h"
#include "sd_read_sequence_event.h"
#include "sorted_entry.h"
#include "text_box_runtime.h"
#include "text_box_update_channels.h"
#include "text_constants.h"
#include "text_stream_commands.h"
#include "two_player_save_setup.h"
#include "../overlays/duel_effects/card_flash_semantic_views.h"
#include "../overlays/free_duel/free_duel.h"
#include "../overlays/password/dialog_choice_ref.h"

/* Resident .sbss storage from 0x8009B14C to 0x8009B4A8, previously part of the
   generated zero-filled BSS image (#2602). Unnamed bytes keep address names. */
u8 gFade_bTargetRed __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B14D __asm__("D_8009B14D") __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B14E __asm__("D_8009B14E") __attribute__((section(".sbss"))) = 0;
u16 gDuel_wPlacementCardId __attribute__((section(".sbss"))) = 0;
u8 gDuel_bConfirmFlags __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B153 __asm__("D_8009B153") __attribute__((section(".sbss"))) = 0;
s16 gDuel_wPendingEquipBonus __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B156 __asm__("D_8009B156") __attribute__((section(".sbss"))) = 0;
s32 gDuel_dwViewTweenHeadingAccumulator __attribute__((section(".sbss"))) = 0;
s32 gDuel_dwViewTweenOrbitAngleAccumulator __attribute__((section(".sbss"))) = 0;
s8 gDuelCursor_Direction __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B161 __asm__("D_8009B161") __attribute__((section(".sbss"))) = 0;
u16 gDuel_wDisplayTransitionFlags __attribute__((section(".sbss"))) = 0;
u8 gDuel_bQuitDialogState __attribute__((section(".sbss"))) = 0;
u8 gDuel_bWinnerSide __attribute__((section(".sbss"))) = 0;
s16 gDuel_wViewTweenReferenceZTarget __attribute__((section(".sbss"))) = 0;
s32 gDuel_dwViewTweenReferenceZStep __attribute__((section(".sbss"))) = 0;
u16 gDuel_wOutcomeFlags __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B16E __asm__("D_8009B16E") __attribute__((section(".sbss"))) = 0;
u16 gDuel_awSavedStatModifiers __attribute__((section(".sbss"))) = 0;
u16 gDuel_wSavedTargetStatModifier __attribute__((section(".sbss"))) = 0;
u8 gDuel_bSceneSubStep __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B175 __asm__("D_8009B175") __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B176 __asm__("D_8009B176") __attribute__((section(".sbss"))) = 0;
u16 gDuel_awSavedRecordFlags __attribute__((section(".sbss"))) = 0;
u16 gDuel_wSavedTargetRecordFlags __attribute__((section(".sbss"))) = 0;
DuelEffectRequest * gDuelEffect_pTrackedRequest __attribute__((section(".sbss"))) = 0;
u8 * tent_DebugEffectObject0 __attribute__((section(".sbss"))) = 0;
u8 * tent_DebugEffectObject1 __attribute__((section(".sbss"))) = 0;
u32 bss_gDuel_pFlipLeftMarker __asm__("gDuel_pFlipLeftMarker") __attribute__((section(".sbss"))) = 0;
u32 bss_gDuel_pFlipRightMarker __asm__("gDuel_pFlipRightMarker") __attribute__((section(".sbss"))) = 0;
s16 gDuel_wViewTweenHeadingTarget __attribute__((section(".sbss"))) = 0;
s16 gDuel_wViewTweenOrbitAngleTarget __attribute__((section(".sbss"))) = 0;
s32 gDuel_dwViewTweenHeadingStep __attribute__((section(".sbss"))) = 0;
s32 gDuel_dwViewTweenOrbitAngleStep __attribute__((section(".sbss"))) = 0;
u8 gDuel_bCardMoveSlot __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B19D __asm__("D_8009B19D") __attribute__((section(".sbss"))) = 0;
s16 gDuel_wAiHandStackPending __attribute__((section(".sbss"))) = 0;
s16 gDuel_RitualResultCardId __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B1A2 __asm__("D_8009B1A2") __attribute__((section(".sbss"))) = 0;
u16 bss_gDuel_awBattleDamage __asm__("gDuel_awBattleDamage") __attribute__((section(".sbss"))) = 0;
u16 bss_D_8009B1A6 __asm__("D_8009B1A6") __attribute__((section(".sbss"))) = 0;
s16 gDuel_sCardEffectIndex __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B1AA __asm__("D_8009B1AA") __attribute__((section(".sbss"))) = 0;
s16 gDuel_wMonsterRemovalCriterion __attribute__((section(".sbss"))) = 0;
s16 gDuel_wMonsterRemovalZone __attribute__((section(".sbss"))) = 0;
u8 bss_gDuel_abBattleOutcome __asm__("gDuel_abBattleOutcome") __attribute__((section(".sbss"))) = 0;
u8 bss_D_8009B1B1 __asm__("D_8009B1B1") __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B1B2 __asm__("D_8009B1B2") __attribute__((section(".sbss"))) = 0;
DuelSelectionRecord * gDuel_pActiveSelectionRecord __attribute__((section(".sbss"))) = 0;
u8 gDuel_bSelectedTrapSlot __attribute__((section(".sbss"))) = 0;
u8 D_8009B1B9 __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B1BA __asm__("D_8009B1BA") __attribute__((section(".sbss"))) = 0;
s32 gDuel_dwFieldDimTarget __attribute__((section(".sbss"))) = 0;
DisplayObject * gDuel_pCardPreviewObject __attribute__((section(".sbss"))) = 0;
s32 gDuel_dwViewTweenDistanceAccumulator __attribute__((section(".sbss"))) = 0;
DuelSideState * gDuel_pActiveSideState __attribute__((section(".sbss"))) = 0;
u32 bss_gDuel_pActionCard __asm__("gDuel_pActionCard") __attribute__((section(".sbss"))) = 0;
u16 gDuel_wStepCounter __attribute__((section(".sbss"))) = 0;
s16 gDuel_wEffectCardID __attribute__((section(".sbss"))) = 0;
u8 gDuelCursor_PickFlags __attribute__((section(".sbss"))) = 0;
u8 gDuel_bActiveSide __attribute__((section(".sbss"))) = 0;
u8 gDuelCursor_TargetColumn __attribute__((section(".sbss"))) = 0;
u8 gDuelCursor_TargetRow __attribute__((section(".sbss"))) = 0;
u8 * gDuel_pSide0SaveData __attribute__((section(".sbss"))) = 0;
u8 * gDuel_pSide1SaveData __attribute__((section(".sbss"))) = 0;
u16 gDuelResult_wBgmId __attribute__((section(".sbss"))) = 0;
u8 gDuel_bAiSelectionCursor __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B1E3 __asm__("D_8009B1E3") __attribute__((section(".sbss"))) = 0;
u16 gDuel_wAiSelectionStepDelay __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B1E6 __asm__("D_8009B1E6") __attribute__((section(".sbss"))) = 0;
DuelResultDisplayState * gDuel_pResultDisplayState __attribute__((section(".sbss"))) = 0;
u8 gDuel_bPendingDrawCount __attribute__((section(".sbss"))) = 0;
u8 gDuel_bDrawStepCountdown __attribute__((section(".sbss"))) = 0;
s16 gDuel_wViewTweenDistanceTarget __attribute__((section(".sbss"))) = 0;
DuelEffectRequest
    * gDuel_apSwordsEffectObjects[2 ] __attribute__((section(".sbss"))) = {0};
u32 bss_gDuel_pActionReadyMarker __asm__("gDuel_pActionReadyMarker") __attribute__((section(".sbss"))) = 0;
s32 gDuel_dwViewTweenDistanceStep __attribute__((section(".sbss"))) = 0;
s16 D_8009B200 __attribute__((section(".sbss"))) = 0;
s16 D_8009B202 __attribute__((section(".sbss"))) = 0;
s16 gDuel_wViewTweenFramesRemaining __attribute__((section(".sbss"))) = 0;
s16 gDuel_wEquipCardId __attribute__((section(".sbss"))) = 0;
u8 bss_D_8009B208 __asm__("D_8009B208") __attribute__((section(".sbss"))) = 0;
u8 bss_D_8009B209 __asm__("D_8009B209") __attribute__((section(".sbss"))) = 0;
u16 bss_D_8009B20A __asm__("D_8009B20A") __attribute__((section(".sbss"))) = 0;
u16 bss_D_8009B20C __asm__("D_8009B20C") __attribute__((section(".sbss"))) = 0;
u16 bss_D_8009B20E __asm__("D_8009B20E") __attribute__((section(".sbss"))) = 0;
u16 gDuel_wTrapPresentationMode __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B212 __asm__("D_8009B212") __attribute__((section(".sbss"))) = 0;
DisplayObject * gDuel_pTerrainIcon __attribute__((section(".sbss"))) = 0;
s8 D_8009B218 __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B219 __asm__("D_8009B219") __attribute__((section(".sbss"))) = 0;
u8 D_8009B21A __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B21B __asm__("D_8009B21B") __attribute__((section(".sbss"))) = 0;
DisplayObject * gDuel_pLifePointsPanel __attribute__((section(".sbss"))) = 0;
u16 gDuel_wCardEffectFlags __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B222 __asm__("D_8009B222") __attribute__((section(".sbss"))) = 0;
s32 gDuel_dwViewTweenReferenceZAccumulator __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B228 __asm__("D_8009B228") __attribute__((section(".sbss"))) = 0;
u8 gDuel_bAnimatedBattleRequested __attribute__((section(".sbss"))) = 0;
s16 gDuel_wSelectedTrapCardId __attribute__((section(".sbss"))) = 0;
u8 * gDuel_pActiveFieldGrid __attribute__((section(".sbss"))) = 0;
u16 gDuel_wTwoPlayerCardViewMode __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B232 __asm__("D_8009B232") __attribute__((section(".sbss"))) = 0;
u16 gDuel_wTwoPlayerSide0LifePoints __attribute__((section(".sbss"))) = 0;
u16 gDuel_wTwoPlayerSide1LifePoints __attribute__((section(".sbss"))) = 0;
s8 gDuel_bOutroWinnerSide __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B239 __asm__("D_8009B239") __attribute__((section(".sbss"))) = 0;
u16 D_8009B23A __attribute__((section(".sbss"))) = 0;
u32 bss_pad_8009B23C __asm__("D_8009B23C") __attribute__((section(".sbss"))) = 0;
DisplayObject * gDuel_pCardViewerBackground __attribute__((section(".sbss"))) = 0;
u16 gDuel_wEffectDialogTextID __attribute__((section(".sbss"))) = 0;
u16 gDuel_wViewerCardID __attribute__((section(".sbss"))) = 0;
u8 D_8009B248 __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B249 __asm__("D_8009B249") __attribute__((section(".sbss"))) = 0;
u8 gDuel_bActiveEffectState __attribute__((section(".sbss"))) = 0;
u8 gDuel_bCardViewerYOffset __attribute__((section(".sbss"))) = 0;
DisplayObject * gDuel_pCardViewerCard __attribute__((section(".sbss"))) = 0;
DuelEffectChannel * gDuel_pCardViewerTextBox __attribute__((section(".sbss"))) = 0;
u8 gDuel_bEffectState __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B255 __asm__("D_8009B255") __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B256 __asm__("D_8009B256") __attribute__((section(".sbss"))) = 0;
s8 gCardGrid_bCursorColumn __attribute__((section(".sbss"))) = 0;
s8 gCardGrid_bCursorRow __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B25A __asm__("D_8009B25A") __attribute__((section(".sbss"))) = 0;
u32 bss_pad_8009B25C __asm__("D_8009B25C") __attribute__((section(".sbss"))) = 0;
u8 gDuel_bEffectRequestStatus __attribute__((section(".sbss"))) = 0;
u8 gDuelEffect_bRequestFinished __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B262 __asm__("D_8009B262") __attribute__((section(".sbss"))) = 0;
DuelEffectRequest * gDuelEffect_pActiveRequest __attribute__((section(".sbss"))) = 0;
u8 D_8009B268 __attribute__((section(".sbss"))) = 0;
u8 D_8009B269 __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B26A __asm__("D_8009B26A") __attribute__((section(".sbss"))) = 0;
u8 bss_D_8009B26C __asm__("D_8009B26C") __attribute__((section(".sbss"))) = 0;
u8 D_8009B26D __attribute__((section(".sbss"))) = 0;
u8 gMain_bModeRunnerStep __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B26F __asm__("D_8009B26F") __attribute__((section(".sbss"))) = 0;
u16 gScript_ImageOperand __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B272 __asm__("D_8009B272") __attribute__((section(".sbss"))) = 0;
MenuRecord * D_8009B274 __attribute__((section(".sbss"))) = 0;
s16 gScript_WaitCounter __attribute__((section(".sbss"))) = 0;
u16 bss_gCampaignSceneIndex __asm__("gCampaignSceneIndex") __attribute__((section(".sbss"))) = 0;
u16 gScript_wCommandWord __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B27E __asm__("D_8009B27E") __attribute__((section(".sbss"))) = 0;
struct DisplayObject * gScript_TransitionObject __attribute__((section(".sbss"))) = 0;
s32 gScript_ViewportAccumulatorX __attribute__((section(".sbss"))) = 0;
s32 gScript_ViewportAccumulatorY __attribute__((section(".sbss"))) = 0;
u16 gScript_DialogCommandLatch __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B28E __asm__("D_8009B28E") __attribute__((section(".sbss"))) = 0;
u8 * gScript_StreamCursor __attribute__((section(".sbss"))) = 0;
s32 gScript_ViewportDeltaX __attribute__((section(".sbss"))) = 0;
s32 gScript_ViewportDeltaY __attribute__((section(".sbss"))) = 0;
u16 gScript_SharedOperand __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B29E __asm__("D_8009B29E") __attribute__((section(".sbss"))) = 0;
DisplayObject * gCampaign_PrimaryDisplayObject __attribute__((section(".sbss"))) = 0;
u16 gScript_EventState __attribute__((section(".sbss"))) = 0;
u16 gScript_SavePromptSceneOperand __attribute__((section(".sbss"))) = 0;
u16 gScript_ViewportTargetX __attribute__((section(".sbss"))) = 0;
u16 gScript_ViewportTargetY __attribute__((section(".sbss"))) = 0;
u32 bss_pad_8009B2AC __asm__("D_8009B2AC") __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B2B0 __asm__("D_8009B2B0") __attribute__((section(".sbss"))) = 0;
u8 D_8009B2B2 __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B2B3 __asm__("D_8009B2B3") __attribute__((section(".sbss"))) = 0;
u8 tent_DebugInterfaceFieldB4 __attribute__((section(".sbss"))) = 0;
u8 tent_DebugInterfaceFieldB5 __attribute__((section(".sbss"))) = 0;
u8 tent_DebugInterfaceFieldB6 __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B2B7 __asm__("D_8009B2B7") __attribute__((section(".sbss"))) = 0;
u8 tent_DebugInterfaceFieldB8 __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B2B9 __asm__("D_8009B2B9") __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B2BA __asm__("D_8009B2BA") __attribute__((section(".sbss"))) = 0;
u8 gDebugMenu_bCampaignEditField __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B2BD __asm__("D_8009B2BD") __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B2BE __asm__("D_8009B2BE") __attribute__((section(".sbss"))) = 0;
u8 tent_DebugInterfaceFieldC0 __attribute__((section(".sbss"))) = 0;
u8 tent_DebugInterfaceFieldC1 __attribute__((section(".sbss"))) = 0;
u8 tent_DebugInterfaceFieldC2 __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B2C3 __asm__("D_8009B2C3") __attribute__((section(".sbss"))) = 0;
u16 gDebug_nLastSoundID __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B2C6 __asm__("D_8009B2C6") __attribute__((section(".sbss"))) = 0;
u16 gDebug_nSceneOrSoundID __attribute__((section(".sbss"))) = 0;
u16 gDebug_nEditedBgmId __attribute__((section(".sbss"))) = 0;
u16 gDebug_nEditedCdSoundId __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B2CE __asm__("D_8009B2CE") __attribute__((section(".sbss"))) = 0;
u32 bss_pad_8009B2D0[2] __asm__("D_8009B2D0") __attribute__((section(".sbss"))) = {0};
u16 bss_pad_8009B2D8 __asm__("D_8009B2D8") __attribute__((section(".sbss"))) = 0;
u16 gDebug_nLastBgmId __attribute__((section(".sbss"))) = 0;
s8 gDebug_bEditorRow __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B2DD __asm__("D_8009B2DD") __attribute__((section(".sbss"))) = 0;
u16 D_8009B2DE __attribute__((section(".sbss"))) = 0;
u8 tent_DebugInterfaceFieldE0 __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B2E1 __asm__("D_8009B2E1") __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B2E2 __asm__("D_8009B2E2") __attribute__((section(".sbss"))) = 0;
DisplayObject * gDebugMenu_pCursorQuad __attribute__((section(".sbss"))) = 0;
u8 gDebugMenu_bLastMapId __attribute__((section(".sbss"))) = 0;
s8 gDebug_bEditorDigit __attribute__((section(".sbss"))) = 0;
u8 tent_DebugInterfaceFlags __attribute__((section(".sbss"))) = 0;
u8 gDebugMenu_bStep __attribute__((section(".sbss"))) = 0;
u8 * tent_DebugInterfaceData __attribute__((section(".sbss"))) = 0;
u8 gDebugMenu_bPage __attribute__((section(".sbss"))) = 0;
s8 gDebugMenu_bCursor __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B2F2 __asm__("D_8009B2F2") __attribute__((section(".sbss"))) = 0;
u32 bss_pad_8009B2F4 __asm__("D_8009B2F4") __attribute__((section(".sbss"))) = 0;
u8 gBuildDeck_bConfirmMode __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B2F9 __asm__("D_8009B2F9") __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B2FA __asm__("D_8009B2FA") __attribute__((section(".sbss"))) = 0;
BuildDeckTransitionState * gBuildDeck_pState __attribute__((section(".sbss"))) = 0;
u32 gModel_dwFogBaseColor __attribute__((section(".sbss"))) = 0;
SortedEntry * gSortedEntry_pBase __attribute__((section(".sbss"))) = 0;
u32 gSortedEntry_nSortedCount __attribute__((section(".sbss"))) = 0;
u32 gModel_dwFogDrawFlags __attribute__((section(".sbss"))) = 0;
SortedEntry * gSortedEntry_pCursor __attribute__((section(".sbss"))) = 0;
u32 gSortedEntry_nCount __attribute__((section(".sbss"))) = 0;
u8 gMovie_bPlaybackState __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B319 __asm__("D_8009B319") __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B31A __asm__("D_8009B31A") __attribute__((section(".sbss"))) = 0;
u32 bss_pad_8009B31C __asm__("D_8009B31C") __attribute__((section(".sbss"))) = 0;
u8 gText_SelectedCardColorIndex __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B321 __asm__("D_8009B321") __attribute__((section(".sbss"))) = 0;
s16 gText_WaitCounter __attribute__((section(".sbss"))) = 0;
u8 gDuelEffect_OccupancyStartColumn __attribute__((section(".sbss"))) = 0;
u8 gDuelEffect_OccupancyStartRow __attribute__((section(".sbss"))) = 0;
s8 gDisplayEffect_ResourceCacheWriteSlot __attribute__((section(".sbss"))) = 0;
u8 gDialog_bInputState __attribute__((section(".sbss"))) = 0;
MenuRecord * gText_DisplayEffectRecord __attribute__((section(".sbss"))) = 0;
s8 gText_PageAdvanceHoldCountdown __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B32D __asm__("D_8009B32D") __attribute__((section(".sbss"))) = 0;
u16 gText_StagedStringId __attribute__((section(".sbss"))) = 0;
s32 D_8009B330 __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B334 __asm__("D_8009B334") __attribute__((section(".sbss"))) = 0;
u8 gText_CdSoundWaitPhase __attribute__((section(".sbss"))) = 0;
u8 gDialog_bChoiceEnabled __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B337 __asm__("D_8009B337") __attribute__((section(".sbss"))) = 0;
s16 gDuel_wSelectedCardID __attribute__((section(".sbss"))) = 0;
volatile u16 gText_CurrentStreamCode __attribute__((section(".sbss"))) = 0;
s16 gText_CdSoundPollBudget __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B33E __asm__("D_8009B33E") __attribute__((section(".sbss"))) = 0;
u32 bss_gText_ChoiceLayoutCallback __asm__("gText_ChoiceLayoutCallback") __attribute__((section(".sbss"))) = 0;
u8 gText_GuardianGlyphCode __attribute__((section(".sbss"))) = 0;
s8 gDialog_bChoiceCount __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B346 __asm__("D_8009B346") __attribute__((section(".sbss"))) = 0;
u16 gText_ShakeViewportOrigin[2] __attribute__((section(".sbss"))) = {0};
u8 D_8009B34C __attribute__((section(".sbss"))) = 0;
s8 D_8009B34D __attribute__((section(".sbss"))) = 0;
u8 gText_BranchSelectorA __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B34F __asm__("D_8009B34F") __attribute__((section(".sbss"))) = 0;
volatile s32 gText_CommandDisposition __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B354 __asm__("D_8009B354") __attribute__((section(".sbss"))) = 0;
u8 gText_BranchSelectorB __attribute__((section(".sbss"))) = 0;
u8 gText_PreviousGlyphKind __attribute__((section(".sbss"))) = 0;
u8 gText_ScriptDispatchGate __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B358 __asm__("D_8009B358") __attribute__((section(".sbss"))) = 0;
s16 gText_PumpedGlyphIndex __attribute__((section(".sbss"))) = 0;
u32 bss_pad_8009B35C __asm__("D_8009B35C") __attribute__((section(".sbss"))) = 0;
s8 D_8009B360 __attribute__((section(".sbss"))) = 0;
u8 bss_gDuel_bOpponentID __asm__("gDuel_bOpponentID") __attribute__((section(".sbss"))) = 0;
u8 gDuel_bPlayerLost __attribute__((section(".sbss"))) = 0;
u8 bss_D_8009B363 __asm__("D_8009B363") __attribute__((section(".sbss"))) = 0;
u8 bss_gDuel_bTerrain __asm__("gDuel_bTerrain") __attribute__((section(".sbss"))) = 0;
u8 gFreeDuel_bReturnFlags __attribute__((section(".sbss"))) = 0;
s8 gFreeDuel_bCursorColumn __attribute__((section(".sbss"))) = 0;
s8 gFreeDuel_bCursorRow __attribute__((section(".sbss"))) = 0;
u8 D_8009B368 __attribute__((section(".sbss"))) = 0;
u8 D_8009B369 __attribute__((section(".sbss"))) = 0;
u16 gDuel_wBgmId __attribute__((section(".sbss"))) = 0;
s8 gFreeDuel_bTargetColumn __attribute__((section(".sbss"))) = 0;
s8 gFreeDuel_bTargetRow __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B36E __asm__("D_8009B36E") __attribute__((section(".sbss"))) = 0;
u16 D_8009B370 __attribute__((section(".sbss"))) = 0;
u16 D_8009B372 __attribute__((section(".sbss"))) = 0;
u16 D_8009B374 __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B376 __asm__("D_8009B376") __attribute__((section(".sbss"))) = 0;
s32 gGameOver_pTitleObject __attribute__((section(".sbss"))) = 0;
u8 gOptions_bState __attribute__((section(".sbss"))) = 0;
s8 gOptions_bOutputType __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B37E __asm__("D_8009B37E") __attribute__((section(".sbss"))) = 0;
DisplayObject * gOptions_pOutputCursor __attribute__((section(".sbss"))) = 0;
s8 gOptions_bSelection __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B385 __asm__("D_8009B385") __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B386 __asm__("D_8009B386") __attribute__((section(".sbss"))) = 0;
DisplayObject * gOptions_pSelectionCursor __attribute__((section(".sbss"))) = 0;
u32 bss_pad_8009B38C __asm__("D_8009B38C") __attribute__((section(".sbss"))) = 0;
u32 gInput_dwPreviousHeld __attribute__((section(".sbss"))) = 0;
u16 gInput_wPad1Repeat __attribute__((section(".sbss"))) = 0;
u16 gInput_wPad2Repeat __attribute__((section(".sbss"))) = 0;
u16 gInput_wPad1Pressed __attribute__((section(".sbss"))) = 0;
u16 gInput_wPad2Pressed __attribute__((section(".sbss"))) = 0;
u8 gInput_bRepeatDelay __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B39D __asm__("D_8009B39D") __attribute__((section(".sbss"))) = 0;
u16 gInput_wPad1RepeatBackup __attribute__((section(".sbss"))) = 0;
u16 gInput_wPad1PressedBackup __attribute__((section(".sbss"))) = 0;
u8 gInput_bRepeatInterval __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B3A3 __asm__("D_8009B3A3") __attribute__((section(".sbss"))) = 0;
u16 gInput_wPad1Held __attribute__((section(".sbss"))) = 0;
u16 gInput_wPad2Held __attribute__((section(".sbss"))) = 0;
u32 gInput_dwPendingHeld __attribute__((section(".sbss"))) = 0;
u16 gInput_wPad1HeldBackup __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B3AE __asm__("D_8009B3AE") __attribute__((section(".sbss"))) = 0;
u32 gInput_dwDeferredRepeat __attribute__((section(".sbss"))) = 0;
u32 gInput_dwDeferredPressed __attribute__((section(".sbss"))) = 0;
s32 gSaveDataSequence __attribute__((section(".sbss"))) = 0;
s32 gMemCardDialog_SyncStatus __attribute__((section(".sbss"))) = 0;
u8 gSaveData_PairMessageCode __attribute__((section(".sbss"))) = 0;
u8 D_8009B3C1 __attribute__((section(".sbss"))) = 0;
u16 gMemCardDialog_TransferSize __attribute__((section(".sbss"))) = 0;
u16 gMemCardDialog_TransferOffset __attribute__((section(".sbss"))) = 0;
u8 gMemCardDialog_MessageCode __attribute__((section(".sbss"))) = 0;
u8 gMemCardWork_OperationMode __attribute__((section(".sbss"))) = 0;
s32 D_8009B3C8 __attribute__((section(".sbss"))) = 0;
u16 D_8009B3CC __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B3CE __asm__("D_8009B3CE") __attribute__((section(".sbss"))) = 0;
u8 gMemCardWork_OperationState __attribute__((section(".sbss"))) = 0;
u8 * gMemCard_pPrimaryTransferCursor __attribute__((section(".sbss"))) = 0;
u8 gSaveData_AllowIdentityBypass __attribute__((section(".sbss"))) = 0;
u8 D_8009B3D5 __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B3D6 __asm__("D_8009B3D6") __attribute__((section(".sbss"))) = 0;
DisplayObject * gMemCard_pDialogObject __attribute__((section(".sbss"))) = 0;
u8 gMemCardDialog_BlockCount __attribute__((section(".sbss"))) = 0;
u8 D_8009B3DD __attribute__((section(".sbss"))) = 0;
u8 gMemCardDialog_StepIndex __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B3DF __asm__("D_8009B3DF") __attribute__((section(".sbss"))) = 0;
u8 * gMemCard_pSecondaryTransferCursor __attribute__((section(".sbss"))) = 0;
s32 D_8009B3E4 __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B3E8 __asm__("D_8009B3E8") __attribute__((section(".sbss"))) = 0;
u8 gSaveData_LoadPairState __attribute__((section(".sbss"))) = 0;
u8 gMemCardDialog_OperationState __attribute__((section(".sbss"))) = 0;
u8 gMemCardDialog_RetryCounter __attribute__((section(".sbss"))) = 0;
u8 gSaveData_PairSetupState __attribute__((section(".sbss"))) = 0;
u8 gMemCardDialog_ChannelIndex __attribute__((section(".sbss"))) = 0;
u8 gMemCardDialog_Outcome __attribute__((section(".sbss"))) = 0;
s32 gMemCardDialog_SyncCommand __attribute__((section(".sbss"))) = 0;
s32 gMemCardDialog_SyncResult __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B3F8 __asm__("D_8009B3F8") __attribute__((section(".sbss"))) = 0;
u8 gMemCardDialog_Port __attribute__((section(".sbss"))) = 0;
u16 gMemCard_wDialogFlags __attribute__((section(".sbss"))) = 0;
u32 bss_pad_8009B3FC __asm__("D_8009B3FC") __attribute__((section(".sbss"))) = 0;
u32 gSD_dwCurrentBgmCommand __attribute__((section(".sbss"))) = 0;
u32 D_8009B404 __attribute__((section(".sbss"))) = 0;
s8 gSD_bOutputType __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B409 __asm__("D_8009B409") __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B40A __asm__("D_8009B40A") __attribute__((section(".sbss"))) = 0;
u32 bss_pad_8009B40C __asm__("D_8009B40C") __attribute__((section(".sbss"))) = 0;
u16 gDisplayObject_wUnlinkCount __attribute__((section(".sbss"))) = 0;
u16 gDisplayObject_wAcquireCount __attribute__((section(".sbss"))) = 0;
u32 bss_pad_8009B414[4] __asm__("D_8009B414") __attribute__((section(".sbss"))) = {0};
s32 gDisplayObject_fRenderRetry __attribute__((section(".sbss"))) = 0;
u8 gMain_bFrontendPhase __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B429 __asm__("D_8009B429") __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B42A __asm__("D_8009B42A") __attribute__((section(".sbss"))) = 0;
u32 bss_pad_8009B42C __asm__("D_8009B42C") __attribute__((section(".sbss"))) = 0;
s32 gMemCard_pRequestBuf __attribute__((section(".sbss"))) = 0;
u16 gMemCard_wRequestSize __attribute__((section(".sbss"))) = 0;
u8 D_8009B436 __attribute__((section(".sbss"))) = 0;
u8 gMemCard_bChannel __attribute__((section(".sbss"))) = 0;
s32 gMemCard_nFreeBlocks __attribute__((section(".sbss"))) = 0;
u8 gMemCard_bRetries __attribute__((section(".sbss"))) = 0;
u8 gMemCard_bLoadStep __attribute__((section(".sbss"))) = 0;
s8 gMemCard_bRequest __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B43F __asm__("D_8009B43F") __attribute__((section(".sbss"))) = 0;
s32 gMemCard_nDirEntries __attribute__((section(".sbss"))) = 0;
struct DIRENTRY * gMemCard_pDirEntries __attribute__((section(".sbss"))) = 0;
u32 bss_pad_8009B448 __asm__("D_8009B448") __attribute__((section(".sbss"))) = 0;
u16 gMemCard_wRequestOffset __attribute__((section(".sbss"))) = 0;
u8 gMemCard_bDirFlags __attribute__((section(".sbss"))) = 0;
u8 gMemCard_bRequestStep __attribute__((section(".sbss"))) = 0;
s32 gMemCard_nIOResult __attribute__((section(".sbss"))) = 0;
u32 bss_pad_8009B454 __asm__("D_8009B454") __attribute__((section(".sbss"))) = 0;
SDSecondaryState * gSD_pSecondaryState __attribute__((section(".sbss"))) = 0;
SDValue * g_SDValue __attribute__((section(".sbss"))) = 0;
FileRequestSlot * gSD_pFileRequest __attribute__((section(".sbss"))) = 0;
u32 bss_pad_8009B464 __asm__("D_8009B464") __attribute__((section(".sbss"))) = 0;
u16 bss_gModel_ImageCopyRect __asm__("gModel_ImageCopyRect") __attribute__((section(".sbss"))) = 0;
s16 gModel_ImageCopyRectY __attribute__((section(".sbss"))) = 0;
s16 gModel_ImageCopyRectW __attribute__((section(".sbss"))) = 0;
s16 gModel_ImageCopyRectH __attribute__((section(".sbss"))) = 0;
s16 gModel_sTextureTintRectX __attribute__((section(".sbss"))) = 0;
s16 gModel_sTextureTintRectY __attribute__((section(".sbss"))) = 0;
s16 gModel_sTextureTintRectW __attribute__((section(".sbss"))) = 0;
s16 gModel_sTextureTintRectH __attribute__((section(".sbss"))) = 0;
u16 gModel_wViewDistance __attribute__((section(".sbss"))) = 0;
u16 gModel_wViewYaw __attribute__((section(".sbss"))) = 0;
u16 gModel_wViewPitch __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B47E __asm__("D_8009B47E") __attribute__((section(".sbss"))) = 0;
u16 bss_gModel_ViewSnapshot __asm__("gModel_ViewSnapshot") __attribute__((section(".sbss"))) = 0;
u16 bss_gModel_wViewYawSnapshot __asm__("gModel_wViewYawSnapshot") __attribute__((section(".sbss"))) = 0;
u32 bss_gModel_wViewPitchSnapshot __asm__("gModel_wViewPitchSnapshot") __attribute__((section(".sbss"))) = 0;
u16 bss_gModel_awSlotModelIds __asm__("gModel_awSlotModelIds") __attribute__((section(".sbss"))) = 0;
u16 bss_D_8009B48A __asm__("D_8009B48A") __attribute__((section(".sbss"))) = 0;
u16 bss_D_8009B48C __asm__("D_8009B48C") __attribute__((section(".sbss"))) = 0;
u8 bss_gModelDebug_abSlotAfFlags __asm__("gModelDebug_abSlotAfFlags") __attribute__((section(".sbss"))) = 0;
u8 bss_D_8009B48F __asm__("D_8009B48F") __attribute__((section(".sbss"))) = 0;
u8 bss_gModelDebug_abSlotGfFlags __asm__("gModelDebug_abSlotGfFlags") __attribute__((section(".sbss"))) = 0;
u8 bss_D_8009B491 __asm__("D_8009B491") __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B492 __asm__("D_8009B492") __attribute__((section(".sbss"))) = 0;
u32 bss_pad_8009B494 __asm__("D_8009B494") __attribute__((section(".sbss"))) = 0;
u8 * gMovie_pWorkArea __attribute__((section(".sbss"))) = 0;
CdlLOC gMovie_StreamLoc __attribute__((section(".sbss"))) = {0};
u8 D_8009B4A0 __attribute__((section(".sbss"))) = 0;
u8 D_8009B4A1 __attribute__((section(".sbss"))) = 0;
u8 D_8009B4A2 __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B4A3 __asm__("D_8009B4A3") __attribute__((section(".sbss"))) = 0;
u32 bss_pad_8009B4A4 __asm__("D_8009B4A4") __attribute__((section(".sbss"))) = 0;
