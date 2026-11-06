#include "../../types.h"

/* SLES-03947 build of src/game/duel_scene.c: the values and storage
 * that differ in this release, collected from the former per-unit wrappers. */

/* duel_field_card_render */
#define DUEL_FIELD_CARD_RENDER_STORAGE_ORDER
/* This build lays out the shared source's zero-initialized storage at
 * 0x800EAE58-0x800EB150 in a different order from the North American one;
 * the shared source defines it in first-declaration order (#2602). */
extern struct DuelSideState gDuel_aSideStates[];
extern struct DuelHandSlot gDuelHand_aSlots[];
extern struct ScreenPair gDuel_aCardSlotScreenPositions[];
extern union DisplayObjectWorkSlots gDisplayObject_apWorkSlots;
extern u8 gDuel_abSelectionSides[];
extern struct DuelEffectResourceRecord gCardPreview_Resources[];
#define DUEL_FIELD_CARD_RENDER_REGIONAL_SMALL_STORAGE
/* This build's own zero-initialized storage at 0x8009C0A8-0x8009C1B8 (#2602); its
 * layout differs from the North American block the shared source defines. */
u16 gDuel_wAiSelectionStepDelay;
s16 gDuel_wViewTweenDistanceTarget;
s16 gDuel_wViewTweenFramesRemaining;
static PADDING_VARIABLE(2);
struct DuelEffectRequest *gDuel_apSwordsEffectObjects[2];
s8 gDuel_bBattleRemovalSlot0;
s8 gDuel_bBattleRemovalSlot1;
static PADDING_VARIABLE(2);
s32 gDuel_dwViewTweenDistanceStep;
u8 gDuel_bAiSelectionCursor;
s8 gDuel_bPresentationCursor;
s16 gDuel_wAiHandStackPending;
s16 gDuel_sEffectWorkCounter;
static PADDING_VARIABLE(2);
struct DisplayObject *gDuel_pTerrainIcon;
u8 gDuel_bFieldActionAutoCommit;
s32 gDuel_dwFieldDimTarget;
static PADDING_VARIABLE(4);
struct SaveDataState *gDuel_pSide0SaveData;
struct SaveDataState *gDuel_pSide1SaveData;
struct DuelSideState *gDuel_pActiveSideState;
u8 gDuel_bActiveSide;
static PADDING_VARIABLE(1);
static PADDING_VARIABLE(2);
struct DuelResultDisplayState *gDuel_pResultDisplayState;
u16 gDuelResult_wBgmId;
u8 gDuel_bDrawStepCountdown;
s8 gDuelCursor_TargetColumn;
s8 gDuelCursor_TargetRow;
u8 gDuel_bTwoPlayerCardViewMode;
static PADDING_VARIABLE(2);
s16 gDuel_sGroundReferenceScreenX;
s16 gDuel_sGroundReferenceScreenY;
s32 gDuel_dwViewTweenReferenceZAccumulator;
u8 *gDuel_pActiveFieldGrid;
u16 gDuel_wTrapPresentationMode;
s8 gDuel_bPlacementNeedsCardMoveSetup;
static PADDING_VARIABLE(1);
s16 gDuel_wEquipCardId;
static PADDING_VARIABLE(2);
struct DisplayObject *gDuel_pActionReadyMarker;
s8 gDuel_bAiCursorTarget;
u16 gDuel_wActionSubstate;
s16 gDuel_wViewTweenReferenceZTarget;
static PADDING_VARIABLE(2);
s32 gDuel_dwViewTweenReferenceZStep;
u8 gDebugEffect_bPlacementCounter;
static PADDING_VARIABLE(1);
static PADDING_VARIABLE(2);
s32 gDuel_dwViewTweenHeadingAccumulator;
s32 gDuel_dwViewTweenOrbitAngleAccumulator;
static PADDING_VARIABLE(4);
struct DisplayObject *gDuel_pFlipLeftMarker;
struct DisplayObject *gDuel_pFlipRightMarker;
u16 gDuel_wCardEffectFlags;
static PADDING_VARIABLE(2);
u16 gDuel_awSavedStatModifiers[2];
s16 gDuel_wSelectedTrapCardId;
static PADDING_VARIABLE(2);
struct DisplayObject *gDuel_pLifePointsPanel;
u8 gDuel_bConfirmFlags;
s8 gDuel_bOutroWinnerSide;
s16 gDuel_wViewTweenHeadingTarget;
u16 gDuel_wSceneStateFlags;
s16 gDuel_wViewTweenOrbitAngleTarget;
u8 gDuel_bAnimatedBattleRequested;
s32 gDuel_dwViewTweenHeadingStep;
s32 gDuel_dwViewTweenOrbitAngleStep;
u16 gDuel_wPlacementCardId;
static PADDING_VARIABLE(2);
u16 gDuel_wTwoPlayerSide0LifePoints;
u16 gDuel_wTwoPlayerSide1LifePoints;
s16 gDuel_wPendingEquipBonus;
u16 gDuel_wDisplayTransitionFlags;
u8 gDuel_bQuitDialogState;
static PADDING_VARIABLE(1);
u16 gDuel_wOutcomeFlags;
u8 gDuel_bWinnerSide;
u8 gDuel_bSceneSubStep;
static PADDING_VARIABLE(2);
u8 *gDebugEffect_pPreviewCard0;
u8 *gDebugEffect_pPreviewCard1;
s8 gDuelCursor_Direction;
static PADDING_VARIABLE(1);
static PADDING_VARIABLE(2);
u16 gDuel_awSavedRecordFlags[2];
struct DuelEffectRequest *gDuelEffect_pTrackedRequest;
u8 gDuel_bCardMoveSlot;
static PADDING_VARIABLE(1);
s16 gDuel_RitualResultCardId;
s16 gDuel_awBattleDamage[2];
s16 gDuel_sCardEffectIndex;
static PADDING_VARIABLE(2);
s16 gDuel_wMonsterRemovalCriterion;
s16 gDuel_wMonsterRemovalZone;
s8 gDuel_abBattleOutcome[2];
static PADDING_VARIABLE(2);
struct DuelSelectionRecord *gDuel_pActiveSelectionRecord;
u8 gDuel_bSelectedTrapSlot;
static PADDING_VARIABLE(1);
static PADDING_VARIABLE(2);
struct DisplayObject *gDuel_pActionCard;
struct DisplayObject *gDuel_pCardPreviewObject;
u8 gDuelCursor_PickFlags;
static PADDING_VARIABLE(1);
u16 gDuel_wStepCounter;
s16 gDuel_wEffectCardID;
static PADDING_VARIABLE(2);
s32 gDuel_dwViewTweenDistanceAccumulator;
u8 gDuel_bPendingDrawCount;
static PADDING_VARIABLE(1);
static PADDING_VARIABLE(2);
u8 gDuel_bActiveEffectState;
struct DisplayObject *gDuel_pCardViewerBackground;

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). Only Duel_RenderFieldCard3D
 * reads them. */
