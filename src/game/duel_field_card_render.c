/* The duel field's per-card draw paths, in address order:
   Duel_DrawProjectedFieldCardFrame projects a card's ground position and
   draws its card frame at that screen point, and Duel_RenderFieldCard3D
   draws the card as a lit 3D quad with a projected ground sprite. Both are
   called from Duel_DrawFieldCards and build with the RTPS-normalizing
   profile. Both use ordering-table slot 2,
   gGraphics_apOrderingTables[2]. */

#include "../types.h"

#ifndef DUEL_FIELD_CARD_RENDER_STORAGE_ORDER
/* Zero-initialized storage this unit owns at 0x800E9EF0-0x800EA1E8. GCC lays
 * it out in first-declaration order, so it is declared here in address order
 * ahead of the headers and defined once they supply the types (#2602). A
 * regional wrapper that lays it out differently declares its own order and
 * defines DUEL_FIELD_CARD_RENDER_STORAGE_ORDER. */
extern struct DisplayObject *gDisplayObject_apWorkSlots[];
extern u8 gDuel_abSelectionSides[];
extern struct DuelSideState gDuel_aSideStates[];
extern struct DuelHandSlot gDuelHand_aSlots[];
extern struct ScreenPair gDuel_aCardSlotScreenPositions[];
extern struct DuelEffectResourceRecord gCardPreview_Resources[];
#endif

#ifndef DUEL_FIELD_CARD_RENDER_REGIONAL_SMALL_STORAGE
/* Zero-initialized storage this unit owns at 0x8009B150-0x8009B244, defined in
 * address order ahead of the headers that declare it (#2602). */
