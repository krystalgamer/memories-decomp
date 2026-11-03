#include "../../types.h"
#define DUEL_FIELD_CARD_RENDER_REGIONAL_STORAGE
/* This build's own zero-initialized storage at 0x800EAE58-0x800EB150 (#2602); its
 * layout differs from the North American block the shared source defines. */
u8 D_800EAE58_sles_03947[0x2F8];
#define DUEL_FIELD_CARD_RENDER_REGIONAL_SMALL_STORAGE
/* This build's own zero-initialized storage at 0x8009C0AC-0x8009C1B8 (#2602); its
 * layout differs from the North American block the shared source defines. */
s16 gDuel_wViewTweenFramesRemaining;
u16 D_8009C0AE_sles_03947;
u32 D_8009C0B0_sles_03947;
u32 D_8009C0B4_sles_03947;
s8 D_8009C0B8_sles_03947;
u8 D_8009C0B9_sles_03947;
u16 D_8009C0BA_sles_03947;
s32 gDuel_dwViewTweenDistanceStep;
u8 D_8009C0C0_sles_03947;
s8 gDuel_bPresentationCursor;
u16 D_8009C0C2_sles_03947;
u32 D_8009C0C4_sles_03947;
struct DisplayObject *gDuel_pTerrainIcon;
u32 D_8009C0CC_sles_03947;
s32 gDuel_dwFieldDimTarget;
u32 D_8009C0D4_sles_03947;
u32 D_8009C0D8_sles_03947;
u32 D_8009C0DC_sles_03947;
struct DuelSideState *gDuel_pActiveSideState;
u8 gDuel_bActiveSide;
u8 D_8009C0E5_sles_03947;
u16 D_8009C0E6_sles_03947;
struct DuelResultDisplayState *gDuel_pResultDisplayState;
u16 gDuelResult_wBgmId;
u8 gDuel_bDrawStepCountdown;
s8 gDuelCursor_TargetColumn;
s8 gDuelCursor_TargetRow;
u8 D_8009C0F1_sles_03947;
u16 D_8009C0F2_sles_03947;
s16 gDuel_sGroundReferenceScreenX;
s16 gDuel_sGroundReferenceScreenY;
s32 gDuel_dwViewTweenReferenceZAccumulator;
u8 *gDuel_pActiveFieldGrid;
u16 gDuel_wTrapPresentationMode;
s8 D_8009B218;
u8 D_8009C103_sles_03947;
s16 gDuel_wEquipCardId;
u16 D_8009C106_sles_03947;
u16 D_8009C108_sles_03947;
u16 D_8009C10A_sles_03947;
u16 D_8009C10C_sles_03947;
u16 D_8009C10E_sles_03947;
s16 gDuel_wViewTweenReferenceZTarget;
u16 D_8009C112_sles_03947;
s32 gDuel_dwViewTweenReferenceZStep;
u8 D_8009B16E;
u8 D_8009C119_sles_03947;
u16 D_8009C11A_sles_03947;
s32 gDuel_dwViewTweenHeadingAccumulator;
s32 gDuel_dwViewTweenOrbitAngleAccumulator;
u32 D_8009C124_sles_03947;
struct DisplayObject *gDuel_pFlipLeftMarker;
struct DisplayObject *gDuel_pFlipRightMarker;
u16 gDuel_wCardEffectFlags;
u16 D_8009C132_sles_03947;
u16 gDuel_awSavedStatModifiers[2];
s16 gDuel_wSelectedTrapCardId;
u16 D_8009C13A_sles_03947;
struct DisplayObject *gDuel_pLifePointsPanel;
u8 gDuel_bConfirmFlags;
s8 gDuel_bOutroWinnerSide;
s16 gDuel_wViewTweenHeadingTarget;
u16 gDuel_wSceneStateFlags;
s16 gDuel_wViewTweenOrbitAngleTarget;
u32 D_8009C148_sles_03947;
s32 gDuel_dwViewTweenHeadingStep;
s32 gDuel_dwViewTweenOrbitAngleStep;
u16 gDuel_wPlacementCardId;
u16 D_8009C156_sles_03947;
u16 gDuel_wTwoPlayerSide0LifePoints;
u16 gDuel_wTwoPlayerSide1LifePoints;
s16 gDuel_wPendingEquipBonus;
u16 gDuel_wDisplayTransitionFlags;
u8 gDuel_bQuitDialogState;
u8 D_8009C161_sles_03947;
u16 gDuel_wOutcomeFlags;
u8 gDuel_bWinnerSide;
u8 gDuel_bSceneSubStep;
u16 D_8009C166_sles_03947;
u8 *tent_DebugEffectObject0;
u8 *tent_DebugEffectObject1;
s8 gDuelCursor_Direction;
u8 D_8009C171_sles_03947;
u16 D_8009C172_sles_03947;
u16 gDuel_awSavedRecordFlags[2];
struct DuelEffectRequest *gDuelEffect_pTrackedRequest;
u8 gDuel_bCardMoveSlot;
u8 D_8009C17D_sles_03947;
s16 gDuel_RitualResultCardId;
s16 gDuel_awBattleDamage[2];
s16 gDuel_sCardEffectIndex;
u16 D_8009C186_sles_03947;
s16 gDuel_wMonsterRemovalCriterion;
s16 gDuel_wMonsterRemovalZone;
s8 gDuel_abBattleOutcome[2];
u16 D_8009C18E_sles_03947;
struct DuelSelectionRecord *gDuel_pActiveSelectionRecord;
u8 gDuel_bSelectedTrapSlot;
u8 D_8009C195_sles_03947;
u16 D_8009C196_sles_03947;
struct DisplayObject *gDuel_pActionCard;
struct DisplayObject *gDuel_pCardPreviewObject;
u8 gDuelCursor_PickFlags;
u8 D_8009C1A1_sles_03947;
u16 gDuel_wStepCounter;
s16 gDuel_wEffectCardID;
u16 D_8009C1A6_sles_03947;
s32 gDuel_dwViewTweenDistanceAccumulator;
u8 gDuel_bPendingDrawCount;
u8 D_8009C1AD_sles_03947;
u16 D_8009C1AE_sles_03947;
u32 D_8009C1B0_sles_03947;
u32 D_8009C1B4_sles_03947;

/* SLES-03947 build of src/game/duel_field_card_render.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). Only Duel_RenderFieldCard3D
 * reads them. */
#define DUEL_FIELD_CARD_CLUT_Y 0xE1
#define DUEL_FIELD_CARD_CLUT_X16 0x28

#include "../duel_field_card_render.c"