#define DUEL_FIELD_CARD_CLUT_Y 0xE1
#define DUEL_FIELD_CARD_CLUT_X16 0x28

/* duel_draw_field_cards */
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_FIELD_CARDS_CLUT_1 0x3829
#define DUEL_FIELD_CARDS_CLUT_3 0x3868

/* duel_card_and_status_draw */
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_CARD_FRAME_SCREEN_HEIGHT 0x100
#define DUEL_SPRITE_CLUT_CXCY 0xE10280

/* duel_load_package_stage */
#define VERSION_EUROPE

/* duel_init_model_scene */
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_MODEL_SCENE_BUFFER_SIZE 0x65800

/* duel_load_terrain_package */
#define DUEL_TERRAIN_LOAD_FIRST_SECTOR 0x1B88
#define DUEL_TERRAIN_LOAD_SECTOR_COUNT 0xF0
/* value * 0xF0 */
#define DUEL_TERRAIN_LOAD_OFFSET(value) (((value) * 15) * 16)

/* duel_init_scene */
#define DUEL_INIT_TERRAIN_SECTOR(value) ((((value) * 15) * 16) + 0x1B88)
#define DUEL_INIT_TERRAIN_SECTOR_COUNT 0xF0
#define DUEL_INIT_DUELIST_DATA_FIRST_SECTOR 0x2218
#define DUEL_INIT_RESOURCE_CLUT_X 0x280
#define DUEL_INIT_RESOURCE_CLUT_Y 0xDF