u16 gDuel_wPlacementCardId;
u8 gDuel_bConfirmFlags;
s16 gDuel_wPendingEquipBonus;
s32 gDuel_dwViewTweenHeadingAccumulator;
s32 gDuel_dwViewTweenOrbitAngleAccumulator;
s8 gDuelCursor_Direction;
u16 gDuel_wDisplayTransitionFlags;
u8 gDuel_bQuitDialogState;
u8 gDuel_bWinnerSide;
s16 gDuel_wViewTweenReferenceZTarget;
s32 gDuel_dwViewTweenReferenceZStep;
u16 gDuel_wOutcomeFlags;
u8 D_8009B16E;
u16 gDuel_awSavedStatModifiers[2];
u8 gDuel_bSceneSubStep;
u16 gDuel_awSavedRecordFlags[2];
struct DuelEffectRequest *gDuelEffect_pTrackedRequest;
u8 *gDebugEffect_pPreviewCard0;
u8 *gDebugEffect_pPreviewCard1;
struct DisplayObject *gDuel_pFlipLeftMarker;
struct DisplayObject *gDuel_pFlipRightMarker;
s16 gDuel_wViewTweenHeadingTarget;
s16 gDuel_wViewTweenOrbitAngleTarget;
s32 gDuel_dwViewTweenHeadingStep;
s32 gDuel_dwViewTweenOrbitAngleStep;
u8 gDuel_bCardMoveSlot;
s16 gDuel_wAiHandStackPending;
s16 gDuel_RitualResultCardId;
s16 gDuel_awBattleDamage[2];
s16 gDuel_sCardEffectIndex;
static u16 D_8009B1AA;
s16 gDuel_wMonsterRemovalCriterion;
s16 gDuel_wMonsterRemovalZone;
s8 gDuel_abBattleOutcome[2];
struct DuelSelectionRecord *gDuel_pActiveSelectionRecord;
u8 gDuel_bSelectedTrapSlot;
s8 gDuel_bPresentationCursor;
s32 gDuel_dwFieldDimTarget;
struct DisplayObject *gDuel_pCardPreviewObject;
s32 gDuel_dwViewTweenDistanceAccumulator;
struct DuelSideState *gDuel_pActiveSideState;
struct DisplayObject *gDuel_pActionCard;
u16 gDuel_wStepCounter;
s16 gDuel_wEffectCardID;
u8 gDuelCursor_PickFlags;
u8 gDuel_bActiveSide;
s8 gDuelCursor_TargetColumn;
s8 gDuelCursor_TargetRow;
struct SaveDataState *gDuel_pSide0SaveData;
struct SaveDataState *gDuel_pSide1SaveData;
u16 gDuelResult_wBgmId;
u8 gDuel_bAiSelectionCursor;
u16 gDuel_wAiSelectionStepDelay;
struct DuelResultDisplayState *gDuel_pResultDisplayState;
u8 gDuel_bPendingDrawCount;
u8 gDuel_bDrawStepCountdown;
s16 gDuel_wViewTweenDistanceTarget;
struct DuelEffectRequest *gDuel_apSwordsEffectObjects[2 ];
struct DisplayObject *gDuel_pActionReadyMarker;
s32 gDuel_dwViewTweenDistanceStep;
s16 gDuel_sGroundReferenceScreenX;
s16 gDuel_sGroundReferenceScreenY;
s16 gDuel_wViewTweenFramesRemaining;
s16 gDuel_wEquipCardId;
s8 gDuel_bBattleRemovalSlot0;
s8 gDuel_bBattleRemovalSlot1;
s8 gDuel_bAiCursorTarget;
u16 gDuel_wActionSubstate;
s16 gDuel_sEffectWorkCounter;
u16 gDuel_wTrapPresentationMode;
struct DisplayObject *gDuel_pTerrainIcon;
s8 gDuel_bPlacementNeedsCardMoveSetup;
static u8 D_8009B219;
u8 gDuel_bFieldActionAutoCommit;
struct DisplayObject *gDuel_pLifePointsPanel;
u16 gDuel_wCardEffectFlags;
s32 gDuel_dwViewTweenReferenceZAccumulator;
static u8 D_8009B228;
u8 gDuel_bAnimatedBattleRequested;
s16 gDuel_wSelectedTrapCardId;
u8 *gDuel_pActiveFieldGrid;
u8 gDuel_bTwoPlayerCardViewMode;
static u16 D_8009B232;
u16 gDuel_wTwoPlayerSide0LifePoints;
u16 gDuel_wTwoPlayerSide1LifePoints;
s8 gDuel_bOutroWinnerSide;
u16 gDuel_wSceneStateFlags;
static u32 D_8009B23C;
struct DisplayObject *gDuel_pCardViewerBackground;
#else
extern u16 gDuel_wPlacementCardId;
extern u8 gDuel_bConfirmFlags;
extern s16 gDuel_wPendingEquipBonus;
extern s32 gDuel_dwViewTweenHeadingAccumulator;
extern s32 gDuel_dwViewTweenOrbitAngleAccumulator;
extern s8 gDuelCursor_Direction;
extern u16 gDuel_wDisplayTransitionFlags;
extern u8 gDuel_bQuitDialogState;
extern u8 gDuel_bWinnerSide;
extern s16 gDuel_wViewTweenReferenceZTarget;
extern s32 gDuel_dwViewTweenReferenceZStep;
extern u16 gDuel_wOutcomeFlags;
extern u8 D_8009B16E;
extern u16 gDuel_awSavedStatModifiers[2];
extern u8 gDuel_bSceneSubStep;
extern u16 gDuel_awSavedRecordFlags[2];
extern struct DuelEffectRequest *gDuelEffect_pTrackedRequest;
extern u8 *gDebugEffect_pPreviewCard0;
extern u8 *gDebugEffect_pPreviewCard1;
extern struct DisplayObject *gDuel_pFlipLeftMarker;
extern struct DisplayObject *gDuel_pFlipRightMarker;
extern s16 gDuel_wViewTweenHeadingTarget;
extern s16 gDuel_wViewTweenOrbitAngleTarget;
extern s32 gDuel_dwViewTweenHeadingStep;
extern s32 gDuel_dwViewTweenOrbitAngleStep;
extern u8 gDuel_bCardMoveSlot;
extern s16 gDuel_wAiHandStackPending;
extern s16 gDuel_RitualResultCardId;
extern s16 gDuel_awBattleDamage[2];
extern s16 gDuel_sCardEffectIndex;
extern s16 gDuel_wMonsterRemovalCriterion;
extern s16 gDuel_wMonsterRemovalZone;
extern s8 gDuel_abBattleOutcome[2];
extern struct DuelSelectionRecord *gDuel_pActiveSelectionRecord;
extern u8 gDuel_bSelectedTrapSlot;
extern s8 gDuel_bPresentationCursor;
extern s32 gDuel_dwFieldDimTarget;
extern struct DisplayObject *gDuel_pCardPreviewObject;
extern s32 gDuel_dwViewTweenDistanceAccumulator;
extern struct DuelSideState *gDuel_pActiveSideState;
extern struct DisplayObject *gDuel_pActionCard;
extern u16 gDuel_wStepCounter;
extern s16 gDuel_wEffectCardID;
extern u8 gDuelCursor_PickFlags;
extern u8 gDuel_bActiveSide;
extern s8 gDuelCursor_TargetColumn;
extern s8 gDuelCursor_TargetRow;
extern struct SaveDataState *gDuel_pSide0SaveData;
extern struct SaveDataState *gDuel_pSide1SaveData;
extern u16 gDuelResult_wBgmId;
extern u8 gDuel_bAiSelectionCursor;
extern u16 gDuel_wAiSelectionStepDelay;
extern struct DuelResultDisplayState *gDuel_pResultDisplayState;
extern u8 gDuel_bPendingDrawCount;
extern u8 gDuel_bDrawStepCountdown;
extern s16 gDuel_wViewTweenDistanceTarget;
extern struct DuelEffectRequest *gDuel_apSwordsEffectObjects[2 ];
extern struct DisplayObject *gDuel_pActionReadyMarker;
extern s32 gDuel_dwViewTweenDistanceStep;
extern s16 gDuel_sGroundReferenceScreenX;
extern s16 gDuel_sGroundReferenceScreenY;
extern s16 gDuel_wViewTweenFramesRemaining;
extern s16 gDuel_wEquipCardId;
extern s8 gDuel_bBattleRemovalSlot0;
extern s8 gDuel_bBattleRemovalSlot1;
extern s8 gDuel_bAiCursorTarget;
extern u16 gDuel_wActionSubstate;
extern s16 gDuel_sEffectWorkCounter;
extern u16 gDuel_wTrapPresentationMode;
extern struct DisplayObject *gDuel_pTerrainIcon;
extern s8 gDuel_bPlacementNeedsCardMoveSetup;
extern u8 gDuel_bFieldActionAutoCommit;
extern struct DisplayObject *gDuel_pLifePointsPanel;
extern u16 gDuel_wCardEffectFlags;
extern s32 gDuel_dwViewTweenReferenceZAccumulator;
extern u8 gDuel_bAnimatedBattleRequested;
extern s16 gDuel_wSelectedTrapCardId;
extern u8 *gDuel_pActiveFieldGrid;
extern u8 gDuel_bTwoPlayerCardViewMode;
extern u16 gDuel_wTwoPlayerSide0LifePoints;
extern u16 gDuel_wTwoPlayerSide1LifePoints;
extern s8 gDuel_bOutroWinnerSide;
extern u16 gDuel_wSceneStateFlags;
extern struct DisplayObject *gDuel_pCardViewerBackground;
#endif

#include "../ygo_types.h"
#include "duel_side_state.h"
#include "display_object.h"
#include "display_projection.h"
#include "screen_projection.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/inline_c.h"
#include "duel_draw_card_frame.h"
#include "ordering_tables.h"
#include "duel_field_card_render.h"
#include "duel_field_card_render_data.h"
#include "duel_selection_layout.h"
#include "duel_screen_tables.h"
#include "duel_effect_resource_record.h"

/* Five work slots and the terminator word DisplayObject_CopyWorkSlots
 * appends; both regional layouts reserve 0x20 bytes, so two more words
 * follow. */
DisplayObject *gDisplayObject_apWorkSlots[8];
u8 gDuel_abSelectionSides[DUEL_SIDE_COUNT * DUEL_SELECTION_SIDE_SIZE];
DuelSideState gDuel_aSideStates[DUEL_SIDE_COUNT];
DuelHandSlot gDuelHand_aSlots[HAND_SIZE];
ScreenPair gDuel_aCardSlotScreenPositions[DUEL_SCREEN_CARD_POSITION_COUNT];
DuelEffectResourceRecord gCardPreview_Resources[4];

/* Volatile scratchpad stores and the second record reload preserve retail
 * ordering around the official Psy-Q RTPS macros. The scratchpad word is the
 * SVECTOR gte_ldv0 loads, and the qualifier lives on that pointer, so its
 * three stores are plain member stores.
 *
 * Immediately follows DisplayObject_ProjectLinkedGroundPosition in the image and shares its projection
 * setup: same 0x1F8003E0 scratchpad pair, same rtps, same write-back through
 * swc2 $14. Here the source coordinates come from the object's own target
 * record and the projected pair is biased into the object's screen fields. */