/* duel_phase_entry */
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_PHASE_CURSOR_FIELD_0C 0xBE
#define DUEL_PHASE_FIELD_40_BASE 0x360
#define DUEL_PHASE_HAND_CARD_Y 0x2A2

/* duel_draw_resolution */
#define DUEL_DRAW_RESOLUTION_CARD_Y 0xA2
#define DUEL_CARD_READBACK_HEIGHT 0xD4

/* duel_create_card_effect_overlay */
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_CARD_EFFECT_OVERLAY_HEIGHT 0xD4

/* duel_scene_update_card_use */
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_CARD_USE_OBJECT_Y 0xE

/* duel_scene_card_placement */
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_CARD_PLACEMENT_SCREEN_HEIGHT 256

/* duel_scene_card_move */
#define DUEL_CARD_OFFSCREEN_Y -0x100

#define DUEL_HAND_STACK_INNER_Y 210
#define DUEL_HAND_STACK_COUNT_FIELD_5D 0xB0
#define DUEL_HAND_STACK_COUNT_CX 0x2D0
#define DUEL_SELECTION_CURSOR_FIELD_0C 0x84

/* duel_scene_hand_actions */
#define DUEL_HAND_ACTIONS_LOG_INVALID_AI_SELECTION 0

/* duel_scene_field_actions */
#define DUEL_FIELD_ACTIONS_INITIAL_CURSOR_FIELD_0C 0xBE
#define DUEL_FIELD_ACTIONS_TARGET_CURSOR_FIELD_0C 0x84

/* duel_battle_stats */

/* duel_scene_battle */
#define DUEL_BATTLE_EFFECT_REQUEST_Y_OFFSET 0x6A
#define DUEL_BATTLE_STAT_DISPLAY_Y 0xE

/* duel_result_runtime */
/* One 0x22-sector outro package per text language, from 0x2290; the
   duelist data from 0x2218, as for the scene initializer above. */
#define DUEL_RESULT_OUTRO_START_SECTOR (0x2290 + gText_bLanguageIndex * 0x22)
#define DUEL_RESULT_DUELIST_DATA_FIRST_SECTOR 0x2218
#define DUEL_RESULT_PAGE_TEXT_Y 0x38
#define DUEL_RESULT_PAGE_TEXT_HEIGHT 0xF0

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_RESULT_STARCHIP_Y 210
#define DUEL_RESULT_RANK_Y 40

/* debug_effect_screen */
#define DEBUG_EFFECT_PAGE1_FIELD_32 0x0E

/* duel_field_display_objects */
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_FIELD_SCREEN_HEIGHT 0x100

/* duel_update_card_pick_cursor */
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_PICK_CURSOR_FIELD_0C 0x84
/* The four language builds (src/game/spanish/duel_scene.c) supply their own. */
#ifndef DUEL_VIEWER_CARD_ID_ADDRESS
#define DUEL_VIEWER_CARD_ID_ADDRESS 0x8009C1B8
#define DUEL_VIEWER_Y_OFFSET_ADDRESS 0x8009C1BD
#endif

/* duel_scene_update */
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_QUIT_BOX_FLAGS 0x40
#define DUEL_QUIT_BOX_HEIGHT 0x30

/* The US block defines these two together with the height; they are the US
 * values. */
#define DUEL_QUIT_BOX_X 0x78
#define DUEL_QUIT_BOX_WIDTH 0x50

/* duel_action_lock */
#define DUEL_TERRAIN_EFFECT_SECTOR(terrain) \
    ((((terrain) * 15) * 16) + 0x1C58)

/* duel_field_effects */
#define DUEL_RITUAL_CARD_START_Y -42
#define DUEL_RITUAL_CARD_REST_Y 14
#define DUEL_RITUAL_SCREEN_HEIGHT 256

#include "../duel_scene.c"