void Duel_DrawProjectedFieldCardFrame(DuelCardRecord *card)
{
    ProjectedPair p;
    s32 y;

    GsSetLsMatrix(&gGraphics_LightSourceMatrix);
    {
        u16 x = ((DisplayObject *)card->object)->xy.h.x;
        volatile SVECTOR *scratch = (volatile SVECTOR *)0x1F8003E0;

        scratch->vy = 0;
        scratch->vx = x;
        scratch->vz =
            *(u16 *)&((DisplayObject *)((volatile DuelCardRecord *)card)
                         ->object)->field_34.h.field_34;
        gte_ldv0((u8 *)scratch);
        gte_rtps();
        gte_stsxy(&p);
    }
    card->screen_x = p.x - 0x1A;
    y = p.y;
    card->screen_y = y - 0x1E;
    if (gDuel_bActiveSide != 0) {
        if ((s8)card->table_index < 0xF) {
            card->screen_y = y - 0x1D;
        }
    } else {
        if ((s8)card->table_index >= 0xF) {
            card->screen_y = y - 0x1D;
        }
    }
    Duel_DrawCardFrame((DisplayObject *)card->object, gGraphics_apOrderingTables[2], card->screen_x,
                  card->screen_y);
    SetGeomOffset(0xA0, 0x6C);
}

/* Regional values: the CLUT's y base and its x in 16-pixel units (0xF1 ->
 * 0xE1, 0x10 -> 0x28). The European build (src/game/european/) defines its
 * own. */
#ifndef DUEL_FIELD_CARD_CLUT_Y
#define DUEL_FIELD_CARD_CLUT_Y 0xF1
#endif
#ifndef DUEL_FIELD_CARD_CLUT_X16
#define DUEL_FIELD_CARD_CLUT_X16 0x10
#endif

#define CVECTOR_VIEW(color) ((CVECTOR *)(color))

/* Draws one 3D duel card. The display object's Euler bytes and position build
 * a local matrix in the scratchpad, four card corners are rotated, lit with
 * RotColorDpq into the POLY_GT4 and sorted at their mean depth, and the same
 * corners flattened to y = 0 are projected with RTPS into the POLY_FT4 that
 * is sorted at the back as the card's ground sprite.
 *
 * What the retail listing fixes about the source:
 * - The scale test reads the 16-bit flags word against GsROTOFF (1 << 27).
 *   cse cannot see that the mask misses every bit of a halfword; combine can,
 *   but only after the test is its own instruction, so retail keeps a literal
 *   zero, a branch that never goes, and the dead temporary's stack slot.
 * - The matrix, the first corner, the scale, the light normal, the rotation
 *   and the depth words are pointers set once after the guard, in address
 *   order. Corners 1-3 and copies 1-3 get their own pointers where they are
 *   first used; copy 0 is rot[4].
 * - Each chained store writes the right-hand corner first.
 * - The RTPS inputs are literal scratchpad addresses, which reload
 *   materialises in $t0-$t3.
 * - u0 and v0 are both read back before either is biased, and setPolyGT4
 *   follows the clut store.
 *
 * The GTE words come from the official Psy-Q inline_c.h macros; the profile
 * expands Psy-Q's RTPS placeholder to the real COP2 opcode. */
void Duel_RenderFieldCard3D(DuelCardRecord *record, POLY_GT4 *prim, POLY_FT4 *sprite, s32 *color)
{
    DuelCardRecord *holder = record;
    GsOT **tab;
    DisplayObject *obj;
    s32 a;
    s32 c;
    s32 d;
    s32 y;
    MATRIX *lm;
    SVECTOR *q;
    VECTOR *scale;
    SVECTOR *up;
    SVECTOR *rot;
    s32 *depth;
    SVECTOR *q1;
    SVECTOR *q2;
    SVECTOR *q3;
    SVECTOR *c1;
    SVECTOR *c2;
    SVECTOR *c3;

    obj = (DisplayObject *)holder->object;
    if ((obj->flags & 0x40) == 0) {
        return;
    }

    lm = (MATRIX *)0x1F8002C0;
    q = (SVECTOR *)0x1F800300;
    scale = (VECTOR *)0x1F800320;
    up = (SVECTOR *)0x1F800380;
    rot = (SVECTOR *)0x1F8003A0;
    depth = (s32 *)0x1F8003E0;
    up->vx = 0;
    up->vy = 0x1000;
    up->vz = 0;
    *color = obj->rgb;
    rot[0].vx = obj->field_20.b.rotation_x << 4;
    y = obj->field_20.b.rotation_y << 4;
    rot[0].vy = y;
    rot[0].vz = obj->field_20.b.rotation_z << 4;
    if ((s8)holder->table_index >= 0xF) {
        rot[0].vy = y + 0x800;
    }
    lm->t[0] = (s16)obj->xy.h.x;
    a = (s16)obj->xy.h.y;
    lm->t[1] = a;
    d = obj->field_34.h.field_34;
    lm->t[2] = d;
    a += 0xFF;
    if (a < 0) {
        a = 0;
    }
    sprite->b0 = a;
    sprite->g0 = a;
    sprite->r0 = a;
    RotMatrixZYX_gte(rot, lm);
    if ((obj->flags & GsROTOFF) == 0) {
        scale->vx = obj->field_44.scale.x;
        scale->vy = 0x1000;
        scale->vz = obj->field_44.scale.y;
        ScaleMatrix(lm, scale);
    }

    q2 = (SVECTOR *)0x1F800310;
    q3 = (SVECTOR *)0x1F800318;
    q1 = (SVECTOR *)0x1F800308;
    q[0].vx = q2->vx = -0x19;
    q[1].vx = q3->vx = 0x1A;
    q[0].vz = q1->vz = 0x1D;
    q[2].vz = q3->vz = -0x1E;
    q[0].vy = q1->vy = q2->vy = q3->vy = 0;
    GsSetLsMatrix(lm);

    RotTransSV(&q[0], &rot[0], (long *)depth);
    RotTransSV(&q[1], &rot[1], (long *)depth);
    RotTransSV(&q[2], &rot[2], (long *)depth);
    RotTransSV(&q[3], &rot[3], (long *)depth);

    c1 = (SVECTOR *)0x1F8003C8;
    c2 = (SVECTOR *)0x1F8003D0;
    c3 = (SVECTOR *)0x1F8003D8;
    *(Bytes8 *)&rot[4] = *(Bytes8 *)&rot[0];
    *(Bytes8 *)c1 = *(Bytes8 *)&rot[1];
    *(Bytes8 *)c2 = *(Bytes8 *)&rot[2];
    *(Bytes8 *)c3 = *(Bytes8 *)&rot[3];

    GsSetLightMatrix(lm);
    GsSetLsMatrix(&gGraphics_LightSourceMatrix);

    depth[4] = RotColorDpq(&rot[0], up, CVECTOR_VIEW(color),
                (long *)&prim->x0, CVECTOR_VIEW(&prim->r0),
                (long *)&depth[0]);
    depth[5] = RotColorDpq(&rot[1], up, CVECTOR_VIEW(color),
                (long *)&prim->x1, CVECTOR_VIEW(&prim->r1),
                (long *)&depth[1]);
    depth[6] = RotColorDpq(&rot[2], up, CVECTOR_VIEW(color),
                (long *)&prim->x2, CVECTOR_VIEW(&prim->r2),
                (long *)&depth[2]);
    depth[7] = RotColorDpq(&rot[3], up, CVECTOR_VIEW(color),
                (long *)&prim->x3, CVECTOR_VIEW(&prim->r3),
                (long *)&depth[3]);

    if ((depth[0] | depth[1] | depth[2]
         | depth[3]) < 0) {
        return;
    }

    prim->u0 = prim->u2 = ((u8 *)&obj->field_5C)[0];
    prim->v0 = prim->v1 = ((u8 *)&obj->field_5C)[1];
    if (NormalClip(*(long *)&prim->x0, *(long *)&prim->x1,
                   *(long *)&prim->x2) <= 0) {
        prim->u2 = 0x38;
        prim->u0 = 0x38;
        prim->v1 = 0x80;
        prim->v0 = 0x80;
    }
    c = prim->u0;
    d = prim->v0;
    c += 0x34;
    d += 0x3C;
    prim->u3 = c;
    prim->u1 = c;
    prim->v3 = d;
    prim->v2 = d;
    prim->clut = (((u16)obj->field_40.clut.y + DUEL_FIELD_CARD_CLUT_Y) << 6) | DUEL_FIELD_CARD_CLUT_X16;
    setPolyGT4(prim);
    SetSemiTrans(prim, 0);
    prim->tpage = prim->tpage & 0xFF9F;
    prim->tpage = prim->tpage | ((obj->attribute >> 23) & 0x60);
    if ((obj->attribute & GsALON) != 0) {
        SetSemiTrans(prim, 1);
        sprite->r0 = ((u8 *)&obj->rgb)[0];
        sprite->g0 = ((u8 *)&obj->rgb)[1];
        sprite->b0 = ((u8 *)&obj->rgb)[2];
    }

    depth[0] = (depth[4] + depth[5]
                          + depth[6] + depth[7]) / 4 >> 2;
    tab = gGraphics_apOrderingTables;
    GsSortPoly(prim, tab[2], *(u16 *)depth);

    c3->vy = 0;
    c2->vy = 0;
    c1->vy = 0;
    rot[4].vy = 0;
    gte_ldv0((SVECTOR *)0x1F8003C0);
    gte_rtps();
    gte_stsxy(&sprite->x0);
    gte_ldv0((SVECTOR *)0x1F8003C8);
    gte_rtps();
    gte_stsxy(&sprite->x1);
    gte_ldv0((SVECTOR *)0x1F8003D0);
    gte_rtps();
    gte_stsxy(&sprite->x2);
    gte_ldv0((SVECTOR *)0x1F8003D8);
    gte_rtps();
    gte_stsxy(&sprite->x3);
    GsSortPoly(sprite, tab[2], 0xFFF);
}
