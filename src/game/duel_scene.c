#include "../types.h"
#include "scratchpad.h"

/* The duel field's per-card draw paths, in address order:
   Duel_DrawProjectedFieldCardFrame projects a card's ground position and
   draws its card frame at that screen point, and Duel_RenderFieldCard3D
   draws the card as a lit 3D quad with a projected ground sprite. Both are
   called from Duel_DrawFieldCards and build with the RTPS-normalizing
   profile. Both use ordering-table slot 2,
   gGraphics_apOrderingTables[2]. */

/*
 * The US 948-byte callback and 52-byte jump table match with the uniform
 * gcc_2_8_1_g8_split profile. The European wrapper adds its language upload
 * and seventeenth stage while retaining the common transfer-stage bodies.
 *
 * Stage 10 uses separate cleared and reloaded flag values. The reload
 * capture preserves its register lifetime; the scoped enable-mask value
 * places the mask before the zero-offset store in the load-delay slot.
 * The thirteen stage contracts, both image submissions and volatile flag
 * accesses are preserved without register bindings or instruction assembly.
 */

/* Duel ground projection: both functions project ground-plane points through
 * the GTE at screen offset (0xA0, 0x6C) from the scratchpad vector at
 * 0x1F8003E0. Typed scratchpad stores and official Psy-Q GTE macros reproduce
 * the projection blocks through the RTPS-normalizing profile. */

/*
 * Draw resolution and the Exodia result: Duel_UpdateDrawCardSlide eases a
 * drawn card into place, Duel_HasAllExodiaPieces and
 * DuelScene_UpdateDrawResolution resolve the drawn card, DuelScene_UpdateExodiaResult
 * stages the five-piece presentation, and DuelCard_CaptureRoundedTexture reads a
 * drawn card back into the staging buffer. One object in every build.
 */

/* Scratchpad C stores and official Psy-Q GTE macros reproduce the projection
 * block through the RTPS-normalizing profile. */

/*
 * Duel scene-state 4: the player's hand. It builds the hand cursor, runs the
 * scripted selection when the side is computer controlled, lets the player
 * walk the hand, inspect, flip and mark cards, and hands the marked cards to
 * the placement state in the order they were picked.
 *
 * Matches retail under gcc_2_8_1_g8_split. What the listing fixes about the
 * source:
 * - The pad words are volatile: every test re-reads them.
 * - The AI selection bytes are read as elements of gAi_Selection.bytes ([1], [6] and
 *   [8]). Byte 8 is fields.face_down, the hand face-down request; the
 *   byte-index form is what puts the %hi ahead of the neighbouring
 *   constants.
 * - The substate word is a scalar. sched lets a struct-member load through a
 *   pointer pass a store to a fixed scalar, which is the order of every
 *   "substate, then object flags" pair here; an array-element spelling
 *   would hold those loads back.
 * - The object's position pairs are copied as whole words. The copy that waits
 *   behind the substate store is a plain word access; the one that moves ahead
 *   of the state store goes through the union member.
 * - One variable serves the scripted card id, the cursor step, the card kind,
 *   the viewer request, the grid id and the pick-order counter; it is the
 *   $s0 of every block. hand is reused to walk the table when a pick is
 *   withdrawn, and obj holds the cursor sprite while the state is set up.
 * - The pick-order scan is a one-pass do/while around a for (;;) whose first
 *   statement is the hit test. The hit block rotates to the loop end and is
 *   copied in front of the loop, where the slot index folds to zero, and the
 *   enclosing pass keeps loop.c from moving the miss block out of the loop.
 *   Duel_ApplyAiHandSelection next door has the same one-pass do/while.
 */

/*
 * Duel scene-state 5: the field. It runs the scripted field path for a
 * computer-controlled side, moves the field cursor, opens the card viewer,
 * picks the attacker and its target, commits a fusion or sacrifice selection
 * and fades the field for the battle state.
 *
 * Matches retail under gcc_2_8_1_g8_split. What the listing fixes about the
 * source:
 * - The pad word is volatile: every test re-reads it.
 * - The side records are the DuelSelectionRecord struct, not byte views:
 *   the cursor-object loads pass the state stores only as struct members.
 *   Record 3 of the current side is gDuel_abSelectionSides + 0x54, written from
 *   the table base so cse can derive one from the other, and the pick cursor is
 *   record 2 of the same table.
 * - The slot index row * 5 + col is a separate statement into the loop
 *   counter i wherever retail computes the sum into its own register.
 * - One variable holds the scripted card id, the viewer request, the attack
 *   row, the commit flag, the fade level and the sprite tint: it is the $s4
 *   of every block. flags is the substate word and the fade target. The
 *   constants that retail lifts to a block top (1 at entry, 2/3/4 in the
 *   attack branches) go through the multiply-assigned x and y.
 * - The row == 2 test lets cse fold row * 5 to 10 in the grid expression.
 * - The commit block sits before sel = 0 so the store survives cse, and case 5
 *   keeps its copy through the branch slot. Case 6's input handling lives in
 *   an if (0) block so case 5 jumps over it straight into the commit block.
 * - The bank reads take the deck base through a pointer so the 0x36BC
 *   member offset stays in the load displacement.
 */

/*
 * Duel scene-state 9: the battle. It presents the attacker and the defender,
 * resolves the exchange, shows the damage and the guardian-star bonus, and
 * hands off to the trap, fusion-result and turn-switch states.
 *
 * Matches retail with no inline assembly,
 * register pins or asm links. What the listing fixes about the source:
 * - One s32 (result) carries the card stats, the battle result, the level
 *   nibble, the damage and the fade step: retail keeps all of them in $s2
 *   because the same variable crosses the sound call in state 9.
 * - The two saved stat words and the two saved flag words are side pairs
 *   (u16 [2]): as struct elements their loads cannot pass the record stores,
 *   which is why retail reloads gDuel_awSavedRecordFlags[1] after each flags store.
 * - State 4 falls through to its flag tests when there is no defender (the
 *   candidate returned there), and the do-while gives both exits one label so
 *   cse cannot thread the first test past the reload.
 * - State 3, 8, 9 and 10 store their next state inline; cross-jumping merges
 *   the identical tails and reorg lifts the li into the branch slot.
 * - Each state reads gDuel_bSceneSubStep into its own block-local variable; the
 *   function-wide copy conflicted with the temporaries retail keeps in $v1.
 * - The state-4 request block reads the flags before the field stores into a
 *   block-local, keeps the x coordinate and the level index in one shared
 *   variable so the second assignment waits for the first store, and the
 *   record pointer is &gCardPreview_Resources[gDuel_bPresentationCursor] with the meter copied through
 *   the sound-id scratch (retail's $a0).
 * - State 8 copies the position words through non-struct views so the copy
 *   cannot pass the flag byte store, and the shake block's flags are an s32
 *   so the mask writes the same register.
 * - State 5 writes the model properties in member order (model id first);
 *   the defender's stat word goes through a byte view so the pair loads for
 *   Duel_CompareBattleStrength wait for it.
 * - The two replay bytes are scalar definitions, gDuel_bBattleRemovalSlot0 and
 *   gDuel_bBattleRemovalSlot1: only a definition in this unit makes gcc treat them as small
 *   data and emit the load-delay nops retail has before their gp-relative
 *   stores, and only two symbols let the stores keep retail's order without
 *   cse deriving the second address from the first. The attacker's card id
 *   is read through a byte view so it stays behind those stores.
 */

/* The combined-deck request and producer and one duel card record's
   lifecycle, in address order. Duel_RequestCombinedDeckData sorts and
   de-duplicates the combined deck's card ids and requests their card data;
   Duel_PopulateCombinedDeckData then builds the deck records and copied
   card image blocks that Duel_SetupCardRecord consumes. The remaining
   functions release/reset a record, derive its terrain boost, fill it from
   that deck data, upload its thumbnail and CLUT, and create/attach the
   world-space field-card object.

   The record-lifecycle sources were recorded at gcc_2_8_1_g8,
   gcc_2_8_1_g8_split and gcc_2_8_1_g0_split, and the request was its own
   gcc_2_8_1_g8_split unit. Every member compiles to an identical object at
   gcc_2_8_1_g8_split, which is the profile duel_terrain_boost.h's notes on
   gDuel_bTerrain assume for Duel_GetTerrainBoost, so the unit builds there. */

#include "duel_scene.h"
#include "display_object.h"
#include "display_object_work_slots.h"
#include "display_projection.h"
#include "model_slot_support.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/inline_c.h"
#include "ordering_tables.h"

/* Zero-initialized storage this unit owns. Each build's generated
 * duel_scene_commons and duel_scene_bss_commons subsegments place it at the
 * retail addresses. In the European and language builds,
 * fade_runtime_commons places gDuel_wAiSelectionStepDelay and
 * gDuel_wViewTweenDistanceTarget; their common definitions here resolve to
 * those slots, which keeps the duel's reads gp-relative. */
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
u8 gDebugEffect_bPlacementCounter;
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
u8 gDuel_bFieldActionAutoCommit;
struct DisplayObject *gDuel_pLifePointsPanel;
u16 gDuel_wCardEffectFlags;
s32 gDuel_dwViewTweenReferenceZAccumulator;
u8 gDuel_bAnimatedBattleRequested;
s16 gDuel_wSelectedTrapCardId;
u8 *gDuel_pActiveFieldGrid;
u8 gDuel_bTwoPlayerCardViewMode;
u16 gDuel_wTwoPlayerSide0LifePoints;
u16 gDuel_wTwoPlayerSide1LifePoints;
s8 gDuel_bOutroWinnerSide;
u16 gDuel_wSceneStateFlags;
struct DisplayObject *gDuel_pCardViewerBackground;

DisplayObjectWorkSlots gDisplayObject_apWorkSlots;
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
        volatile SVECTOR *scratch = (volatile SVECTOR *)SCRATCH_ADDR(0x3E0);

        scratch->vy = 0;
        scratch->vx = x;
        scratch->vz =
            (u16)((DisplayObject *)((volatile DuelCardRecord *)card)
                         ->object)->field_34.h.z;
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

    lm = (MATRIX *)SCRATCH_ADDR(0x2C0);
    q = (SVECTOR *)SCRATCH_ADDR(0x300);
    scale = (VECTOR *)SCRATCH_ADDR(0x320);
    up = (SVECTOR *)SCRATCH_ADDR(0x380);
    rot = (SVECTOR *)SCRATCH_ADDR(0x3A0);
    depth = (s32 *)SCRATCH_ADDR(0x3E0);
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
    d = obj->field_34.h.z;
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

    q2 = (SVECTOR *)SCRATCH_ADDR(0x310);
    q3 = (SVECTOR *)SCRATCH_ADDR(0x318);
    q1 = (SVECTOR *)SCRATCH_ADDR(0x308);
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

    c1 = (SVECTOR *)SCRATCH_ADDR(0x3C8);
    c2 = (SVECTOR *)SCRATCH_ADDR(0x3D0);
    c3 = (SVECTOR *)SCRATCH_ADDR(0x3D8);
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

    prim->u0 = prim->u2 = ((u8 *)&obj->texture_uv)[0];
    prim->v0 = prim->v1 = ((u8 *)&obj->texture_uv)[1];
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
    gte_ldv0((SVECTOR *)SCRATCH_ADDR(0x3C0));
    gte_rtps();
    gte_stsxy(&sprite->x0);
    gte_ldv0((SVECTOR *)SCRATCH_ADDR(0x3C8));
    gte_rtps();
    gte_stsxy(&sprite->x1);
    gte_ldv0((SVECTOR *)SCRATCH_ADDR(0x3D0));
    gte_rtps();
    gte_stsxy(&sprite->x2);
    gte_ldv0((SVECTOR *)SCRATCH_ADDR(0x3D8));
    gte_rtps();
    gte_stsxy(&sprite->x3);
    GsSortPoly(sprite, tab[2], 0xFFF);
}

#include "view_state_orbit.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "sorted_entry_render.h"
#include "duel_card.h"
#include "card_presentation.h"
#include "display_projection.h"
#include "../game/model.h"

/* Regional values: the two CLUTs (0x3C11 -> 0x3829, 0x3C50 -> 0x3868). The
 * European build (src/game/european/) defines its own. */
#ifndef DUEL_FIELD_CARDS_CLUT_1
#define DUEL_FIELD_CARDS_CLUT_1 0x3C11
#endif
#ifndef DUEL_FIELD_CARDS_CLUT_3
#define DUEL_FIELD_CARDS_CLUT_3 0x3C50
#endif

/* Primes the two scratchpad primitives Duel_RenderFieldCard3D draws every field card
   with -- a POLY_FT4 at 0x1F800140 (len 9, code 0x2C, then 0x2E for
   semi-transparency) and the texture fields of a POLY_GT4 at 0x1F800180 --
   and hands each occupied card of both sides to it, or to Duel_DrawProjectedFieldCardFrame for
   cards flagged 0x400. */
void Duel_DrawFieldCards(void) {
    DuelCardRecord *e;
    DisplayObject *a;
    POLY_FT4 *field_ft4;
    POLY_GT4 *field_gt4;
    s32 *color_word;
    s32 n;
    s32 h;
    s32 c;
    s32 f;
    ViewState *q;
    DuelCardRecord *t;

    q = &gViewState_Current;
    SetGeomScreen(q->projection);
    SetGeomOffset(0xA0, 0x6C);
    SetFarColor(0, 0, 0);
    do {
        SetFogNearFar(0x28A, 0x320, q->projection);
        Model_DrawSlot(2);

        if ((gModel_dwFogDrawFlags & 2) != 0) {
            gSortedEntry_nCount = gSortedEntry_nCount + 1;
            if (gSortedEntry_nCount >= gSortedEntry_nSortedCount) {
                gModel_dwFogDrawFlags = gModel_dwFogDrawFlags & ~3;
            }
            gSortedEntry_pCursor = gSortedEntry_pBase;
        }

        color_word = (s32 *)SCRATCH_ADDR(0xC0);
        field_ft4 = (POLY_FT4 *)SCRATCH_ADDR(0x140);
        field_gt4 = (POLY_GT4 *)SCRATCH_ADDR(0x180);
        c = DUEL_DISPLAY_COLOR_NORMAL;
        e = gDuel_aFieldCardRecords;
        n = 0;

        setlen(field_ft4, 9);
        field_ft4->code = 0x2C;
        field_ft4->r0 = 0xFF;
        field_ft4->g0 = 0xFF;
        field_ft4->b0 = 0xFF;
        field_ft4->tpage = 0x5F;
        f = 0x80;
        field_ft4->clut = DUEL_FIELD_CARDS_CLUT_1;
    } while (0);
    field_ft4->u3 = 0xAF;
    field_ft4->u1 = 0xAF;
    field_ft4->v3 = 0x37;
    field_ft4->v2 = 0x37;
    field_ft4->code = 0x2E;
    field_ft4->u2 = f;
    field_ft4->u0 = f;
    field_ft4->v1 = 0;
    field_ft4->v0 = 0;
    field_gt4->tpage = 0x9E;
    field_gt4->clut = DUEL_FIELD_CARDS_CLUT_3;
    field_gt4->v1 = f;
    field_gt4->v0 = f;
    field_gt4->v3 = 0xBC;
    field_gt4->v2 = 0xBC;
    *color_word = c;

    do {
        h = e->flags;
        if ((h & DUEL_CARD_FLAG_OCCUPIED) != 0) {
            a = e->object;
            if (a != 0) {
                if ((h & 0x400) != 0) {
                    Duel_DrawProjectedFieldCardFrame(e);
                } else {
                    *color_word = a->rgb;
                    Duel_RenderFieldCard3D(e, field_gt4, field_ft4, color_word);
                }
            }
        }
        n++;
        e++;
    } while (n < DUEL_FIELD_SIDE_ZONE_COUNT);

    t = gDuel_aFieldCardRecords;
    e = t + DUEL_CARD_SIDE_RECORD_COUNT;
    n = 0;
    do {
        h = e->flags;
        if ((h & DUEL_CARD_FLAG_OCCUPIED) != 0) {
            a = e->object;
            if (a != 0) {
                if ((h & 0x400) != 0) {
                    Duel_DrawProjectedFieldCardFrame(e);
                } else {
                    *color_word = a->rgb;
                    Duel_RenderFieldCard3D(e, field_gt4, field_ft4, color_word);
                }
            }
        }
        n++;
        e++;
    } while (n < DUEL_FIELD_SIDE_ZONE_COUNT);

    SetGeomOffset(0, 0);
}

#include "display_object_system.h"

void DisplayObject_SetResourceVariantFromSign(
    DisplayObject *object,
    u32 value
)
{
    object->resource_variant = value >> 31;
}

#include "duel_grid.h"
#include "input.h"
#include "duel_card.h"
#include "duel_card_layout.h"
#include "card_constants.h"

/* The duel screen's digit-sprite drawing, in address order. The card-frame
   drawer Duel_DrawCardFrame and its output-position callback
   Duel_DrawCardAtOutputPosition draw a card with its number digits, and the
   life-point and deck-count readout follows: Duel_DrawDecimalDigits sorts one row of
   digit sprites, Duel_UpdateLifePointDisplay steps the drawn life-point
   counter a frame towards the real total, and
   Duel_DrawLifePointsAndDeckCounts calls both for each side. Both halves
   draw from the same texture page and CLUT, so one regional value names it.

   The callback was recorded at gcc_2_8_1_g8, and Duel_DrawDecimalDigits and
   Duel_UpdateLifePointDisplay at gcc_2_8_1_g0_split and gcc_2_8_1_g8; all
   compile to identical objects at this unit's gcc_2_8_1_g8_split. */

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "text_encode_decimal_digits.h"
#include "duel_card.h"
#include "display_object_layout.h"
#include "display_object.h"
#include "card_constants.h"
#include "duel_card_layout.h"
#include "sprite_primitive.h"
#include "../psyq/libgs.h"
#include "../ygo_types.h"
#include "duel_grid.h"
#include "ordering_tables.h"

/* Regional values: the screen height the frame is drawn above (0xF0 ->
 * 0x100), and the cxcy of the CLUT that the card frame and the status digits
 * both draw with on texture page 0x1E, CLUT (0x100, 0xF1) -> (0x280, 0xE1).
 * The European build (src/game/european/) defines its own. */
#ifndef DUEL_CARD_FRAME_SCREEN_HEIGHT
#define DUEL_CARD_FRAME_SCREEN_HEIGHT 0xF0
#endif
#ifndef DUEL_SPRITE_CLUT_CXCY
#define DUEL_SPRITE_CLUT_CXCY 0xF10100
#endif

#define POLY_FT4_BYTES(packet) ((u8 *)(packet))
#define CARD_FRAME_SCRATCH_BYTES(scratch) ((u8 *)(scratch))

/* Draws one card's frame on the duel field: position, the hand/field
 * flags, the card-number digits (via Duel_CalcCardStats and
 * Text_EncodeDecimalDigits) and the face/back sprite, through
 * DisplayObject_SubmitPacket on the 0x1F8003xx scratchpad records.
 *
 * The two records it fills are SpritePrim, the GsSPRITE sprite_primitive.h
 * already spells for the two display-object renderers, which build theirs at
 * the same 0x1F800320. This one builds a second at 0x1F800000 for the card
 * art and reuses 0x1F800320 for the number digits, the spell-class frame and
 * the card face, resetting the fields it needs between submissions.
 *
 * The third scratch record, at 0x1F800344, is the POLY_FT4 the clip path
 * primes (len 9, code 0x2C) with the object's colour, and the record at
 * 0x1F8003E0 is CardFrameScratch above.
 *
 * All three reads of the colour word at 0x0C sit between scratchpad stores,
 * and the target keeps every one of them where the source puts it. That used
 * to need the read spelled through a cast, because a member read is a struct
 * reference and GCC 2.8.1 floats one across stores that are not. With the
 * scratchpad records typed, those stores are struct references too, the
 * reads stay put, and every offset this function touches is a member. */
void Duel_DrawCardFrame(DisplayObject *object, GsOT *ordering_table, s32 screen_x, s32 screen_y) {
    u8 attack_digits[8];
    u8 defense_digits[8];
    DuelCardRecord *card;
    s32 fl;
    s32 i;
    SpritePrim *k;
    CardFrameScratch *o;
    s32 n;
    s32 d;
    s32 g1;
    s32 g2;
    s32 t;
    SpritePrim *z;
    POLY_FT4 *y;

    if ((u32)(screen_x + 0x33) < 0x173) {
        if (screen_y >= -0x3B) {
            if (screen_y < DUEL_CARD_FRAME_SCREEN_HEIGHT) {
                o = (CardFrameScratch *)SCRATCH_ADDR(0x3E0);
                k = (SpritePrim *)SCRATCH_ADDR(0x320);
                y = (POLY_FT4 *)SCRATCH_ADDR(0x344);
                z = (SpritePrim *)SCRATCH_ADDR(0);
                card = &gDuel_aCardRecords[object->field_6A];
                o->pos.h.x = screen_x;
                o->pos.h.y = screen_y;
                fl = object->ot_depth | 0x10000;
                t = object->attribute;
                k->attribute = t;
                z->attribute = t;
                object->resource_variant = 0;
                if (object->flags & DISPLAY_OBJECT_FLAG_CLIP_TEST) {
                    fl = object->ot_depth | 0xF0000;
                    if (DisplayObject_ProjectRotationFacing(object,
                                      (s16)o->pos.h.x + 0x1A,
                                      (s16)o->pos.h.y + 0x1E,
                                      (struct ProjectionOut *)SCRATCH_ADDR(0x3E0)) < 0) {
                        return;
                    }
                    g1 = object->rgb;
                    setlen(y, 9);
                    *(s32 *)&y->r0 = g1;
                    y->code = 0x2C;
                    if ((object->field_20.word & 0xFFFFFF) == 0) {
                        k->attribute = k->attribute | 0x80;
                        z->attribute = k->attribute;
                    }
                }
                k->tpage = 0x1E;
                g2 = object->rgb;
                k->cxcy.word = DUEL_SPRITE_CLUT_CXCY;
                k->rgb = g2;
                n = object->resource_root_index;
                if (n != 0) {
                    if (n < 0x29) {
                        k->extent.word = 0x10000C;
                        k->uv.b.hi = 0x70;
                        k->xy.h.x = o->pos.h.x + 0xF;
                        k->xy.h.y = o->pos.h.y + 0xF;
                        k->uv.b.lo = (n / 10) * 0xC;
                        DisplayObject_SubmitPacket(
                            k, POLY_FT4_BYTES(y), ordering_table, fl,
                            CARD_FRAME_SCRATCH_BYTES(o));
                        k->uv.b.lo = (n % 10) * 0xC;
                        k->xy.h.x = k->xy.h.x + 0xC;
                        DisplayObject_SubmitPacket(
                            k, POLY_FT4_BYTES(y), ordering_table, fl,
                            CARD_FRAME_SCRATCH_BYTES(o));
                    }
                } else if (object->resource_variant == 0) {
                    k->extent.word = 0x100020;
                    k->uv.word = 0x6000;
                    k->xy.h.x = o->pos.h.x + 0xA;
                    k->xy.h.y = o->pos.h.y + 0x28;
                    switch (object->resource_branch_index) {
                    case CARD_TYPE_EQUIP:
                        k->uv.b.lo = 0x20;
                    case CARD_TYPE_MAGIC:
                        DisplayObject_SubmitPacket(
                            k, POLY_FT4_BYTES(y), ordering_table, fl,
                            CARD_FRAME_SCRATCH_BYTES(o));
                        k->cxcy.h.cy = k->cxcy.h.cy + 1;
                        break;
                    case CARD_TYPE_TRAP:
                        k->uv.b.lo = 0x40;
                        DisplayObject_SubmitPacket(
                            k, POLY_FT4_BYTES(y), ordering_table, fl,
                            CARD_FRAME_SCRATCH_BYTES(o));
                        k->cxcy.h.cy = k->cxcy.h.cy + 2;
                        break;
                    case CARD_TYPE_RITUAL:
                        k->uv.b.lo = 0x60;
                        DisplayObject_SubmitPacket(
                            k, POLY_FT4_BYTES(y), ordering_table, fl,
                            CARD_FRAME_SCRATCH_BYTES(o));
                        k->cxcy.h.cy = k->cxcy.h.cy + 3;
                        break;
                    default:
                        k->extent.word = 0x100008;
                        k->uv.word = 0x7078;
                        k->xy.h.x = o->pos.h.x + 5;
                        DisplayObject_SubmitPacket(
                            k, POLY_FT4_BYTES(y), ordering_table, fl,
                            CARD_FRAME_SCRATCH_BYTES(o));
                        k->uv.b.hi = 0x58;
                        k->extent.wh.h = 8;
                        k->xy.h.x = o->pos.h.x + 0xE;
                        d = Duel_CalcCardStats(card);
                        Text_EncodeDecimalDigits((s16)d, 4, attack_digits);
                        Text_EncodeDecimalDigits(d >> 0x10, 4, defense_digits);
                        i = 3;
                        do {
                            k->uv.b.lo = attack_digits[i] * 8;
                            DisplayObject_SubmitPacket(
                                k, POLY_FT4_BYTES(y), ordering_table, fl,
                                CARD_FRAME_SCRATCH_BYTES(o));
                            k->xy.h.y = k->xy.h.y + 8;
                            k->uv.b.lo = defense_digits[i] * 8;
                            DisplayObject_SubmitPacket(
                                k, POLY_FT4_BYTES(y), ordering_table, fl,
                                CARD_FRAME_SCRATCH_BYTES(o));
                            i--;
                            k->xy.h.x = k->xy.h.x + 8;
                            k->xy.h.y = k->xy.h.y - 8;
                        } while (i >= 0);
                        break;
                    }
                    z->tpage = 0xE;
                    z->rgb = object->rgb;
                    z->xy.h.x = o->pos.h.x + 6;
                    z->xy.h.y = o->pos.h.y + 6;
                    i = (s8)card->table_index;
                    z->cxcy.h.cx = 0x380;
                    z->cxcy.h.cy = i + 0xE0;
                    z->uv.b.lo = (i % 5) * 0x28;
                    z->extent.word = 0x200028;
                    z->uv.b.hi = (i / 5) << 5;
                    DisplayObject_SubmitPacket(
                        z, POLY_FT4_BYTES(y), ordering_table, fl,
                        CARD_FRAME_SCRATCH_BYTES(o));
                }
                k->extent.word = 0x3C0034;
                k->uv.word = 0x8000;
                k->xy.word = o->pos.word;
                if (object->resource_root_index != 0) {
                    k->uv.word = 0xC000;
                }
                if (object->resource_variant != 0) {
                    k->uv.word = 0x8038;
                    k->cxcy.word = DUEL_SPRITE_CLUT_CXCY;
                }
                DisplayObject_SubmitPacket(
                    k, POLY_FT4_BYTES(y), ordering_table, fl,
                    CARD_FRAME_SCRATCH_BYTES(o));
            }
        }
    }
}

void Duel_DrawCardAtOutputPosition(DisplayObjectPosition *object, s32 ordering_table)
{
    Duel_DrawCardFrame((DisplayObject *)object, (GsOT *)ordering_table, object->out_x, object->out_y);
}

void Duel_DrawDecimalDigits(
    DisplayObject *style,
    GsSPRITE *digit,
    int source,
    int count
) {
    u8 temp[8];
    int i;

    Text_EncodeDecimalDigits(source, count, temp);
    for (i = count - 1; i >= 0; i--) {
        digit->u = temp[i] << 3;
        GsSortFastSprite(
            digit,
            gGraphics_apOrderingTables[style->ot_index],
            style->ot_depth
        );
        digit->x += 8;
    }
}

/* Steps the drawn life-point counter one frame towards the real total. The
 * step grows with the distance left, which is what makes the readout race
 * for a large swing and crawl for a small one.
 */
void Duel_UpdateLifePointDisplay(DuelSideState *side)
{
    s32 difference =
        side->displayed_life_points - side->life_points.signed_value;
    /* This order and the builtin preserve the target a1/a2 register roles. */
    s32 step;
    s32 magnitude;

    if (difference == 0) {
        return;
    }
    magnitude = __builtin_abs(difference);
    step = 9;
    if (magnitude >= 300) {
        step = 19;
    }
    if (magnitude >= 1000) {
        step = 47;
    }
    if (magnitude >= 3000) {
        step = 97;
    }
    if (difference > 0) {
        difference -= step;
        if (difference < 0) {
            difference = 0;
        }
    } else {
        difference += step;
        if (difference > 0) {
            difference = 0;
        }
    }
    side->displayed_life_points =
        side->life_points.unsigned_value + difference;
}

/* Draws the paired digit-sprite readout for both players' gDuel_aSideStates slots.
   gDuel_bActiveSide selects which side (0 or 1) currently renders in the "active"
   grey shade (0x808080) vs the dim shade (0x404040); the other side always
   gets the opposite shade. Each side draws two digit groups (4-digit then
   2-digit) offset from the display object widget->field_50 points at. The values
   are displayed life points and DECK_SIZE minus the signed deck_draw_cursor
   of the corresponding gDuel_aSideStates entry. */

#define GS_SPRITE_VIEW(sprite) ((GsSPRITE *)(sprite))

void Duel_DrawLifePointsAndDeckCounts(DisplayObject *widget)
{
    DisplayObject *pos;
    DuelStatusDigitPacket *scratch;
    u32 tmp10;

    Duel_UpdateLifePointDisplay(&gDuel_aSideStates[0]);
    Duel_UpdateLifePointDisplay(&gDuel_aSideStates[1]);
    pos = (DisplayObject *)widget->field_50.word;

    scratch = (DuelStatusDigitPacket *)SCRATCH_ADDR(0x320);
    scratch->attribute = 0x09000000;
    tmp10 = DUEL_SPRITE_CLUT_CXCY;
    scratch->cxcy = tmp10;
    scratch->tpage = 0x1E;
    scratch->uv = 0x5800;
    scratch->wh = 0x80008;
    scratch->rgb = DUEL_DISPLAY_COLOR_NORMAL;
    if (gDuel_bActiveSide == 0) {
        scratch->rgb = DUEL_DISPLAY_COLOR_DIMMED;
    }

    scratch->x = pos->xy.h.x - 3;
    scratch->y = pos->xy.h.y - 0xD;
    Duel_DrawDecimalDigits(
        pos,
        GS_SPRITE_VIEW(scratch),
        gDuel_aSideStates[1].displayed_life_points,
        4
    );

    scratch->x = pos->xy.h.x + 0xE;
    scratch->y = pos->xy.h.y - 5;
    Duel_DrawDecimalDigits(
        pos,
        GS_SPRITE_VIEW(scratch),
        DECK_SIZE - gDuel_aSideStates[1].deck_draw_cursor,
        2
    );

    scratch->rgb = DUEL_DISPLAY_COLOR_NORMAL;
    if (gDuel_bActiveSide != 0) {
        scratch->rgb = DUEL_DISPLAY_COLOR_DIMMED;
    }

    scratch->x = pos->xy.h.x - 3;
    scratch->y = pos->xy.h.y + 0xD;
    Duel_DrawDecimalDigits(
        pos,
        GS_SPRITE_VIEW(scratch),
        gDuel_aSideStates[0].displayed_life_points,
        4
    );

    scratch->x = pos->xy.h.x + 0xE;
    scratch->y = pos->xy.h.y + 5;
    Duel_DrawDecimalDigits(
        pos,
        GS_SPRITE_VIEW(scratch),
        DECK_SIZE - gDuel_aSideStates[0].deck_draw_cursor,
        2
    );
}

/* The two per-card checks DuelScene_UpdateFieldActions makes on the grid card
   under the cursor: whether it can still act this turn, and whether Triangle
   opens the card viewer for it. */

int DuelCard_CanActThisTurn(DuelCardRecord *object)
{
    u16 flags = object->flags;

    if (flags & DUEL_CARD_FLAG_OCCUPIED) {
        if (!(flags & DUEL_CARD_FLAG_USED_THIS_TURN)) {
            return 1;
        }
    }
    return 0;
}

s32 Duel_GetCardViewerRequestId(DuelCardRecord *card)
{
#ifdef VERSION_JAPAN
    /* The Japanese build asks only that the card be occupied and carry no
       display marker (flags 0x8000 set, 0x2000 clear), read as the word that
       ends at flags, as Duel_UpdateCardDisplayMarker does. The US build instead reads the
       cursor's grid cell and rejects a face-down card when that cell's value
       is DUEL_FIELD_SIDE_GRID_SLOT_COUNT or more. */
    if ((gInput_awPadPressed[0] & PAD_BUTTON_TRIANGLE) &&
        (*(s32 *)&card->terrain_modifier & 0xA0000000) == 0x80000000)
        return card->card_id;
    return 0;
#else
    DuelSelectionRecord *position = gDuel_pActiveSelectionRecord;
    s32 valid;
    u32 value =
        gDuel_abFieldGridCardRecordIndices.flat[position->row * DUEL_FIELD_ROW_SIZE + position->col];

    if (value < DUEL_FIELD_SIDE_GRID_SLOT_COUNT)
        valid = 1;
    else {
        s32 mask = card->flags & DUEL_CARD_FLAG_FACE_DOWN;
        valid = (u32)mask < 1;
    }

    if ((gInput_awPadPressed[0] & PAD_BUTTON_TRIANGLE) &&
        (card->flags & DUEL_CARD_FLAG_OCCUPIED) && valid)
        return card->card_id;
    return 0;
#endif
}

/* Effective attack and defense for one card, packed into a single word:
 * defense in the high half, attack in the low half. Both modifiers apply to
 * both stats, and each half is clamped to 0..CARD_STAT_MAX independently.
 *
 * Callers take one half or the other -- `& 0xFFFF` or a (u16) cast for the
 * attack, `>> 16` for the defense -- so the packing is the whole point of the
 * return value and it must stay a full word.
 */
s32 Duel_CalcCardStats(DuelCardRecord *card)
{
    s32 attack = card->attack + card->stat_modifier + card->terrain_modifier;
    s32 defense;
    if (attack < 0) attack = 0;
    if (attack > CARD_STAT_MAX) attack = CARD_STAT_MAX;
    defense = card->defense + card->stat_modifier + card->terrain_modifier;
    if (defense < 0) defense = 0;
    if (defense > CARD_STAT_MAX) defense = CARD_STAT_MAX;
    return (defense << 16) | attack;
}

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

void ViewState_ResetDefaults(void)
{
    ViewState *s = &gViewState_Current;
    /* This second base pointer is load-bearing, not leftover noise. Written
       as s->view.vrx, s->view.vry and s->view.vrz the three stores share the
       first base with plain displacements and the function comes out one
       instruction short of retail; the original clearly had a cursor here. */
    s32 *words = (s32 *)&s->view.vpx;

    s->distance = 0x258;
    s->angle = 0x400;
    s->orbit_angle = 0x100;
    s->field_0C = 0;
    s->view.rz = 0;
    s->view.super = 0;
    s->projection = MODEL_DEFAULT_PROJECTION;
    GsSetProjection(MODEL_DEFAULT_PROJECTION);
    words[3] = 0;
    s->field_06 = 0;
    words[4] = 0;
    s->field_08 = 0;
    words[5] = 0;
    s->field_0A = 0;
    ViewState_ApplyOrbit();
}

#include "duel_effect_object_pool.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

#include "file_transfer.h"
#include "main.h"
#ifdef VERSION_EUROPE
#include "card_preview.h"
#endif

#include "high_memory_addresses.h"
#include "shared_work_base.h"

/* Stage 7's sector count. A regional build defines its own. */
#ifndef DUEL_PACKAGE_STAGE7_SECTORS
#define DUEL_PACKAGE_STAGE7_SECTORS 44
#endif

void Duel_LoadPackageStage(FileTransferDescriptor *d, s32 stage)
{
    u32 flags;
    u32 mask;
#ifdef VERSION_EUROPE
    u32 destination;
    u32 masked;
#endif

    switch (stage) {
    case 0:
#ifdef VERSION_EUROPE
        mask = 0xFFDDFFFF;
        d->field_30.h.counter = 0x300;
        d->field_30.h.field_32 = 0x100;
        d->w = 0x40;
        d->h = 0x10;
        flags = gFile_dwTransferState;
        d->phase_size = 64 * FILE_SECTOR_SIZE;
        gFile_dwTransferState = flags & mask;
        gFile_dwTransferState |= 0x10000;
        d->transfer_mode = 2;
        d->phase_index = 0x10;
        d->sector_destination = (u32)gFile_TransferBufferBase;
        d->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
#else
        d->field_30.h.counter = 0x300;
        d->field_30.h.field_32 = 0x100;
        d->w = 0x40;
        d->h = 0x10;
        gFile_dwTransferState &= 0xFFDDFFFF;
        gFile_dwTransferState |= 0x10000;
        d->transfer_mode = 2;
        d->phase_size = 64 * FILE_SECTOR_SIZE;
        d->sector_destination = (u32)gFile_TransferBufferBase;
        d->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
#endif
        break;
#ifdef VERSION_EUROPE
    case 16:
        /* Keep both stores before the flag load, then share the later tail so
           phase index and transfer mode retain separate constant materializations. */
        mask = 0xFFDCFFFF;
        *(s32 *)&d->phase_size = 5 * FILE_SECTOR_SIZE;
        *(s32 *)&d->phase_index = 1;
        masked = gFile_dwTransferState;
        destination = (u32)gFile_TransferBufferBase;
        masked &= mask;
        goto europe_transfer_tail;
#endif
    case 1:
#ifdef VERSION_EUROPE
        gGraphics_aImageTransferRects[0].x = 0x380;
        gGraphics_aImageTransferRects[0].y = 0x160;
        gGraphics_aImageTransferRects[0].w = 0x40;
        gGraphics_aImageTransferRects[0].h = 0x10;
        LoadImage2(
            &gGraphics_aImageTransferRects[0],
            (u32 *)(gFile_TransferBufferBase + gText_bLanguageIndex * FILE_SECTOR_SIZE));
#endif
        d->phase_size = 4 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        d->sector_destination = d->alternate_sector_destination = (u32)gFile_TransferBufferBase;
        d->transfer_mode = 1;
        break;
    case 2:
#ifdef VERSION_EUROPE
        gGraphics_aImageTransferRects[0].x = 0x280;
        gGraphics_aImageTransferRects[0].y = 0xE0;
#else
        gGraphics_aImageTransferRects[0].x = 0x100;
        gGraphics_aImageTransferRects[0].y = 0xF0;
#endif
        gGraphics_aImageTransferRects[0].w = 0x100;
        gGraphics_aImageTransferRects[0].h = 0x10;
        LoadImage2(&gGraphics_aImageTransferRects[0], (u32 *)gFile_TransferBufferBase);
        d->sector_destination = d->alternate_sector_destination = (u32)gDuel_awEquipTable;
        d->phase_size = 5 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        d->transfer_mode = 1;
        break;
    case 3:
        d->sector_destination = d->alternate_sector_destination = (u32)gDuel_aFusionTable;
        d->phase_size = 32 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        d->transfer_mode = 1;
        break;
    case 4:
        d->sector_destination = d->alternate_sector_destination = (u32)gDuel_awRitualData;
        d->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        d->transfer_mode = 1;
        break;
    case 5:
        d->phase_size = 2 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        d->sector_destination = d->alternate_sector_destination = (u32)gFile_TransferBufferBase;
        d->transfer_mode = 1;
        break;
    case 6: {
        u32 flags;
#ifdef VERSION_EUROPE
        gGraphics_aImageTransferRects[0].x = 0x280;
        gGraphics_aImageTransferRects[0].y = 0xD0;
#else
        gGraphics_aImageTransferRects[0].x = 0;
        gGraphics_aImageTransferRects[0].y = 0xF0;
#endif
        gGraphics_aImageTransferRects[0].w = 0x100;
        gGraphics_aImageTransferRects[0].h = 8;
        LoadImage2(&gGraphics_aImageTransferRects[0], (u32 *)gFile_TransferBufferBase);
        d->field_30.h.counter = 0x200;
        d->field_30.h.field_32 = 0x100;
        d->w = 0x40;
        mask = 0xFFDDFFFF;
        flags = gFile_dwTransferState & mask;
        {
            /* Keep this tail distinct from the $v0 tails in cases 0 and 10. */
            u8 *image_ptr;

            gFile_dwTransferState = flags;
            mask = 0x10000;
            d->phase_size = mask;
            gFile_dwTransferState |= mask;
            d->transfer_mode = 2;
            d->h = 0x10;
            image_ptr = gFile_TransferBufferBase;
            d->sector_destination = (u32)image_ptr;
            d->alternate_sector_destination = (u32)(image_ptr + FILE_SECTOR_SIZE);
            break;
        }
    }
    case 7:
        mask = 0xFFDCFFFF;
        d->phase_size = DUEL_PACKAGE_STAGE7_SECTORS * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= mask;
        d->sector_destination = d->alternate_sector_destination = (u32)gFile_pDuelBankBase;
        d->transfer_mode = 1;
        break;
    case 8:
        d->sector_destination = d->alternate_sector_destination = (u32)gFile_StagingBuffer;
        d->phase_size = 3 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        d->transfer_mode = 1;
        break;
    case 9:
        d->sector_destination = d->alternate_sector_destination = (u32)gAiScript_abFieldScript;
        d->phase_size = 3 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        d->transfer_mode = 1;
        break;
    case 10: {
        u32 cleared;
        u32 loaded;
        u32 enabled;
        d->field_30.h.counter = 0x340;
        d->w = 0x40;
        d->h = 0x10;
        cleared = gFile_dwTransferState & 0xFFDDFFFF;
        gFile_dwTransferState = cleared;
        do { loaded = gFile_dwTransferState; } while (0);
        do { enabled = 0x10000; } while (0);
        d->field_30.h.field_32 = 0;
        gFile_dwTransferState = loaded | enabled;
        d->transfer_mode = 2;
        d->phase_size = 8 * FILE_SECTOR_SIZE;
        d->sector_destination = (u32)gFile_TransferBufferBase;
        d->alternate_sector_destination = (u32)(gFile_TransferBufferBase + FILE_SECTOR_SIZE);
        break;
    }
    case 11:
#ifdef VERSION_EUROPE
        mask = 0xFFDCFFFF;
        /* As in case 16, a plain u32 member store moves after the flag load. */
        *(s32 *)&d->phase_size = 5 * FILE_SECTOR_SIZE;
        masked = gFile_dwTransferState;
        destination = (u32)gRuntime_pSharedWorkBase;
        masked &= mask;
europe_transfer_tail:
        gFile_dwTransferState = masked;
        d->alternate_sector_destination = destination;
        d->sector_destination = destination;
        d->transfer_mode = 1;
        break;
#else
        d->phase_size = 5 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        d->sector_destination = d->alternate_sector_destination = (u32)gRuntime_pSharedWorkBase;
        d->transfer_mode = 1;
        break;
#endif
    case 12: {
        u32 flags;
        d->field_30.h.counter = 0x280;
        d->field_30.h.field_32 = 0x100;
        d->w = 0x40;
        mask = 0xFFDDFFFF;
        flags = gFile_dwTransferState & mask;
        {
            /* Keep this tail distinct from the $v0 tails in cases 0 and 10. */
            u8 *image_ptr;

            gFile_dwTransferState = flags;
            mask = 0x10000;
            d->phase_size = mask;
            gFile_dwTransferState |= mask;
            d->transfer_mode = 2;
            d->h = 0x10;
            image_ptr = gFile_TransferBufferBase;
            d->sector_destination = (u32)image_ptr;
            d->alternate_sector_destination = (u32)(image_ptr + FILE_SECTOR_SIZE);
            break;
        }
    }
    }
}

#include "model_slot_control.h"

#include "shared_work_base.h"

#ifndef DUEL_MODEL_SCENE_BUFFER_SIZE
#define DUEL_MODEL_SCENE_BUFFER_SIZE 0x63000
#endif

void Duel_InitModelScene(void)
{
    Model_ResetSceneView();
    Model_ResetSceneState();
    Model_LoadSlot(
        2,
        gRuntime_pSharedWorkBase,
        DUEL_MODEL_SCENE_BUFFER_SIZE,
        4
    );
}

#include "ai_script.h"
#include "card_constants.h"
#include "duel_grid.h"
#include "duel_card.h"

#define DUEL_SELECTION_RECORDS(address) ((DuelSelectionRecord *)(address))

void Duel_InitSideStates(void) {
    u16 sp[DUEL_SIDE_COUNT];
    DuelSideState *e;
    s32 k;
    s32 j;
    u32 m;
    u16 t;
    u8 *q;
    s8 *r;

    e = gDuel_aSideStates;
    if (gDuel_bOpponentID < 0) {
        sp[0] = gDuel_wTwoPlayerSide0LifePoints;
        sp[1] = gDuel_wTwoPlayerSide1LifePoints;
    } else {
        sp[1] = DUEL_STARTING_LIFE_POINTS;
        sp[0] = DUEL_STARTING_LIFE_POINTS;
    }

    for (k = 0; k < DUEL_SIDE_COUNT; k++, e++) {
        j = 4;
        r = (s8 *)e + j;
        /* Preserve the backward cursor while deriving the hand-array base. */
        for (; j >= 0; j--, r--) {
            r[(u32)&((DuelSideState *)0)->hand] = -1;
        }
        q = (u8 *)e;
        e->deck_draw_cursor = 0;
        t = sp[k];
        e->displayed_life_points = 0;
        e->swords_turns_remaining = 0;
        e->life_points.signed_value = t;
        e->max_life_points = t;
        e->card_view_mode = 0;
        for (m = 0; m < sizeof(DuelRankStatistics); m++) {
            *q = 0;
            q++;
        }
    }

    if (gDuel_bSide0ControllerSelector[0] < 0) {
        if (gDuel_bOpponentID < 0) {
            gDuel_aSideStates[1].card_view_mode = gDuel_bTwoPlayerCardViewMode;
            gDuel_aSideStates[0].card_view_mode = gDuel_bTwoPlayerCardViewMode;
        } else {
            gDuel_aSideStates[1].card_view_mode = -1;
        }
    } else {
        if (gDuel_bOpponentID >= 0) {
            gDuel_aSideStates[1].card_view_mode = -1;
        }
    }
    gDuel_pActiveSideState = &gDuel_aSideStates[gDuel_bActiveSide];
}

void Duel_ClearHandSlots(void)
{
    DuelHandSlot *entry = gDuelHand_aSlots;
    int i = 0;
    u8 *field_09 = &entry->selection_order;

    do {
        *(int *)(field_09 - 5) = 0;
        entry->object = 0;
        *field_09 = 0;
        field_09 += sizeof(DuelHandSlot);
        i++;
        entry++;
    } while (i < HAND_SIZE);
}

void Duel_InitSelectionRecords(void) {
    s32 row, j;
    for (row = 0; row < DUEL_SIDE_COUNT; row++) {
        DuelSelectionRecord *p = DUEL_SELECTION_RECORDS(
            gDuel_abSelectionSides + row * DUEL_SELECTION_SIDE_SIZE);
        for (j = 0; j < DUEL_SELECTION_RECORDS_PER_SIDE; p++, j++) {
            p->info_panel = 0;
            p->cursor_object = 0;
            p->hand = (DuelHandSlot *)0;
            p->field_actions_mode = 0;
            p->panel_variant = 1;
            p->record_index = j;
#ifdef VERSION_JAPAN
            p->text_channel = j;
#else
            p->text_channel = (j != 3) ? j : 1;
#endif
        }
    }
    DUEL_SELECTION_RECORDS(gDuel_abSelectionSides)[0].panel_variant = 0;
    DUEL_SELECTION_RECORDS(gDuel_abSelectionSides)[DUEL_SELECTION_RECORDS_PER_SIDE]
        .panel_variant = 0;
}

void Duel_ResetCardRecords(void)
{
    DuelCardRecord *entry = gDuel_aCardRecords;
    int i = 0;
    DuelCardFlagsCursor *flags = (DuelCardFlagsCursor *)&entry->flags;

    do {
        entry->object = 0;
        /* Keep the flags-relative cursor while deriving the data distance. */
        *(s32 *)(&flags->flags -
            ((u32)&((DuelCardRecord *)0)->flags -
             (u32)&((DuelCardRecord *)0)->data) / sizeof(flags->flags)) = 0;
        flags->flags = 0;
        flags++;
        i++;
        entry++;
    } while (i < DUEL_CARD_RECORD_COUNT);
}

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/inline_c.h"
#include "duel_card.h"
#ifdef DUEL_RESULT_LANGUAGE_SPRITE_TABLES
#endif

/* Small data at 0x8009AF20: opposing headings for the two duel sides.
   Field transitions and row movement feed the view-tween heading channel;
   Duel_InitGroundProjectionReference samples element 0 directly. */
u16 gDuel_awSideViewHeadings[2] = {
    0x0400,
    0x0C00,
};

/* Initialized data this unit owns at 0x800908A0 through 0x80090998, previously a generated
   blob (#2602).

   gDuel_aCardSlotGroundPositions is the thirty X/Z pairs Duel_ProjectCardSlotScreenPositions projects through
   the GTE, one per card slot, writing the biased result to gDuel_aCardSlotScreenPositions. The
   pairs form a five-column grid at x = -140, -70, 0, 70 and 140.
   debug_effect_screen.c also reads slot 6's pair as halfwords 0xC/0xD.
   Projection and object creation supply their own height, 0 or -24;
   these pairs are not projected screen pixels.

   The owning source records those signed pairs as DuelFieldPosition entries.
   The shared header preserves a flat u16 view for the projection candidate
   and debug screen because their exact code generation depends on halfword
   pointer arithmetic.

   gDuel_ExodiaCardPoses supplies the five Exodia card poses consumed by the retained
   DuelScene_UpdateExodiaResult candidate. Each card ID from 0x11 through 0x15 selects a
   record, whose first byte chooses one of the five shared display-object work
   slots and whose remaining bytes become object x/y after removing the
   candidate's 0x1A/0x1E coordinate biases. The trailing byte preserves the
   table's 16-byte extent before gDuelResult_aSpriteSpecsOpponent.

   gDuelResult_aSpriteSpecsOpponent and gDuelResult_aSpriteSpecsNoOpponent are the duel-result sprite tables, for a real
   opponent and for none. The outro already declared both as
   DuelResultSpriteSpec[][DUEL_RESULT_SPRITE_COUNT], and the extracted sizes
   agree: two winning sides by seven sprites by a four-byte spec. */
DuelFieldPosition gDuel_aCardSlotGroundPositions[DUEL_SCREEN_CARD_POSITION_COUNT] = {
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { -140, -95 },
    { -70, -95 },
    { 0, -95 },
    { 70, -95 },
    { 140, -95 },
    { -140, -161 },
    { -70, -161 },
    { 0, -161 },
    { 70, -161 },
    { 140, -161 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 140, 95 },
    { 70, 95 },
    { 0, 95 },
    { -70, 95 },
    { -140, 95 },
    { 140, 161 },
    { 70, 161 },
    { 0, 161 },
    { -70, 161 },
    { -140, 161 },
};

DuelExodiaCardPoseTable gDuel_ExodiaCardPoses = {
    {
        { 3, 104, 206 },
        { 2, 216, 206 },
        { 4, 69, 98 },
        { 1, 251, 98 },
        { 0, 160, 32 },
    },
    0,
};

#ifdef DUEL_RESULT_LANGUAGE_SPRITE_TABLES
/* The four language builds carry one opponent/no-opponent pair per
 * language (duel_scene.h) instead of the two tables
 * below, each indexed by the winning side and then by up to ten specs. */
DuelResultSpriteSpec gDuelResult_aFrenchSpriteSpecsOpponent[2][10] = {
    {
        { 60, 68, 6, 20 },
        { 108, 68, 10, 20 },
        { 150, 68, 6, 20 },
        { 198, 68, 11, 20 },
        { 242, 68, 13, 20 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
    {
        { 75, 68, 1, 20 },
        { 115, 68, 2, 20 },
        { 155, 68, 3, 20 },
        { 200, 68, 4, 20 },
        { 225, 68, 5, 8 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
};

DuelResultSpriteSpec gDuelResult_aFrenchSpriteSpecsNoOpponent[2][10] = {
    {
        { 136, 68, 7, 12 },
        { 184, 68, 9, 20 },
        { 60, 124, 6, 20 },
        { 108, 124, 10, 20 },
        { 150, 124, 6, 20 },
        { 198, 124, 11, 20 },
        { 242, 124, 13, 20 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
    {
        { 136, 68, 8, 12 },
        { 184, 68, 9, 20 },
        { 60, 124, 6, 20 },
        { 108, 124, 10, 20 },
        { 150, 124, 6, 20 },
        { 198, 124, 11, 20 },
        { 242, 124, 13, 20 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
};

DuelResultSpriteSpec gDuelResult_aGermanSpriteSpecsOpponent[2][10] = {
    {
        { 45, 68, 11, 20 },
        { 85, 68, 2, 20 },
        { 125, 68, 10, 20 },
        { 169, 68, 6, 8 },
        { 187, 68, 5, 20 },
        { 231, 68, 5, 20 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
    {
        { 78, 68, 1, 20 },
        { 118, 68, 2, 20 },
        { 158, 68, 3, 20 },
        { 201, 68, 4, 20 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 12, 20 },
        { 0, 0, 12, 20 },
        { 0, 0, 12, 20 },
        { 0, 0, 12, 20 },
    },
};

DuelResultSpriteSpec gDuelResult_aGermanSpriteSpecsNoOpponent[2][10] = {
    {
        { 136, 68, 7, 12 },
        { 184, 68, 9, 20 },
        { 45, 124, 11, 20 },
        { 85, 124, 2, 20 },
        { 125, 124, 10, 20 },
        { 169, 124, 6, 8 },
        { 187, 124, 5, 20 },
        { 231, 124, 5, 20 },
        { 0, 0, 2, 20 },
        { 0, 0, 5, 20 },
    },
    {
        { 136, 68, 8, 12 },
        { 184, 68, 9, 20 },
        { 45, 124, 11, 20 },
        { 85, 124, 2, 20 },
        { 125, 124, 10, 20 },
        { 169, 124, 6, 8 },
        { 187, 124, 5, 20 },
        { 231, 124, 5, 20 },
        { 0, 0, 2, 20 },
        { 0, 0, 5, 20 },
    },
};

DuelResultSpriteSpec gDuelResult_aItalianSpriteSpecsOpponent[2][10] = {
    {
        { 84, 68, 10, 20 },
        { 118, 68, 5, 8 },
        { 146, 68, 11, 20 },
        { 190, 68, 13, 20 },
        { 223, 68, 5, 8 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
    {
        { 71, 68, 1, 20 },
        { 111, 68, 2, 20 },
        { 150, 68, 3, 20 },
        { 194, 68, 4, 20 },
        { 236, 68, 5, 8 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
};

DuelResultSpriteSpec gDuelResult_aItalianSpriteSpecsNoOpponent[2][10] = {
    {
        { 136, 68, 7, 12 },
        { 184, 68, 9, 20 },
        { 84, 124, 10, 20 },
        { 118, 124, 5, 8 },
        { 146, 124, 11, 20 },
        { 190, 124, 13, 20 },
        { 223, 124, 5, 8 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
    {
        { 136, 68, 8, 12 },
        { 184, 68, 9, 20 },
        { 84, 124, 10, 20 },
        { 118, 124, 5, 8 },
        { 146, 124, 11, 20 },
        { 190, 124, 13, 20 },
        { 223, 124, 5, 8 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
};

DuelResultSpriteSpec gDuelResult_aSpanishSpriteSpecsOpponent[2][10] = {
    {
        { 80, 68, 6, 20 },
        { 132, 68, 13, 8 },
        { 164, 68, 5, 20 },
        { 218, 68, 13, 20 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
    {
        { 65, 68, 4, 20 },
        { 105, 68, 11, 8 },
        { 123, 68, 2, 20 },
        { 163, 68, 3, 20 },
        { 209, 68, 1, 20 },
        { 247, 68, 2, 20 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
};

DuelResultSpriteSpec gDuelResult_aSpanishSpriteSpecsNoOpponent[2][10] = {
    {
        { 136, 68, 7, 12 },
        { 184, 68, 9, 20 },
        { 80, 124, 6, 20 },
        { 132, 124, 13, 8 },
        { 164, 124, 5, 20 },
        { 218, 124, 13, 20 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
    {
        { 136, 68, 8, 12 },
        { 184, 68, 9, 20 },
        { 80, 124, 6, 20 },
        { 132, 124, 13, 8 },
        { 164, 124, 5, 20 },
        { 218, 124, 13, 20 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
};

#else
DuelResultSpriteSpec gDuelResult_aSpriteSpecsOpponent[][DUEL_RESULT_SPRITE_COUNT] = {
    {
        { 112, 68, 1, 20 },
        { 160, 68, 2, 20 },
        { 208, 68, 3, 20 },
        { 88, 124, 4, 20 },
        { 136, 124, 5, 8 },
        { 184, 124, 6, 20 },
        { 232, 124, 7, 8 },
    },
    {
        { 112, 68, 8, 20 },
        { 160, 68, 9, 20 },
        { 208, 68, 10, 20 },
        { 88, 124, 11, 20 },
        { 136, 124, 12, 8 },
        { 184, 124, 13, 20 },
        { 232, 124, 14, 8 },
    },
};

DuelResultSpriteSpec gDuelResult_aSpriteSpecsNoOpponent[][DUEL_RESULT_SPRITE_COUNT] = {
    {
        { 136, 68, 16, 12 },
        { 184, 68, 17, 20 },
        { 0, 0, 0, 20 },
        { 88, 124, 4, 20 },
        { 136, 124, 5, 8 },
        { 184, 124, 6, 20 },
        { 232, 124, 7, 8 },
    },
    {
        { 136, 68, 24, 12 },
        { 184, 68, 25, 20 },
        { 0, 0, 0, 20 },
        { 88, 124, 26, 20 },
        { 136, 124, 27, 8 },
        { 184, 124, 28, 20 },
        { 232, 124, 29, 8 },
    },
};
#endif

/* Projects all duel-slot coordinates into the biased screen-position table. */
void Duel_ProjectCardSlotScreenPositions(void)
{
    ProjectedPair p;
    SVECTOR *scratch;
    s32 i;

    SetGeomScreen(gViewState_Current.projection);
    SetGeomOffset(0xA0, 0x6C);
    GsSetLsMatrix(&gGraphics_LightSourceMatrix);
    scratch = (SVECTOR *)SCRATCH_ADDR(0x3E0);
    i = 0;
    do {
        s32 y;
        u16 x = gDuel_aCardSlotGroundPositions[i].x;

        scratch->vy = 0;
        scratch->vx = x;
        scratch->vz = (u16)gDuel_aCardSlotGroundPositions[i].y;
        gte_ldv0(scratch);
        gte_rtps();
        gte_stsxy(&p);
        gDuel_aCardSlotScreenPositions[i].x = p.x - 0x1A;
        y = p.y;
        gDuel_aCardSlotScreenPositions[i].y = y - 0x1E;
        if (gDuel_bActiveSide != 0) {
            if (i < DUEL_CARD_SIDE_RECORD_COUNT) {
                gDuel_aCardSlotScreenPositions[i].y = y - 0x1D;
            }
        } else if (i >= DUEL_CARD_SIDE_RECORD_COUNT) {
            gDuel_aCardSlotScreenPositions[i].y = y - 0x1D;
        }
        i++;
    } while (i < DUEL_CARD_RECORD_COUNT);
    SetGeomOffset(0, 0);
}

void Duel_InitGroundProjectionReference(void)
{
    ProjectedPair p;
    s32 y;

    ViewState_ResetDefaults();
    SetGeomScreen(gViewState_Current.projection);
    SetGeomOffset(0xA0, 0x6C);
    gViewState_Current.distance = 0x14E;
    gViewState_Current.orbit_angle = 0x3FE;
    gViewState_Current.angle = gDuel_awSideViewHeadings[0];
    ViewState_ApplyOrbit();
    GsSetLsMatrix(&gGraphics_LightSourceMatrix);
    {
        SVECTOR *scratch = (SVECTOR *)SCRATCH_ADDR(0x3E0);

        scratch->vx = 0x3E8;
        scratch->vy = 0;
        scratch->vz = 0x3E8;
        gte_ldv0(scratch);
        gte_rtps();
        gte_stsxy(&p);
    }
    /* Taking y through an s32 keeps the sign extension in the load, which is
       what makes it lh rather than the lhu the x read gets. */
    y = p.y;
    gDuel_sGroundReferenceScreenX = p.x - 0xA0;
    gDuel_sGroundReferenceScreenY = y - 0x6C;
    SetGeomOffset(0, 0);
    ViewState_ResetDefaults();
}

#include "duel_package.h"
#include "file_transfer.h"

/* Each terrain's package is DUEL_TERRAIN_LOAD_SECTOR_COUNT sectors long, and the
 * multiply is spelled as the shift sequence that matches. A regional build
 * defines all three. */
#ifndef DUEL_TERRAIN_LOAD_FIRST_SECTOR
#define DUEL_TERRAIN_LOAD_FIRST_SECTOR DUEL_TERRAIN_PACKAGE_FIRST_SECTOR
#endif

#ifndef DUEL_TERRAIN_LOAD_SECTOR_COUNT
#define DUEL_TERRAIN_LOAD_SECTOR_COUNT DUEL_TERRAIN_PACKAGE_SECTOR_COUNT
#endif

/* value * 0xEB */
#ifndef DUEL_TERRAIN_LOAD_OFFSET
#define DUEL_TERRAIN_LOAD_OFFSET(value) ((((value) * 15) * 4 - (value)) * 4 - (value))
#endif

void Duel_LoadTerrainPackage(void)
{
    int value = gDuel_bTerrain;
    int index = DUEL_TERRAIN_LOAD_OFFSET(value) + DUEL_TERRAIN_LOAD_FIRST_SECTOR;
    File_RequestAsyncTransfer(
        0, 0, index, DUEL_TERRAIN_LOAD_SECTOR_COUNT,
        Duel_LoadPackageStage, 0, 0);
    File_WaitForTransfers();
}

#include "card_constants.h"
#include "duel_card.h"
#include "mem_card.h"
#include "save_pair_workspace.h"
#include "duel_grid.h"
#include "display_object_layout.h"
#include "duel_package.h"
#include "file_transfer.h"
#include "main_services.h"
#include "sound_pending_entries.h"
#include "sound_voice_selection.h"
#include "duel_session_controls.h"
#include "card_preview.h"
#include "trig_constants.h"
#include "duel_card_layout.h"

/* Initializes the duel scene, then selects and shuffles both deck buffers. */

/* The terrain package's first sector and length. The Japanese WA.MRG gives
   each terrain 0xEF sectors from 0x16B5 (see duel_load_terrain_package.c),
   so a regional build supplies its own. */
#ifndef DUEL_INIT_TERRAIN_SECTOR
#define DUEL_INIT_TERRAIN_SECTOR(value) \
    ((((value) * 15) * 4 - (value)) * 4 - (value)) + \
        DUEL_TERRAIN_PACKAGE_FIRST_SECTOR
#define DUEL_INIT_TERRAIN_SECTOR_COUNT DUEL_TERRAIN_PACKAGE_SECTOR_COUNT
#endif
/* Likewise the duelist data's first sector (duel_result_runtime.c's
   DUEL_RESULT_DUELIST_DATA_FIRST_SECTOR is the same value). */
#ifndef DUEL_INIT_DUELIST_DATA_FIRST_SECTOR
#define DUEL_INIT_DUELIST_DATA_FIRST_SECTOR DUELIST_DATA_FIRST_SECTOR
#endif
#ifndef DUEL_INIT_RESOURCE_CLUT_X
#define DUEL_INIT_RESOURCE_CLUT_X 0
#define DUEL_INIT_RESOURCE_CLUT_Y 0xFF
#endif

void Duel_InitScene(void)
{
    DisplayObject *obj;
    u8 *p;
    u8 *q;
    s32 value;
    s32 side;
    DuelEffectResourceRecord *pane;
    u8 *prev;
    s8 *pid;

    pid = &gDuel_bOpponentID;
    SD_ResetPendingEntries();
    SD_ReplaceValueLink(1);
    Main_AdvanceFrames(4);
    File_WaitForTransfers();
    value = gDuel_bTerrain;
    File_RequestAsyncTransfer(
        0, 0,
        DUEL_INIT_TERRAIN_SECTOR(value),
        DUEL_INIT_TERRAIN_SECTOR_COUNT, Duel_LoadPackageStage, 0, 0);
    File_WaitForTransfers();
    gDuel_bOutroWinnerSide = -1;
    gDuel_wSceneStateFlags = 11;
    gDuel_bQuitDialogState = 0;
    gDuel_wDisplayTransitionFlags = 0;
    gDuelCursor_PickFlags = 0;
    gDuel_wViewTweenFramesRemaining = 0;
    gDuel_wCardEffectFlags = 0;
    gDuel_wOutcomeFlags = 0;
    gDuel_bSceneSubStep = 0;
    if (gDuel_bResumeFromAnimatedBattle != 1) {
        if (gDuel_bOpponentID >= 0) {
            File_RequestAsyncTransfer(
                0, 0,
                gDuel_bOpponentID * (DUELIST_DATA_SECTOR_COUNT - 1) +
                    gDuel_bOpponentID + DUEL_INIT_DUELIST_DATA_FIRST_SECTOR,
                DUELIST_DATA_SECTOR_COUNT, 0, 0,
                (s32)gDuel_awOpponentDeckPool
            );
        }
        gDuel_bActiveSide = 0;
        gDuel_wSceneStateFlags = 1;
        Duel_ResetCardRecords();
        Duel_InitSelectionRecords();
        Duel_InitSideStates();
        gDuel_wOutcomeFlags |= 0x1000;
    }
    gDuel_pActiveSideState = &gDuel_aSideStates[gDuel_bActiveSide];
    Duel_InitGroundProjectionReference();
    gViewState_Current.angle =
        gDuel_bActiveSide * TRIG_ANGLE_HALF_TURN + TRIG_ANGLE_QUARTER_TURN;
    ViewState_ApplyOrbit();
    Duel_ClearHandSlots();
    pane = &gCardPreview_Resources[0];
    DuelEffect_ResetRequestPool();
    DuelEffect_ClearResourceObjectPointers(0);
    pane->src_x = 0;
    pane->src_y = 256;
    pane->clut_x = DUEL_INIT_RESOURCE_CLUT_X;
    pane->clut_y = DUEL_INIT_RESOURCE_CLUT_Y;
    DuelEffect_ClearResourceObjectPointers(1);
    pane[1].src_x = 64;
    pane[1].src_y = 256;
    pane[1].clut_x = DUEL_INIT_RESOURCE_CLUT_X;
    pane[1].clut_y = DUEL_INIT_RESOURCE_CLUT_Y - 1;
    Model_SetFogDrawFlags(0);
    Duel_InitModelScene();
    File_WaitForTransfers();
    gDuel_pActiveFieldGrid =
        &gDuel_abFieldGridCardRecordIndices.flat[gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT];
    obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(obj, 12, 24, 4, 2, gDuel_bTerrain, 11, 732);
    DisplayObject_SelectOrderingTable1(obj);
    /* Both of this function's flag merges go through the member's address
       as s16 instead of obj->flags, and the spelling is load-bearing rather
       than untidy.

       Writing obj->flags lets GCC 2.8.1 schedule the sh into the delay slot
       of the following jal and drop the nops around it, which shortens the
       executable by sixteen bytes. *(u16 *)&obj->flags fails identically,
       because fold turns an address-of cast to the member's own type back
       into the member access; the s16 cast differs from the u16 member, so
       it stays a non-struct store. Each site was measured on its own:
       leaving only the second as member access still loses twelve bytes, and
       the first accounts for the other four. */
    *(s16 *)&obj->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    side = (u32)gDuel_bOpponentID >> 31;
    gDuel_pTerrainIcon = obj;
    obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(
        obj, 280, 32, 4, side, 0, 11, 748
    );
    DisplayObject_SelectOrderingTable1(obj);
    *(s16 *)&obj->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    if (gDuel_bActiveSide != 0) {
        obj->field_40.clut.x += 16;
    }
    gDuel_pLifePointsPanel = obj;
    obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);
    DisplayObject_SelectOrderingTable1(obj);
    DisplayObject_SetDepthOffset(obj, 1);
    obj->field_4C = (s32)Duel_DrawLifePointsAndDeckCounts;
    prev = (u8 *)gDuel_pLifePointsPanel;
    obj->field_50.word = (s32)prev;
    gMain_apFrameServiceCallbacks[3] = Duel_DrawFieldCards;
    if (gDuel_bResumeFromAnimatedBattle != 1) {
        p = 0;
        gDuel_pSide1SaveData = 0;
        gDuel_pSide0SaveData = 0;
        if (pid[-1] < 0) {
            if (gDuel_bOpponentID < 0) {
                gDuel_pSide0SaveData = (SaveDataState *)gSaveData_aPairWorkspace;
                gDuel_pSide1SaveData =
                    (SaveDataState *)(gSaveData_aPairWorkspace + TWO_PLAYER_SAVE_SLOT_STRIDE);
                Duel_ShuffleBothDecks(
                    (const u16 *)gSaveData_aPairWorkspace,
                    (const u16 *)(gSaveData_aPairWorkspace + TWO_PLAYER_SAVE_SLOT_STRIDE)
                );
                return;
            }
            p = (u8 *)gDuel_awPlayerDeck;
            gDuel_pSide0SaveData = (SaveDataState *)p;
            if (gDuel_bOpponentID < DUEL_MASTER_K_OPPONENT_ID) {
                q = 0;
                Duel_ShuffleBothDecks((const u16 *)p, (const u16 *)q);
                return;
            }
        }
        q = p;
        Duel_ShuffleBothDecks((const u16 *)p, (const u16 *)q);
    }
}

void Duel_UpdateCardDisplayMarker(DuelCardDisplayObject *object)
{
    DuelCardRecord *card = &gDuel_aCardRecords[object->card_index];

    if ((*(s32 *)&card->terrain_modifier & 0xA0000000) != 0xA0000000) {
        return;
    }
    if (gDuel_pActiveSideState->card_view_mode != 0) {
        object->deck_number_marker = ((DuelDeckCardRecord *)card->data)->pre_shuffle_index + 1;
    }
    if (gDuel_pActiveSideState->card_view_mode < 0) {
        object->deck_number_marker = 0xFF;
    }
}

void Duel_UpdateCardDisplayState(DuelCardDisplayObject *object)
{
    DuelCardRecord *card = &gDuel_aCardRecords[object->card_index];
    u16 flags;

    if (!(card->flags & DUEL_CARD_FLAG_DISPLAY_MARKER)) {
        object->deck_number_marker = 0;
    }
    flags = object->flags & ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
    object->flags = flags;
    if (card->flags &
        (DUEL_CARD_FLAG_DEFENSE_POSITION | DUEL_CARD_FLAG_FACE_DOWN)) {
        object->flags = flags | DISPLAY_OBJECT_FLAG_CLIP_TEST;
        object->rotation_y = 0;
        if (card->flags & DUEL_CARD_FLAG_FACE_DOWN) {
            object->rotation_y = 0x80;
        }
        object->rotation_z = 0;
        if (card->flags & DUEL_CARD_FLAG_DEFENSE_POSITION) {
            object->rotation_z = 0xC0;
        }
    }
    object->color = DUEL_DISPLAY_COLOR_NORMAL;
    if (card->flags & DUEL_CARD_FLAG_USED_THIS_TURN) {
        object->color = DUEL_DISPLAY_COLOR_DIMMED;
    }
}

DuelCardDisplayObject *Duel_CreateCardDisplayObject(DuelCardRecord *record, s32 x, s32 y)
{
    DuelCardDisplayObject *p =
        DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);
    s32 *tbl;
    s32 k;

    k = record->card_id - 1;
    tbl = gDuel_adwCardStats;
    p->deck_number_marker = 0;
    p->card_type = (tbl[k] >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
    p->is_back_facing = 0;
    p->card_index =
        ((u32)record - (u32)gDuel_aCardRecords) / DUEL_CARD_RECORD_SIZE;
    p->deck_index = ((u8 *)record->data)[2];
    p->out_x = x;
    p->out_y = y;
    p->attribute |= DISPLAY_OBJECT_ATTRIBUTE_8BPP;
    p->back_face_handler.callback = DisplayObject_SetResourceVariantFromSign;
    DisplayObject_SelectOrderingTable1((DisplayObject *)p);
    p->draw_callback = Duel_DrawCardAtOutputPosition;
    Duel_UpdateCardDisplayState(p);
    Duel_UpdateCardDisplayMarker(p);
    return p;
}

DuelCardDisplayObject *Duel_CreateHandCardDisplayObject(DuelCardRecord *card)
{
    DuelCardDisplayObject *result =
        /* The x/y words the caller left in $a1/$a2 pass through. */
        ((DuelCardDisplayObject *(*)())Duel_CreateCardDisplayObject)((u8 *)card);

    if (gDuel_pActiveSideState->card_view_mode != 0) {
        card->flags |= DUEL_CARD_FLAG_DISPLAY_MARKER;
        result->deck_number_marker =
            ((DuelDeckCardRecord *)card->data)->pre_shuffle_index + 1;
        if (gDuel_pActiveSideState->card_view_mode < 0) {
            result->deck_number_marker = 0xFF;
        }
    }
    return result;
}

/* Same gDuel_aCardRecords[] stat table (0x1C-byte stride) as Duel_UpdateCardDisplayState, but with
   the rotation_y/rotation_z bit mapping SWAPPED relative to it: here the face-down
   flag marks rotation_z, while the defense-position flag marks rotation_y.

   Clears the display flags' clip-test bit and rotation_z, then re-derives
   rotation_z and rotation_y from the selected gDuel_aCardRecords record's face-down and
   defense-position flags; always sets color to DUEL_DISPLAY_COLOR_NORMAL, or
   DUEL_DISPLAY_COLOR_DIMMED if DUEL_CARD_FLAG_USED_THIS_TURN is set; runs
   Duel_UpdateCardDisplayMarker, then clears deck_number_marker unless
   DUEL_CARD_FLAG_DISPLAY_MARKER is set. */
void Duel_ApplyCardObjectFlags(DuelCardDisplayObject *object) {
    u16 flags = object->flags;
    s32 type = object->card_index;
    DuelCardRecord *rec;

    object->rotation_z = 0;
    object->flags = flags & ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
    rec = &gDuel_aCardRecords[type];

    if (rec->flags & DUEL_CARD_FLAG_FACE_DOWN) {
        object->rotation_z = DUEL_CARD_DISPLAY_FACE_DOWN_VALUE;
    }
    object->rotation_y = 0;
    if (rec->flags & DUEL_CARD_FLAG_DEFENSE_POSITION) {
        object->rotation_y = DUEL_CARD_DISPLAY_DEFENSE_VALUE;
    }
    object->color = DUEL_DISPLAY_COLOR_NORMAL;
    if (rec->flags & DUEL_CARD_FLAG_USED_THIS_TURN) {
        object->color = DUEL_DISPLAY_COLOR_DIMMED;
    }
    Duel_UpdateCardDisplayMarker(object);
    if (!(rec->flags & DUEL_CARD_FLAG_DISPLAY_MARKER)) {
        object->deck_number_marker = 0;
    }
}

DisplayObject *Duel_CreateFieldCardPanel(int x, int y)
{
    DisplayObject *object =
        DisplayObject_AcquireSlot(DisplayObject_FindFreeSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(object, x, y, 0, 1, 0, 0x1F, 0x100);
    DisplayObject_SelectOrderingTable1(object);
    DisplayObject_SetDepthOffset(object, -2);
    object->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    return object;
}

int Duel_GetCardEffectVariant(CardObject *object)
{
    int value = 1;
    switch (object->type) {
    case CARD_TYPE_MAGIC:
    case CARD_TYPE_EQUIP:
        value = 2;
        break;
    case CARD_TYPE_TRAP:
        value = 3;
        break;
    case CARD_TYPE_RITUAL:
        value = 4;
        break;
    }
    if (object->rotation_z != 0) value |= 0x80;
    return value;
}

#include "display_object.h"
#include "duel_card.h"
#include "card_presentation.h"
#include "duel_card_staging.h"
#include "card_constants.h"
#include "fade.h"
#include "file_transfer.h"

#include "duel_card_layout.h"
#include "duel_grid.h"
#include "sound.h"

/* Regional values: the record's panel_target_y (0xAE -> 0xBE), the base of
 * field_40 (0x2E0 -> 0x360) and the y Duel_CreateHandCardDisplayObject places a hand card at
 * (0x292 -> 0x2A2). The European build (src/game/european/) defines its own. */
#ifndef DUEL_PHASE_PANEL_TARGET_Y
#define DUEL_PHASE_PANEL_TARGET_Y 0xAE
#endif
#ifndef DUEL_PHASE_FIELD_40_BASE
#define DUEL_PHASE_FIELD_40_BASE 0x2E0
#endif
#ifndef DUEL_PHASE_HAND_CARD_Y
#define DUEL_PHASE_HAND_CARD_Y 0x292
#endif

/* Three contiguous entries from the gDuel_apfnSceneStateHandler duel-phase callback table:
   resume/replay reconstruction, initial deck and selection setup, and draw
   phase hand reconstruction. All three use gDuel_wSceneStateFlags as their first-call
   latch and rebuild active duel-card and side state around fade or transfer
   gates. */

void DuelScene_UpdateResume(void)
{
    DuelCardRecord *rec;
    DuelEffectRequest *obj;
    u8 *b;
    DisplayObject *card;
    s32 i;
    s32 keep;
    s32 y;
    u16 flags;
    s8 n;

    if ((gDuel_wSceneStateFlags & DUEL_SCENE_FLAG_INITIALIZED) == 0) {
        gDuel_wSceneStateFlags |= DUEL_SCENE_FLAG_INITIALIZED;
        rec = gDuel_aFieldCardRecords;
        for (i = HAND_SIZE; i < DUEL_CARD_SIDE_RECORD_COUNT; i++, rec++) {
            flags = rec->flags;
            if (flags & DUEL_CARD_FLAG_OCCUPIED) {
                keep = flags & 0x7A00;
                y = rec->stat_modifier;
                Duel_SetupFieldCardFromDeck(
                    i, ((DuelDeckCardRecord *)rec->data)->deck_index);
                rec->flags |= keep;
                rec->stat_modifier = y;
                Duel_ApplyCardObjectFlags(
                    (DuelCardDisplayObject *)rec->object);
            }
        }
        rec = gDuel_aFieldCardRecords + 15;
        for (i = 20; i < DUEL_CARD_RECORD_COUNT; i++, rec++) {
            flags = rec->flags;
            if (flags & DUEL_CARD_FLAG_OCCUPIED) {
                keep = flags & 0x7A00;
                y = rec->stat_modifier;
                Duel_SetupFieldCardFromDeck(
                    i, ((DuelDeckCardRecord *)rec->data)->deck_index);
                rec->flags |= keep;
                rec->stat_modifier = y;
                Duel_ApplyCardObjectFlags(
                    (DuelCardDisplayObject *)rec->object);
            }
        }
        ViewState_ApplyOrbit();
        for (i = 0; i < DUEL_SIDE_COUNT; i++) {
            if (gDuel_aSideStates[i].swords_turns_remaining != 0) {
                obj = DuelEffect_AllocateRequest(0x15);
                obj->phase = i + 2;
                obj->flags |= DUEL_EFFECT_REQUEST_FLAG_NONBLOCKING;
                gDuel_apSwordsEffectObjects[i] = obj;
            }
        }
        if (gDuel_pActiveSideState->rank.result_adjustment ==
            DUEL_RANK_ADJUST_EXODIA_WIN) {
            gDuel_wSceneStateFlags |= 0x2000;
            for (i = 0; i < DUEL_FIELD_SIDE_ZONE_COUNT; i++) {
                rec = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices.flat[
                    i + gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT]];
                if (rec->flags & DUEL_CARD_FLAG_OCCUPIED) {
                    DuelCard_RemoveFromField(rec);
                }
            }
        } else {
            if (gDuel_aSideStates[0].life_points.signed_value != 0 &&
                gDuel_aSideStates[1].life_points.signed_value != 0) {
                SD_BGMPlay(gDuel_wBgmId);
            }
        }
        Fade_StartIn();
        return;
    }

    if ((gDuel_wSceneStateFlags & 0x4000) == 0) {
        if (((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             gFile_dwCancelRequestState) != 0) {
            return;
        }
        if ((gFade_State.flags & FADE_FLAG_ACTIVE) != 0) {
            return;
        }
        gDuel_wSceneStateFlags |= 0x4000;
        gDuel_bPresentationCursor = 2;
        if ((gDuel_wSceneStateFlags & 0x2000) != 0) {
            gDuel_wSceneStateFlags = 0xC;
        }
        return;
    }

    for (;;) {
        n = gDuel_bPresentationCursor - 1;
        gDuel_bPresentationCursor = n;
        if (n < 0) {
            gDuel_wSceneStateFlags = 5;
            return;
        }
        if (DUEL_BATTLE_REMOVAL_SLOT(n) >= 0) {
            break;
        }
    }
    {
        s32 replay_offset = DUEL_CARD_STAGING_REPLAY_BASE_OFFSET;
        DuelCardReplayRecordBlock *replay;

        b = gDuel_abCardStagingBuffer;
        replay = (DuelCardReplayRecordBlock *)(
            (u8 *)&((DuelCardRecord *)b)[DUEL_BATTLE_REMOVAL_SLOT(n)] + replay_offset);
        card = replay->record.object;
    }
    ViewState_ApplyOrbit();
    obj = DuelEffect_CreateRequest(0xB);
    obj->field_00 = card->xy.h.x;
    obj->field_02 = card->xy.h.y;
    obj->field_04 = (u16)card->field_34.h.z;
    obj->phase = Duel_GetCardEffectVariant((CardObject *)card);
    DuelCard_RemoveFromField(&gDuel_aCardRecords[card->field_6A]);
    SD_SEPlayFull(0x1F);
}

void DuelScene_UpdateStartup(void)
{
    u16 *w;
    DuelDeckCardRecord *rec;
    s32 i;
    s32 atk;
    s32 def;
    s32 stat;
    s32 stat2;

    w = (u16 *)&gViewState_Current;
    if ((gDuel_wSceneStateFlags & DUEL_SCENE_FLAG_INITIALIZED) == 0) {
        gDuel_wSceneStateFlags |= DUEL_SCENE_FLAG_INITIALIZED;
        Duel_RequestCombinedDeckData();
        gViewState_Current.distance = 0x4B0;
        w[2] = 0x358;
        w[1] = 0x16C0;
        ViewState_ApplyOrbit();
        gDuel_bSceneSubStep = 2;
        Fade_StartIn();
        return;
    }

    switch (gDuel_bSceneSubStep & 0x1F) {
    case 2:
        gViewState_Current.distance -= 2;
        w[2] -= 2;
        w[1] -= 0x10;
        if ((s16)w[1] <= 0x400) {
            gViewState_Current.distance = 0x258;
            w[2] = 0x100;
            w[1] = 0x400;
            gDuel_bSceneSubStep = 3;
        }
        ViewState_ApplyOrbit();
        break;
    case 3:
        if ((gDuel_bSceneSubStep & 0x80) == 0) {
            gDuel_bSceneSubStep |= 0x80;
            gDuel_wDisplayTransitionFlags = 2;
            gDuel_pActiveSelectionRecord = (DuelSelectionRecord *)(gDuel_abSelectionSides +
                gDuel_bActiveSide * DUEL_SELECTION_SIDE_SIZE);
            gDuel_pActiveSelectionRecord->panel_target_y = DUEL_PHASE_PANEL_TARGET_Y;
        }
        if (gDuel_wDisplayTransitionFlags == 0) {
            gDuel_bSceneSubStep = 4;
        }
        break;
    case 4: {
        u32 mask = FILE_TRANSFER_REQUEST_BLOCKED_MASK;

        if (((gFile_dwTransferState & mask) |
             gFile_dwCancelRequestState) != 0) {
            break;
        }
        SD_BGMPlay(gDuel_wBgmId);
        Duel_PopulateCombinedDeckData();
        atk = 0;
        def = 0;
        rec = gDuel_aDeckCardRecords;
        for (i = 0; i < DECK_SIZE; i++, rec++) {
            stat = gDuel_adwCardStats[rec->id - 1];
            atk += (stat & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
            def += ((stat >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) *
                   CARD_STAT_SCALE;
        }
        gDuel_aSideStates[0].deck_average_attack = atk / DECK_SIZE;
        gDuel_aSideStates[0].deck_average_defense = def / DECK_SIZE;
        atk = 0;
        def = 0;
        rec = &gDuel_aDeckCardRecords[DECK_SIZE];
        for (i = 0; i < DECK_SIZE; i++, rec++) {
            stat2 = gDuel_adwCardStats[rec->id - 1];
            atk += (stat2 & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
            def += ((stat2 >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) *
                   CARD_STAT_SCALE;
        }
        gDuel_aSideStates[1].deck_average_attack = atk / DECK_SIZE;
        gDuel_aSideStates[1].deck_average_defense = def / DECK_SIZE;
        gDuel_bSceneSubStep = 5;
        break;
    }
    case 5:
        Duel_ClearHandSlots();
        gDuel_bPendingDrawCount = HAND_SIZE;
        gDuel_wSceneStateFlags = 3;
        ((DuelSelectionRecord *)(gDuel_abSelectionSides +
            gDuel_bActiveSide * DUEL_SELECTION_SIDE_SIZE))->hand = gDuelHand_aSlots;
        break;
    }
}

/* Draw phase entry. On its first call it sets the phase flag, points the
   side state and selection records at the current side, ticks down the
   side's Swords counter (releasing its effect object when
   it reaches zero), resets the used flag on every card record, then
   rebuilds the hand: the five hand slot bytes are copied out and cleared,
   each valid one gets its card record set up and a hand object spawned at
   a fixed x and a 0x3C-spaced y, and the number of empty slots is stored
   for the draw. Later calls only clear the phase flag once the message
   state has returned to zero. */
void DuelScene_UpdateDrawPhase(void) {
    u8 hand[HAND_SIZE];
    DuelCardRecord *rec;
    u8 *fl;
    DuelCardRecord *placed;
    DuelHandSlot *base;
    DuelHandSlot *slot;
    u8 *p;
    s32 side;
    s32 i;
    s32 n;
    s32 idx;
    s32 y;
    u16 flags;
    s8 c;

    if ((gDuel_wSceneStateFlags & DUEL_SCENE_FLAG_INITIALIZED) == 0) {
        gDuel_wSceneStateFlags |= DUEL_SCENE_FLAG_INITIALIZED;
        gDuel_pLifePointsPanel->field_40.clut.x =
            (gDuel_bActiveSide << 4) | DUEL_PHASE_FIELD_40_BASE;
        Duel_ClearHandSlots();
        side = gDuel_bActiveSide;
        gDuel_pActiveSideState = &gDuel_aSideStates[side];
        gDuel_pActiveSelectionRecord = (DuelSelectionRecord *)(gDuel_abSelectionSides +
                side * DUEL_SELECTION_SIDE_SIZE);
        base = gDuelHand_aSlots;
        gDuel_pActiveSelectionRecord->hand = base;
        if (gDuel_pActiveSideState->swords_turns_remaining != 0) {
            c = gDuel_pActiveSideState->swords_turns_remaining - 1;
            gDuel_pActiveSideState->swords_turns_remaining = c;
            if (c <= 0) {
                gDuel_pActiveSideState->swords_turns_remaining = 0;
                gDuel_apSwordsEffectObjects[gDuel_bActiveSide]->phase =
                    0xFFFD - gDuel_bActiveSide;
                gDuel_apSwordsEffectObjects[gDuel_bActiveSide] = 0;
            }
        }
        gDuel_pActiveSideState->rank.turns_taken++;
        rec = gDuel_aCardRecords;
        for (i = 0; i < DUEL_CARD_RECORD_COUNT; i++, rec++) {
            flags = rec->flags;
            if (flags & DUEL_CARD_FLAG_OCCUPIED) {
                rec->flags = flags & ~DUEL_CARD_FLAG_USED_THIS_TURN;
                Duel_ApplyCardObjectFlags(
                    (DuelCardDisplayObject *)rec->object);
            } else {
                rec->flags = 0;
            }
        }
        for (i = 0; i < HAND_SIZE; i++) {
            hand[i] = gDuel_pActiveSideState->hand[i];
            gDuel_pActiveSideState->hand[i] = -1;
        }
        n = 0;
        i = n;
        slot = base;
        y = 0xE;
        idx = gDuel_bActiveSide * DUEL_CARD_SIDE_RECORD_COUNT;
        rec = &gDuel_aCardRecords[idx];
        placed = rec;
        for (; i < HAND_SIZE; i++, rec++) {
            p = &hand[i];
            rec->flags = 0;
            rec->object = 0;
            if ((s8)*p >= 0) {
                gDuel_pActiveSideState->hand[n] = *p;
                Duel_SetupCardRecord(idx, (s8)*p);
                idx++;
                n++;
                slot->object = (DisplayObject *)((DuelCreateHandCardDisplayObjectAt)Duel_CreateHandCardDisplayObject)(placed, y, DUEL_PHASE_HAND_CARD_Y);
                slot++;
                y += 0x3C;
                placed++;
            }
        }
        gDuel_bPendingDrawCount = HAND_SIZE - n;
        gDuel_wDisplayTransitionFlags = 2;
        gDuel_pActiveSelectionRecord = (DuelSelectionRecord *)(gDuel_abSelectionSides +
                gDuel_bActiveSide * DUEL_SELECTION_SIDE_SIZE);
        gDuel_pActiveSelectionRecord->panel_target_y = DUEL_PHASE_PANEL_TARGET_Y;
    } else if (gDuel_wDisplayTransitionFlags == 0) {
        gDuel_wSceneStateFlags = 3;
    }
}

#include "duel_card_staging.h"
#include "duel_card.h"
#include "duel_grid.h"
#include "card_constants.h"
#include "duel_card_layout.h"
#include "sound.h"
#include "duel_session_controls.h"
#include "rand_get_interval.h"
#include "fade.h"
#include "display_object_work_slots.h"
#include "duel_effect_states.h"
#include "sound_driver.h"
#include "model_scene_states.h"
#include "main_modes.h"
#include "../psyq/rand.h"
#include "main_mode_state.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "color_constants.h"
#include "display_object.h"

/* Regional value: the y Duel_CreateHandCardDisplayObject places the drawn card at (0x92 ->
 * 0xA2). The European build (src/game/european/) defines its own. */
#ifndef DUEL_DRAW_RESOLUTION_CARD_Y
#define DUEL_DRAW_RESOLUTION_CARD_Y 0x92
#endif

void Duel_UpdateDrawCardSlide(DisplayObject *o)
{
    if (!DisplayObject_MarkInitialized((DisplayObjectLifecycle *)o)) {
        o->field_2C.h.field_2C = 0x400 / o->field_60;
        o->position.h.field_28 = o->xy.h.x - 0x140;
        DisplayObject_SavePosition((DisplayObjectSnapshot *)o);
        o->field_60 = -0x400;
    }
    Widget_SlideSine((DisplayObjectPosition *)o, (s16)o->position.h.field_28,
                  (s16)o->xy.h.y, o->field_60);
    o->field_60 += o->field_2C.h.field_2C;
    if (o->field_60 >= 0) {
        o->update = 0;
        o->field_6C = 0;
        o->xy.h.x = (s16)o->position.h.field_28;
    }
}

/*
 * Returns 1 when the active hand holds every Exodia piece: each id from
 * EXODIA_FIRST_CARD_ID up to EXODIA_CARD_ID_END must match a different hand
 * slot. The slot indices are copied first and a matched slot is overwritten
 * with -1 so one card cannot satisfy two pieces. Returns 0 at the first piece
 * the hand lacks.
 *
 * The miss branch comes first and returns from inside the scan, so the hit
 * branch sits after that return and breaks straight to the next piece. The
 * marker goes through index, whose second assignment keeps loop.c from
 * hoisting the -1 out of the scan.
 */
s32 Duel_HasAllExodiaPieces(void) {
    s16 hand[HAND_SIZE];
    s32 i;
    s32 card_id;
    s16 index;

    for (i = 0; i < HAND_SIZE; i++) {
        hand[i] = gDuel_pActiveSideState->hand[i];
    }

    for (card_id = EXODIA_FIRST_CARD_ID;
         card_id < EXODIA_CARD_ID_END;
         card_id++) {
        i = 0;
        while (1) {
            index = hand[i];
            if (index < 0 ||
                ((DuelCardStagingDeckView *)gDuel_abCardStagingBuffer)->cards[index].id != card_id) {
                if (++i >= HAND_SIZE) {
                    return 0;
                }
            } else {
                index = -1;
                hand[i] = index;
                break;
            }
        }
    }

    return 1;
}

void DuelScene_UpdateDrawResolution(void) {
    DisplayObject *p;
    DuelSideState *c;
    DuelCardReplayRecordBlock *g;
    u8 *base;
    s32 i;
    s32 k;
    s32 v;
    s32 t;
    s32 b;
    s32 a;
    s32 n;
    s32 off;
    s32 y;

    v = gDuel_wSceneStateFlags;
    if ((v & DUEL_SCENE_FLAG_INITIALIZED) == 0) {
        gDuel_wSceneStateFlags = v | DUEL_SCENE_FLAG_INITIALIZED;
        gDuel_bDrawStepCountdown = 1;
    }

    if ((gDuel_wSceneStateFlags & 0x4000) == 0) {
        t = gDuel_bDrawStepCountdown - 1;
        gDuel_bDrawStepCountdown = t;
        if ((s8)t > 0) {
            return;
        }
        gDuel_bDrawStepCountdown = 8;
        b = gDuel_bPendingDrawCount - 1;
        c = gDuel_pActiveSideState;
        gDuel_bPendingDrawCount = b;
        if (c->deck_draw_cursor >= DECK_SIZE) {
            gDuel_bWinnerSide = gDuel_bActiveSide ^ 1;
            gDuel_aSideStates[gDuel_bWinnerSide].rank.result_adjustment =
                DUEL_RANK_ADJUST_DECK_OUT_WIN;
            gDuel_wSceneStateFlags = 0xC;
            return;
        }
        i = HAND_SIZE - 1 - (s8)b;
        a = gDuel_abHandCardRecordIndices[i + gDuel_bActiveSide * HAND_SIZE];
        if ((a & 0x80) != 0) {
            k = (a & 0x7F) + DUEL_CARD_SIDE_RECORD_COUNT;
        } else {
            k = a;
        }
        Duel_SetupCardRecord(k, gDuel_pActiveSideState->deck_draw_cursor);
        p = (DisplayObject *)((DuelCreateHandCardDisplayObjectAt)Duel_CreateHandCardDisplayObject)(
            &gDuel_aCardRecords[k],
            i * 60 + 0x14E,
            DUEL_DRAW_RESOLUTION_CARD_Y
        );
        p->field_6C = 1;
        p->field_60 = 0xC;
        p->update = (DisplayObjectCallback)Duel_UpdateDrawCardSlide;
        gDuelHand_aSlots[i].object = p;
        base = gDuel_abCardStagingBuffer;
        g = (DuelCardReplayRecordBlock *)(
            (u8 *)&((DuelCardRecord *)base)[p->field_6A] +
            DUEL_CARD_STAGING_REPLAY_BASE_OFFSET);
        y = ((DuelDeckCardRecord *)g->record.data)->deck_index;
        gDuel_pActiveSideState->hand[i] = y;
        n = (u8)gDuel_pActiveSideState->deck_draw_cursor;
        gDuel_pActiveSideState->deck_draw_cursor = n + 1;
        /* Signed view: retail tests the u8 count with lb. */
        if (*(s8 *)&gDuel_bPendingDrawCount == 0) {
            gDuel_wSceneStateFlags = gDuel_wSceneStateFlags | 0x4000;
        }
        SD_SEPlayFull(0xA);
    } else {
        if (DisplayObject_FindAllocatedByTag(1) == 0) {
            gDuel_wSceneStateFlags = 4;
            if (Duel_HasAllExodiaPieces() != 0) {
                gDuel_wSceneStateFlags = 0xE;
            }
        }
    }
}

/*
 * Stages the five-card Exodia presentation, sparkle phases, centre burst, and
 * result handoff. The actual request owner with integer buffer-word views, typed
 * DisplayObject cursor, and cross-path reuse of `side` preserve the retail
 * scheduling and register allocation. The first position store keeps a signed
 * address-of-member view so GCC materializes -0x40 like retail. The two
 * request-completion tests read the actual DuelEffectRequest::progress byte.
 */
void DuelScene_UpdateExodiaResult(void)
{
    u16 flags;
    DisplayObject *obj;
    u8 *pose;
    DuelHandSlot *rec;
    DuelEffectRequest *fx;
    s32 i;
    s32 anim;
    s32 n;
    s32 r;
    s32 t14;
    u32 sum;
    u8 *cards;
    u8 *poses;
    DisplayObject **objs;
    s32 fnv;
    DuelCardReplayRecordBlock *g;
    s32 py;
    s32 t;
    s32 k;
    DisplayObject **slot;
    s8 side;
    DuelSideState *other;
    DisplayObject *d;

    flags = gDuel_wSceneStateFlags;
    if ((flags & DUEL_SCENE_FLAG_INITIALIZED) == 0) {
        i = 0;
        cards = gDuel_abCardStagingBuffer;
        poses = (u8 *)&gDuel_ExodiaCardPoses;
        r = (s32)DisplayObject_UpdateCosineSlide;
        fnv = r;
        objs = gDisplayObject_apWorkSlots.slots;
        gDuel_wSceneStateFlags = flags | DUEL_SCENE_FLAG_INITIALIZED;
        obj = gDuel_pTerrainIcon;
        side = gDuel_bActiveSide;
        gDuel_pActiveSelectionRecord =
            (DuelSelectionRecord *)&gDuel_abSelectionSides[
                (u8)side * DUEL_SELECTION_SIDE_SIZE
            ];
        /* s16 view: retail materialises li -64; a u16 store gets li 0xffc0. */
        *(s16 *)&obj->position.h.field_28 = -0x40;
        obj->field_2C.h.field_2C = 0x10;
        obj->field_6C = 1;
        obj->update = (DisplayObjectCallback)fnv;
        obj->position.h.field_2A = obj->xy.h.y;
        obj = gDuel_pLifePointsPanel;
        obj->position.h.field_28 = 0x180;
        obj->field_2C.h.field_2C = 0x10;
        obj->field_6C = 1;
        obj->update = (DisplayObjectCallback)fnv;
        obj->position.h.field_2A = obj->xy.h.y;
        rec = gDuelHand_aSlots;
        /* The constant switch enters the slot loop at its case label, so
           loop.c leaves it alone as with retail's goto loop (a plain
           do-while hoists the pose constants out of the loop). */
        switch (0) {
            for (;;) {
            }
            do {
        case 0:
                obj = rec->object;
                /* Keep the typed record offset left of the staging base. */
                g = (DuelCardReplayRecordBlock *)(
                    (u32)&((DuelCardRecord *)0)[obj->field_6A] +
                    (u32)cards + DUEL_CARD_STAGING_REPLAY_BASE_OFFSET
                );
                anim = g->record.card_id - 0x11;
                pose = (u8 *)(anim * 3 + (u32)poses);
                obj->position.h.field_28 = pose[1] - 0x1A;
                py = pose[2];
                obj->field_2C.h.field_2C = 0xB4;
                obj->field_6C = 1;
                obj->update = (DisplayObjectCallback)fnv;
                obj->position.h.field_2A = py - 0x1E;
                objs[pose[0]] = obj;
                rec->object = 0;
                i++;
                rec++;
            } while (i < DISPLAY_OBJECT_WORK_SLOT_COUNT);
        }
        gDisplayObject_apWorkSlots.slots[DISPLAY_OBJECT_WORK_SLOT_COUNT] = 0;
        DisplayObject_ReleaseIfPresent(
            gDuel_pActiveSelectionRecord->cursor_object);
        gDuel_wDisplayTransitionFlags = 8;
        gDuel_wStepCounter = 0;
        gDuel_bPresentationCursor = 0;
        gDuelEffect_pTrackedRequest = 0;
        gDuel_wSceneStateFlags = gDuel_wSceneStateFlags | 0x4000;
        Fade_StartOutKeepOverlay();
        gFade_State.step = 2;
        SD_BGMFadeOutWithStep(2);
        return;
    }
    if (flags & 0x4000) {
        if (flags & 0x2000) {
            if ((gDuel_bEffectRequestStatus & 1) != 0) {
                return;
            }
            gDuel_wSceneStateFlags = flags & 0xBFFF;
            gDuel_bPresentationCursor = 0;
            gDuel_wStepCounter = 0;
            return;
        }
        if (gDuelEffect_pTrackedRequest != 0) {
            if (gDuelEffect_pTrackedRequest->progress == 0) {
                return;
            }
            SD_SEPlayFull(0x1D);
            if (DisplayObject_FindAllocatedByTag(1) == 0) {
                gDuel_wSceneStateFlags = gDuel_wSceneStateFlags | 0x2000;
                return;
            }
        }
        fx = DuelEffect_AllocateRequest(0x13);
        gDuelEffect_pTrackedRequest = fx;
        fx->field_00 = (s16)((rand() & 0xFF) + 0x20);
        r = Rand_GetInterval(0xB0);
        n = (u8)gDuel_bPresentationCursor;
        t14 = (s32)fx->buffer;
        fx->field_02 = (s16)(r + 0x20);
        fx->buffer = (void *)(t14 + ((n & 3) << 13));
        gDuel_bPresentationCursor = n + 1;
        return;
    }
    if (flags & 0x2000) {
        t = gDuel_wStepCounter - 1;
        gDuel_wStepCounter = t;
        if ((s16)t > 0) {
            return;
        }
        if (gDuel_bPresentationCursor >= DISPLAY_OBJECT_WORK_SLOT_COUNT) {
            gDuel_wSceneStateFlags = (flags & 0xDFFF) | 0x1000;
            fx = DuelEffect_AllocateRequest(0x13);
            fx->field_00 = 0xA0;
            fx->field_02 = 0x78;
            gDuelEffect_pTrackedRequest = fx;
            fx->buffer = (void *)((s32)fx->buffer + 0x8000);
            return;
        }
        gDuel_wStepCounter = 4;
        fx = DuelEffect_AllocateRequest(0);
        k = gDuel_bPresentationCursor;
        slot = &gDisplayObject_apWorkSlots.slots[k];
        d = *slot;
        fx->field_00 = (s16)(d->xy.h.x + 0x1A);
        d = *slot;
        fx->field_02 = (s16)(d->xy.h.y + 0x1E);
        sum = (k << 12) + 0xA000;
        anim = (s32)fx->buffer + sum;
        fx->buffer = (void *)anim;
        fx->phase = 9;
        SD_SEPlayFull(0x17);
        gDuel_bPresentationCursor = gDuel_bPresentationCursor + 1;
        return;
    }
    if (flags & 0x1000) {
        if (gDuelEffect_pTrackedRequest->progress == 0) {
            return;
        }
        gDuel_wSceneStateFlags = flags & 0xEFFF;
        SD_SEPlayFull(0x1D);
        DuelEffect_CreateRequest(0x18);
        SD_CdSoundPlay(0x8021);
        return;
    }
    Fade_CutToWhite();
    gMain_aAnimatedBattleModels[0].model_id = MODEL_SPECIAL_BATTLE_ID;
    side = gDuel_bActiveSide;
    gDuel_bWinnerSide = side;
    gDuel_aSideStates[(u8)side].rank.result_adjustment = 0x28;
    other = &gDuel_aSideStates[gDuel_bActiveSide ^ 1];
    other->life_points.unsigned_value = 0;
    other->displayed_life_points = 0;
    SD_BGMPreload(0x7310);
    Model_SetPresentationBgmCommand(0x7310);
    gDuel_bResumeFromAnimatedBattle = 1;
    gMain_bReturnMode = 3;
    gMain_bActiveModeState = 1;
}

void DuelCard_CaptureRoundedTexture(DisplayObject *o)
{
    DuelCardStagingPointer buf;
    DuelCardStagingPointer staging;
    u16 *p;
    s32 xoff;

    while (IsIdleGPU(10) != 0) {
        ;
    }

    xoff = 0;
    if (gGraphics_bActiveBuffer == 0) {
        xoff = 0x140;
    }

    buf.bytes = gDuel_abCardStagingBuffer;
    gGraphics_aImageTransferRects[0].x = o->xy.h.x + xoff;
    gGraphics_aImageTransferRects[0].y = o->xy.h.y;
    gGraphics_aImageTransferRects[0].w = DUEL_CARD_READBACK_WIDTH_WORDS;
    gGraphics_aImageTransferRects[0].h = DUEL_CARD_READBACK_HEIGHT;
    StoreImage2(&gGraphics_aImageTransferRects[0], buf.words);

    p = buf.pixels;
    xoff = DUEL_CARD_READBACK_WORD_COUNT;
    do {
        *p = *p | COLOR_BGR555_STP_MASK;
        p++;
        xoff--;
    } while (xoff != 0);

    /* The loop above set the STP bit on every pixel; these clear twelve
       corner pixels of the 140x196 readback back to 0 -- (0, 0), (0, 1),
       (1, 0) and the matching three at each other corner -- which rounds
       the card's corners. (0, 0) is the store through gDuel_abCardStagingBuffer below. */
    staging.bytes = gDuel_abCardStagingBuffer;
    p = staging.pixels;
    p[(DUEL_CARD_READBACK_HEIGHT - 1) * DUEL_CARD_READBACK_WIDTH_WORDS + 138] = 0;
    p[(DUEL_CARD_READBACK_HEIGHT - 1) * DUEL_CARD_READBACK_WIDTH_WORDS + 139] = 0;
    p[(DUEL_CARD_READBACK_HEIGHT - 1) * DUEL_CARD_READBACK_WIDTH_WORDS + 1] = 0;
    p[(DUEL_CARD_READBACK_HEIGHT - 1) * DUEL_CARD_READBACK_WIDTH_WORDS + 0] = 0;
    p[(DUEL_CARD_READBACK_HEIGHT - 2) * DUEL_CARD_READBACK_WIDTH_WORDS + 139] = 0;
    p[(DUEL_CARD_READBACK_HEIGHT - 2) * DUEL_CARD_READBACK_WIDTH_WORDS + 0] = 0;
    p[1 * DUEL_CARD_READBACK_WIDTH_WORDS + 139] = 0;
    p[1 * DUEL_CARD_READBACK_WIDTH_WORDS + 0] = 0;
    p[0 * DUEL_CARD_READBACK_WIDTH_WORDS + 138] = 0;
    p[0 * DUEL_CARD_READBACK_WIDTH_WORDS + 139] = 0;
    p[0 * DUEL_CARD_READBACK_WIDTH_WORDS + 1] = 0;

    staging.bytes = gDuel_abCardStagingBuffer;
    staging.pixels[0] = 0;
    gGraphics_aImageTransferRects[0].x = 0x140;
    gGraphics_aImageTransferRects[0].y = 0x100;
    gGraphics_aImageTransferRects[0].w = DUEL_CARD_READBACK_WIDTH_WORDS;
    gGraphics_aImageTransferRects[0].h = DUEL_CARD_READBACK_HEIGHT;
    LoadImage2(&gGraphics_aImageTransferRects[0], staging.words);
}

#include "display_object.h"

/* Regional value: the sprite's height argument and the half of it stored
 * beside (0xC4 -> 0xD4). The European build (src/game/european/) defines its
 * own. */
#ifndef DUEL_CARD_EFFECT_OVERLAY_HEIGHT
#define DUEL_CARD_EFFECT_OVERLAY_HEIGHT 0xC4
#endif

DisplayObject *Duel_CreateCardEffectOverlay(DisplayObject *source)
{
    DisplayObject *object =
        (DisplayObject *)DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);

    /* The source position is read signed (lh); xy's halves are u16. */
    DisplayObject_ConfigureScreenSprite(
        object,
        (s16)source->xy.h.x,
        (s16)source->xy.h.y,
        0x8C,
        DUEL_CARD_EFFECT_OVERLAY_HEIGHT,
        0,
        0,
        0x15,
        0,
        0
    );
    object->field_18.center.x = 0x46;
    object->field_48.pivot.x = 0x46;
    object->field_18.center.y = DUEL_CARD_EFFECT_OVERLAY_HEIGHT / 2;
    object->field_48.pivot.y = DUEL_CARD_EFFECT_OVERLAY_HEIGHT / 2;
    DisplayObject_SelectOrderingTable1(object);
    object->attribute |= DISPLAY_OBJECT_ATTRIBUTE_16BPP;
    return object;
}

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "card_constants.h"
#include "duel_card.h"
#include "display_object_layout.h"
#include "file_transfer.h"
#include "display_object_work_slots.h"

/* Regional value: the object's y (0x16 -> 0xE). The European build
 * (src/game/european/) defines its own. */
#ifndef DUEL_CARD_USE_OBJECT_Y
#define DUEL_CARD_USE_OBJECT_Y 0x16
#endif

void DuelScene_UpdateCardUse(void)
{
    DisplayObject *p;
    DuelCardRecord *slot;
    DisplayObject *q0;
    DisplayObject *q1;
    u8 state;
    u16 flags;
    u16 f2;
    u16 f3;
    u8 st;
    s32 v1;
    s32 v2;
    s32 countdown;
    s32 value;
    s32 fld;

    p = gDisplayObject_apWorkSlots.slots[0];
    flags = gDuel_wSceneStateFlags;
    if ((flags & DUEL_SCENE_FLAG_INITIALIZED) == 0) {
        gDuel_wSceneStateFlags = flags | 0xC000;
        slot = &gDuel_aCardRecords[p->field_6A];
        value = slot->card_id;
        gDuel_wPlacementCardId = slot->card_id;
        CardPreview_RequestResourceTransfer(0, value);
        if (p->resource_branch_index == CARD_TYPE_MAGIC) {
            gDuel_pActiveSideState->rank.pure_magic_used =
                gDuel_pActiveSideState->rank.pure_magic_used + 1;
        }
        DuelCard_DeactivateRecord(slot);
        gDuel_bSceneSubStep = 1;
    }
    state = gDuel_bSceneSubStep;
    switch (state & 0xF) {
    case 1:
        if ((state & 0x80) == 0) {
            if (((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                 gFile_dwCancelRequestState) != 0) {
                return;
            }
            f2 = p->flags;
            gDuel_bSceneSubStep = state | 0x80;
            p->flags = f2 | DISPLAY_OBJECT_FLAG_CLIP_TEST;
            p = (DisplayObject *)CardPreview_CreateResourceObjects(0, -1, -1);
            p->xy.h.x = 0x5A;
            p->xy.h.y = DUEL_CARD_USE_OBJECT_Y;
            p->field_20.b.rotation_y = 0xC0;
            DisplayObject_SetDepthOffset(p, -0xA);
            p->flags =
                (p->flags | DISPLAY_OBJECT_FLAG_CLIP_TEST) &
                ~DISPLAY_OBJECT_FLAG_RENDERABLE;
            gDisplayObject_apWorkSlots.slots[1] = p;
            return;
        }
        if ((state & 0x40) == 0) {
            v1 = p->field_20.b.rotation_y + 6;
            p->field_20.b.rotation_y = v1;
            if ((u8)v1 < 0x40) {
                return;
            }
            gDuel_bSceneSubStep = gDuel_bSceneSubStep | 0x40;
            DisplayObject_ReleaseIfPresent(p);
            q1 = gDisplayObject_apWorkSlots.slots[1];
            gDisplayObject_apWorkSlots.slots[0] = 0;
            q1->flags = q1->flags | DISPLAY_OBJECT_FLAG_RENDERABLE;
            return;
        }
        p = gDisplayObject_apWorkSlots.slots[1];
        if ((state & 0x20) == 0) {
            v2 = p->field_20.b.rotation_y + 6;
            p->field_20.b.rotation_y = v2;
            if ((s8)v2 < 0) {
                return;
            }
            p->field_60 = 0x1E;
            p->field_20.b.rotation_y = 0;
            f3 = p->flags;
            st = gDuel_bSceneSubStep | 0x20;
            p->flags = f3 & ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
            gDuel_bSceneSubStep = st;
            return;
        }
        countdown = (u16)p->field_60 - 1;
        p->field_60 = countdown;
        if ((s16)countdown > 0) {
            return;
        }
        gDuel_bSceneSubStep = 2;
        return;
    case 2:
        DuelEffect_StartCardEffect((s16)gDuel_wPlacementCardId, 0);
        gDuel_bSceneSubStep = 3;
        return;
    case 3:
        if ((state & 0x80) == 0) {
            p = gDisplayObject_apWorkSlots.slots[1];
            gDuel_bSceneSubStep = state | 0x80;
            DuelCard_CaptureRoundedTexture(p);
            gDisplayObject_apWorkSlots.slots[0] =
                Duel_CreateCardEffectOverlay(p);
            gDisplayObject_apWorkSlots.slots[0]->attribute = gDisplayObject_apWorkSlots.slots[0]->attribute | (GsALON | GsAONE);
            gDisplayObject_apWorkSlots.slots[0]->attribute = gDisplayObject_apWorkSlots.slots[0]->attribute & ~GsROTOFF;
            gDisplayObject_apWorkSlots.slots[1] =
                Duel_CreateCardEffectOverlay(p);
            DisplayObject_SetDepthOffset(gDisplayObject_apWorkSlots.slots[1], -1);
            gDisplayObject_apWorkSlots.slots[1]->attribute = gDisplayObject_apWorkSlots.slots[1]->attribute | (GsALON | GsATWO);
            gDisplayObject_apWorkSlots.slots[1]->attribute = gDisplayObject_apWorkSlots.slots[1]->attribute & ~GsROTOFF;
            DuelEffect_ReleaseResourceObjects(0);
            return;
        }
        q0 = gDisplayObject_apWorkSlots.slots[0];
        fld = q0->field_44.scale.x;
        value = fld + 0x80;
        q1 = gDisplayObject_apWorkSlots.slots[1];
        q1->field_44.scale.y = value;
        q1->field_44.scale.x = value;
        q0->field_44.scale.y = value;
        q0->field_44.scale.x = value;
        value = (u8)gDisplayObject_apWorkSlots.slots[0]->rgb;
        value -= 4;
        if (value < 0) {
            value = 0;
        }
        value = value | ((value << 16) | (value << 8));
        gDisplayObject_apWorkSlots.slots[0]->rgb = value;
        gDisplayObject_apWorkSlots.slots[1]->rgb = value;
        if (value != 0) {
            return;
        }
        DisplayObject_ReleaseIfPresent(gDisplayObject_apWorkSlots.slots[0]);
        DisplayObject_ReleaseIfPresent(gDisplayObject_apWorkSlots.slots[1]);
        gDuel_bSceneSubStep = 4;
        return;
    case 4:
        if ((state & 0x80) == 0) {
            gDuel_bSceneSubStep = state | 0x80;
            DuelEffect_StartCardEffect((s16)gDuel_wPlacementCardId, 1);
            return;
        }
        gDuel_wSceneStateFlags = 5;
        return;
    }
}

#include "card_constants.h"

#define FUSION_TABLE_BYTES(table) ((u8 *)(table))
#define FUSION_TABLE_OFFSETS(table) ((u16 *)(table))

s32 Duel_CheckEquip(s32 key_card_id, s32 candidate_card_id)
{
    u16 *table_cursor = gDuel_awEquipTable;

    while (1) {
        s32 entry_card_id = table_cursor[0];
        s32 remaining_partners;

        if (entry_card_id == 0) {
            return 0;
        }
        remaining_partners = table_cursor[1];
        table_cursor += 2;
        if (entry_card_id == key_card_id) {
            do {
                if (candidate_card_id == *table_cursor) {
                    return candidate_card_id;
                }
                remaining_partners--;
                table_cursor++;
            } while (remaining_partners != 0);
            return 0;
        }
        table_cursor += remaining_partners;
    }
}

s32 Duel_CheckFusion(s32 card_a, s32 card_b)
{
    u8 *fusion_bytes = FUSION_TABLE_BYTES(gDuel_aFusionTable);
    u8 *pair_cursor;
    s32 table_offset;
    s32 remaining_pairs;
    s32 packed_high_bits;

    if (card_b < card_a) {
        s32 swap_card_id = card_b;
        card_b = card_a;
        card_a = swap_card_id;
    }
    table_offset = FUSION_TABLE_OFFSETS(fusion_bytes)[card_a];
    if (table_offset == 0) {
        return 0;
    }
    pair_cursor = fusion_bytes + table_offset;
    remaining_pairs = pair_cursor[0];
    if (remaining_pairs == 0) {
        remaining_pairs = FUSION_TABLE_EXTENDED_COUNT_BASE - pair_cursor[1];
        pair_cursor++;
    }
    pair_cursor++;
    do {
        packed_high_bits = pair_cursor[0];
        if ((((packed_high_bits << FUSION_TABLE_FIRST_PARTNER_SHIFT) &
              FUSION_TABLE_CARD_ID_HIGH_MASK) | pair_cursor[1]) == card_b) {
            return ((packed_high_bits << FUSION_TABLE_FIRST_RESULT_SHIFT) &
                    FUSION_TABLE_CARD_ID_HIGH_MASK) | pair_cursor[2];
        }
        if ((((packed_high_bits << FUSION_TABLE_SECOND_PARTNER_SHIFT) &
              FUSION_TABLE_CARD_ID_HIGH_MASK) | pair_cursor[3]) == card_b) {
            return ((packed_high_bits << FUSION_TABLE_SECOND_RESULT_SHIFT) &
                    FUSION_TABLE_CARD_ID_HIGH_MASK) | pair_cursor[4];
        }
        pair_cursor += FUSION_TABLE_ENTRY_SIZE;
        remaining_pairs -= FUSION_TABLE_PAIRS_PER_ENTRY;
    } while (remaining_pairs > 0);
    return 0;
}

/* A display object's alpha transition: DisplayObject_StartAlphaTransition
   seeds the start level and the target/step pair and installs
   DisplayObject_UpdateAlphaTransition as the object's update callback, which
   steps byte 0x21 until it passes the target and then uninstalls itself. */

void DisplayObject_UpdateAlphaTransition(DisplayObject *object) {
    s32 n = object->field_20.b.rotation_y +
        (u8)object->position.h.field_2A;
    s32 d;

    object->field_20.b.rotation_y = n;
    if ((s16)object->position.h.field_2A >= 0) {
        d = (u8)object->position.h.field_28 - n;
    } else {
        d = n - (u8)object->position.h.field_28;
    }

    if ((s8)d < 0) {
        u8 c = object->position.h.field_28;

        object->field_6C = 0;
        object->update = 0;
        object->field_20.b.rotation_y = c;
        if (c == 0) {
            object->flags &= ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
        }
    }
}

void DisplayObject_StartAlphaTransition(DisplayObject *object, u8 start, s16 target, s16 step)
{
    object->field_6C = 1;
    object->field_20.b.rotation_y = start;
    object->position.h.field_28 = target;
    object->position.h.field_2A = step;
    object->update = (DisplayObjectCallback)DisplayObject_UpdateAlphaTransition;
    object->flags |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
}

#include "display_object.h"
#include "display_object_layout.h"
#include "duel_card.h"

void Duel_UpdateCardMarkerRemovalTurn(DisplayObject *o)
{
    int current;

    if (!DisplayObject_MarkInitialized((DisplayObjectLifecycle *)o)) {
        o->field_2C.h.field_2E = 0x40;
        o->flags |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
        if (o->field_20.b.rotation_y) {
            o->field_2C.h.field_2E = 0xC0;
        }
    }
    current = o->field_20.b.rotation_y + 8;
    o->field_20.b.rotation_y = current;
    if (o->field_6C & 0x40) {
        if ((signed char)current >= 0) {
            o->field_20.b.rotation_y = 0;
            gDuel_aCardRecords[o->field_6A].flags &= ~DUEL_CARD_FLAG_DISPLAY_MARKER;
            if ((o->field_20.b.rotation_z | o->field_20.b.rotation_x) == 0) {
                o->flags &= ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
            }
            o->field_6C = 0;
            o->update = 0;
        }
    } else if (o->field_20.b.rotation_y >= o->field_2C.h.field_2E) {
        o->field_6C |= 0x40;
        o->field_20.b.rotation_y = 0xC0;
        o->resource_root_index = 0;
    }
}

#include "campaign_flags.h"

void Duel_MarkCreatedCardKnown(s32 card_id)
{
    if (gDuel_bActiveSide == 0 && gDuel_bSide0ControllerSelector[0] < 0 && gDuel_bOpponentID >= 0) {
        Library_UpdateCardUsedFlag((s32)((u32)card_id + CAMPAIGN_FLAG_LIBRARY_CARD_BASE));
    }
}

#include "../psyq/rand.h"
#include "display_object.h"
#include "display_object_work_slots.h"
#include "duel_card.h"
#include "duel_card_staging.h"
#include "duel_effect_text.h"
#include "file_transfer.h"
#include "sound.h"

/* Regional value: the screen height a card leaves at (240 -> 256). The
 * European build (src/game/european/) defines its own. */
#ifndef DUEL_CARD_PLACEMENT_SCREEN_HEIGHT
#define DUEL_CARD_PLACEMENT_SCREEN_HEIGHT 240
#endif

#define PLACEMENT_REPLAY_CARD(object) (((DuelCardStagingDeckView *)gDuel_abCardStagingBuffer)->field_cards[(object)->field_6A])
#define DISPLAY_OBJECT_VELOCITY_VIEW(object) \
    ((DisplayObjectVelocity *)(object))

static __inline__ s32 placement_velocity(s32 distance)
{
    return distance / 8;
}

void DuelScene_UpdateCardPlacement(void)
{
    DuelSelectionRecord *side = (DuelSelectionRecord *)
        (gDuel_abSelectionSides + gDuel_bActiveSide * DUEL_SELECTION_SIDE_SIZE);
    DisplayObject *object;
    DuelCardRecord *card;
    DuelEffectRequest *effect;
    s32 n;
    s32 value;
    s32 phase_clear;
    s32 x;

    if (!(gDuel_wSceneStateFlags & 0x8000)) {
        DisplayObject **slots;
        DisplayObject **cursor;
        DuelHandSlot *hand;
        s16 y;
        x = 260;
        value = 4;
        gDuel_wSceneStateFlags |= 0x8000;
        gDuel_pActiveSelectionRecord = side;
        gDuel_bPlacementNeedsCardMoveSetup = 0;
        gDuel_bPresentationCursor = 0;
        gDuel_wPendingEquipBonus = 0;
        n = 5;
        y = 82;
        slots = gDisplayObject_apWorkSlots.slots;
        cursor = slots + 5;
        do {
            object = *cursor;
            if (object) {
                object->position.h.field_28 = x;
                object->position.h.field_2A = y;
                DisplayObject_SetDepthOffset(object, value);
                x -= 16;
                value++;
            }
            n--;
            cursor--;
        } while (n > 0);
        object = gDisplayObject_apWorkSlots.slots[0];
        object->position.h.field_28 = 64;
        object->position.h.field_2A = 82;
        DisplayObject_SetDepthOffset(object, value);
        if ((u8)(object->field_6A % 15) < 5)
        gDuel_bPlacementNeedsCardMoveSetup = 1;
        slots = gDisplayObject_apWorkSlots.slots;
        if (!slots[1]) {
            if (slots[0]->resource_branch_index < 20)
            gDuel_bPlacementNeedsCardMoveSetup = 1;
            gDuel_wPlacementCardId = PLACEMENT_REPLAY_CARD(object).card_id;
            object->position.h.field_28 = 134;
            object->position.h.field_2A = 42;
            gDuel_wSceneStateFlags |= 0x4000;
        } else {
            gDuel_pActiveSideState->rank.combo_plays++;
        }
        n = 0;
        do {
            hand = &gDuelHand_aSlots[n];
            if (hand->selection_order) {
                DisplayObject *child = hand->order_marker;
                hand->object = 0;
                DisplayObject_ReleaseIfPresent(child);
                hand->order_marker = 0;
            }
            n++;
        } while (n < 5);
        DisplayObject_ReleaseIfPresent(side->cursor_object);
        gDuel_wDisplayTransitionFlags = 8;
        side->cursor_object = 0;
        gDuel_bSceneSubStep = 1;
        return;
    }

    switch (gDuel_bSceneSubStep & 15) {
    case 1:
        {
            DisplayObject **slots;
            {
                DisplayObject **cursor;
                DuelCardRecord *cards;
                DisplayObjectCallback callback;
                s32 active;
                if (!(gDuel_bSceneSubStep & 0x80)) {
                    gDuel_bSceneSubStep |= 0x80;
                    if (gDuel_wSceneStateFlags & 0x4000)
                    return;
                    n = 5;
                    cards = gDuel_aCardRecords;
                    active = 1;
                    callback = (DisplayObjectCallback)Duel_UpdateCardMarkerRemovalTurn;
                    slots = gDisplayObject_apWorkSlots.slots;
                    cursor = slots + 5;
                    do {
                        object = *cursor;
                        if (object && (cards[object->field_6A].flags & 0x2000)) {
                            object->field_6C = active;
                            object->update = callback;
                        }
                        n--;
                        cursor--;
                    } while (n >= 0);
                    return;
                }
                if (DisplayObject_FindAllocatedByTag(1))
                return;
                gDuel_bSceneSubStep = 2;
            }
        }
    case 2:
        {
            DisplayObject **slots;
            {
                DisplayObject **cursor;
                if (!(gDuel_bSceneSubStep & 0x80)) {
                    DisplayObject **cursor;
                    s32 active;
                    gDuel_bSceneSubStep |= 0x80;
                    n = 5;
                    active = 1;
                    slots = gDisplayObject_apWorkSlots.slots;
                    cursor = slots + 5;
                    do {
                        object = *cursor;
                        if (object) {
                            object->field_6C = active;
                            DisplayObject_SavePosition((DisplayObjectSnapshot *)object);
                            object->field_60 = 0;
                        }
                        n--;
                        cursor--;
                    } while (n >= 0);
                }
                value = 0;
                n = 5;
                slots = gDisplayObject_apWorkSlots.slots;
                cursor = slots + 5;
                do {
                    object = *cursor;
                    if (object && object->field_6C) {
                        value = 1;
                        if (!(gDuel_wSceneStateFlags & 0x4000) && object->field_20.b.rotation_y)
                        object->field_20.b.rotation_y += 8;
                        if (object->field_20.b.rotation_z)
                        object->field_20.b.rotation_z += 8;
                        DisplayObject_InterpolatePositionCosine((DisplayObjectPosition *)object,
                        (s16)object->position.h.field_28, (s16)object->position.h.field_2A, object->field_60);
                        object->field_60 += 128;
                        if (object->field_60 >= 2048) {
                            object->field_6C = 0;
                            object->field_20.b.rotation_z = 0;
                            if (!(gDuel_wSceneStateFlags & 0x4000))
                            object->field_20.b.rotation_y = 0;
                            if (!object->field_20.b.rotation_y)
                            object->flags &= ~4;
                            object->xy.h.x = object->position.h.field_28;
                            object->xy.h.y = object->position.h.field_2A;
                        }
                    }
                    n--;
                    cursor--;
                } while (n >= 0);
                if (!(gDuel_wDisplayTransitionFlags | value)) {
                    gDuel_bSceneSubStep = 3;
                    if (gDuel_wSceneStateFlags & 0x4000)
                    gDuel_wSceneStateFlags = 8;
                }
            }
            break;
        }
    case 3:
        {
            DisplayObject **slots;
            if (!(gDuel_bSceneSubStep & 0x80)) {
                gDuel_bSceneSubStep |= 0x80;
                gDuel_bPresentationCursor++;
                if (gDuel_bPresentationCursor >= 6) {
                    gDuel_bSceneSubStep = 8;
                    return;
                }
                {
                    DisplayObject *selected = gDisplayObject_apWorkSlots.slots[gDuel_bPresentationCursor];
                    if (!selected) {
                        gDuel_bSceneSubStep = 8;
                        return;
                    }
                    object = selected;
                }
                gDisplayObject_apWorkSlots.slots[1] = object;
                DisplayObject_ResetVelocity(
                    DISPLAY_OBJECT_VELOCITY_VIEW(object));
                object->field_60 = 8;
                object->field_34.h.field_36 = placement_velocity((64 - (s16)object->xy.h.x) * 256);
                object->field_38.h.field_38 = placement_velocity((82 - (s16)object->xy.h.y) * 256);
                DisplayObject_SetDepthOffset(gDisplayObject_apWorkSlots.slots[0], (s8)((u8)object->depth_offset - 1));
                SD_SEPlayFull(10);
                gDuel_wPlacementCardId = 0;
                if (gDisplayObject_apWorkSlots.slots[0]) {
                    do {
                        s32 result = Duel_CheckFusion(PLACEMENT_REPLAY_CARD(gDisplayObject_apWorkSlots.slots[0]).card_id,
                        PLACEMENT_REPLAY_CARD(object).card_id);
                        gDuel_wPlacementCardId = result;
                        if ((s16)result) {
                            gDuel_wPlacementCardId = result | 0x8000;
                        } else {
                            s32 equip_result = Duel_CheckEquip(PLACEMENT_REPLAY_CARD(object).card_id,
                            PLACEMENT_REPLAY_CARD(gDisplayObject_apWorkSlots.slots[0]).card_id);
                            gDuel_wPlacementCardId = equip_result;
                            if ((s16)equip_result) {
                                break;
                            }
                            gDuel_wPlacementCardId = Duel_CheckEquip(PLACEMENT_REPLAY_CARD(gDisplayObject_apWorkSlots.slots[0]).card_id,
                            PLACEMENT_REPLAY_CARD(object).card_id);
                        }
                        if (!((DuelPlacementCardIdCell *)&gDuel_wPlacementCardId)->value) {
                            return;
                        }
                    } while (0);
                    CardPreview_RequestResourceTransfer(0, gDuel_wPlacementCardId & 0xFFF);
                }
                return;
            }
            slots = gDisplayObject_apWorkSlots.slots;
            object = slots[1];
            DisplayObject_StepPositionX(
                DISPLAY_OBJECT_VELOCITY_VIEW(object));
            object->field_60--;
            if (object->field_60)
            return;
            gDuel_wTrapPresentationMode = 0;
            if (!slots[0]) {
                slots[0] = object;
                gDuel_bSceneSubStep = 3;
                return;
            }
            object->xy.word = slots[0]->xy.word;
            if (!((DuelPlacementCardIdCell *)&gDuel_wPlacementCardId)->value) {
                gDuel_bSceneSubStep = 7;
                gDuel_bPlacementNeedsCardMoveSetup = -1;
            } else if (((DuelPlacementCardIdCell *)&gDuel_wPlacementCardId)->value & 0x8000) {
                gDuel_bPlacementNeedsCardMoveSetup = 1;
                gDuel_wPendingEquipBonus = 0;
                gDuel_bSceneSubStep = 6;
                gDuel_pActiveSideState->rank.fusions_initiated++;
            } else {
                gDuel_bSceneSubStep = 5;
                gDuel_wPendingEquipBonus += 500;
                gDuel_pActiveSideState->rank.equips_used++;
            }
            break;
        }
    case 4:
        {
            DisplayObject **slots;
            if ((gFile_dwTransferState & 0x2000030) | gFile_dwCancelRequestState)
            return;
            if (!(gDuel_bSceneSubStep & 0x80)) {
                gDuel_bSceneSubStep |= 0x80;
                gDuel_wTrapPresentationMode = 0;
                return;
            }
            switch (gDuel_wTrapPresentationMode & 15) {
            case 0:
                {
                    DisplayObject **slots;
                    {
                        s32 card_index = gDisplayObject_apWorkSlots.slots[0]->field_6A;
                        DuelEffectResourceRecord *resource = gCardPreview_Resources;
                        card = &gDuel_aCardRecords[card_index];
                        slots = gDisplayObject_apWorkSlots.slots;
                        if (!(gDuel_wTrapPresentationMode & 0x80)) {
                            gDuel_wTrapPresentationMode |= 0x80;
                            if ((s16)gDuel_wPlacementCardId & 0x8000) {
                                ((DuelDeckCardRecord *)card->data)->id = gDuel_wPlacementCardId & 0xFFF;
                                gDuel_wTrapPresentationMode |= 0x40;
                                card->card_id = gDuel_wPlacementCardId & 0xFFF;
                                value = ((DuelDeckCardRecord *)card->data)->data_block_index;
                                resource->rects[0].x = resource->src_x + 56;
                                resource->rects[0].y = resource->src_y;
                                resource->rects[0].w = 8;
                                resource->rects[0].h = 88;
                                StoreImage2(&resource->rects[0], (u32 *)&gDuel_aCardThumbnailBlocks[value]);
                            }
                            return;
                        }
                        if (gDuel_wTrapPresentationMode & 0x40) {
                            Duel_SetupCardRecord(
                                card_index,
                                ((DuelDeckCardRecord *)card->data)->deck_index
                            );
                            DuelCard_RemoveFromField(card);
                        }
                        value = Duel_CalcCardStats(card);
                        object = (DisplayObject *)CardPreview_CreateResourceObjects(0, value & 0xFFFF, value >> 16);
                        object->xy.h.y = 16;
                        object->xy.h.x = gDisplayObject_apWorkSlots.slots[0]->xy.h.x - 44;
                        object->flags &= ~0x40;
                        gDuel_pCardPreviewObject = object;
                        DisplayObject_StartAlphaTransition(gDisplayObject_apWorkSlots.slots[0], 0, 64, 8);
                        if (slots[1]) {
                            s32 second = (s16)slots[1]->xy.h.x;
                            s32 first = (s16)gDisplayObject_apWorkSlots.slots[0]->xy.h.x;
                            second += 52;
                            gDuel_pCardPreviewObject->xy.h.x = (first + second) / 2 - 70;
                            DisplayObject_StartAlphaTransition(slots[1], 0, 64, 8);
                        }
                        gDuel_wTrapPresentationMode = 1;
                    }
                    break;
                }
            case 1:
                {
                    DisplayObject **slots;
                    s32 alpha;
                    if (DisplayObject_FindAllocatedByTag(1))
                    return;
                    if (!(gDuel_wTrapPresentationMode & 0x80)) {
                        slots = gDisplayObject_apWorkSlots.slots;
                        slots[0]->flags &= ~0x40;
                        gDuel_wTrapPresentationMode |= 0x80;
                        if (slots[1])
                        slots[1]->flags &= ~0x40;
                        gDuel_pCardPreviewObject->field_20.b.rotation_y = 192;
                        gDuel_pCardPreviewObject->flags |= 0x44;
                        return;
                    }
                    alpha = gDuel_pCardPreviewObject->field_20.b.rotation_y + 8;
                    gDuel_pCardPreviewObject->field_20.b.rotation_y = alpha;
                    if ((s8)alpha >= 0) {
                        DisplayObject *entry = gDuel_pCardPreviewObject;
                        u16 flags = entry->flags;
                        entry->field_20.b.rotation_y = 0;
                        gDuel_wTrapPresentationMode = 2;
                        entry->flags = flags & ~4;
                    }
                    break;
                }
            case 2:
                {
                    DisplayObject **slots;
                    card = &gDuel_aCardRecords[gDisplayObject_apWorkSlots.slots[0]->field_6A];
                    object = gDuel_pCardPreviewObject;
                    if (!(gDuel_wTrapPresentationMode & 0x80)) {
                        gDuel_wTrapPresentationMode |= 0x80;
                        object->field_60 = 30;
                        if (!((s16)gDuel_wPlacementCardId & 0x8000)) {
                            object->position.h.field_28 = 0;
                            object->position.h.field_2A = 500;
                            gDuel_wTrapPresentationMode |= 0x40;
                            object->field_2C.h.field_2C = card->stat_modifier;
                            if (gDuel_wEquipCardId == 0x291) {
                                object->position.h.field_2A = 1000;
                                gDuel_wPendingEquipBonus += 500;
                            }
                        }
                    }
                    if (gDuel_wTrapPresentationMode & 0x40) {
                        s32 below;
                        u16 limit;
                        object->position.h.field_28 += 31;
                        below = (s16)object->position.h.field_28 < (s16)object->position.h.field_2A;
                        limit = object->position.h.field_2A;
                        if (!below) {
                            gDuel_wTrapPresentationMode &= ~0x40;
                            object->position.h.field_28 = limit;
                            object->field_60 = 16;
                        }
                        card->stat_modifier = object->field_2C.h.field_2C + object->position.h.field_28;
                        value = Duel_CalcCardStats(card);
                        gCardPreview_Resources[0].attack = value;
                        gCardPreview_Resources[0].defense = value >> 16;
                        return;
                    }
                    object->field_60--;
                    if (object->field_60 > 0)
                    return;
                    {
                        DisplayObject *entry = gDuel_pCardPreviewObject;
                        /* A plain member store reschedules this function (it shrinks). */
                        *(u16 *)((u32)entry + (u32)&((DisplayObject *)0)->flags) = entry->flags | 4;
                    }
                    slots = gDisplayObject_apWorkSlots.slots;
                    DisplayObject_ReleaseIfPresent(slots[0]);
                    DisplayObject_ReleaseIfPresent(slots[1]);
                    slots[1] = 0;
                    slots[0] = 0;
                    object = (DisplayObject *)Duel_CreateCardDisplayObject(card, 64, 82);
                    gDisplayObject_apWorkSlots.slots[0] = object;
                    object->field_20.word = 0;
                    gDuel_wTrapPresentationMode = 3;
                    object->flags &= ~0x44;
                    break;
                }
            case 3:
                {
                    DisplayObject **slots;
                    s32 alpha;
                    object = gDisplayObject_apWorkSlots.slots[0];
                    if (!(gDuel_wTrapPresentationMode & 0x80)) {
                        alpha = gDuel_pCardPreviewObject->field_20.b.rotation_y + 8;
                        gDuel_pCardPreviewObject->field_20.b.rotation_y = alpha;
                        if ((u8)alpha < 64)
                        return;
                        {
                            u16 position = gDuel_pCardPreviewObject->xy.h.x;
                            gDuel_wTrapPresentationMode |= 0x80;
                            object->xy.h.x = position + 44;
                        }
                        DisplayObject_StartAlphaTransition(object, 192, 0, 8);
                        object->flags |= 0x40;
                        DuelEffect_ReleaseResourceObjects(0);
                        return;
                    }
                    if (!DisplayObject_FindAllocatedByTag(1))
                    gDuel_wTrapPresentationMode = 4;
                    break;
                }
            case 4:
                {
                    DisplayObject **slots;
                    object = gDisplayObject_apWorkSlots.slots[0];
                    if (!(gDuel_wTrapPresentationMode & 0x80)) {
                        gDuel_wTrapPresentationMode |= 0x80;
                        DisplayObject_SavePosition((DisplayObjectSnapshot *)object);
                        object->field_60 = 0;
                    }
                    DisplayObject_InterpolatePositionCosine((DisplayObjectPosition *)object, 64, 82, object->field_60);
                    object->field_60 += 128;
                    if (object->field_60 >= 2048)
                    gDuel_bSceneSubStep = 3;
                    break;
                }
            }
            break;
        }
    case 5:
        {
            DisplayObject **slots;
            if (!(gDuel_bSceneSubStep & 0x80)) {
                DisplayObject **slots;
                slots = gDisplayObject_apWorkSlots.slots;
                slots[0]->flags &= ~0x40;
                gDuel_bSceneSubStep |= 0xC0;
                DisplayObject_StartAlphaTransition(slots[1], 0, 64, 8);
                object = gDisplayObject_apWorkSlots.slots[0];
                if (object->resource_branch_index != 23)
                object = slots[1];
                gDuel_wEquipCardId = PLACEMENT_REPLAY_CARD(object).card_id;
                return;
            }
            if (gDuel_bSceneSubStep & 0x40) {
                DisplayObject **slots;
                if (DisplayObject_FindAllocatedByTag(1))
                return;
                {
                    s32 phase = gDuel_bSceneSubStep;
                    if (phase & 0x20) {
                        phase_clear = phase & 0x9F;
                        gDuel_bSceneSubStep = phase_clear;
                        return;
                    }
                }
                {
                    DisplayObject **release_slots = gDisplayObject_apWorkSlots.slots;
                    gDuel_bSceneSubStep |= 0x20;
                    if (release_slots[1]->resource_branch_index < 20) {
                        DisplayObject_ReleaseIfPresent(gDisplayObject_apWorkSlots.slots[0]);
                        gDisplayObject_apWorkSlots.slots[0] = release_slots[1];
                    } else {
                        DisplayObject_ReleaseIfPresent(release_slots[1]);
                    }
                }
                {
                    DisplayObject **publish_slots = gDisplayObject_apWorkSlots.slots;
                    publish_slots[1] = 0;
                    gDisplayObject_apWorkSlots.slots[0]->flags |= 0x40;
                    DisplayObject_StartAlphaTransition(gDisplayObject_apWorkSlots.slots[0], 192, 0, 8);
                }
                return;
            }
            if (!(gDuel_bSceneSubStep & 0x20)) {
                DisplayObject **slots;
                gDuel_bSceneSubStep |= 0x20;
                effect = DuelEffect_AllocateRequest(
                    ((u32)((u16)gDuel_wEquipCardId - 301) < 2) * 4);
                effect->field_00 = gDisplayObject_apWorkSlots.slots[0]->xy.h.x + 26;
                gDuelEffect_pTrackedRequest = effect;
                effect->field_02 = gDisplayObject_apWorkSlots.slots[0]->xy.h.y + 30;
                if (gDuel_wEquipCardId == 302)
                effect->phase = 1;
                if (gDuel_wEquipCardId == 0x291)
                effect->phase = 10;
                SD_SEPlayFull(22);
            } else if (gDuelEffect_pTrackedRequest->progress) {
                gDuel_bSceneSubStep = 4;
            }
            break;
        }
    case 6:
        {
            DisplayObject **slots;
            if (!(gDuel_bSceneSubStep & 0x80)) {
                DisplayObject **slots;
                gDuel_bSceneSubStep |= 0xC0;
                Duel_MarkCreatedCardKnown((u32)(gDuel_wPlacementCardId & 0xFFF));
                slots = gDisplayObject_apWorkSlots.slots;
                slots[0]->flags &= ~0x40;
                DisplayObject_StartAlphaTransition(slots[1], 0, 64, 8);
            }
            if (gDuel_bSceneSubStep & 0x40) {
                DisplayObject **slots;
                if (DisplayObject_FindAllocatedByTag(1))
                return;
                {
                    s32 phase = gDuel_bSceneSubStep;
                    if (!(phase & 0x20)) {
                        slots = gDisplayObject_apWorkSlots.slots;
                        gDuel_bSceneSubStep |= 0x20;
                        DisplayObject_StartAlphaTransition(slots[1], 192, 0, 8);
                        slots[1]->xy.h.x = 140;
                        gDisplayObject_apWorkSlots.slots[0]->flags |= 0x40;
                        DisplayObject_StartAlphaTransition(gDisplayObject_apWorkSlots.slots[0], 192, 0, 8);
                        return;
                    }
                    phase_clear = phase & 0x9F;
                }
                gDuel_bSceneSubStep = phase_clear;
                return;
            }
            if (!(gDuel_bSceneSubStep & 0x20)) {
                DisplayObject **slots;
                gDisplayObject_apWorkSlots.slots[0]->field_60 = 0;
                gDisplayObject_apWorkSlots.slots[0]->position.h.field_28 = 1;
                gDisplayObject_apWorkSlots.slots[0]->position.h.field_2A = 0x2680;
                gDuel_bSceneSubStep |= 0x20;
                gDisplayObject_apWorkSlots.slots[0]->field_2C.h.field_2E = 1;
                {
                    DuelEffectRequest *request;
                    request = DuelEffect_AllocateRequest(1);
                    request->field_00 = 128;
                    /* A plain member store reschedules this function (it shrinks). */
                    (*(s16 *)((u32)request + (u32)&((DuelEffectRequest *)0)->field_02)) = gDisplayObject_apWorkSlots.slots[0]->xy.h.y + 30;
                    gDuelEffect_pTrackedRequest = request;
                    slots = gDisplayObject_apWorkSlots.slots;
                    if (slots[0]->resource_branch_index >= 20 || slots[1]->resource_branch_index >= 20)
                    request->phase = 1;
                }
                SD_SEPlayFull(24);
                return;
            }
            if (gDuelEffect_pTrackedRequest->progress) {
                DisplayObject **slots;
                slots = gDisplayObject_apWorkSlots.slots;
                slots[0]->flags &= ~0x40;
                slots[1]->flags &= ~0x40;
                SD_SEPlayFull(25);
                gDuel_bSceneSubStep = 4;
            } else {
                object = gDisplayObject_apWorkSlots.slots[0];
                object->field_2C.h.field_2E += 8;
                object->field_60 += gDisplayObject_apWorkSlots.slots[0]->field_2C.h.field_2E;
                object->position.h.field_2A -= object->field_60 / 12;
                n = (s16)object->position.h.field_2A >> 8;
                x = rcos(object->field_60) * n / 4096;
                value = rsin(object->field_60) * n / 4096;
                object->xy.h.x = 102 - x;
                object->xy.h.y = 82 - value;
                slots = gDisplayObject_apWorkSlots.slots;
                slots[1]->xy.h.x = x + 102;
                slots[1]->xy.h.y = value + 82;
            }
            break;
        }
    case 7:
        {
            DisplayObject **slots;
            slots = gDisplayObject_apWorkSlots.slots;
            object = slots[0];
            if (!(gDuel_bSceneSubStep & 0x80)) {
                gDuel_bSceneSubStep |= 0x80;
                object->flags |= 4;
                object->attribute &= ~0x08000000;
                SD_SEPlayFull(26);
                if (slots[1]->resource_branch_index >= 20 && object->resource_branch_index < 20) {
                    DisplayObject *other = slots[1];
                    slots[1] = object;
                    object = other;
                    slots[0] = other;
                }
                DisplayObject_ResetVelocity(
                    DISPLAY_OBJECT_VELOCITY_VIEW(object));
                object->field_34.h.field_36 = -((rand() & 255) + 768);
                object->field_38.h.field_38 = -640;
            }
            object->field_34.h.field_36 = DisplayObject_StepTowardZero(object->field_34.h.field_36, 8);
            object->field_38.h.field_38 = DisplayObject_StepToward(object->field_38.h.field_38, 2048, 96);
            DisplayObject_StepPositionXY(
                DISPLAY_OBJECT_VELOCITY_VIEW(object));
            if ((s16)object->xy.h.x < -52 || (s16)object->xy.h.y >= DUEL_CARD_PLACEMENT_SCREEN_HEIGHT) {
                DisplayObject_ReleaseIfPresent(object);
                gDisplayObject_apWorkSlots.slots[0] = gDisplayObject_apWorkSlots.slots[1];
                TextBox_Destroy(gText_aChannels);
                gDuel_bSceneSubStep = 3;
            }
            break;
        }
    case 8:
        {
            DisplayObject *object;
            DuelCardRecord *card;
            DisplayObject **slots;
            object = gDisplayObject_apWorkSlots.slots[0];
            gDuel_wSceneStateFlags = 6;
            if (object->resource_branch_index < 20) {
                s32 index = object->field_6A;
                DuelCardRecord *base = gDuel_aCardRecords;
                gDuel_wSceneStateFlags = 0x4008;
                card = &base[index];
                card->flags &= ~0x1000;
                if (gDuel_bPlacementNeedsCardMoveSetup)
                gDuel_wSceneStateFlags = 8;
            }
            break;
        }
    }
}

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/inline_c.h"

/* Projects one live-slot ground pair at height -24; returns signed screen X. */
s16 Duel_ProjectCardSlotScreenX(s32 index)
{
    s32 coord = index;
    ProjectedPair result;

    SetGeomScreen(gViewState_Current.projection);
    coord *= 4;
    SetGeomOffset(0xA0, 0x6C);
    GsSetLsMatrix(&gGraphics_LightSourceMatrix);
    {
        SVECTOR *scratch = (SVECTOR *)SCRATCH_ADDR(0x3E0);
        u16 *source = (u16 *)((u8 *)gDuel_aCardSlotGroundPositions + coord);
        u16 x = source[0];

        scratch->vy = -0x18;
        scratch->vx = x;
        scratch->vz = source[1];
        gte_ldv0(scratch);
        gte_rtps();
        gte_stsxy(&result);
    }
    SetGeomOffset(0, 0);
    return result.x;
}

#include "ai.h"
#include "display_object_work_slots.h"
#include "duel_card.h"
#include "duel_card_staging.h"
#include "text_helpers.h"
#include "display_object.h"
#include "card_constants.h"
#include "sound.h"
#include "ai_constants.h"
#include "duel_card_layout.h"
#include "duel_grid.h"

/* One screen height above the top: -0xF0 in the US build, -0x100 in the
   European one, whose wrapper defines its own. */
#ifndef DUEL_CARD_OFFSCREEN_Y
#define DUEL_CARD_OFFSCREEN_Y -0xF0
#endif
#include "input.h"
#include "sound.h"

#ifndef DUEL_CARD_STAGING_CONFIRM_MASK
#define DUEL_CARD_STAGING_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#endif

#define DISPLAY_OBJECT_POSITION_VIEW(object) \
    ((DisplayObjectPosition *)(object))
#define DISPLAY_OBJECT_SNAPSHOT_VIEW(object) \
    ((DisplayObjectSnapshot *)(object))

void DuelScene_UpdateCardMove(void)
{
    DisplayObject *object;
    DuelEffectChannel *box;
    DuelCardRecord *card;
    DuelCardRecord *scratch;
    DuelEffectRequest *request;
    u16 flags;
    u16 z;
    s32 slot;

    object = gDisplayObject_apWorkSlots.slots[0];
    if (!(gDuel_wSceneStateFlags & DUEL_SCENE_FLAG_INITIALIZED)) {
        gDuel_wSceneStateFlags |= DUEL_SCENE_FLAG_INITIALIZED;
        if (gDuel_wSceneStateFlags & 0x4000) {
            gDuel_bSceneSubStep = 4;
            return;
        }
        if (gDuel_bSide0ControllerSelector[gDuel_bActiveSide] >= 0 &&
            object->resource_branch_index < 0x14) {
            gDuel_aCardRecords[object->field_6A].flags &= ~0x200;
            if (gAi_Selection.bytes[7] & 1) {
                gDuel_aCardRecords[object->field_6A].flags |= 0x200;
            }
            gDuel_bSceneSubStep = 4;
            return;
        }
        gDuel_bSceneSubStep = 1;
        DisplayObject_SavePosition(DISPLAY_OBJECT_SNAPSHOT_VIEW(object));
        object->field_60 = 0;
        if ((s16)object->xy.h.x == 0x86 ||
            (s16)object->xy.h.y == 0x2A) {
            gDuel_bSceneSubStep = 2;
            return;
        }
    }

    switch (gDuel_bSceneSubStep & 0xF) {
    case 1:
        DisplayObject_InterpolatePositionCosine(
            DISPLAY_OBJECT_POSITION_VIEW(object),
            0x86, 0x2A, object->field_60);
        object->field_60 += 0x80;
        if (object->field_60 < 0x800) {
            break;
        }
        object->xy.h.x = 0x86;
        object->xy.h.y = 0x2A;
        gDuel_bSceneSubStep = 2;
        /* Fall through to the next presentation phase in the same frame. */
    case 2:
        if (object->resource_branch_index >= 0x14) {
            gDuel_bSceneSubStep = 4;
            break;
        }
        gDuel_bSceneSubStep = 3;
        /* Fall through to initialize the choice dialog. */
    case 3:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            gDuel_wSelectedCardID =
                ((DuelCardStagingDeckView *)gDuel_abCardStagingBuffer)->field_cards[object->field_6A].card_id;
#ifdef VERSION_JAPAN
            box = TextBox_CreateFlagged(0, 0x21, 0x50, 0x6E, 0xA0, 0x30, 0x20);
#elif defined(VERSION_EUROPE)
            /* The European build makes neither the DuelEffect_SetEntryRangeLayout call nor
               the two field stores, and flags the box 0x40. */
            box = TextBox_CreateFlagged(0, 0x21, 0x48, 0x6E, 0xB0, 0x30, 0x40);
#else
            DuelEffect_SetEntryRangeLayout(0, 0xB);
            box = TextBox_CreateFlagged(0, 0x21, 0x48, 0x6E, 0xB0, 0x30, 0x20);
            box->glyph_advance_x = 8;
            box->line_advance_y = 0x10;
#endif
            do {
                TextBox_UpdateChannels();
            } while (box->prompt_object == 0);
            break;
        }
        if (gDuel_bSceneSubStep & 0x10) {
            gDuel_aCardRecords[object->field_6A].flags &= ~0x200;
            if (gDialog_bChoice) {
                gDuel_aCardRecords[object->field_6A].flags |= 0x200;
            }
            TextBox_Destroy(gText_aChannels);
            gDuel_bSceneSubStep = 4;
            break;
        }
        if (!Dialog_ReadChoiceInput(gText_aChannels) &&
            (gInput_awPadPressed[0] & DUEL_CARD_STAGING_CONFIRM_MASK)) {
            SD_SEPlayFull(7);
            gDuel_bSceneSubStep |= 0x10;
        }
        break;
    case 4:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            object->field_2C.h.field_2E = Duel_ProjectCardSlotScreenX(gDuel_bCardMoveSlot) - 0x1E;
            DisplayObject_SavePosition(DISPLAY_OBJECT_SNAPSHOT_VIEW(object));
            object->field_60 = 0x400;
        }
        if (!(gDuel_bSceneSubStep & 0x40)) {
            Widget_SlideSine(DISPLAY_OBJECT_POSITION_VIEW(object),
                             object->field_2C.h.field_2E, -0xBC,
                             object->field_60);
            object->field_60 -= 0x2A;
            if (object->field_60 > 0) {
                break;
            }
            gDuel_bSceneSubStep |= 0x40;
            scratch = (DuelCardRecord *)SCRATCH_ADDR(0);
            *scratch = gDuel_aCardRecords[object->field_6A];
            Duel_SetupFieldCardFromDeck(gDuel_bCardMoveSlot, object->field_6B);
            card = &gDuel_aCardRecords[gDuel_bCardMoveSlot];
            flags = card->flags | (scratch->flags & 0x7E00);
            card->flags = flags & ~0x400;
            if (!(flags & 0x1000)) {
                card->flags = flags & ~0x2400;
            }
            card->stat_modifier = scratch->stat_modifier;
            DisplayObject_ReleaseIfPresent(object);
            gDisplayObject_apWorkSlots.slots[0] = card->object;
            object = gDisplayObject_apWorkSlots.slots[0];
            Duel_ApplyCardObjectFlags((DuelCardDisplayObject *)object);
            DISPLAY_OBJECT_POSITION_VIEW(object)->out_y = DUEL_CARD_OFFSCREEN_Y;
            DisplayObject_SavePosition(DISPLAY_OBJECT_SNAPSHOT_VIEW(object));
            object->field_60 = -0x400;
            break;
        }
        Widget_SlideSine(DISPLAY_OBJECT_POSITION_VIEW(object),
                         (s16)object->xy.h.x, -0x18,
                         object->field_60);
        object->field_60 += 0x2A;
        if (object->field_60 < 0) {
            break;
        }
        DISPLAY_OBJECT_POSITION_VIEW(object)->out_y = -0x18;
        gDuel_bSceneSubStep = 5;
        SD_SEPlayFull(0xC);
        break;
    case 5:
        if (gDuel_wPendingEquipBonus &&
            Duel_SelectTrapByCardId(DUEL_REVERSE_TRAP_CARD_ID)) {
            gDuel_bSceneSubStep = 6;
            break;
        }
        gDuel_wSceneStateFlags = 5;
        break;
    case 6:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0xC0;
            gDuel_wTrapPresentationMode = 0;
        }
        if (gDuel_bSceneSubStep & 0x40) {
            if (Duel_UpdateTrapPresentation()) {
                break;
            }
            request = DuelEffect_CreateRequest(0xD);
            request->field_00 = object->xy.h.x;
            request->field_02 = object->xy.h.y;
            z = object->field_34.h.z;
            request->number = -gDuel_wPendingEquipBonus;
            request->field_04 = z;
            SD_SEPlayFull(0x21);
            gDuel_bSceneSubStep &= ~0x40;
            break;
        }
        slot = object->field_6A;
        gDuel_wSceneStateFlags = 5;
        card = &gDuel_aCardRecords[slot];
        card->stat_modifier -= gDuel_wPendingEquipBonus * 2;
        break;
    }
}

/* Regional values: the inner object's y (194 -> 210), and the count sprite's
 * field_5D (0xB8 -> 0xB0) and field_40, its CLUT x (0x250 -> 0x2D0). The
 * European build (src/game/european/) defines its own. */
#ifndef DUEL_HAND_STACK_INNER_Y
#define DUEL_HAND_STACK_INNER_Y 194
#endif
#ifndef DUEL_HAND_STACK_COUNT_FIELD_5D
#define DUEL_HAND_STACK_COUNT_FIELD_5D 0xB8
#endif
#ifndef DUEL_HAND_STACK_COUNT_CX
#define DUEL_HAND_STACK_COUNT_CX 0x250
#endif

void DuelHand_PositionSelectionCursor(DuelSelectionRecord *object)
{
    DisplayObject *inner = object->cursor_object;

    inner->xy.h.x = (s8)object->hand_cursor_slot * 60 + 14;
    inner->xy.h.y = DUEL_HAND_STACK_INNER_Y;
}

void DuelHand_CreateSelectionOrderMarker(DuelSelectionRecord *arg)
{
 register DuelSelectionRecord*object=arg;register DuelHandSlot*slot=&gDuelHand_aSlots[(s8)object->hand_cursor_slot];register DisplayObject*child;
 slot->object->xy.s.y-=4;child=DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(),1);DisplayObject_ConfigureScreenSprite(child,slot->object->xy.s.x,slot->object->xy.s.y,0x10,0x10,object->ordered_pick_count<<4,DUEL_HAND_STACK_COUNT_FIELD_5D,0xB,DUEL_HAND_STACK_COUNT_CX,0xFC);
 DisplayObject_SelectOrderingTable1(child);DisplayObject_SetDepthOffset(child,(s8)((u8)slot->object->depth_offset+1));slot->order_marker=child;object->ordered_pick_count++;slot->selection_order=object->ordered_pick_count;SD_SEPlayFull(0x2F);
}

void DuelHand_RefreshSelectionColors(DuelSelectionRecord *side) {
    s32 i;
    for (i = 0; i < HAND_SIZE; i++)
        if (gDuelHand_aSlots[i].selection_order == 0)
            gDuelHand_aSlots[i].object->rgb =
                DUEL_DISPLAY_COLOR_DIMMED;
    if (side->ordered_pick_count == 0)
        gDuelHand_aSlots[(s8)side->hand_cursor_slot].object->rgb =
            DUEL_DISPLAY_COLOR_NORMAL;
}

/* Regional value: the record's panel_target_y (0x74 -> 0x84). The European build
 * (src/game/european/) defines its own. */
#ifndef DUEL_SELECTION_PANEL_TARGET_Y
#define DUEL_SELECTION_PANEL_TARGET_Y 0x74
#endif

/* Private helpers of the same duel action controller,
   DuelScene_UpdateHandActions:
   field-placement selection setup followed by execution of the
   AI-script hand/field selection in state 0. */

void Duel_BeginFieldPlacementSelection(DuelSelectionRecord *selection) {
    DisplayObject *r;
    DuelCardRecord *e;
    u8 *b;
    DuelCardReplayRecordBlock *g;
    s32 k;
    s32 c;
    s32 i;

    gDuel_pActiveSelectionRecord = (DuelSelectionRecord *)((gDuel_abSelectionSides + 2 * DUEL_SELECTION_RECORD_SIZE) +
                                       gDuel_bActiveSide * DUEL_SELECTION_SIDE_SIZE);
    gDuel_pActiveSelectionRecord->status = 0;
    gDuel_pActiveSelectionRecord->panel_variant = 1;
    gDuel_pActiveSelectionRecord->panel_target_y = DUEL_SELECTION_PANEL_TARGET_Y;
    gDuel_pActiveSelectionRecord->field_actions_mode = 0;
    gDuel_pActiveSelectionRecord->row_min = 2;
    gDuel_pActiveSelectionRecord->row_end = 3;

    if (selection->ordered_pick_count == 0) {
        b = gDuel_abCardStagingBuffer;
        r = gDuelHand_aSlots[(s8)selection->hand_cursor_slot].object;
        g = (DuelCardReplayRecordBlock *)(
            (u8 *)&((DuelCardRecord *)b)[r->field_6A] +
            DUEL_CARD_STAGING_REPLAY_BASE_OFFSET);
        i = (gDuel_adwCardStats[((DuelDeckCardRecord *)g->record.data)->id - 1] >>
             CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
    k = i;
        if (k >= CARD_TYPE_MAGIC) {
            if (r->field_20.b.rotation_y != 0 || k == CARD_TYPE_TRAP) {
                gDuel_pActiveSelectionRecord->row_min = 3;
                gDuel_pActiveSelectionRecord->row_end = 4;
            }
        }
    }

    gDuel_pActiveSelectionRecord->row = gDuel_pActiveSelectionRecord->row_min;
    c = gDuel_abFieldGridCardRecordIndices.flat[
        gDuel_pActiveSelectionRecord->row * DUEL_FIELD_ROW_SIZE +
        gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT
    ];
    e = &gDuel_aCardRecords[c];

    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++, e++) {
        if ((e->flags & DUEL_CARD_FLAG_OCCUPIED) == 0) {
            gDuel_pActiveSelectionRecord->col = i;
            break;
        }
    }

    gDuel_wDisplayTransitionFlags = 3;
}

/* The deck records sit 0x31E0 bytes below the active-card table, and retail
   forms their base from that table's address rather than from a label of its
   own. */
#define DUEL_DECK_RECORDS_BELOW_ACTIVE_CARDS \
    ((DuelDeckCardRecord *)((u8 *)gDuel_aActiveCards - 0x31E0))

/* Applies gAi_Selection to the hand: reserves existing hand picks in the
 * local availability list, swaps pool-card records into other occupied hand
 * slots, rebuilds their objects, and rewrites picks as hand-slot codes.
 *
 * The slot search is entered through a constant switch, and the order and
 * card-record bases are added as integers: those forms keep the retail
 * loop-invariant placement, operand order and register roles. */
void Duel_ApplyAiHandSelection(void)
{
    s8 sel[HAND_SIZE];
    DuelDeckCardRecord tmp;
    DuelDeckCardRecord *rec;
    DuelDeckCardRecord *other;
    DuelCardDisplayObject *spawned;
    DuelHandSlot *slot;
    DuelCardRecord *records;
    AiActiveCard *card;
    u8 *order;
    s32 i;
    s32 j;
    s32 v;
    s32 id;
    s32 k;

    for (i = 0; i < HAND_SIZE; i++) {
        sel[i] = gDuel_pActiveSideState->hand[i];
    }
    for (i = 0; i < HAND_SIZE; i++) {
        v = gAi_Selection.bytes[i];
        if (v == 0) {
            break;
        }
        if (v < 0x10) {
            sel[v - 0xB] = -1;
        }
    }
    for (i = 0; i < HAND_SIZE; i++) {
        v = gAi_Selection.bytes[i];
        if (v == 0) {
            return;
        }
        if (v >= 0x10) {
            j = 0;
            order = gDuel_abHandCardRecordIndices;
            records = gDuel_aCardRecords;
            slot = gDuelHand_aSlots;
            /* The constant switch enters the hand scan at its case label, so
               loop.c leaves it alone as with retail's goto loop (a structured
               scan hoists an address and grows the frame). */
            switch (0) {
                for (;;) {
                }
                do {
            case 0:
                    if (sel[j] >= 0) {
                        /* The empty loop keeps jump.c from moving this arm
                           ahead of the slot test. */
                        do {
                        } while (0);
                        card = &gDuel_aActiveCards[v];
                        rec = DUEL_DECK_RECORDS_BELOW_ACTIVE_CARDS + sel[j];
                        other = DUEL_DECK_RECORDS_BELOW_ACTIVE_CARDS +
                                card->deck_index;
                        v = rec->deck_index;
                        rec->deck_index = other->deck_index;
                        other->deck_index = v;
                        tmp = *rec;
                        *rec = *other;
                        *other = tmp;
                        k = gDuel_bActiveSide * HAND_SIZE;
                        /* Integer address arithmetic keeps retail's addu operand order;
                           u8 pointer arithmetic swaps it. */
                        id = *(u8 *)((j + k) + (s32)order);
                        spawned = (DuelCardDisplayObject *)slot->object;
                        Duel_SetupCardRecord(id, rec->deck_index);
                        slot->object = (DisplayObject *)((DuelCreateHandCardDisplayObjectAt)Duel_CreateHandCardDisplayObject)(
                            (DuelCardRecord *)(id * DUEL_CARD_RECORD_SIZE +
                                               (s32)records),
                            spawned->out_x, spawned->out_y);
                        DisplayObject_ReleaseIfPresent(spawned);
                        gDuel_pActiveSideState->hand[j] = rec->deck_index;
                        gAi_Selection.bytes[i] = j + 0xB;
                        sel[j] = -1;
                        break;
                    }
                    j++;
                    slot++;
                } while (j < HAND_SIZE);
            }
        }
    }
}

#undef DUEL_DECK_RECORDS_BELOW_ACTIVE_CARDS

#include "input.h"

int Duel_CheckQuitInput(void)
{
    if (gDuel_bOpponentID < 0) {
        if (gInput_awPadPressed[0] & PAD_BUTTON_SELECT) {
            gDuel_bQuitDialogState = 1;
            return 1;
        }
    }
    return 0;
}

#include "../psyq/stdio.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_card_staging.h"
#include "duel_grid.h"
#include "duel_card_viewer.h"
#include "display_object.h"
#include "display_object_work_slots.h"
#include "input.h"
#include "ai.h"
#include "script_scene.h"
#include "sound.h"
#include "display_object_layout.h"

#define SIDE_CURSOR(base) \
    ((DuelSelectionRecord *)((base) + gDuel_bActiveSide * DUEL_SELECTION_SIDE_SIZE))
#define HAND_CARD_INDEX(side) \
    (gDuel_abHandCardRecordIndices[(s8)(side)->hand_cursor_slot + gDuel_bActiveSide * 5])
#define FIELD_CURSOR \
    (((DuelSelectionSide *)gDuel_abSelectionSides)[gDuel_bActiveSide].records[2])
#define CARD_RECORD(i) (((DuelCardStagingDeckView *)gDuel_abCardStagingBuffer)->field_cards[i])
#define CARD_ID(i) (CARD_RECORD(i).card_id)
#define CARD_KIND(id) \
    ((gDuel_adwCardStats[(id) - 1] >> CARD_STAT_TYPE_SHIFT) & \
     CARD_STAT_TYPE_MASK)
/* The two choice sprites set their flags through the halfword, not the
 * record: the store stays ahead of the pointer's own store. */
#define SPRITE_FLAGS(sprite) (*(u16 *)((u8 *)(sprite) + 8))

#ifndef DUEL_HAND_ACTIONS_LOG_INVALID_AI_SELECTION
#define DUEL_HAND_ACTIONS_LOG_INVALID_AI_SELECTION 1
#endif

#ifdef VERSION_JAPAN
/* The Japanese build cancels with Cross, confirms with Circle or Square, and
 * reads the card viewer's cell from the side's own column and row. */
#define HAND_ACTIONS_CANCEL_MASK PAD_BUTTON_CROSS
#define HAND_ACTIONS_CONFIRM_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#define HAND_ACTIONS_VIEWER_CELL (side->row * 5 + side->col)
#else
#define HAND_ACTIONS_CANCEL_MASK PAD_BUTTON_CANCEL
#define HAND_ACTIONS_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#define HAND_ACTIONS_VIEWER_CELL (FIELD_CURSOR.row * 5 + FIELD_CURSOR.col)
#endif

void DuelScene_UpdateHandActions(void)
{
    DuelSelectionRecord *side;
    DuelHandSlot *hand;
    DuelHandSlot *slot;
    DuelHandSlot *first_slot;
    DisplayObject **out;
    DisplayObject *sprite;
    DisplayObject *obj;
    DuelCardRecord *card;
    s32 n;
    s32 i;
    s32 v;
    s32 value;
    s32 card_id;

    side = (DuelSelectionRecord *)(gDuel_abSelectionSides + gDuel_bActiveSide * DUEL_SELECTION_SIDE_SIZE);
    if (!(gDuel_wSceneStateFlags & 0x8000)) {
        gDuel_wSceneStateFlags |= 0x8000;
        for (n = 6; n >= 0; n--) {
            gDisplayObject_apWorkSlots.slots[n] = 0;
        }
        gDuel_pFlipRightMarker = 0;
        gDuel_pFlipLeftMarker = 0;
        gDuel_pActiveSelectionRecord = side;
        gDuel_wDisplayTransitionFlags = 0;
        side->hand_cursor_slot = 0;
        side->ordered_pick_count = 0;
        obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteResource(
            obj, 3, 0, 2, 0xB, 0x20C);
        obj->flags |= 0x28;
        DisplayObject_SelectOrderingTable1(obj);
        DisplayObject_SetDepthOffset(obj, 0xA);
        side->cursor_object = obj;
        DuelHand_PositionSelectionCursor(side);
        DuelField_CreateCardInfoTextBox(side, HAND_CARD_INDEX(side));
        gDuel_bSceneSubStep = 1;
        if (gDuel_bSide0ControllerSelector[gDuel_bActiveSide] >= 0) {
            gDuel_bSceneSubStep = 2;
        }
        return;
    }
    if (gDuel_wSceneStateFlags & 0x4000) {
        Duel_UpdateCardPickCursor(gDuel_pActiveSelectionRecord);
        if (gDuelCursor_PickFlags != 0) {
            return;
        }
        gDuel_pActiveSelectionRecord = SIDE_CURSOR(gDuel_abSelectionSides);
        gDuel_wSceneStateFlags &= 0xBFFF;
        return;
    }

    hand = &gDuelHand_aSlots[(s8)side->hand_cursor_slot];
    switch (gDuel_bSceneSubStep & 0xF) {
    case 2:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            gDuel_wActionSubstate = 0;
        }
        switch (gDuel_wActionSubstate & 0xF) {
        case 0:
            if (!(gDuel_wActionSubstate & 0x8000)) {
                gDuel_wActionSubstate |= 0x8000;
                Ai_RefreshBoardSnapshot();
                AiScript_Init(gFile_StagingBuffer);
                return;
            }
            if (AiScript_Run() != 0) {
                /* s8 view: retail stores li -1, not li 255. */
                *(s8 *)&gDuel_bAiSelectionCursor = -1;
                gDuel_wAiHandStackPending = 0;
                if (gAi_Selection.bytes[1] != 0) {
                    gDuel_wAiHandStackPending = 1;
                }
                gDuel_wActionSubstate = 1;
                Duel_ApplyAiHandSelection();
                return;
            }
            return;
        case 1:
            if (!(gDuel_wActionSubstate & 0x8000)) {
                gDuel_wActionSubstate |= 0x8000;
                gDuel_bAiSelectionCursor++;
                value = gAi_Selection.bytes[(s8)gDuel_bAiSelectionCursor];
                if (value == 0) {
                    DuelHand_RefreshSelectionColors(side);
                    gDuel_wActionSubstate = 2;
                    gDuel_wAiHandStackPending &= 1;
                    if (gDuel_wAiHandStackPending != 0) {
                        gDuel_wActionSubstate = 3;
                    }
                    SD_SEPlayFull(7);
                    return;
                }
                if (value < 0xB) {
#if DUEL_HAND_ACTIONS_LOG_INVALID_AI_SELECTION
                    printf(gDuel_szComSelCardError);
#endif
                    return;
                }
                gDuel_bAiCursorTarget = value - 0xB;
                gDuel_wAiSelectionStepDelay = 6;
            }
            gDuel_wAiSelectionStepDelay--;
            if ((s16)gDuel_wAiSelectionStepDelay <= 0) {
                gDuel_wAiSelectionStepDelay = 6;
                if ((s8)side->hand_cursor_slot != gDuel_bAiCursorTarget) {
                    SD_SEPlayFull(6);
                    if ((s8)side->hand_cursor_slot > gDuel_bAiCursorTarget) {
                        side->hand_cursor_slot -= 1;
                    } else {
                        side->hand_cursor_slot += 1;
                    }
                    DuelHand_PositionSelectionCursor(side);
                    DuelField_CreateCardInfoTextBox(side, HAND_CARD_INDEX(side));
                    return;
                }
                if (gDuel_wAiHandStackPending & 1) {
                    DuelHand_CreateSelectionOrderMarker(side);
                }
                gDuel_wActionSubstate &= 0x7FFF;
                return;
            }
            return;
        case 2:
            obj = hand->object;
            if (!(gDuel_wActionSubstate & 0x8000)) {
                gDuel_wActionSubstate |= 0xC000;
                card_id = CARD_ID(obj->field_6A);
                if (CARD_KIND(card_id) < CARD_TYPE_MAGIC) {
                    if (gAi_Selection.bytes[8] == 0) {
                        gDuel_wActionSubstate = 3;
                        return;
                    }
                }
                obj->position.s.x = 0x86;
                obj->position.s.y = 0x5A;
                obj->field_2C.h.field_2C = 0x10;
                obj->field_6C = 1;
                obj->update = (DisplayObjectCallback)DisplayObject_UpdateCosineSlide;
                DisplayObject_SetDepthOffset(
                    obj, (s8)(obj->depth_offset + 4));
                side->cursor_object->flags &= 0xFFBF;
                return;
            }
            if (gDuel_wActionSubstate & 0x4000) {
                if (DisplayObject_FindAllocatedByTag(1) == 0) {
                    gDuel_wActionSubstate &= 0xBFFF;
                    if (gDuel_wActionSubstate & 0x1000) {
                        DisplayObject_SetDepthOffset(
                            obj, (s8)(obj->depth_offset - 4));
                        gDuel_wActionSubstate = 3;
                        side->cursor_object->flags |= 0x40;
                        return;
                    }
                    obj->field_2C.h.field_2C = obj->field_34.h.field_36;
                    obj->field_2C.h.field_2E = obj->field_38.h.field_38;
                    if (gAi_Selection.bytes[8] != 0) {
                        SD_SEPlayFull(0xB);
                        gDuel_wActionSubstate |= 0x2000;
                        obj->flags |= 4;
                        return;
                    }
                }
            } else if (gDuel_wActionSubstate & 0x2000) {
                obj->field_20.b.rotation_y += 8;
                if ((s8)obj->field_20.b.rotation_y < 0) {
                    obj->field_20.b.rotation_y = 0x80;
                    gDuel_aCardRecords[obj->field_6A].flags |= 0x1000;
                    gDuel_wActionSubstate &= 0xDFFF;
                    return;
                }
            } else {
                card_id = CARD_ID(obj->field_6A);
                value = CARD_KIND(card_id);
                if (value >= CARD_TYPE_MAGIC && obj->field_20.b.rotation_y == 0 &&
                    value != CARD_TYPE_EQUIP && value != CARD_TYPE_TRAP) {
                    gDisplayObject_apWorkSlots.slots[0] = obj;
                    gDuel_pActiveSideState->hand[(s8)side->hand_cursor_slot] = -1;
                    gDuel_bSceneSubStep = 5;
                    return;
                }
                obj->update = (DisplayObjectCallback)DisplayObject_UpdateCosineSlide;
                gDuel_wActionSubstate |= 0x5000;
                /* Copying the .word members reschedules the stores. */
                *(s32 *)&obj->position = *(s32 *)&obj->field_2C;
                obj->field_2C.h.field_2C = 0x10;
                obj->field_6C = 1;
                return;
            }
            return;
        case 3:
            if (gDuel_wDisplayTransitionFlags != 0) {
                gDuelCursor_Direction = -1;
                return;
            }
            if (!(gDuel_wActionSubstate & 0x8000)) {
                gDuel_wActionSubstate |= 0xC000;
                Duel_BeginFieldPlacementSelection(side);
                gDuel_bAiCursorTarget = gAi_Selection.bytes[6] - 6;
                if (gDuel_bAiCursorTarget < 0) {
                    gDuel_bAiCursorTarget = gAi_Selection.bytes[6] - 1;
                }
                gDuelCursor_Direction = -1;
                return;
            }
            if (((DuelCursorUpdateDirectionWithDir)DuelCursor_UpdateDirection)(gDuel_pActiveSelectionRecord, gDuelCursor_Direction) == 0) {
                if (gDuel_pActiveSelectionRecord->col == gDuel_bAiCursorTarget) {
                    break;
                }
                gDuelCursor_Direction = 0;
                if (gDuel_bAiCursorTarget < gDuel_pActiveSelectionRecord->col) {
                    gDuelCursor_Direction = 2;
                }
            }
            return;
        default:
            return;
        }
        /* Cases 1, 3 and 4's input handling sit in an if (0) block, so the
           AI cursor's break above skips them straight into the confirm. */
        if (0) {
    case 1:
            if (!(gDuel_bSceneSubStep & 0x80)) {
                gDuel_bSceneSubStep |= 0x80;
                for (n = 0; n < 5; n++) {
                    gDuelHand_aSlots[n].object->rgb = 0x808080;
                }
            }
            if (Duel_CheckQuitInput() != 0) {
                break;
            }
            if (gInput_awPadPressed[0] & 0x10) {
                card_id = CARD_ID(HAND_CARD_INDEX(side));
                gDuel_bCardViewerYOffset = 0x14;
                gDuel_bEffectState = 2;
                gDuel_wViewerCardID = card_id;
                return;
            }
            if (gInput_awPadRepeat[0] & 0xA000) {
                value = (s8)side->hand_cursor_slot;
                if (gInput_awPadRepeat[0] & 0x2000) {
                    value++;
                    if (value >= 5) {
                        return;
                    }
                } else {
                    value--;
                    if (value < 0) {
                        return;
                    }
                }
                side->hand_cursor_slot = value;
                DuelHand_PositionSelectionCursor(side);
                DuelField_CreateCardInfoTextBox(side, HAND_CARD_INDEX(side));
                SD_SEPlayFull(6);
                return;
            }
            if ((gInput_awPadPressed[0] & 0xFFFF) == 0x1000 && gInput_awPadHeld[0] == 0x1000) {
                if (hand->selection_order == 0) {
                    DuelHand_CreateSelectionOrderMarker(side);
                    return;
                }
            } else if ((gInput_awPadPressed[0] & HAND_ACTIONS_CANCEL_MASK) ||
                       ((gInput_awPadPressed[0] & 0xFFFF) == 0x4000 &&
                        gInput_awPadHeld[0] == 0x4000)) {
                value = hand->selection_order;
                if (value != 0) {
                    SD_SEPlayFull(0x2F);
                    hand->selection_order = 0;
                    hand->object->xy.h.y += 4;
                    DisplayObject_ReleaseIfPresent(hand->order_marker);
                    hand->order_marker = 0;
                    side->ordered_pick_count--;
                    hand = gDuelHand_aSlots;
                    for (n = 0; n < 5; n++, hand++) {
                        v = hand->selection_order;
                        if (v >= value) {
                            hand->selection_order = v - 1;
                            *(u8 *)&hand->order_marker->texture_uv =
                                (v - 2) * 0x10;
                        }
                    }
                    return;
                }
            } else {
                if (gInput_awPadHeld[0] & 3) {
                    gDuelCursor_PickFlags = 0;
                    gDuel_pActiveSelectionRecord = SIDE_CURSOR(gDuel_abSelectionSides + 1 * DUEL_SELECTION_RECORD_SIZE);
                    gDuel_wSceneStateFlags |= 0x4000;
                    return;
                }
                if ((gInput_awPadPressed[0] & HAND_ACTIONS_CONFIRM_MASK) && side->ordered_pick_count != 1) {
                    SD_SEPlayFull(7);
                    gDuel_bSceneSubStep = 4;
                    DuelHand_RefreshSelectionColors(side);
                    if (side->ordered_pick_count == 0) {
                        gDuel_bSceneSubStep = 3;
                        return;
                    }
                }
            }
            break;
        case 3:
            obj = hand->object;
            if (!(gDuel_bSceneSubStep & 0x80)) {
                gDuel_bSceneSubStep |= 0xC0;
                obj->position.s.x = 0x86;
                obj->position.s.y = 0x5A;
                obj->field_2C.h.field_2C = obj->xy.h.x;
                obj->field_2C.h.field_2E = obj->xy.h.y;
                DisplayObject_SavePosition((void *)obj);
                obj->field_60 = 0;
                DisplayObject_SetDepthOffset(
                    obj, (s8)(obj->depth_offset + 4));
                side->cursor_object->flags &= 0xFFBF;
            }
            if (gDuel_bSceneSubStep & 0x40) {
                DisplayObject_InterpolatePositionCosine(
                    (void *)obj, obj->position.s.x, obj->position.s.y, obj->field_60);
                obj->field_60 = obj->field_60 + 0xAA;
                if (obj->field_60 >= 0x800) {
                    gDuel_bSceneSubStep &= 0xBF;
                    obj->xy.word = obj->position.word;
                    if (gDuel_bSceneSubStep & 0x10) {
                        side->cursor_object->flags |= 0x40;
                        DisplayObject_SetDepthOffset(
                            obj, (s8)(obj->depth_offset - 4));
                        if (gDuel_bSceneSubStep & 0x20) {
                            gDuel_bSceneSubStep = 4;
                            return;
                        }
                        gDuel_bSceneSubStep = 1;
                        return;
                    }
                    card_id = ((DuelDeckCardRecord *)CARD_RECORD(obj->field_6A).data)->id;
                    value = CARD_KIND(card_id);
                    if ((value < CARD_TYPE_MAGIC || value == CARD_TYPE_TRAP) && obj->field_20.b.rotation_y == 0) {
                        obj->field_60 = 0x10;
                        gDuel_bSceneSubStep |= 0x20;
                        SD_SEPlayFull(0xB);
                    }
                    sprite = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
                    DisplayObject_ConfigureSpriteAtPosition(
                        sprite, (s16)obj->xy.h.x - 8, (s16)obj->xy.h.y + 0x1E,
                        3, 1, 2, 0xB, 0x20C);
                    DisplayObject_SelectOrderingTable1(sprite);
                    DisplayObject_SetDepthOffset(sprite, 0xA);
                    SPRITE_FLAGS(sprite) |= 0x28;
                    gDuel_pFlipLeftMarker = sprite;
                    sprite = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
                    DisplayObject_ConfigureSpriteAtPosition(
                        sprite, (s16)obj->xy.h.x + 0x3C, (s16)obj->xy.h.y + 0x1E,
                        3, 1, 0, 0xB, 0x20C);
                    DisplayObject_SelectOrderingTable1(sprite);
                    DisplayObject_SetDepthOffset(sprite, 0xA);
                    SPRITE_FLAGS(sprite) |= 0x28;
                    gDuel_pFlipRightMarker = sprite;
                    return;
                }
            } else if (gDuel_bSceneSubStep & 0x20) {
                obj->flags |= 4;
                obj->field_20.b.rotation_y += obj->field_60;
                if (!(obj->field_20.b.rotation_y & 0x7F)) {
                    if (obj->field_20.b.rotation_y == 0) {
                        obj->flags &= 0xFFFB;
                    }
                    gDuel_bSceneSubStep &= 0xDF;
                    if (gDuel_bSceneSubStep & 0x10) {
                    restore:
                        DisplayObject_ReleaseIfPresent(gDuel_pFlipLeftMarker);
                        DisplayObject_ReleaseIfPresent(gDuel_pFlipRightMarker);
                        gDuel_pFlipRightMarker = 0;
                        gDuel_pFlipLeftMarker = 0;
                        *(s32 *)&obj->position = *(s32 *)&obj->field_2C;
                        DisplayObject_SavePosition((void *)obj);
                        obj->field_60 = 0;
                        gDuel_bSceneSubStep |= 0x50;
                        return;
                    }
                }
            } else {
                if (gInput_awPadRepeat[0] & 0xA000) {
                    obj->field_60 = 0x10;
                    gDuel_bSceneSubStep |= 0x20;
                    if (gInput_awPadRepeat[0] & 0x2000) {
                        obj->field_60 = -0x10;
                    }
                    SD_SEPlayFull(0xB);
                    return;
                }
                if (gInput_awPadPressed[0] & HAND_ACTIONS_CANCEL_MASK) {
                    SD_SEPlayFull(8);
                    if (obj->field_20.b.rotation_y != 0) {
                        obj->field_60 = 0x10;
                        gDuel_bSceneSubStep |= 0x30;
                        return;
                    }
                    goto restore;
                }
                if (gInput_awPadPressed[0] & HAND_ACTIONS_CONFIRM_MASK) {
                    SD_SEPlayFull(7);
                    gDuel_aCardRecords[obj->field_6A].flags &= 0xEFFF;
                    if (obj->field_20.b.rotation_y != 0) {
                        gDuel_aCardRecords[obj->field_6A].flags |= 0x1000;
                    }
                    card_id = CARD_ID(obj->field_6A);
                    value = CARD_KIND(card_id);
                    if (value >= CARD_TYPE_MAGIC && obj->field_20.b.rotation_y == 0 &&
                        value != CARD_TYPE_EQUIP && value != CARD_TYPE_TRAP) {
                        DisplayObject_ReleaseIfPresent(gDuel_pFlipLeftMarker);
                        DisplayObject_ReleaseIfPresent(gDuel_pFlipRightMarker);
                        gDisplayObject_apWorkSlots.slots[0] = obj;
                        gDuel_pFlipRightMarker = 0;
                        gDuel_pFlipLeftMarker = 0;
                        gDuel_pActiveSideState->hand[(s8)side->hand_cursor_slot] = -1;
                        gDuel_bSceneSubStep = 5;
                        return;
                    }
                    gDuel_bSceneSubStep |= 0x70;
                    goto restore;
                }
            }
            break;
    case 4:
            if (!(gDuel_bSceneSubStep & 0x80)) {
                gDuel_bSceneSubStep |= 0x80;
                gDuel_wTrapPresentationMode = 0;
                Duel_BeginFieldPlacementSelection(side);
                gDuel_bSceneSubStep |= 0x40;
            }
            if (gDuel_bSceneSubStep & 0x40) {
                if (gDuel_wDisplayTransitionFlags == 0) {
                    gDuel_bSceneSubStep &= 0xBF;
                    if (gDuel_bSceneSubStep & 0x10) {
                        gDuel_bSceneSubStep = 1;
                        gDuel_pActiveSelectionRecord = SIDE_CURSOR(gDuel_abSelectionSides);
                        if (side->ordered_pick_count == 0) {
                            gDuel_bSceneSubStep = 3;
                            return;
                        }
                    }
                }
                break;
            }
            if (DuelCursor_UpdateFromInput(gDuel_pActiveSelectionRecord) != 0) {
                break;
            }
            value = Duel_GetCardViewerRequestId(
                &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices.flat[HAND_ACTIONS_VIEWER_CELL +
                                       gDuel_bActiveSide * 20]]);
            if (value != 0) {
                gDuel_bCardViewerYOffset = 0x14;
                gDuel_wViewerCardID = value;
                gDuel_bEffectState = 2;
                return;
            }
            if (gInput_awPadPressed[0] & HAND_ACTIONS_CANCEL_MASK) {
                gDuel_wDisplayTransitionFlags = 0xC;
                gDuel_bSceneSubStep |= 0x50;
                SD_SEPlayFull(8);
                return;
            }
            if (!(gInput_awPadPressed[0] & HAND_ACTIONS_CONFIRM_MASK)) {
                break;
            }
        }
        SD_SEPlayFull(7);
        for (n = 6; n >= 0; n--) {
            gDisplayObject_apWorkSlots.slots[n] = 0;
        }
        n = gDuel_pActiveSelectionRecord->row * 5 + gDuel_pActiveSelectionRecord->col;
        value = gDuel_abFieldGridCardRecordIndices.flat[n + gDuel_bActiveSide * 20];
        card = &gDuel_aCardRecords[value];
        gDuel_bCardMoveSlot = value;
        i = 0;
        if (card->flags & 0x8000) {
            obj = card->object;
            i = 1;
            gDisplayObject_apWorkSlots.slots[0] = (DisplayObject *)
                Duel_CreateCardDisplayObject(
                    card, card->screen_x, card->screen_y);
            DuelCard_DeactivateRecord(&gDuel_aCardRecords[obj->field_6A]);
        } else if (side->ordered_pick_count == 0 &&
                   hand->object->resource_branch_index ==
                       CARD_TYPE_EQUIP &&
                   gDuel_pActiveSelectionRecord->row == 2) {
            return;
        }
        if (side->ordered_pick_count == 0) {
            hand->selection_order = 1;
            if (hand->object->field_20.b.rotation_y != 0) {
                gDuel_pActiveSideState->rank.face_down_plays++;
            }
        }
        /* Collect the marked cards in pick order. A missing order
         * ends the walk: 8 is past the loop bound. */
        for (value = 1, first_slot = gDuelHand_aSlots; value < 6; value++) {
            s8 none = -1;

            do {
                n = 0;
                out = &gDisplayObject_apWorkSlots.slots[i];
                slot = first_slot;
                for (;;) {
                    if (slot->selection_order == value) {
                        gDuel_pActiveSideState->hand[n] = none;
                        i++;
                        *out = slot->object;
                        break;
                    }
                    n++;
                    slot++;
                    if (n >= 5) {
                        value = 8;
                        break;
                    }
                }
            } while (0);
        }
        gDuel_bSceneSubStep = 6;
        return;
    case 6:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_wDisplayTransitionFlags = 0xC;
            gDuel_bSceneSubStep |= 0x80;
            gDuel_pActiveSelectionRecord = SIDE_CURSOR(gDuel_abSelectionSides + 2 * DUEL_SELECTION_RECORD_SIZE);
        }
        if (gDuel_wDisplayTransitionFlags == 0) {
            gDuel_wSceneStateFlags = 7;
        }
        break;
    case 5:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            hand->object = 0;
            gDuel_wDisplayTransitionFlags = 8;
            gDuel_pActiveSelectionRecord = SIDE_CURSOR(gDuel_abSelectionSides);
        }
        if (gDuel_wDisplayTransitionFlags == 0) {
            gDuel_wSceneStateFlags = 6;
        }
        break;
    }
}

#undef SIDE_CURSOR
#undef HAND_CARD_INDEX
#undef FIELD_CURSOR
#undef CARD_RECORD
#undef CARD_ID
#undef CARD_KIND
#undef SPRITE_FLAGS

void DuelCard_UpdateDefenseRotation(DisplayObject *o)
{
    if (!DisplayObject_MarkInitialized((DisplayObjectLifecycle *)o)) {
        o->flags |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
        if (!o->field_20.b.rotation_z) {
            /* s16 view: retail materialises li -8; a u16 store gets li 0xfff8. */
            *(s16 *)&o->position.h.field_28 = -8;
            o->position.h.field_2A = 0xC0;
        } else {
            o->position.h.field_28 = 8;
            o->position.h.field_2A = 0;
        }
    }
    o->field_20.b.rotation_z += (s16)o->position.h.field_28;
    if (o->field_20.b.rotation_z < 0xC0) {
        DuelCardRecord *e = &gDuel_aCardRecords[o->field_6A];

        e->flags |= DUEL_CARD_FLAG_DEFENSE_POSITION;
        o->field_20.b.rotation_z = (s16)o->position.h.field_2A;
        if (!o->field_20.b.rotation_z) {
            e->flags &= ~DUEL_CARD_FLAG_DEFENSE_POSITION;
            if (!o->field_20.b.rotation_y) {
                o->flags &= ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
            }
        }
        o->field_6C = 0;
        o->update = 0;
    }
}

#include "display_object_layout.h"
#include "display_object.h"
#include "trig_constants.h"

#define DISPLAY_OBJECT_BYTES(object) ((u8 *)(object))
#define DISPLAY_OBJECT_COLOR_BYTES(object) ((u8 *)&(object)->rgb)
#define DISPLAY_OBJECT_SIGNED_HALFWORD(bytes, offset) \
    (*(s16 *)((bytes) + (offset)))

void DisplayObject_UpdateColorTransition(DisplayObject *object)
{
    s32 step = object->field_60;
    s32 remaining = 3;
    s32 i = 0;
    /* Byte cursors even though the object is typed: the three colour bytes
       of rgb step against the halfwords at 0x28, 0x2A and 0x2C, which
       are position.h and field_2C.h -- two different members, so no member
       array covers the walk. */
    u8 *current = DISPLAY_OBJECT_COLOR_BYTES(object);
    u8 *target = DISPLAY_OBJECT_BYTES(object);

    for (; i < 3; i++) {
        s32 value = *current;
        s32 limit = DISPLAY_OBJECT_SIGNED_HALFWORD(
            target, (u32)&((DisplayObject *)0)->position.h.field_28);

        if (value < limit) {
            value += step;
            if (value >= limit) {
                value = limit;
                remaining--;
            }
        } else {
            value -= step;
            if (value <= limit) {
                value = limit;
                remaining--;
            }
        }
        *current = value;
        current++;
        target += 2;
    }

    if (remaining == 0) {
        object->field_6C = 0;
        object->update = 0;
    }
}

void DisplayObject_UpdatePositionRotationTransition(DisplayObject*o)
{
 if(!DisplayObject_MarkInitialized((DisplayObjectLifecycle*)o)){if((s16)o->field_2C.h.field_2C!=o->field_20.b.rotation_y)o->flags|=DISPLAY_OBJECT_FLAG_CLIP_TEST;o->field_2C.h.field_2E=o->field_60;DisplayObject_SavePosition((DisplayObjectSnapshot*)o);o->field_60=0;}
 if(o->field_20.b.rotation_y!=(s16)o->field_2C.h.field_2C)o->field_20.b.rotation_y+=0x80/o->field_2C.h.field_2E;
 DisplayObject_InterpolatePositionCosine((DisplayObjectPosition *)o,(s16)o->position.h.field_28,(s16)o->position.h.field_2A,o->field_60);o->field_60+=TRIG_ANGLE_HALF_TURN/o->field_2C.h.field_2E;
 if(o->field_60>=TRIG_ANGLE_HALF_TURN){u8 target=(s16)o->field_2C.h.field_2C;int position=o->position.word;o->field_20.b.rotation_y=target;o->xy.word=position;if(!(target&0xFF))o->flags&=~DISPLAY_OBJECT_FLAG_CLIP_TEST;o->field_6C=0;o->update=0;}
}

#include "display_object_layout.h"
#include "display_object.h"
#include "display_projection.h"
#include "duel_grid.h"

DisplayObject *Duel_GetOrCreateActionReadyMarker(DisplayObject *source)
{
    DisplayObject *object = gDuel_pActionReadyMarker;
    if (object == 0) {
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteResource(object, 4, 3, 8, 0xB, 0x1F0);
        object->field_6A = source->pool_index;
        DisplayObject_SetDepthOffset(object, 1);
        object->update = (DisplayObjectCallback)DisplayObject_ProjectLinkedGroundPosition;
        object->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    }
    return object;
}

s32 DuelCursor_StepTowardTarget(DuelSelectionRecord *cursor)
{
    if (gDuel_wDisplayTransitionFlags != 0) { gDuelCursor_Direction = -1; return 1; }
    if (((DuelCursorUpdateDirectionWithDir)DuelCursor_UpdateDirection)(cursor, gDuelCursor_Direction) != 0) { return 1; }
    gDuelCursor_Direction = -1;
    if (gDuelCursor_TargetRow != cursor->row) {
        gDuelCursor_Direction = 1;
        if (gDuelCursor_TargetRow < cursor->row) { gDuelCursor_Direction = 3; }
        return 1;
    } else if (gDuelCursor_TargetColumn != cursor->col) {
        gDuelCursor_Direction = 0;
        if (gDuelCursor_TargetColumn < cursor->col) { gDuelCursor_Direction = 2; }
        return 1;
    } else {
        return 0;
    }
    return 1;
}

#include "duel_card.h"
#include "duel_card_staging.h"
#include "duel_grid.h"
#include "duel_card_viewer.h"
#include "ai.h"
#include "display_object.h"
#include "display_object_work_slots.h"
#include "input.h"
#include "sound.h"

/* The confirm buttons (Cross|Square). The Japanese release swaps Cross and
 * Circle, so a regional build names its own. */
#ifndef DUEL_FIELD_ACTIONS_CONFIRM_MASK
#define DUEL_FIELD_ACTIONS_CONFIRM_MASK 0xC0
#endif

/* The cancel button (Circle), swapped the same way. */
#ifndef DUEL_FIELD_ACTIONS_CANCEL_BUTTON
#define DUEL_FIELD_ACTIONS_CANCEL_BUTTON 0x20
#endif

/* European layout shifts both panel_target_y values by 0x10. */
#ifndef DUEL_FIELD_ACTIONS_INITIAL_PANEL_TARGET_Y
#define DUEL_FIELD_ACTIONS_INITIAL_PANEL_TARGET_Y 0xAE
#endif

#ifndef DUEL_FIELD_ACTIONS_TARGETING_PANEL_TARGET_Y
#define DUEL_FIELD_ACTIONS_TARGETING_PANEL_TARGET_Y 0x74
#endif

#define B(p, o) (*((u8 *)(p) + (o)))
#define H(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define W(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define SB(p, o) (*(s8 *)((u8 *)(p) + (o)))
#define SIDE_SIZE 4
/* Record 3 of the side's selection records: the same symbol as record 0. */
#define SEL_REC3 ((DuelSelectionRecord *)(gDuel_abSelectionSides + 0x54))
#define FIELD_CURSOR (((DuelSelectionSide *)gDuel_abSelectionSides)[gDuel_bActiveSide].records[2])
/* Grid card, two spellings: the [side][slot] form and the flat form. */
#define GRID_CARD(s) (&gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices.flat[(s)->row * 5 + (s)->col + gDuel_bActiveSide * 20]])
#define GRID_CARD_FLAT(s) \
    (&gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices.flat[(s)->row * 5 + (s)->col + gDuel_bActiveSide * 20]])
#define BANK(i, o) (big = 0x48000, H(pw + (i) * 0x1C + big, (o)))
#define CARD_KIND(id) ((gDuel_adwCardStats[(id) - 1] >> 0x1A) & 0x1F)

void DuelScene_UpdateFieldActions(void)
{
    DuelSelectionRecord *side;
    DuelSelectionRecord *rec;
    DisplayObject *o;
    DisplayObject *other;
    DuelCardRecord *card;
    DuelCardRecord *pick;
    DisplayObject *co;
    s32 a;
    s32 k;
    s32 n;
    s32 i;
    s32 x;
    s32 y;
    s32 t;
    s32 lvl;
    s32 r;
    s32 d;
    s32 big;
    u8 *pw;
    s32 idx;
    s8 row;
    u32 f;
    s32 flags;

    side = (DuelSelectionRecord *)(gDuel_abSelectionSides + 2 * DUEL_SELECTION_RECORD_SIZE) + gDuel_bActiveSide * SIDE_SIZE;
    if (!(gDuel_wSceneStateFlags & 0x8000)) {
        gDuel_wSceneStateFlags |= 0x8000;
        if (gDuel_aSideStates[0].life_points.signed_value == 0 ||
            gDuel_aSideStates[1].life_points.signed_value == 0) {
            gDuel_bWinnerSide = gDuel_bActiveSide;
            if (gDuel_aSideStates[gDuel_bActiveSide].life_points.signed_value == 0) {
                gDuel_bWinnerSide = gDuel_bActiveSide ^ 1;
            }
            gDuel_aSideStates[gDuel_bWinnerSide].rank.result_adjustment = 2;
            gDuel_wSceneStateFlags = 0xC;
            return;
        }
        gDuel_pActionReadyMarker = 0;
        gDuel_bFieldActionAutoCommit = 0;
        gDuel_pActiveSelectionRecord = side;
        side->row_end = 4;
        side->panel_target_y = DUEL_FIELD_ACTIONS_INITIAL_PANEL_TARGET_Y;
        y = 1;
        side->field_actions_mode = y;
        side->panel_variant = 0;
        side->row_min = 0;
        gDuel_wDisplayTransitionFlags = 3;
        gDuel_bSceneSubStep = y;
    }

    switch (gDuel_bSceneSubStep & 0xF) {
    case 1:
        if (gDuel_wDisplayTransitionFlags == 0) {
            gDuel_bSceneSubStep = 3;
        }
        return;
    case 2:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            Ai_RefreshBoardSnapshot();
            AiScript_Init(gAiScript_abFieldScript);
            gDuel_wActionSubstate = 0;
            return;
        }
        flags = gDuel_wActionSubstate;
        k = flags & 0xF;
        switch (k) {
        case 0:
            a = AiScript_Run();
            if (a == 0) {
                return;
            }
            if (a == 3) {
                gDuel_wOutcomeFlags &= 0xEFFF;
                DisplayObject_ReleaseIfPresent(gDuel_pActionReadyMarker);
                gDuel_pActionReadyMarker = 0;
                gDuel_bSceneSubStep = 0xA;
                SD_SEPlayFull(0x30);
                return;
            }
            gDuel_wActionSubstate = 1;
            return;
        case 1:
            if (!(flags & 0x8000)) {
                a = gAi_Selection.bytes[9];
                gDuel_wActionSubstate = flags | 0x8000;
                gDuelCursor_Direction = -1;
                gDuelCursor_TargetRow = 2;
                gDuelCursor_TargetColumn = (a - 1) % 5;
                if (a >= 6) {
                    gDuelCursor_TargetRow = 3;
                }
            }
            if (DuelCursor_StepTowardTarget(side) != 0) {
                return;
            }
            if (gAi_Selection.bytes[0xA] == 0 && gAi_Selection.bytes[0xB] == 0) {
                card = GRID_CARD(side);
                f = card->flags;
                if (f & 0x8000) {
                    card->flags = f | 0x4000;
                    Duel_UpdateCardDisplayState((DuelCardDisplayObject *)card->object);
                }
                gDuel_bSceneSubStep = 2;
                return;
            }
            gDuel_wActionSubstate = 3;
            if (gAi_Selection.bytes[0xB] != 0 && gAi_Selection.bytes[9] < 6) {
                gDuel_wActionSubstate = 2;
            }
            return;
        case 2:
            card = GRID_CARD(side);
            if (!(flags & 0x8000)) {
                f = card->flags;
                gDuel_wActionSubstate = flags | 0x8000;
                if (f & 0x8000) {
                    if (!(f & 0x800)) {
                        o = card->object;
                        o->update =
                            (DisplayObjectCallback)DuelCard_UpdateDefenseRotation;
                        o->field_6C = 0xF;
                        return;
                    }
                    card->flags = f | 0x4000;
                    Duel_UpdateCardDisplayState((DuelCardDisplayObject *)card->object);
                }
                gDuel_bSceneSubStep = 2;
                return;
            }
            if (DisplayObject_FindAllocatedByTag(0xF) == 0) {
                card->flags |= 0x4000;
                Duel_UpdateCardDisplayState((DuelCardDisplayObject *)card->object);
                gDuel_bSceneSubStep = k;
            }
            return;
        case 3:
            if (!(flags & 0x8000)) {
                gDuel_wActionSubstate = flags | 0x8000;
                card = GRID_CARD(side);
                if (card->flags & 0x800) {
                    gDuel_wActionSubstate = flags | 0xC000;
                    o = card->object;
                    o->update =
                        (DisplayObjectCallback)DuelCard_UpdateDefenseRotation;
                    o->field_6C = 0xF;
                }
                return;
            }
            if (!(flags & 0x4000) || DisplayObject_FindAllocatedByTag(0xF) == 0) {
                gDuel_bFieldActionAutoCommit = 1;
                break;
            }
            return;
        default:
            return;
        }
        /* Case 3's input handling sits in an if (0) block, so the AI
           auto-commit break above skips it straight into the attack. */
        if (0) {
    case 3:
            if (!(gDuel_bSceneSubStep & 0x80)) {
                gDuel_bSceneSubStep |= 0xC0;
                gDuel_bFieldActionAutoCommit = 0;
                if (gDuel_bSide0ControllerSelector[gDuel_bActiveSide] >= 0) {
                    gDuel_bSceneSubStep = 2;
                    return;
                }
                side->status = 0;
            }
            if (DisplayObject_FindAllocatedByTag(0xF) != 0) {
                return;
            }
            if (DuelCursor_UpdateFromInput(side) != 0) {
                gDuel_bSceneSubStep |= 0x40;
                return;
            }
            if (gDuel_bSceneSubStep & 0x40) {
                gDuel_bSceneSubStep &= 0xBF;
                if (side->row == 2
                    && DuelCard_CanActThisTurn(GRID_CARD(side)) != 0) {
                    gDuel_pActionReadyMarker = Duel_GetOrCreateActionReadyMarker(side->cursor_object);
                } else {
                    DisplayObject_ReleaseIfPresent(gDuel_pActionReadyMarker);
                    gDuel_pActionReadyMarker = 0;
                }
            }
            if (gDuel_bSide0ControllerSelector[gDuel_bActiveSide] < 0 && (gInput_awPadPressed[0] & 0x800)) {
                gDuel_bSceneSubStep = 0xB;
                return;
            }
            if (Duel_CheckQuitInput() != 0) {
                return;
            }
            if (gInput_awPadPressed[0] & 0xC) {
                r = side->row;
                card = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices.flat[r * 5 + side->col + gDuel_bActiveSide * 20]];
                if (r == 2 && DuelCard_CanActThisTurn(card) != 0) {
                    o = card->object;
                    o->update =
                        (DisplayObjectCallback)DuelCard_UpdateDefenseRotation;
                    o->field_6C = 0xF;
                    SD_SEPlayFull(0xB);
                }
                return;
            }
            a = Duel_GetCardViewerRequestId(GRID_CARD(side));
            if (a != 0) {
                gDuel_bCardViewerYOffset = 0x14;
                gDuel_wViewerCardID = a;
                gDuel_bEffectState = 2;
                return;
            }
            if (!(gInput_awPadPressed[0] & DUEL_FIELD_ACTIONS_CONFIRM_MASK)) {
                return;
            }
            if (side->row == 2) {
                if (gDuel_pActiveSideState->swords_turns_remaining != 0) {
                    SD_SEPlayFull(9);
                    return;
                }
                if (gDuel_wOutcomeFlags & 0x1000) {
                    SD_SEPlayFull(9);
                    return;
                }
            }
            if (side->row < 2) {
                SD_SEPlayFull(9);
                return;
            }
        }
        i = side->row * 5 + side->col;
        card = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices.flat[i + gDuel_bActiveSide * 20]];
        if ((W(card, 0x14) & 0xC8000000) == 0x80000000) {
            SD_SEPlayFull(7);
            DisplayObject_ReleaseIfPresent(gDuel_pActionReadyMarker);
            gDuel_pActionReadyMarker = 0;
            a = side->row;
            if (a >= 2) {
                side = SEL_REC3 + gDuel_bActiveSide * SIDE_SIZE;
                if (a >= 3) {
                    gDuel_bSceneSubStep = 7;
                    if (CARD_KIND(card->card_id) == 0x17) {
                        x = 2;
                        y = 3;
                        side->row_min = x;
                        side->row = x;
                        side->row_end = y;
                        x = 4;
                        gDuel_bSceneSubStep = x;
                    }
                } else {
                    x = 4;
                    y = 1;
                    gDuel_bSceneSubStep = x;
                    side->row_min = y;
                    x = 2;
                    side->row_end = x;
                    side->row = y;
                }
            }
        } else {
            SD_SEPlayFull(9);
        }
        return;
    case 4:
        side = SEL_REC3 + gDuel_bActiveSide * SIDE_SIZE;
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            gDuel_pActiveSelectionRecord = side;
            side->field_actions_mode = 0;
            side->panel_target_y = DUEL_FIELD_ACTIONS_TARGETING_PANEL_TARGET_Y;
            Duel_CreateFieldCursorObject(side);
            Duel_StartViewTween(0x10, 0x14E, 0x3FE, gDuel_awSideViewHeadings[gDuel_bActiveSide],
                gDuel_awFieldGridRowViewTargetZ[gDuel_bActiveSide][side->field_actions_mode][side->row]);
            o = side->cursor_object;
            other = *(DisplayObject **)((u8 *)SEL_REC3 + gDuel_bActiveSide * 0x70 - 0x18);
            DisplayObject_ResetVelocity((void *)o);
            o->field_60 = 0x10;
            o->field_6C = 0;
            W(o, 0x2C) = W(o, 0x28);
            o->field_34.h.field_36 =
                ((S(o, 0x28) - S(other, 0x28)) << 8) / 16;
            o->field_38.h.field_3A =
                ((S(o, 0x2A) - S(other, 0x2A)) << 8) / 16;
            gDuel_wDisplayTransitionFlags = 0x52;
            o->position.word =
                other->position.word;
            return;
        }
        if (gDuel_wDisplayTransitionFlags == 0) {
            DuelField_RefreshCursorCardInfo(side);
            gDuel_bSceneSubStep = 6;
            if (gDuel_bFieldActionAutoCommit != 0) {
                gDuel_bSceneSubStep = 5;
            }
        }
        return;
    case 5:
        side = SEL_REC3 + gDuel_bActiveSide * SIDE_SIZE;
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            gDuel_bAnimatedBattleRequested = 0;
            a = gAi_Selection.bytes[10];
            if (a < 0x38) {
                if (a >= 6) {
                    gDuelCursor_TargetColumn = a - 6;
                    x = 3;
                } else {
                    gDuelCursor_TargetColumn = a - 1;
                    x = 2;
                }
            } else {
                gDuelCursor_TargetColumn = 0x3C - a;
                x = 1;
            }
            gDuelCursor_TargetRow = x;
        }
        if (DuelCursor_StepTowardTarget(side) != 0) {
            return;
        }
        if (0) {
    case 6:
            side = SEL_REC3 + gDuel_bActiveSide * SIDE_SIZE;
            if (gDuel_bSceneSubStep & 0x80) {
                gDuel_bSceneSubStep |= 0x80;
                gDuel_pActiveSelectionRecord = side;
            }
            if (gDuel_bSceneSubStep & 0x40) {
                if (gDuel_wDisplayTransitionFlags == 0) {
                    DisplayObject_ReleaseIfPresent(side->cursor_object);
                    side->cursor_object = 0;
                    gDuel_bSceneSubStep = 3;
                    gDuel_pActiveSelectionRecord =
                        &((DuelSelectionSide *)gDuel_abSelectionSides)[gDuel_bActiveSide].records[2];
                }
                return;
            }
            if (DuelCursor_UpdateFromInput(side) != 0) {
                return;
            }
            if (gInput_awPadPressed[0] & DUEL_FIELD_ACTIONS_CANCEL_BUTTON) {
                Duel_StartViewTween(0x10, 0x14E, 0x3FE, gDuel_awSideViewHeadings[gDuel_bActiveSide],
                    gDuel_awFieldGridRowViewTargetZ[gDuel_bActiveSide][FIELD_CURSOR.field_actions_mode][FIELD_CURSOR.row]);
                o = side->cursor_object;
                other = FIELD_CURSOR.cursor_object;
                DisplayObject_ResetVelocity((void *)o);
                o->field_60 = 0x10;
                o->field_6C = 0;
                o->field_34.h.field_36 =
                    ((S(other, 0x28) - S(o, 0x28)) << 8) / 16;
                o->field_38.h.field_3A =
                    ((S(other, 0x2A) - S(o, 0x2A)) << 8) / 16;
                W(o, 0x2C) = W(other, 0x28);
                gDuel_wDisplayTransitionFlags = 0x58;
                gDuel_bSceneSubStep |= 0x40;
                return;
            }
            a = Duel_GetCardViewerRequestId(GRID_CARD(side));
            if (a != 0) {
                gDuel_bCardViewerYOffset = 0x14;
                gDuel_wViewerCardID = a;
                gDuel_bEffectState = 2;
                return;
            }
            if (!(gInput_awPadPressed[0] & DUEL_FIELD_ACTIONS_CONFIRM_MASK)) {
                return;
            }
            gDuel_bAnimatedBattleRequested = 0;
            if (gInput_awPadPressed[0] & 0x80) {
                gDuel_bAnimatedBattleRequested = 1;
            }
        }
        a = 0;
        if (side->row == 1) {
            i = 5;
            a = 1;
            for (n = 0; n < 5; n++, i++) {
                pick = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices.flat[i + gDuel_bActiveSide * 20]];
                if (pick->flags & 0x8000) {
                    a = 0;
                    break;
                }
            }
        }
        i = side->row * 5 + side->col;
        pick = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices.flat[i + gDuel_bActiveSide * 20]];
        if (a == 0 && !(pick->flags & 0x8000)) {
            SD_SEPlayFull(9);
            return;
        }
        SD_SEPlayFull(7);
        for (n = 6; n >= 0; n--) {
            gDisplayObject_apWorkSlots.slots[n] = 0;
        }
        i = FIELD_CURSOR.row * 5 + FIELD_CURSOR.col;
        pick = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices.flat[i + gDuel_bActiveSide * 20]];
        gDuel_awSavedRecordFlags[0] = pick->flags;
        gDuel_awSavedStatModifiers[0] = H(pick, 0x12);
        o = (DisplayObject *)Duel_CreateCardDisplayObject(pick, S(pick, 8), S(pick, 0xA));
        DisplayObject_SetDepthOffset(o, -0xA);
        gDisplayObject_apWorkSlots.slots[0] = o;
        d = pick->card_id;
        DuelCard_DeactivateRecord(pick);
        i = side->row * 5 + side->col;
        pick = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices.flat[i + gDuel_bActiveSide * 20]];
        if (a == 0) {
            gDuel_awSavedRecordFlags[1] = pick->flags;
            gDuel_awSavedStatModifiers[1] = H(pick, 0x12);
            o = (DisplayObject *)Duel_CreateCardDisplayObject(pick, S(pick, 8), S(pick, 0xA));
            gDisplayObject_apWorkSlots.slots[1] = o;
            gDuel_bCardMoveSlot = o->field_6A;
            DuelCard_DeactivateRecord(pick);
            if (CARD_KIND(d) == 0x17) {
                gDuel_bSceneSubStep = 8;
                return;
            }
        }
        gDuel_wSceneStateFlags = 9;
        return;
    case 7:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            i = side->row * 5 + side->col;
            card = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices.flat[i + gDuel_bActiveSide * 20]];
            o = (DisplayObject *)Duel_CreateCardDisplayObject(card, card->screen_x, card->screen_y);
            card->flags &= 0x7FFF;
            if (card->flags & 0x1000) {
                o->field_20.b.rotation_y = 0x80;
                H(o, 8) |= 4;
            }
            o->field_60 = 0xC;
            o->update =
                (DisplayObjectCallback)DisplayObject_UpdatePositionRotationTransition;
            o->position.h.field_28 = 0x86;
            o->position.h.field_2A = 0x5A;
            o->field_2C.h.field_2C = 0;
            o->field_6C = 1;
            gDuel_pActionCard = o;
            co = side->cursor_object;
            co->flags &= 0xFFBF;
            if (gDuel_bFieldActionAutoCommit == 0) {
                gDuel_dwFieldDimTarget = 0x202020;
                card = gDuel_aCardRecords;
                for (n = 0; n < 0x1E; card++, n++) {
                    if (card->flags & 0x8000) {
                        o = card->object;
                        o->field_6C = 1;
                        o->field_60 = 8;
                        o->update =
                            (DisplayObjectCallback)DisplayObject_UpdateColorTransition;
                        a = 0x20;
                        if (card->flags & 0x4000) {
                            a = 0x10;
                        }
                        o->field_2C.h.field_2C = a;
                        o->position.h.field_2A = a;
                        o->position.h.field_28 = a;
                    }
                }
                gDuel_bConfirmFlags = 0;
                gDuel_bSceneSubStep |= 0x20;
            }
            return;
        }
        if (gDuel_bFieldActionAutoCommit != 0) {
            if (DisplayObject_FindAllocatedByTag(1) != 0) {
                return;
            }
            DuelCard_RemoveFromField(&gDuel_aCardRecords[B(gDuel_pActionCard, 0x6A)]);
            gDuel_bSceneSubStep = 9;
            gDisplayObject_apWorkSlots.slots[0] = gDuel_pActionCard;
            return;
        }
        if (gDuel_bSceneSubStep & 0x20) {
            a = (u8)gModel_dwFogBaseColor;
            flags = (u8)gDuel_dwFieldDimTarget;
            if (a >= flags) {
                a -= 8;
                t = a < flags;
            } else {
                a += 8;
                t = flags < a;
            }
            if (t != 0) {
                a = flags;
            }
            t = (a << 16) | (a << 8) | a;
            gModel_dwFogBaseColor = t;
            if (t == gDuel_dwFieldDimTarget && DisplayObject_FindAllocatedByTag(1) == 0) {
                gDuel_bSceneSubStep &= 0xDF;
            }
            return;
        }
        if (gDuel_bSceneSubStep & 0x40) {
            if (gDuel_bSceneSubStep & 0x10) {
                gDuel_aCardRecords[B(gDuel_pActionCard, 0x6A)].flags |= 0x8000;
                DisplayObject_ReleaseIfPresent(gDuel_pActionCard);
                gDuel_bSceneSubStep = 3;
                co = side->cursor_object;
                co->flags |= 0x40;
                return;
            }
            DuelCard_RemoveFromField(&gDuel_aCardRecords[B(gDuel_pActionCard, 0x6A)]);
            gDuel_bSceneSubStep = 9;
            gDisplayObject_apWorkSlots.slots[0] = gDuel_pActionCard;
            return;
        }
        a = Duel_UpdateCardUseConfirmation();
        if (a == 0) {
            return;
        }
        if (a != 1) {
            gDuel_bSceneSubStep |= 0x10;
            SD_SEPlayFull(8);
            o = gDuel_pActionCard;
            o->field_60 = 0xC;
            pw = gDuel_abCardStagingBuffer;
            o->update =
                (DisplayObjectCallback)DisplayObject_UpdatePositionRotationTransition;
            o->position.h.field_28 =
                BANK(o->field_6A, 0x36BC);
            o->position.h.field_2A =
                BANK(o->field_6A, 0x36BE);
            o->field_2C.h.field_2C = 0;
            if (gDuel_aCardRecords[o->field_6A].flags
                & 0x1000) {
                o->field_2C.h.field_2C = 0x80;
            }
            o->field_6C = 1;
        } else {
            SD_SEPlayFull(7);
        }
        gDuel_dwFieldDimTarget = 0x808080;
        card = gDuel_aCardRecords;
        gDuel_bSceneSubStep |= 0x60;
        for (n = 0; n < 0x1E; card++, n++) {
            if (card->flags & 0x8000) {
                o = card->object;
                o->field_6C = 1;
                o->field_60 = 8;
                o->update =
                    (DisplayObjectCallback)DisplayObject_UpdateColorTransition;
                a = 0x80;
                if (card->flags & 0x4000) {
                    a = 0x40;
                }
                o->field_2C.h.field_2C = a;
                o->position.h.field_2A = a;
                o->position.h.field_28 = a;
            }
        }
        return;
    case 8:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            rec = SEL_REC3 + gDuel_bActiveSide * SIDE_SIZE;
            co = rec->cursor_object;
            gDuel_bSceneSubStep |= 0x80;
            gDuel_pActiveSelectionRecord = rec;
            DisplayObject_ReleaseIfPresent(co);
            gDuel_wDisplayTransitionFlags = 8;
            gDuel_pActiveSelectionRecord->cursor_object = 0;
        }
        if (gDuel_wDisplayTransitionFlags != 0) {
            return;
        }
        if (!(gDuel_bSceneSubStep & 0x40)) {
            gDuel_bSceneSubStep |= 0x40;
            gDuel_wDisplayTransitionFlags = 0xC;
            gDuel_pActiveSelectionRecord =
                &((DuelSelectionSide *)gDuel_abSelectionSides)[gDuel_bActiveSide].records[2];
            return;
        }
        gDuel_wSceneStateFlags = 7;
        return;
    case 9:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            gDuel_wDisplayTransitionFlags = 0xC;
        }
        if (gDuel_wDisplayTransitionFlags == 0) {
            gDuel_wSceneStateFlags = 6;
        }
        return;
    case 11:
        gDuel_wOutcomeFlags &= 0xEFFF;
        DisplayObject_ReleaseIfPresent(gDuel_pActionReadyMarker);
        gDuel_pActionReadyMarker = 0;
        gDuel_bSceneSubStep = 0xA;
        SD_SEPlayFull(0x30);
        return;
    case 10:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            gDuel_wDisplayTransitionFlags = 0xC;
        }
        if (gDuel_wDisplayTransitionFlags == 0) {
            gDuel_wSceneStateFlags = 0xA;
        }
        return;
    }
}

#include "display_object.h"
#include "display_object_layout.h"
#include "trig_constants.h"

void DisplayObject_UpdateCosineSlide(DisplayObject *object)
{
    if (!DisplayObject_MarkInitialized((DisplayObjectLifecycle *)object)) {
        DisplayObject_SavePosition((DisplayObjectSnapshot *)object);
        object->field_60 = 0;
        object->field_2C.h.field_2E = 0;
    }
    DisplayObject_InterpolatePositionCosine((DisplayObjectPosition *)object, (s16)object->position.h.field_28,
                 (s16)object->position.h.field_2A, object->field_60);
    object->field_60 +=
        TRIG_ANGLE_HALF_TURN / (s16)object->field_2C.h.field_2C;
    if (object->field_60 >= TRIG_ANGLE_HALF_TURN) {
        object->field_6C = 0;
        object->update = 0;
    }
}

void DisplayObject_UpdateCosineSlideWithRollReset(DisplayObject *object)
{
    if (!DisplayObject_MarkInitialized((DisplayObjectLifecycle *)object)) {
        DisplayObject_SavePosition((DisplayObjectSnapshot *)object);
        object->field_60 = 0;
        object->field_2C.h.field_2E = 0;
    }
    if (object->field_20.b.rotation_z) {
        object->field_20.b.rotation_z += 0x40 / (s16)object->field_2C.h.field_2C;
    }
    DisplayObject_InterpolatePositionCosine((DisplayObjectPosition *)object, (s16)object->position.h.field_28,
                 (s16)object->position.h.field_2A, object->field_60);
    object->field_60 +=
        TRIG_ANGLE_HALF_TURN / (s16)object->field_2C.h.field_2C;
    if (object->field_60 >= TRIG_ANGLE_HALF_TURN) {
        object->xy.word = object->position.word;
        object->field_20.b.rotation_z = 0;
        if (!object->field_20.b.rotation_y) {
            object->flags &= ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
        }
        object->field_6C = 0;
        object->update = 0;
    }
}

#include "card_constants.h"
#include "display_object.h"
#include "duel_card_stat_rules.h"
#include "duel_card.h"
#include "duel_card_staging.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "sound.h"

/* Battle arithmetic between two duel cards, built up in address order.

   Duel_CalcGuardianStarBonus reads each card's active guardian star from
   gDuel_adwCardStats and asks the wheel for the signed bonus. The attack and
   defence calculators add that bonus to the card's own stat and clamp it.
   Duel_CompareBattleStrength then picks attack or defence by each card's position and
   compares the result against the opponent's stat, for the AI's field scans.

   These were four sources at two profiles, gcc_2_8_1_g8 for the calculators
   and gcc_2_8_1_g8_split for the bonus and the comparison. The calculators
   compile to identical objects at gcc_2_8_1_g8_split, so the unit builds
   there. It is bounded below by the display-object motion pair and above by
   the attack-trap selector, which is a different mechanism. */

s32 Duel_CalcGuardianStarBonus(DuelCardRecord *left, DuelCardRecord *right)
{
    int left_value, right_value;
    if (right == 0) return 0;
    if (left->flags & DUEL_CARD_FLAG_USE_GUARDIAN_STAR_2)
        left_value =
            (gDuel_adwCardStats[left->card_id - 1] >>
             CARD_STAT_GUARDIAN_STAR_2_SHIFT) &
            CARD_STAT_GUARDIAN_STAR_MASK;
    else
        left_value =
            (gDuel_adwCardStats[left->card_id - 1] >>
             CARD_STAT_GUARDIAN_STAR_1_SHIFT) &
            CARD_STAT_GUARDIAN_STAR_MASK;
    if (right->flags & DUEL_CARD_FLAG_USE_GUARDIAN_STAR_2)
        right_value =
            (gDuel_adwCardStats[right->card_id - 1] >>
             CARD_STAT_GUARDIAN_STAR_2_SHIFT) &
            CARD_STAT_GUARDIAN_STAR_MASK;
    else
        right_value =
            (gDuel_adwCardStats[right->card_id - 1] >>
             CARD_STAT_GUARDIAN_STAR_1_SHIFT) &
            CARD_STAT_GUARDIAN_STAR_MASK;
    return Duel_CalcGuardianStarMatchup(left_value, right_value);
}

s32 Duel_CalcBattleAttack(DuelCardRecord *card, DuelCardRecord *opponent)
{
    s32 value = (u16)Duel_CalcCardStats(card)
        + Duel_CalcGuardianStarBonus(card, opponent);

    if (value > CARD_STAT_MAX) {
        value = CARD_STAT_MAX;
    }
    return value;
}

s32 Duel_CalcBattleDefense(DuelCardRecord *card, DuelCardRecord *opponent)
{
    s32 value = (Duel_CalcCardStats(card) >> 16)
        + Duel_CalcGuardianStarBonus(card, opponent);

    if (value > CARD_STAT_MAX) {
        value = CARD_STAT_MAX;
    }
    return value;
}

s32 Duel_CompareBattleStrength(DisplayObject *left, DisplayObject *right)
{
    DuelCardRecord *a = &gDuel_aCardRecords[left->field_6A];
    DuelCardRecord *b;
    s32 packed;
    s32 wanted;
    s32 actual;

    if (right == 0)
        return Duel_CalcCardStats(a) & 65535;
    b = &gDuel_aCardRecords[right->field_6A];
    packed = Duel_CalcCardStats(b);
    wanted = packed & 65535;
    if (b->flags & DUEL_CARD_FLAG_DEFENSE_POSITION)
        wanted = (u32)packed >> 16;
    if (a->flags & DUEL_CARD_FLAG_DEFENSE_POSITION)
        actual = Duel_CalcBattleDefense(a, b);
    else
        actual = Duel_CalcBattleAttack(a, b);
    if (actual != wanted)
        return actual - wanted;
    if ((a->flags & DUEL_CARD_FLAG_DEFENSE_POSITION) ||
        (b->flags & DUEL_CARD_FLAG_DEFENSE_POSITION))
        return 0;
    return -1;
}

#define DUEL_ATTACK_TRAP_SCRATCH_VIEW(value) \
    ((DuelAttackTrapScratchBlock *)(value))

/* Small data at 0x8009AF24, owned here: the attack threshold of each trap
   from House of Adhesive Tape through Widespread Ruin, which
   Duel_SelectAttackTrap
   scales by 100 before comparing. The last entry is the 255 that keeps
   Widespread Ruin unconditional. */
u8 gDuel_abTrapAttackThresholds[DUEL_ATTACK_TRAP_COUNT] = {
    DUEL_HOUSE_OF_ADHESIVE_TAPE_ATTACK_THRESHOLD /
        DUEL_ATTACK_TRAP_THRESHOLD_SCALE,
    DUEL_EATGABOON_ATTACK_THRESHOLD / DUEL_ATTACK_TRAP_THRESHOLD_SCALE,
    DUEL_BEAR_TRAP_ATTACK_THRESHOLD / DUEL_ATTACK_TRAP_THRESHOLD_SCALE,
    DUEL_INVISIBLE_WIRE_ATTACK_THRESHOLD / DUEL_ATTACK_TRAP_THRESHOLD_SCALE,
    DUEL_ACID_TRAP_HOLE_ATTACK_THRESHOLD / DUEL_ATTACK_TRAP_THRESHOLD_SCALE,
    DUEL_WIDESPREAD_RUIN_ATTACK_THRESHOLD / DUEL_ATTACK_TRAP_THRESHOLD_SCALE,
};

s32 Duel_SelectAttackTrap(const DisplayObject *p) {
    s32 i;
    s32 off1;
    u8 *b1;
    s32 n;
    u8 *tbl2;
    u8 *rec2;
    u8 *b2;
    s32 off2;
    s32 h2;
    DuelCardRecord *e;
    s32 id;
    s32 sx;
    s32 sx2;
    s32 th;
    s32 sel;
    s32 off3;
    u8 *b3;
    u8 *q;
    u8 *tb;
    s32 v;
    DisplayObject *w;
    s32 off4;
    u8 *b4;
    s32 j2;
    u8 *tbl3;
    u8 *rec3;
    s32 h3;
    s32 k;

    i = 0;
    /* Keep the unrebased byte cursors and the record-index word-slot bias. */
    off1 = DUEL_CARD_STAGING_TRAP_BASE_OFFSET;
    b1 = gDuel_abCardStagingBuffer;
    for (; i < DUEL_ATTACK_TRAP_COUNT; i++) {
        DUEL_ATTACK_TRAP_SCRATCH_VIEW(
            b1 + i * sizeof(u16) + off1)->table.entries.card_ids[0] = 0;
    }
    n = 0;
    i = n;
    tbl2 = gDuel_abFieldGridCardRecordIndices.flat;
    rec2 = (u8 *)gDuel_aCardRecords;
    b2 = gDuel_abCardStagingBuffer;
    off2 = DUEL_CARD_STAGING_TRAP_BASE_OFFSET;
    h2 = gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
    for (; i < DUEL_FIELD_ROW_SIZE; i++) {
        /* Integer address arithmetic keeps retail's addu operand order here
           and below; u8 pointer arithmetic swaps it. */
        e = (DuelCardRecord *)(*(u8 *)(i + h2 + (s32)tbl2) *
            DUEL_CARD_RECORD_SIZE + (s32)rec2);
        if ((e->flags & DUEL_CARD_FLAG_OCCUPIED) != 0) {
            id = (u16)e->card_id;
            if ((u32)(id - DUEL_ATTACK_TRAP_FIRST_CARD_ID) <
                DUEL_ATTACK_TRAP_COUNT) {
                sx = (s16)id;
                th = sx - DUEL_ATTACK_TRAP_FIRST_CARD_ID;
                n++;
                DUEL_ATTACK_TRAP_SCRATCH_VIEW(
                    b2 + th * sizeof(u16) + off2)->table.entries.card_ids[0] = id;
                sx2 = sx - (DUEL_ATTACK_TRAP_FIRST_CARD_ID -
                    DUEL_CARD_STAGING_TRAP_RECORD_INDEX_BIAS);
                DUEL_ATTACK_TRAP_SCRATCH_VIEW(
                    b2 + sx2 * sizeof(u16) + off2)->table.words[0] =
                    ((DisplayObject *)e->object)->field_6A;
            }
        }
    }
    if (n != 0) {
        do {
            th = Duel_CalcCardStats(
                &gDuel_aCardRecords[p->field_6A]
            ) & 0xFFFF;
        } while (0);
        sel = -1;
        off3 = DUEL_CARD_STAGING_TRAP_BASE_OFFSET;
        b3 = gDuel_abCardStagingBuffer;
        q = b3 + 0xA;
        i = DUEL_ATTACK_TRAP_COUNT - 1;
        tb = gDuel_abTrapAttackThresholds;
        do {
            if (DUEL_ATTACK_TRAP_SCRATCH_VIEW(q + off3)->table.entries.card_ids[0] != 0) {
                v = *(u8 *)(i + (s32)tb);
                if (v * DUEL_ATTACK_TRAP_THRESHOLD_SCALE < th) {
                    break;
                }
                sel = i;
            }
            i--;
            q -= 2;
        } while (i >= 0);
        if (sel >= 0) {
            off4 = DUEL_CARD_STAGING_TRAP_BASE_OFFSET;
            gDuel_wSelectedTrapCardId = sel + DUEL_ATTACK_TRAP_FIRST_CARD_ID;
            b4 = gDuel_abCardStagingBuffer;
            j2 = sel + DUEL_CARD_STAGING_TRAP_RECORD_INDEX_BIAS;
            gDuel_bSelectedTrapSlot =
                *(u8 *)&DUEL_ATTACK_TRAP_SCRATCH_VIEW(
                    b4 + j2 * sizeof(u16) + off4)->table.words[0];
            return 1;
        }
    }
    i = 0;
    tbl3 = gDuel_abFieldGridCardRecordIndices.flat;
    rec3 = (u8 *)gDuel_aCardRecords;
    h3 = gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
    k = DUEL_FAKE_TRAP_CARD_ID;
    for (; i < DUEL_FIELD_ROW_SIZE; i++) {
        e = (DuelCardRecord *)(*(u8 *)(i + h3 + (s32)tbl3) *
            DUEL_CARD_RECORD_SIZE + (s32)rec3);
        if ((e->flags & DUEL_CARD_FLAG_OCCUPIED) != 0) {
            v = e->card_id;
            if (v == k) {
                /* loop.c moves this exit back after the threshold pass's
                   `return 1`, where retail keeps it. */
                w = e->object;
                gDuel_wSelectedTrapCardId = v;
                gDuel_bSelectedTrapSlot = w->field_6A;
                return 1;
            }
        }
    }
    return 0;
}

/* Four-state presentation sequencer on the gDuel_wTrapPresentationMode mode byte: mode 0
 * starts the first screen effect and arms the 0x14-frame counter; mode 1
 * copies the selected card's position into a type-8 request, updates
 * the card record and plays the SE when that counter expires; mode 2 starts
 * the second screen effect; mode 3 waits once more, advances the opposing
 * side's state byte at +6 and completes. Returns 1 while busy. */
s32 Duel_UpdateTrapPresentation(void) {
    DuelEffectRequest *e;
    DuelCardReplayRecordBlock *g;
    DisplayObject *p;
    DuelSideState *q;
    u8 *r;
    s32 one;
    s32 v;
    u16 t;
    u16 *q34;
    u16 *d;

    if (gDuel_wDisplayTransitionFlags != 0) {
        return 1;
    }

    one = 1;
    v = gDuel_wTrapPresentationMode & 0xF;

    /* The default arm comes first so the dispatch keeps retail's two
       separate returns. The do-while(0) spans modes 0-2 (entered at its
       case labels) to steer the delay-slot scheduling, as retail does. */
    switch (v) {
    default:
        return 1;
    case 0:
        Duel_StartViewTween(0x10, 0x208, 0x200, gViewState_Current.angle,
                            0xB2 - gDuel_bActiveSide * 0x164);
        gDuel_wDisplayTransitionFlags = 0x10;
        gDuel_wTrapPresentationMode = one;
        gDuel_wStepCounter = 0x14;
        do {
            return 1;

    case 1:
            t = gDuel_wStepCounter - 1;
            gDuel_wStepCounter = t;
            if ((s16)t <= 0) {
                r = gDuel_abCardStagingBuffer;
                g = (DuelCardReplayRecordBlock *)(
                    (u8 *)&((DuelCardRecord *)r)[gDuel_bSelectedTrapSlot] +
                    DUEL_CARD_STAGING_REPLAY_BASE_OFFSET);
                p = (DisplayObject *)g->record.object;
                e = DuelEffect_CreateRequest(8);
                e->field_00 = (s16)p->xy.h.x;
                e->field_02 = (s16)p->xy.h.y;
                q34 = (u16 *)&p->field_34.h.z;
                *(d = (u16 *)&e->field_04) = *q34;
                DuelCard_RemoveFromField(&gDuel_aCardRecords[p->field_6A]);
                SD_SEPlayFull(0x17);
                gDuel_wTrapPresentationMode = 2;
            }
            return 1;

    case 2:
            Duel_StartViewTween(0x10, 0x258, 0x100,
                                gViewState_Current.angle, 0);
            gDuel_wDisplayTransitionFlags = 0x10;
            gDuel_wTrapPresentationMode = 3;
            gDuel_wStepCounter = 0x14;
        } while (0);
        return 1;

    case 3:
        t = gDuel_wStepCounter - 1;
        gDuel_wStepCounter = t;
        if ((s16)t > 0) {
            return 1;
        }
        q = &gDuel_aSideStates[gDuel_bActiveSide ^ 1];
        q->rank.traps_triggered = q->rank.traps_triggered + 1;
        return 0;
    }
}

#include "../psyq/rand.h"
#include "file_transfer.h"
#include "duel_card_staging.h"
#include "duel_card.h"
#include "display_object.h"
#include "display_object_work_slots.h"
#include "fade.h"
#include "main_mode_state.h"
#include "sound.h"
#include "main_modes.h"
#include "sound_effect_request.h"
#include "model_scene_states.h"
#include "duel_session_controls.h"
#include "duel_grid.h"

#define H(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define B(p, o) (*((u8 *)(p) + (o)))

/* Regional battle layouts override these two vertical coordinates. */
#ifndef DUEL_BATTLE_STAT_DISPLAY_Y
#define DUEL_BATTLE_STAT_DISPLAY_Y 0x16
#endif

#ifndef DUEL_BATTLE_EFFECT_REQUEST_Y_OFFSET
#define DUEL_BATTLE_EFFECT_REQUEST_Y_OFFSET 0x62
#endif

void DuelScene_UpdateBattle(void)
{
    DisplayObject *o;
    DisplayObject *side;
    DuelCardRecord *rec;
    DuelCardRecord *left;
    DuelCardRecord *right;
    DuelEffectRequest *req;
    s32 result;
    s32 id;
    s32 level;
    s32 arg;
    u16 life;
    u8 flags;
    s32 ix;
    s8 *wins;
    DuelSideState *view;
    AnimatedBattleModelProperties *models;
    DisplayObject **pair;
    DisplayObject **slots;
    DisplayObjectCallback cb;
    s32 one;
    s32 y;
    s32 h;
    s32 w;
    s32 big;
    s32 pos;
    DuelSelectionRecord *cur;
    u8 *pw;
    DuelEffectResourceRecord *effects;

    if (!(gDuel_wSceneStateFlags & 0x8000)) {
        big = 0x48000;
        gDuel_wSceneStateFlags |= 0x8000;
        w = 0x10;
        one = 1;
        slots = gDisplayObject_apWorkSlots.slots;
        pw = gDuel_abCardStagingBuffer;
        o = gDuel_pTerrainIcon;
        y = 0x5C;
        S(o, 0x28) = -0x74;
        cb = (DisplayObjectCallback)DisplayObject_UpdateCosineSlideWithRollReset;
        o->field_2C.h.field_2C = w;
        o->field_6C = one;
        o->update = cb;
        o->position.h.field_2A = o->xy.h.y;
        o = gDuel_pLifePointsPanel;
        h = 0x18;
        o->position.h.field_28 = 0x198;
        o->field_2C.h.field_2C = w;
        o->field_6C = one;
        o->update = cb;
        o->position.h.field_2A = o->xy.h.y;
        o = slots[0];
        o->position.h.field_28 = 0x38;
        o->position.h.field_2A = y;
        o->field_2C.h.field_2C = h;
        o->field_6C = one;
        o->update = cb;
        CardPreview_RequestResourceTransfer(0,
            ((DuelCardReplayRecordBlock *)(
                pw + gDisplayObject_apWorkSlots.slots[0]->field_6A * 0x1C + big))->record.card_id);
        o = slots[1];
        gDuel_wSelectedTrapCardId = 0;
        gDuel_bSelectedTrapSlot = 0;
        if (o != 0) {
            o->position.h.field_28 = 0xD8;
            o->position.h.field_2A = y;
            o->field_2C.h.field_2C = h;
            o->field_6C = one;
            o->update = cb;
        } else {
            gDuel_bAnimatedBattleRequested = 0;
        }
        if (Duel_SelectAttackTrap(gDisplayObject_apWorkSlots.slots[0]) != 0) {
            gDuel_bAnimatedBattleRequested = 0;
        }
        cur = (DuelSelectionRecord *)((gDuel_abSelectionSides + 3 * DUEL_SELECTION_RECORD_SIZE) + gDuel_bActiveSide * 0x70);
        gDuel_pActiveSelectionRecord = cur;
        DisplayObject_ReleaseIfPresent(cur->cursor_object);
        gDuel_wDisplayTransitionFlags = 8;
        gDuel_bSceneSubStep = 1;
        gDuel_pActiveSelectionRecord->cursor_object = 0;
    }

    switch (gDuel_bSceneSubStep & 0xF) {
    case 1:
        if (!(gDuel_bSceneSubStep & 0x80) && ((gFile_dwTransferState & 0x02000030) | gFile_dwCancelRequestState) == 0) {
            if (!(gDuel_bSceneSubStep & 0x40)) {
                id = gDuel_wSelectedTrapCardId;
                gDuel_bSceneSubStep |= 0x40;
                if (id == 0) {
                    if (gDisplayObject_apWorkSlots.slots[1] != 0) {
                        u8 *pw1 = gDuel_abCardStagingBuffer;
                        s32 big1 = 0x48000;

                        id = ((DuelCardReplayRecordBlock *)(
                            pw1 + gDisplayObject_apWorkSlots.slots[1]->field_6A * 0x1C + big1))->record.card_id;
                        CardPreview_RequestResourceTransfer(1, id);
                        return;
                    }
                    break;
                }
                CardPreview_RequestResourceTransfer(1, id);
                return;
            }
            gDuel_bSceneSubStep |= 0x80;
        }
        if (!(gDuel_bSceneSubStep & 0x20) && gDuel_wDisplayTransitionFlags == 0) {
            if (!(gDuel_bSceneSubStep & 0x10)) {
                gDuel_bSceneSubStep |= 0x10;
                gDuel_wDisplayTransitionFlags = 0xC;
                gDuel_pActiveSelectionRecord = (DuelSelectionRecord *)((gDuel_abSelectionSides + 2 * DUEL_SELECTION_RECORD_SIZE) + gDuel_bActiveSide * 0x70);
            } else {
                gDuel_bSceneSubStep |= 0x20;
            }
        }
        if ((gDuel_bSceneSubStep & 0xA0) == 0xA0 && DisplayObject_FindAllocatedByTag(1) == 0) {
            gDuel_bSceneSubStep = 2;
            return;
        }
        break;

    case 2: {
        u8 f = gDuel_bSceneSubStep;

        if (!(f & 0x80)) {
            o = gDisplayObject_apWorkSlots.slots[0];
            gDuel_bSceneSubStep = f | 0x80;
            {
                s32 b21 = o->field_20.b.rotation_y;

                DisplayObject_StartAlphaTransition(o, b21, b21 + 0x40, 8);
            }
            result = Duel_CalcCardStats(&gDuel_aCardRecords[o->field_6A]);
            o = (DisplayObject *)CardPreview_CreateResourceObjects(0, result & 0xFFFF, result >> 16);
            effects = gCardPreview_Resources;
            effects[0].stat_palette_flags |= 0x40;
            o->xy.h.x = 0xA;
            o->xy.h.y = DUEL_BATTLE_STAT_DISPLAY_Y;
            o->field_20.b.rotation_y = 0xC0;
            DisplayObject_SetDepthOffset(o, -0xA);
            o->flags = (o->flags | 4) & 0xFFBF;
            gDisplayObject_apWorkSlots.slots[2] = o;
            o = gDisplayObject_apWorkSlots.slots[1];
            gDisplayObject_apWorkSlots.slots[3] = 0;
            if (o != 0) {
                {
                    s32 b21 = o->field_20.b.rotation_y;

                    DisplayObject_StartAlphaTransition(o, b21, b21 + 0x40, 8);
                }
                result = Duel_CalcCardStats(&gDuel_aCardRecords[o->field_6A]);
                o = (DisplayObject *)CardPreview_CreateResourceObjects(1, result & 0xFFFF, result >> 16);
                if (gDuel_awSavedRecordFlags[1] & 0x800) {
                    effects[1].stat_palette_flags |= 0x80;
                } else {
                    effects[1].stat_palette_flags |= 0x40;
                }
            } else if (gDuel_wSelectedTrapCardId != 0) {
                o = (DisplayObject *)CardPreview_CreateResourceObjects(1, -1, -1);
            }
            if (o != 0) {
                o->xy.h.x = 0xAA;
                o->xy.h.y = DUEL_BATTLE_STAT_DISPLAY_Y;
                o->field_20.b.rotation_y = 0xC0;
                DisplayObject_SetDepthOffset(o, -0xA);
                o->flags = (o->flags | 4) & 0xFFBF;
                gDisplayObject_apWorkSlots.slots[3] = o;
            }
        }
        f = gDuel_bSceneSubStep;
        if (f & 0x20) {
            if (Duel_UpdateTrapPresentation() == 0) {
                gDuel_bSceneSubStep &= 0xDF;
                return;
            }
        } else if (!(f & 0x40)) {
            if (DisplayObject_FindAllocatedByTag(1) == 0) {
                gDisplayObject_apWorkSlots.slots[0]->flags &= 0xFFBF;
                gDisplayObject_apWorkSlots.slots[2]->flags |= 0x40;
                if (gDisplayObject_apWorkSlots.slots[1] != 0) {
                    gDisplayObject_apWorkSlots.slots[1]->flags &= 0xFFBF;
                }
                if (gDisplayObject_apWorkSlots.slots[3] != 0) {
                    gDisplayObject_apWorkSlots.slots[3]->flags |= 0x40;
                }
                {
                    u8 g = gDuel_bSceneSubStep;

                    gDuel_bSceneSubStep = g | 0x40;
                    if (gDuel_wSelectedTrapCardId != 0) {
                        gDuel_bSceneSubStep = g | 0x60;
                    gDuel_wTrapPresentationMode = 0;
                    return;
                    }
                }
            }
        } else {
            gDisplayObject_apWorkSlots.slots[2]->field_20.b.rotation_y += 8;
            if ((s8)gDisplayObject_apWorkSlots.slots[2]->field_20.b.rotation_y >= 0) {
                gDisplayObject_apWorkSlots.slots[2]->field_20.b.rotation_y = 0;
                gDuel_bSceneSubStep = 3;
                gDisplayObject_apWorkSlots.slots[2]->flags &= 0xFFFB;
            }
            if (gDisplayObject_apWorkSlots.slots[3] != 0) {
                u8 fade = gDisplayObject_apWorkSlots.slots[2]->field_20.b.rotation_y;

                gDisplayObject_apWorkSlots.slots[3]->field_20.b.rotation_y = fade;
                if (fade == 0) {
                    gDisplayObject_apWorkSlots.slots[3]->flags &= 0xFFFB;
                    return;
                }
            }
        }
        break;
    }

    case 3:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            Fade_StartOutKeepOverlayAndHideSecondaryTables();
            return;
        }
        if (!(gFade_State.flags & 0x80)) {
            gDuel_bSceneSubStep = 4;
            if (gDuel_wSelectedTrapCardId != 0) {
                gDuel_bPresentationCursor = gDuel_wSelectedTrapCardId == 0x2B2;
                gDuel_bSceneSubStep = 0xA;
                return;
            }
            result = Duel_CompareBattleStrength(gDisplayObject_apWorkSlots.slots[0], gDisplayObject_apWorkSlots.slots[1]);
            if (result >= 0) {
                gDuel_abBattleOutcome[0] = 0;
                gDuel_awBattleDamage[0] = 0;
                gDuel_abBattleOutcome[1] = 1;
                gDuel_awBattleDamage[1] = 0;
                if (result != 0) {
                    if (gDisplayObject_apWorkSlots.slots[1] != 0 && (gDuel_awSavedRecordFlags[1] & 0x800)) {
                        gDuel_pActiveSideState->rank.card_destructions++;
                        gDuel_abBattleOutcome[1] = -1;
                        gDuel_aSideStates[gDuel_bActiveSide ^ 1].rank.defensive_losses++;
                        return;
                    }
                    view = &gDuel_aSideStates[gDuel_bActiveSide ^ 1];
                    life = view->life_points.unsigned_value - result;
                    view->life_points.unsigned_value = life;
                    if ((s16)life < 0) {
                        view->life_points.unsigned_value = 0;
                    }
                    if (gDisplayObject_apWorkSlots.slots[1] != 0) {
                        gDuel_pActiveSideState->rank.card_destructions++;
                        gDuel_pActiveSideState->rank.effective_attacks++;
                    }
                    gDuel_abBattleOutcome[1] = -1;
                    gDuel_awBattleDamage[1] = result;
                    return;
                }
            } else {
                gDuel_abBattleOutcome[0] = -1;
                gDuel_abBattleOutcome[1] = -1;
                gDuel_awBattleDamage[0] = 0;
                gDuel_awBattleDamage[1] = 0;
                if (result < -1) {
                    view = &gDuel_aSideStates[gDuel_bActiveSide];
                    life = view->life_points.unsigned_value + result;
                    view->life_points.unsigned_value = life;
                    if ((s16)life < 0) {
                        view->life_points.unsigned_value = 0;
                    }
                    gDuel_awBattleDamage[0] = result;
                    gDuel_abBattleOutcome[1] = 1;
                    if (gDuel_awSavedRecordFlags[1] & 0x800) {
                        gDuel_abBattleOutcome[0] = 1;
                        gDuel_aSideStates[gDuel_bActiveSide ^ 1].rank.defensive_wins++;
                        return;
                    }
                }
            }
        }
        break;

    case 4:
        do {
            if (gDuel_bSceneSubStep & 0x80) {
                break;
            }
            gDuel_bSceneSubStep |= 0x80;
            if (gDisplayObject_apWorkSlots.slots[1] == 0) {
                break;
            }
            {
                result = Duel_CalcGuardianStarBonus(&gDuel_aCardRecords[gDisplayObject_apWorkSlots.slots[0]->field_6A],
                                                    &gDuel_aCardRecords[gDisplayObject_apWorkSlots.slots[1]->field_6A]);
                if (result != 0) {
                    side = gDisplayObject_apWorkSlots.slots[2];
                    gDuel_bPresentationCursor = 0;
                    if (result < 0) {
                        side = gDisplayObject_apWorkSlots.slots[3];
                        gDuel_bPresentationCursor = 1;
                    }
                    rec = &gDuel_aCardRecords[gDisplayObject_apWorkSlots.slots[gDuel_bPresentationCursor]->field_6A];
                    if (rec->flags & 0x200) {
                        level = gDuel_adwCardStats[rec->card_id - 1] >> 0x12;
                    } else {
                        level = gDuel_adwCardStats[rec->card_id - 1] >> 0x16;
                    }
                    result = level & 0xF;
                    SD_SEPlayFull(0x1D);
                    req =
                        DuelEffect_AllocateRequest(0xE);
                    {
                        u8 t = gDuel_bSceneSubStep;

                        gDuelEffect_pTrackedRequest = req;
                        pos = side->xy.h.x;
                        pos += 0x46;
                        req->field_00 = pos;
                        gDuel_wStepCounter = 0;
                        arg = side->xy.h.y;
                        arg += DUEL_BATTLE_EFFECT_REQUEST_Y_OFFSET;
                        req->field_02 = arg;
                        pos = result - 1;
                        req->phase = pos;
                        gDuel_bSceneSubStep = t | 0x60;
                    }
                }
                return;
            }
        } while (0);
        {
            u8 f = gDuel_bSceneSubStep;

            if (f & 0x20) {
                if (!(gDuelEffect_pTrackedRequest->flags & 0x80)) {
                    gDuel_bSceneSubStep = f & 0xDF;
                }
            }
            f = gDuel_bSceneSubStep;
            if (f & 0x40) {
                /* Separate from effects to preserve state 2's register setup. */
                DuelEffectResourceRecord *resource;

                gDuel_wStepCounter += 0x10;
                if ((s16)gDuel_wStepCounter >= 0x1F4) {
                    gDuel_bSceneSubStep = f & 0xBF;
                    gDuel_wStepCounter = 0x1F4;
                }
                resource = &gCardPreview_Resources[gDuel_bPresentationCursor];
                arg = gDuel_wStepCounter;
                resource->attack_adjustment = arg;
                resource->defense_adjustment = arg;
            }
            if (!(gDuel_bSceneSubStep & 0x60)) {
                gDuel_bSceneSubStep = 7;
                if (gDuel_bAnimatedBattleRequested != 0) {
                    gDuel_bSceneSubStep = 5;
                    return;
                }
            }
        }
        break;

    case 5:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            SD_BGMFadeOut();
            Fade_StartOut();
            gFade_State.level = 0xFF;
            Fade_FillBandLevels(0xFF);
            return;
        }
        if (!(gFade_State.flags & 0x80) && SD_IsBgmFadeActive() == 0) {
            pair = gDisplayObject_apWorkSlots.slots;
            SD_BGMPreload(gDuel_wAnimatedBattleBgmCommand);
            Model_SetPresentationBgmCommand(gDuel_wAnimatedBattleBgmCommand);
            left = &gDuel_aCardRecords[pair[0]->field_6A];
            right = &gDuel_aCardRecords[pair[1]->field_6A];
            Duel_SetupFieldCardFromDeck(pair[0]->field_6A, pair[0]->field_6B);
            left->flags |= (gDuel_awSavedRecordFlags[0] & 0xA00) | 0x4000;
            left->stat_modifier = gDuel_awSavedStatModifiers[0];
            Duel_SetupFieldCardFromDeck(pair[1]->field_6A, pair[1]->field_6B);
            right->flags |= gDuel_awSavedRecordFlags[1] & 0xA00;
            H(right, 0x12) = gDuel_awSavedStatModifiers[1];
            models = gMain_aAnimatedBattleModels;
            gDuel_bBattleRemovalSlot1 = -1;
            gDuel_bBattleRemovalSlot0 = -1;
            models[0].model_id = H(left, 0xC);
            models[0].defense_position = 0;
            models[0].attack_compare_override = 0;
            models[0].defense_compare_override = 0;
            models[0].guardian_star_selection = (gDuel_awSavedRecordFlags[0] >> 9) & 1;
            models[1].model_id = right->card_id;
            models[1].guardian_star_selection = (gDuel_awSavedRecordFlags[1] >> 9) & 1;
            models[1].attack_compare_override = 0;
            models[1].defense_compare_override = 0;
            models[1].defense_position = (gDuel_awSavedRecordFlags[1] >> 0xB) & 1;
            result = Duel_CompareBattleStrength(pair[0], pair[1]);
            if (result == -1) {
                gDuel_bBattleRemovalSlot0 = pair[0]->field_6A;
                gDuel_bBattleRemovalSlot1 = pair[1]->field_6A;
            } else {
                if (result > 0) {
                    models[0].attack_compare_override = 1;
                    gDuel_bBattleRemovalSlot0 = pair[1]->field_6A;
                }
                if (result < 0) {
                    if (!(right->flags & 0x800)) {
                        models[1].attack_compare_override = 1;
                        gDuel_bBattleRemovalSlot0 = pair[0]->field_6A;
                    }
                }
            }
            gDuel_bResumeFromAnimatedBattle = 1;
            gMain_bReturnMode = 3;
            gMain_bActiveModeState = 1;
            return;
        }
        break;

    case 7:
        if (gDisplayObject_apWorkSlots.slots[1] == 0) {
            gDuel_bSceneSubStep = 9;
            return;
        }
        gDuel_bPresentationCursor = 2;
        gDuel_bSceneSubStep = 6;
    case 6:
        gDuel_bPresentationCursor = gDuel_bPresentationCursor - 1;
        if (gDuel_bPresentationCursor >= 0) {
            gDuel_bSceneSubStep = 8;
        case 8:
            flags = gDuel_bSceneSubStep;
            ix = gDuel_bPresentationCursor;
            side = gDisplayObject_apWorkSlots.slots[ix + 2];
            if (!(flags & 0x80)) {
                gDuel_bSceneSubStep = flags | 0xC0;
                /* Copying the .word members reschedules the stores; also below. */
                *(u32 *)&side->position = *(u32 *)&side->xy;
                wins = gDuel_abBattleOutcome;
                if (wins[ix] == 0) {
                    gDuel_bSceneSubStep = 6;
                    return;
                }
                req =
                    DuelEffect_AllocateRequest(2);
                req->field_00 = side->xy.h.x + 0x46;
                req->field_02 =
                    side->xy.h.y +
                    DUEL_BATTLE_EFFECT_REQUEST_Y_OFFSET;
                result = gDuel_awBattleDamage[gDuel_bPresentationCursor];
                gDuelEffect_pTrackedRequest = req;
                req->number = result;
                result = (result < 0 ? -result : result) / 1000;
                if (result >= 3) {
                    result = 2;
                }
                req->phase = result;
                if (wins[gDuel_bPresentationCursor] > 0) {
                    gDuel_bSceneSubStep |= 0x20;
                }
                arg = result + 0x10;
                if (gDuel_bPresentationCursor == 0) {
                    arg = result + 0xD;
                }
                SD_SEPlayFull(arg);
            }
        {
            s32 f = gDuel_bSceneSubStep;

            if (f & 0x40) {
                if (!(gDuelEffect_pTrackedRequest->flags & 0x80)) {
                    f &= 0xBF;
                    gDuel_bSceneSubStep = f;
                    *(u32 *)&side->xy = side->position.word;
                    gDuel_wStepCounter = 0xA;
                    return;
                }
                if (!(f & 0x20) &&
                    gDuelEffect_pTrackedRequest->progress != 0) {
                    side->xy.h.x = side->position.h.field_28 + ((rand() & 3) - 2);
                    side->xy.h.y = side->position.h.field_2A + ((rand() & 3) - 2);
                    return;
                }
            } else {
                gDuel_wStepCounter = gDuel_wStepCounter - 1;
                if ((s16)gDuel_wStepCounter <= 0) {
                    if (gDuel_bSceneSubStep & 0x20) {
                        gDuel_bSceneSubStep = 6;
                        return;
                    }
                    gDuel_bSceneSubStep = 0xA;
                    return;
                }
            }
        }
        } else {
            gDuel_bSceneSubStep = 0xB;
            return;
        }
        break;

    case 9:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            req = DuelEffect_CreateRequest(2);
            req->field_00 = 0xF0;
            req->field_02 = 0x78;
            result = Duel_CompareBattleStrength(gDisplayObject_apWorkSlots.slots[0], 0);
            req->phase = result / 1000;
            if (req->phase >= 3) {
                req->phase = 2;
            }
            SD_SEPlayFull(req->phase + 0x10);
            req->number = result;
            req->phase = req->phase + 3;
            return;
        }
        gDuel_bSceneSubStep = 0xB;
        return;

    case 10:
        side = gDisplayObject_apWorkSlots.slots[gDuel_bPresentationCursor + 2];
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            DuelCard_CaptureRoundedTexture(side);
            SD_SEPlayFull(0x1B);
            req = DuelEffect_AllocateRequest(3);
            gDuelEffect_pTrackedRequest = req;
            req->field_00 = side->xy.h.x + 0x46;
            req->field_02 =
                side->xy.h.y +
                DUEL_BATTLE_EFFECT_REQUEST_Y_OFFSET;
            if (gDuel_wSelectedTrapCardId != 0) {
                req->phase = 1;
            }
            gDuel_wStepCounter = 2;
            return;
        }
        if (!(gDuel_bSceneSubStep & 0x40)) {
            gDuel_wStepCounter = gDuel_wStepCounter - 1;
            if ((s16)gDuel_wStepCounter <= 0) {
                gDuel_bSceneSubStep |= 0x40;
                DuelEffect_ReleaseResourceObjects(gDuel_bPresentationCursor);
                if (gDuel_bPresentationCursor != 1 || gDuel_wSelectedTrapCardId == 0) {
                    gDisplayObject_apWorkSlots.slots[gDuel_bPresentationCursor + 2] = 0;
                    return;
                }
            }
        } else if (!(gDuelEffect_pTrackedRequest->flags & 0x80)) {
            gDuel_bSceneSubStep = 6;
            if (gDuel_wSelectedTrapCardId != 0) {
                gDuel_bSceneSubStep = 0xB;
                return;
            }
        }
        break;

    case 11:
        if (!(gDuel_bSceneSubStep & 0x80)) {
            gDuel_bSceneSubStep |= 0x80;
            if (gDisplayObject_apWorkSlots.slots[2] != 0) {
                Duel_SetupFieldCardFromDeck(gDisplayObject_apWorkSlots.slots[0]->field_6A, gDisplayObject_apWorkSlots.slots[0]->field_6B);
                left = &gDuel_aCardRecords[gDisplayObject_apWorkSlots.slots[0]->field_6A];
                left->flags |= (gDuel_awSavedRecordFlags[0] & 0xA00) | 0x4000;
                left->stat_modifier = gDuel_awSavedStatModifiers[0];
                Duel_ApplyCardObjectFlags((DuelCardDisplayObject *)left->object);
            }
            if (gDisplayObject_apWorkSlots.slots[3] != 0) {
                if (gDisplayObject_apWorkSlots.slots[1] != 0) {
                    Duel_SetupFieldCardFromDeck(gDisplayObject_apWorkSlots.slots[1]->field_6A, gDisplayObject_apWorkSlots.slots[1]->field_6B);
                    left = &gDuel_aCardRecords[gDisplayObject_apWorkSlots.slots[1]->field_6A];
                    left->flags |= gDuel_awSavedRecordFlags[1] & 0xA00;
                    left->stat_modifier = gDuel_awSavedStatModifiers[1];
                    if (gDuel_wSelectedTrapCardId != 0) {
                        left->flags |= gDuel_awSavedRecordFlags[1] & 0x3000;
                    }
                    Duel_ApplyCardObjectFlags((DuelCardDisplayObject *)left->object);
                }
            }
            DisplayObject_ReleaseIfPresent(gDisplayObject_apWorkSlots.slots[0]);
            DisplayObject_ReleaseIfPresent(gDisplayObject_apWorkSlots.slots[1]);
            o = gDuel_pTerrainIcon;
            o->position.h.field_28 = 0xC;
            o->field_2C.h.field_2C = 0x10;
            o->field_6C = 1;
            o->update = (DisplayObjectCallback)DisplayObject_UpdateCosineSlideWithRollReset;
            o->position.h.field_2A = o->xy.h.y;
            o = gDuel_pLifePointsPanel;
            o->position.h.field_28 = 0x118;
            o->field_2C.h.field_2C = 0x10;
            o->field_6C = 1;
            o->update = (DisplayObjectCallback)DisplayObject_UpdateCosineSlideWithRollReset;
            o->position.h.field_2A = o->xy.h.y;
        }
        if (!(gDuel_bSceneSubStep & 0x40)) {
            o = gDisplayObject_apWorkSlots.slots[2];
            if (o != 0) {
                result = B(o, 0xC);
                result -= 8;
                if (result <= 0) {
                    DuelEffect_ReleaseResourceObjects(0);
                    gDisplayObject_apWorkSlots.slots[2] = 0;
                } else {
                    B(o, 0xE) = result;
                    B(o, 0xD) = result;
                    B(o, 0xC) = result;
                }
            }
            o = gDisplayObject_apWorkSlots.slots[3];
            if (o != 0) {
                result = B(o, 0xC);
                result -= 8;
                if (result <= 0) {
                    DuelEffect_ReleaseResourceObjects(1);
                    gDisplayObject_apWorkSlots.slots[3] = 0;
                } else {
                    B(o, 0xE) = result;
                    B(o, 0xD) = result;
                    B(o, 0xC) = result;
                }
            }
            if (gDisplayObject_apWorkSlots.slots[2] == 0 && gDisplayObject_apWorkSlots.slots[3] == 0) {
                gDuel_bSceneSubStep |= 0x40;
                Fade_StartInKeepOverlay();
                return;
            }
        } else {
            gDuel_wSceneStateFlags = 5;
        }
        break;
    }
}

void DuelScene_UpdateTurnSwitch(void){if(!(gDuel_wSceneStateFlags&DUEL_SCENE_FLAG_INITIALIZED)){gDuel_wSceneStateFlags|=DUEL_SCENE_FLAG_INITIALIZED;Duel_StartViewTween(0x30,0x258,0x100,gDuel_bActiveSide?0x400:0xC00,0);gDuel_wDisplayTransitionFlags=0x10;}
 if(!gDuel_wDisplayTransitionFlags){gDuel_bActiveSide^=1;gDuel_wSceneStateFlags=2;gDuel_pActiveSideState=&gDuel_aSideStates[gDuel_bActiveSide];gDuel_pActiveFieldGrid=gDuel_abFieldGridCardRecordIndices.flat+gDuel_bActiveSide*DUEL_FIELD_SIDE_GRID_SLOT_COUNT;}}

#include "display_object.h"
#include "display_object_layout.h"
#include "input.h"

/* The cancel button. The Japanese release swaps Cross and Circle, so a regional
 * build names its own. The function's combined confirm|cancel test is 0xE0 in
 * both builds and keeps its spelling. */
#ifndef DUEL_CARD_USE_CONFIRMATION_CANCEL_BUTTON
#define DUEL_CARD_USE_CONFIRMATION_CANCEL_BUTTON PAD_BUTTON_CANCEL
#endif

s32 Duel_UpdateCardUseConfirmation(void) {
    DisplayObject *q;
    s32 one;
    s32 b;
    s32 c;
    s32 v;
    u8 w;

    if ((gDuel_bConfirmFlags & 0x80) == 0) {
        gDuel_bConfirmFlags = gDuel_bConfirmFlags | 0x80;
        one = 1;
        q = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        b = 0xB;
        c = 0x20C;
        DisplayObject_ConfigureSpriteAtPosition(
            q,
            (s16)gDuel_pActionCard->xy.h.x - 8,
            (s16)gDuel_pActionCard->xy.h.y + 0x1E,
            3, one, 2, b, c
        );
        DisplayObject_SelectOrderingTable1(q);
        DisplayObject_SetDepthOffset(q, 0xA);
        /* This merge, the two below it and the first store to 0x60 go
           through the members' addresses with the other signedness rather
           than being plain q->flags and q->field_60 stores. The spelling is
           load-bearing: as member stores, GCC 2.8.1 fills the following
           call's or branch's delay slot with the store and drops the nop,
           which shortens the executable by twelve bytes. The address-of form
           only keeps them non-struct stores because the cast type differs
           from the member's; with the same type fold turns it back into the
           member store. The second 0x60 store can be a plain member store.
           Reads convert normally -- it is only stores standing next to a
           branch that get hoisted. */
        *(s16 *)&q->flags =
            *(s16 *)&q->flags |
            DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
            DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        gDuel_pFlipLeftMarker = q;

        q = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPosition(
            q,
            (s16)gDuel_pActionCard->xy.h.x + 0x3C,
            (s16)gDuel_pActionCard->xy.h.y + 0x1E,
            3, one, 0, b, c
        );
        DisplayObject_SelectOrderingTable1(q);
        DisplayObject_SetDepthOffset(q, 0xA);
        *(s16 *)&q->flags =
            *(s16 *)&q->flags |
            DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
            DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        gDuel_pFlipRightMarker = q;
        return 0;
    }

    q = gDuel_pActionCard;

    if ((gDuel_bConfirmFlags & 0x40) != 0) {
        *(s16 *)&q->flags =
            *(s16 *)&q->flags | DISPLAY_OBJECT_FLAG_CLIP_TEST;
        v = q->field_20.b.rotation_y + (u8)q->field_60;
        q->field_20.b.rotation_y = v;
        w = v;
        if ((v & 0x7F) != 0) {
            return 0;
        }
        if ((w & 0xFF) == 0) {
            *(s16 *)&q->flags =
                *(s16 *)&q->flags & ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
        }
        do { gDuel_bConfirmFlags = gDuel_bConfirmFlags & 0xBF; } while (0);
        return 0;
    }

    if ((gInput_awPadRepeat[0] & PAD_DIRECTION_HORIZONTAL_MASK) != 0) {
        *(u16 *)&q->field_60 = 0x10;
        if ((gInput_awPadRepeat[0] & PAD_DIRECTION_RIGHT) != 0) {
            q->field_60 = -0x10;
        }
        gDuel_bConfirmFlags = gDuel_bConfirmFlags | 0x40;
        return 0;
    }

    if ((gInput_awPadPressed[0] &
         (PAD_BUTTON_CONFIRM_MASK | PAD_BUTTON_CANCEL)) != 0) {
        DisplayObject_ReleaseIfPresent(gDuel_pFlipLeftMarker);
        DisplayObject_ReleaseIfPresent(gDuel_pFlipRightMarker);
        if ((gInput_awPadPressed[0] & DUEL_CARD_USE_CONFIRMATION_CANCEL_BUTTON) != 0) {
            return -1;
        }
        if (q->field_20.b.rotation_y == 0) {
            return 1;
        }
        return 2;
    }
    return 0;
}

#include "display_asset_banks.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "../ygo_types.h"

void DuelResult_LoadOutroPackageStage(FileTransferDescriptor *p, s32 mode)
{
    s32 one;
    s32 v;
    s32 w;
    s32 e;
    s32 x;
    s32 m;
    s32 d;
    s32 f;

    one = 1;

    switch (mode) {
    case 0:
        m = 0xFFDDFFFF;
        p->field_30.h.field_32 = 0x100;
        v = gFile_dwTransferState;
        f = 0x40;
        do {
            p->field_30.h.counter = 0;
        } while (0);
        p->w = f;
        gFile_dwTransferState = v & m;
        w = gFile_dwTransferState;
        m = 0x10000;
        do {
            p->phase_size = m;
            gFile_dwTransferState = w | m;
            p->transfer_mode = 2;
            d = gFile_TransferBufferBase;
            p->h = 0x10;
        } while (0);
        p->sector_destination = d;
        p->alternate_sector_destination = d + FILE_SECTOR_SIZE;
        break;

    case 1:
        p->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState = gFile_dwTransferState & 0xFFDCFFFF;
        p->alternate_sector_destination = gFile_TransferBufferBase;
        p->sector_destination = gFile_TransferBufferBase;
        p->transfer_mode = one;
        break;

    case 2:
#ifdef VERSION_EUROPE
        /* The European build uploads the row at (640, 216). */
        p->x = 0x280;
        p->y = 0xD8;
        p->w = 0x100;
        e = gFile_TransferBufferBase;
#else
        p->y = 0xF8;
        p->w = 0x100;
        e = gFile_TransferBufferBase;
        p->x = 0;
#endif
        p->h = 4;
        LoadImage2((RECT *)p, (u32 *)e);
        p->alternate_sector_destination = (s32)gDisplay_abResourceBank0;
        p->sector_destination = (s32)gDisplay_abResourceBank0;
        x = gFile_dwTransferState;
        p->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState = x & 0xFFDCFFFF;
        p->transfer_mode = one;
        break;
    }
}

#include "display_object.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object_layout.h"
#include "../psyq/rand.h"
#include "input.h"
#include "rand_get_interval.h"
#include "trig_constants.h"
#include "file_transfer.h"
#include "sound.h"
#include "fade.h"
#include "duel_package.h"
#include "card_constants.h"
#include "duel_grid.h"
#include "text_staging.h"
#include "text_message_staging.h"
#include "text_constants.h"
#include "display_asset_banks.h"
#ifdef VERSION_EUROPE
#endif

#ifdef DUEL_RESULT_LOCALIZED_SPRITES

/* The localized builds also read the outro sprite count through $gp. It is
   initialized small data at 0x8009C2BC, between the trap-attack thresholds
   and the effect-preview state. */
s32 gDuelResult_nOutroSpriteCount = 0;

#define DUEL_RESULT_OUTRO_SPRITE_LIMIT 10
#define DUEL_RESULT_OUTRO_ACTIVE_COUNT gDuelResult_nOutroSpriteCount
#else
#define DUEL_RESULT_OUTRO_SPRITE_LIMIT DUEL_RESULT_SPRITE_COUNT
#define DUEL_RESULT_OUTRO_ACTIVE_COUNT DUEL_RESULT_SPRITE_COUNT
#endif

#define DUEL_RESULT_ORBIT_ANGLE_STEP 0x30

/* The complete duel-result outro, rank, drop, award, and rewards-screen runtime.

   The duel-result outro and the two update callbacks it hangs on its
   confetti: DuelResult_UpdateOrbitSprite orbits a sprite around its spawn
   point, DuelResult_UpdateDepartingSprite sends it flying off, and
   DuelScene_UpdateResultOutro is the outro sequence that spawns the sprites
   on the first and retargets them at the second. The three sources were
   recorded at gcc_2_8_1_g8_no_split,
   gcc_2_8_1_g8 and gcc_2_8_1_g8_split; all nine functions compile at
   gcc_2_8_1_g8_split. Bounded below by the outro package's transfer callback
   DuelResult_LoadOutroPackageStage, which cannot reproduce its object under this unit's
   split profile, and above by the debug effect screen at DebugEffect_UpdateViewControls. */

/* Per-frame update for an object that orbits a fixed base position
   (field_2C/field_2E) at DUEL_RESULT_ORBIT_ANGLE_STEP per frame, with radius
   the object's field_28 (which also serves as its countdown timer -- it decays
   by 2/frame). When the timer reaches 0: clears field_6C/update, snaps the
   current position (xy.h.x/xy.h.y) back to the base position in one 32-bit
   copy, and returns. Otherwise, while gGraphics_dwSyncedFrameCount's bit 0 is set, spawns a
   companion slot object at the object's current position (tagged via
   sub_table_lookup_set_flag using depth_offset-1), then advances the angle and
   recomputes the orbit position from base + (rcos,rsin)*radius/ONE. */

void DuelResult_UpdateOrbitSprite(DisplayObject *object) {
    s16 timer;
    u16 angle;
    DisplayObject *slot;
    s32 vy;

    timer = (s16)object->position.h.field_28 - 2;
    object->position.h.field_28 = timer;
    if (timer <= 0) {
        object->field_6C = 0;
        object->update = 0;
        object->xy.word = object->field_2C.word;
        return;
    }

    if (gGraphics_dwSyncedFrameCount & 1) {
        slot = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        if (slot != 0) {
            DisplayObject_ConfigureSpriteAtPositionWithResource(slot, (s16)object->xy.h.x,
                                   (s16)object->xy.h.y, 0,
                                   object->resource_branch_index, object->resource_variant, 0x11, 9,
                                   gDisplay_abResourceBank0);
            slot->field_40.clut.x = (u16)object->field_40.clut.x + 0x80;
            slot->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                           DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
            slot->attribute |= (GsALON | GsAONE);
            DisplayObject_SetDepthOffset(slot, (u8)object->depth_offset - 1);
            slot->field_60 = 8;
            slot->update = (DisplayObjectCallback)DisplayObject_FadeBrightnessAndRelease;
        }
    }

    angle = object->position.h.field_2A + DUEL_RESULT_ORBIT_ANGLE_STEP;
    object->position.h.field_2A = angle;
    object->xy.h.x =
        (s16)object->field_2C.h.field_2C +
        rcos((s16) angle) * (s16)object->position.h.field_28 / ONE;
    vy = rsin((s16) object->position.h.field_2A) * (s16)object->position.h.field_28;
    object->xy.h.y = object->field_2C.h.field_2E + vy / ONE;
}

void DuelResult_UpdateDepartingSprite(DuelCardDisplayObject *object)
{
    if (DisplayObject_MarkInitialized((DisplayObjectLifecycle *)object) == 0) {
        object->flags |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
    }
    object->rotation_y -= 2;
    if (object->rotation_y < 0xC0) {
        DisplayObject_ReleaseIfPresent((u8 *)object);
    }
}

#ifndef DUEL_RESULT_OUTRO_START_SECTOR
#define DUEL_RESULT_OUTRO_START_SECTOR FILE_WA_DUEL_RESULTS_START_SECTOR
#endif

#ifndef DUEL_RESULT_DUELIST_DATA_FIRST_SECTOR
#define DUEL_RESULT_DUELIST_DATA_FIRST_SECTOR DUELIST_DATA_FIRST_SECTOR
#endif

/* Duel-result outro sequence, driven from the scene state word gDuel_wSceneStateFlags.

   The first call (bit 0x8000 clear) fades the BGM out, records the winning
   side in gDuel_bPlayerLost/gDuel_bOutroWinnerSide, requests the outro package (0x1DAB, 0x22
   sectors) through DuelResult_LoadOutroPackageStage, picks the win or lose track for
   gDuelResult_wBgmId -- 0x72F0 only when side 1 won (the player lost) against
   a real opponent, otherwise 0x72E0 --
   and hands gDuel_pTerrainIcon and gDuel_pLifePointsPanel to DisplayObject_UpdateCosineSlide
   before entering step 1 of gDuel_bSceneSubStep.

   Afterwards the low nibble of gDuel_bSceneSubStep is the step and bit 0x80 marks
   "step already entered":

     1  wait for the DMA/queue flags to drain, then reload the selected
        three-sector duelist data block into 0x801781D8. The adjacent
        SD_BGMPreload call handles gDuelResult_wBgmId separately; once the transfer
        has drained, SD_BGMPlay starts that track.
     2  spawn the seven confetti sprites from the gDuelResult_aSpriteSpecsOpponent (real opponent)
        or gDuelResult_aSpriteSpecsNoOpponent (no opponent) table row for the winning side, each on
        DuelResult_UpdateOrbitSprite with a random radius and orbit key, and
        remember them in the gDuel_awRitualData slot table; then wait for
        DisplayObject_FindAllocatedByTag.
     3  hold for 0x258 frames or until the player presses one of the 0xE0
        buttons, then retarget every spawned sprite at DuelResult_UpdateDepartingSprite so it
        flies off, and wait for DisplayObject_FindAllocatedByTag again.
     4  once the fade at gFade_State.flags has finished, hand the scene over to
        state 0xD.  */

void DuelScene_UpdateResultOutro(void)
{
    DuelResultSpriteSlot *slots;
    DisplayObject *obj;
    DuelResultSpriteSpec *spec;
    s32 i;
    s32 flags;
    s32 state;
    s32 id;
    s32 timer;
    s32 mode;
    s32 v;

    slots = (DuelResultSpriteSlot *)gDuel_awRitualData;

    v = gDuel_wSceneStateFlags;
    if ((v & DUEL_SCENE_FLAG_INITIALIZED) == 0) {
        gDuel_wSceneStateFlags = v | DUEL_SCENE_FLAG_INITIALIZED;
        SD_BGMFadeOut();
        id = gDuel_bWinnerSide;
        gDuel_bPlayerLost = 0;
        if (id != 0) {
            gDuel_bPlayerLost = 1;
        }
        gDuel_bOutroWinnerSide = id;
        File_RequestAsyncTransfer(
            0, 0,
            DUEL_RESULT_OUTRO_START_SECTOR,
            FILE_WA_DUEL_RESULTS_SECTOR_COUNT,
            DuelResult_LoadOutroPackageStage, 0, 0
        );
        mode = 0x72E0;
        if (gDuel_bWinnerSide != 0) {
            if (gDuel_bOpponentID >= 0) {
                mode = 0x72F0;
            }
        }
        obj = gDuel_pTerrainIcon;
        gDuelResult_wBgmId = mode;
        /* The record spells 0x28 u16 and this is the one site that puts a
           negative value there. Through the plain member GCC materialises
           -116 as `ori 0xff8c` where retail has `addiu -116`; the store at
           0x198 below is unaffected, so the divergence is the constant's
           sign, not the member. */
        *(s16 *)&obj->position.h.field_28 = -116;
        obj->field_2C.h.field_2C = 0x30;
        obj->field_6C = 1;
        obj->update = (DisplayObjectCallback)DisplayObject_UpdateCosineSlide;
        obj->position.h.field_2A = (s16)obj->xy.h.y;
        obj = gDuel_pLifePointsPanel;
        obj->position.h.field_28 = 0x198;
        obj->field_2C.h.field_2C = 0x30;
        obj->field_6C = 1;
        obj->update = (DisplayObjectCallback)DisplayObject_UpdateCosineSlide;
        gDuel_bSceneSubStep = 1;
        obj->position.h.field_2A = (s16)obj->xy.h.y;
        return;
    }

    if (gDuel_wDisplayTransitionFlags != 0) {
        return;
    }

    flags = gDuel_bSceneSubStep;
    state = flags & 0xF;
    switch (state) {
    case 1:
        if ((flags & 0x80) == 0) {
            if ((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                gFile_dwCancelRequestState) {
                return;
            }
            gDuel_bSceneSubStep = flags | 0x80;
            gDuel_wDisplayTransitionFlags = 8;
            id = gDuel_bOpponentID;
            if (id < 0) {
                id = 1;
            }
            File_RequestAsyncTransfer(
                0, 0,
                id * DUELIST_DATA_SECTOR_COUNT +
                    DUEL_RESULT_DUELIST_DATA_FIRST_SECTOR,
                DUELIST_DATA_SECTOR_COUNT, 0, 0,
                (s32)((u8 *)slots - 0x1800));
            SD_BGMPreload(gDuelResult_wBgmId);
        } else {
            if ((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                gFile_dwCancelRequestState) {
                return;
            }
            gDuel_bSceneSubStep = 2;
            SD_BGMPlay(gDuelResult_wBgmId);
        }
        break;
    case 2:
        if ((flags & 0x80) == 0) {
            gDuel_bSceneSubStep = flags | 0x80;
            for (i = 0; i < DUEL_RESULT_OUTRO_SPRITE_LIMIT; i++) {
#ifdef DUEL_RESULT_LOCALIZED_SPRITES
                /* Fixed-language builds keep the selector in 1..4. */
                if (gText_bLanguageIndex == 1) {
                    if (gDuel_bOpponentID >= 0) {
                        spec = &gDuelResult_aFrenchSpriteSpecsOpponent[gDuel_bWinnerSide][i];
                    } else {
                        spec = &gDuelResult_aFrenchSpriteSpecsNoOpponent[gDuel_bWinnerSide][i];
                    }
                } else if (gText_bLanguageIndex == 2) {
                    if (gDuel_bOpponentID >= 0) {
                        spec = &gDuelResult_aGermanSpriteSpecsOpponent[gDuel_bWinnerSide][i];
                    } else {
                        spec = &gDuelResult_aGermanSpriteSpecsNoOpponent[gDuel_bWinnerSide][i];
                    }
                } else if (gText_bLanguageIndex == 3) {
                    if (gDuel_bOpponentID >= 0) {
                        spec = &gDuelResult_aItalianSpriteSpecsOpponent[gDuel_bWinnerSide][i];
                    } else {
                        spec = &gDuelResult_aItalianSpriteSpecsNoOpponent[gDuel_bWinnerSide][i];
                    }
                } else if (gText_bLanguageIndex == 4) {
                    if (gDuel_bOpponentID >= 0) {
                        spec = &gDuelResult_aSpanishSpriteSpecsOpponent[gDuel_bWinnerSide][i];
                    } else {
                        spec = &gDuelResult_aSpanishSpriteSpecsNoOpponent[gDuel_bWinnerSide][i];
                    }
                }
                if (spec->x == 0) {
                    continue;
                }
                gDuelResult_nOutroSpriteCount = i + 1;
#else
                if (gDuel_bOpponentID >= 0) {
                    spec = &gDuelResult_aSpriteSpecsOpponent[gDuel_bWinnerSide][i];
                } else {
                    spec = &gDuelResult_aSpriteSpecsNoOpponent[gDuel_bWinnerSide][i];
                }
#endif
                slots[i].object = 0;
                if (spec->kind != 0) {
                    obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
                    DisplayObject_ConfigureSpriteAtPositionWithResource(obj, spec->x, spec->y, 0,
                                  gDuel_bWinnerSide, spec->kind, 0x11, 9,
                                  gDisplay_abResourceBank0);
                    if (obj->resource_variant >= 0x1A) {
                        obj->field_40.clut.x =
                            (u16)obj->field_40.clut.x + 0x10;
                    }
                    obj->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                                  DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
                    obj->attribute |= (GsALON | GsAONE);
                    obj->field_48.pivot.x = spec->tag;
                    obj->field_48.pivot.y = 0x18;
                    DisplayObject_SelectOrderingTable1(obj);
                    obj->field_2C.word = obj->xy.word;
                    obj->position.h.field_28 = (rand() & 0x3F) + 0x140;
                    obj->position.h.field_2A =
                        Rand_GetInterval(TRIG_ANGLE_FULL_TURN);
                    obj->field_6C = 1;
                    obj->update =
                        (DisplayObjectCallback)DuelResult_UpdateOrbitSprite;
                    slots[i].object = obj;
                }
            }
        } else {
            if ((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                gFile_dwCancelRequestState) {
                return;
            }
            if (DisplayObject_FindAllocatedByTag(1) == 0) {
                gDuel_bSceneSubStep = 3;
            }
        }
        break;
    case 3:
        if ((flags & 0x80) == 0) {
            gDuel_bSceneSubStep = flags | 0x80;
            gDuel_wStepCounter = 0x258;
        }
        if ((gDuel_bSceneSubStep & 0x40) == 0) {
            timer = gDuel_wStepCounter - 1;
            gDuel_wStepCounter = timer;
            if ((s16)timer > 0) {
                if ((gInput_awPadPressed[0] &
                     (PAD_BUTTON_CANCEL | PAD_BUTTON_CONFIRM_MASK)) == 0) {
                    if ((SD_GetStatusFlags() & 0x80) != 0) {
                        return;
                    }
                }
            }
            gDuel_bSceneSubStep = gDuel_bSceneSubStep | 0x40;
            for (i = 0; i < DUEL_RESULT_OUTRO_ACTIVE_COUNT; i++) {
                obj = slots[i].object;
                if (obj != 0) {
                    obj->field_6C = 1;
                    obj->update = (DisplayObjectCallback)DuelResult_UpdateDepartingSprite;
                }
            }
        } else {
            if (DisplayObject_FindAllocatedByTag(1) == 0) {
                gDuel_bSceneSubStep = 4;
            }
        }
        break;
    case 4:
        if ((flags & 0x80) == 0) {
            gDuel_bSceneSubStep = flags | 0x80;
        }
        if ((gFade_State.flags & FADE_FLAG_ACTIVE) == 0) {
            gDuel_wSceneStateFlags = 0xD;
        }
        break;
    }
}

/* The duel's reward step: Duel_ShowResultPage shows one of the three result
   pages and opens its text box, Duel_CalcRankScoreChange is the per-rule
   lookup Duel_CalcRankScore sums into that display's side scores, and the
   card drop and award follow. The first two were recorded at gcc_2_8_1_g8
   and gcc_2_8_1_g0_split and compile to identical objects at this unit's
   gcc_2_8_1_g8_split. */

/* Regional values: the result page's text box y and height (0x28/0x120 ->
   0x38/0xF0). The European build (src/game/european/) defines its own. */
#ifndef DUEL_RESULT_PAGE_TEXT_Y
#define DUEL_RESULT_PAGE_TEXT_Y 0x28
#endif
#ifndef DUEL_RESULT_PAGE_TEXT_HEIGHT
#define DUEL_RESULT_PAGE_TEXT_HEIGHT 0x120
#endif

void Duel_ShowResultPage(s32 page)
{
    s32 i;
    DisplayObject *child;
    void *object;

    DisplayObject_SetResourceVariant(gDuel_pResultDisplayState->root, page);
    if (page == 0) {
        DuelResultDisplayState *state = gDuel_pResultDisplayState;

        for (i = 0; i < DUEL_RESULT_DISPLAY_CHILD_COUNT; i++) {
            child = state->children[i];
            if (child == 0) {
                break;
            }
            child->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
        }
    } else {
        DuelResultDisplayState *state = gDuel_pResultDisplayState;

        for (i = 0; i < DUEL_RESULT_DISPLAY_CHILD_COUNT; i++) {
            child = state->children[i];
            if (child == 0) {
                break;
            }
            child->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
        }
    }
    object = TextBox_Create(
        0, gDuel_pResultDisplayState->page_text_ids[page], 0x1A, DUEL_RESULT_PAGE_TEXT_Y, 0x120,
        DUEL_RESULT_PAGE_TEXT_HEIGHT
    );
    TextBox_BuildBlocking(object);
}

s32 Duel_CalcRankScoreChange(s32 rule, s32 value)
{
    DuelRankScoreChangeEntry *p = &gDuel_awRankScoreChange[rule][0];

    while (1) {
        if (value < p->threshold) {
            return p->score_change;
        }
        p++;
    }
}

/* Initializes result-message selectors at +0x34; the winner's signed
   end-reason adjustment selects the middle variant, not a rank letter.
   Both side scores at +0x2C start at DUEL_RANK_SCORE_INITIAL, then receive
   the end-reason and threshold-rule adjustments. Raw statistics are also
   copied into the separate gText_StagedValues[0].rank_rows[stat][side] display
   table. */
/* The results page that shows the selected text. */
#ifdef VERSION_EUROPE
#define DUEL_RESULT_SELECTOR_PAGE 0
#else
#define DUEL_RESULT_SELECTOR_PAGE 1
#endif

void Duel_CalcRankScore(void) {
    DuelResultDisplayState *p;
    DuelSideState *e;
    s32 i;
    s32 v;
    s32 *q;

    p = gDuel_pResultDisplayState;
    e = gDuel_aSideStates;
    q = &gText_StagedValues[0].rank_rows[0][0];
#ifdef VERSION_EUROPE
    /* The European build shows the selected text on the first page and
       0x44 on the second. */
    p->page_text_ids[0] = DUEL_RESULT_TEXT_SELECTOR_DEFAULT;
    p->page_text_ids[1] = 0x44;
#else
    p->page_text_ids[0] = 0x44;
    p->page_text_ids[1] = DUEL_RESULT_TEXT_SELECTOR_DEFAULT;
#endif
    p->page_text_ids[2] = 0x45;
    if (gDuel_aSideStates[gDuel_bWinnerSide].rank.result_adjustment ==
        DUEL_RANK_ADJUST_EXODIA_WIN) {
        p->page_text_ids[DUEL_RESULT_SELECTOR_PAGE] =
            DUEL_RESULT_TEXT_SELECTOR_EXODIA;
    }
    if (gDuel_aSideStates[gDuel_bWinnerSide].rank.result_adjustment ==
        DUEL_RANK_ADJUST_DECK_OUT_WIN) {
        p->page_text_ids[DUEL_RESULT_SELECTOR_PAGE] =
            DUEL_RESULT_TEXT_SELECTOR_DECK_OUT;
    }

    p->side_scores[1] = DUEL_RANK_SCORE_INITIAL;
    p->side_scores[0] = DUEL_RANK_SCORE_INITIAL;
    for (i = 0; i < DUEL_SIDE_COUNT; i++, e++, q++) {
        p->side_scores[i] += e->rank.result_adjustment;
        v = e->deck_draw_cursor; q[0 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_CARDS_USED, v);
        v = e->life_points.signed_value; q[1 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_REMAINING_LP, v);
        q[2 * DUEL_SIDE_COUNT] = e->deck_average_attack;
        q[3 * DUEL_SIDE_COUNT] = e->rank.card_destructions;
        v = e->rank.effective_attacks; q[4 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_EFFECTIVE_ATTACKS, v);
        q[5 * DUEL_SIDE_COUNT] = e->deck_average_defense;
        q[6 * DUEL_SIDE_COUNT] = e->rank.defensive_losses;
        v = e->rank.defensive_wins; q[7 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_DEFENSIVE_WINS, v);
        q[8 * DUEL_SIDE_COUNT] = e->rank.combo_plays;
        v = e->rank.face_down_plays; q[9 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_FACE_DOWN_PLAYS, v);
        v = e->rank.fusions_initiated; q[10 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_INITIATE_FUSION, v);
        v = e->rank.equips_used; q[11 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_EQUIP_MAGIC, v);
        q[12 * DUEL_SIDE_COUNT] = e->rank.field_magic_used;
        v = e->rank.pure_magic_used; q[13 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_PURE_MAGIC, v);
        v = e->rank.traps_triggered; q[14 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_TRAPS_TRIGGERED, v);
        v = e->rank.turns_taken; q[15 * DUEL_SIDE_COUNT] = v;
        p->side_scores[i] +=
            Duel_CalcRankScoreChange(DUEL_RANK_RULE_TURNS, v);
    }
}

s32 Duel_SelectCardDrop(s32 pool_index)
{
    DuelDropTable *table = &gDuel_awSaPowCardDrops[pool_index];
    s32 threshold = (rand() & (DUEL_DROP_WEIGHT_TOTAL - 1)) + 1;
    s32 sum = 0;
    s32 i;

    for (i = 0; i < CARD_COUNT; i++) {
        sum += table->weights[i];
        if (sum >= threshold)
            return i + 1;
    }
    return 0;
}

void Duel_AwardCard(s32 card_id)
{
    s32 i;
    u8 *base = (u8 *)gDuel_awPlayerDeck;
    u8 *quantity = base + (card_id + 0x4F);
    u16 *destination = (u16 *)(base + 0x5BC);
    u16 *entry;

    (*quantity)++;
    if (*quantity >= CARD_CHEST_QUANTITY_MAX + 1)
        *quantity = CARD_CHEST_QUANTITY_MAX;
    i = DUEL_RECENT_CARD_DROP_COUNT - 2;
    entry = destination + (DUEL_RECENT_CARD_DROP_COUNT - 2);
    do {
        entry[1] = entry[0];
        entry--;
        i--;
    } while (i >= 0);
    *destination = card_id;
}

/* Regional values: the y of the starchip prize sprites (192 -> 210) and of
 * the TEC and rank sprites (16 -> 40). The European build
 * (src/game/european/) defines its own. */
#ifndef DUEL_RESULT_STARCHIP_Y
#define DUEL_RESULT_STARCHIP_Y 192
#endif
#ifndef DUEL_RESULT_RANK_Y
#define DUEL_RESULT_RANK_Y 16
#endif

/* The confirm button (Cross). The Japanese release swaps Cross and Circle, so a
 * regional build names its own. */
#ifndef DUEL_RESULT_REWARDS_CONFIRM_BUTTON
#define DUEL_RESULT_REWARDS_CONFIRM_BUTTON 0x40
#endif

void DuelScene_UpdateResultRewards(void)
{
    DisplayObject *object;
    s32 score;
    s32 sound;
    s32 count;
    s32 offset;
    s32 x;
    s16 dropped_card;
    u16 value;
    s8 page;
    u8 opponent;

    gViewState_Current.angle = (u16)gViewState_Current.angle + 2;
    ViewState_ApplyOrbit();
    if (!(gDuel_wSceneStateFlags & 0x8000)) {
        gDuel_wSceneStateFlags |= 0x8000;
        Fade_StartOutKeepOverlay();
        Fade_SetTargetLevel(128, 2);
        gDuel_pResultDisplayState = (DuelResultDisplayState *)gDuel_awRitualData;
        gGraphics_sViewportY = 0;
        gGraphics_sViewportX = 0;
        gText_BranchSelectorA = 0;
        gText_BranchSelectorB = 0;
        gText_abColorSlots[0] = 4;
        gText_abColorSlots[1] = 4;
        gText_abColorSlots[gDuel_bWinnerSide] = 0;
        sound = 0x72E1;
        if (gDuel_bWinnerSide) {
            sound = 0x72F1;
            opponent = (u8)gDuel_bOpponentID;
            gText_BranchSelectorB = 1;
            gText_StagedStringId = (s8)opponent - 31960;
        }
        SD_BGMPlay(sound);
        if (gDuel_bSide0ControllerSelector[0] < 0 && gDuel_bOpponentID < 0) {
            gText_BranchSelectorA = 1;
            gText_BranchSelectorB += 2;
        }
        gDuel_pResultDisplayState->is_tec_rank = 0;
        Duel_CalcRankScore();
        score = gDuel_pResultDisplayState->side_scores[gDuel_bWinnerSide];
        if (score < 50) {
            gDuel_pResultDisplayState->is_tec_rank = 1;
            if (score < 0)
                score = 0;
            score = 99 - score;
        }
        if (score >= 100)
            score = 99;
        score -= 50;
        gDuel_pResultDisplayState->rank_tier = score / 10;
        gDuel_pResultDisplayState->page_index = 0;
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPosition(object, 32, 16, 3, 1, 2, 11, 524);
        DisplayObject_SelectOrderingTable1(object);
        object->flags |= 0x28;
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPosition(object, 288, 16, 3, 1, 0, 11, 524);
        DisplayObject_SelectOrderingTable1(object);
        count = 9;
        object->flags |= 0x28;
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 8, 0, 4, 0, 16, 8, gDisplay_abResourceBank0);
        DisplayObject_SelectOrderingTable1(object);
        DisplayObject_SetDepthOffset(object, -1);
        object->flags |= 0x20;
        gDuel_pResultDisplayState->root = object;
        do {
            gDuel_pResultDisplayState->children[count] = 0;
            count--;
        } while (count >= 0);
        gDuel_wSelectedCardID = 0;
        if (gDuel_bSide0ControllerSelector[0] < 0 && gDuel_bOpponentID >= 0 &&
            !gDuel_bWinnerSide) {
            gDuel_pResultDisplayState->starchip_prize = gDuel_pResultDisplayState->rank_tier + 1;
            score = 2 * (gDuel_pResultDisplayState->is_tec_rank != 0);
            if (gDuel_pResultDisplayState->rank_tier < 3)
                score = 1;
            dropped_card = Duel_SelectCardDrop(score);
            count = 0;
            gDuel_wSelectedCardID = dropped_card;
            gDuel_pResultDisplayState->dropped_card_id = dropped_card;
            gDuel_DroppedCardTextValue[0] = dropped_card;
            offset = 8;
            if (gDuel_pResultDisplayState->starchip_prize) {
                x = 160;
                do {
                    count++;
                    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
                    DisplayObject_ConfigureSpriteAtPosition(object, x, DUEL_RESULT_STARCHIP_Y, 3, 4, 0, 11, 524);
                    DisplayObject_SelectOrderingTable1(object);
                    x += 20;
                    object->flags |= 0x20;
                    *(DisplayObject **)((u8 *)gDuel_pResultDisplayState + offset + 4) = object;
                    offset += 4;
                } while (count < gDuel_pResultDisplayState->starchip_prize);
            }
        }
        if (gDuel_bSide0ControllerSelector[gDuel_bWinnerSide] < 0) {
            object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
            DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, DUEL_RESULT_RANK_Y, 0, 5, gDuel_pResultDisplayState->is_tec_rank,
                16, 8, gDisplay_abResourceBank0);
            DisplayObject_SelectOrderingTable1(object);
            DisplayObject_SetDepthOffset(object, -2);
            object->flags |= 0x20;
            gDuel_pResultDisplayState->children[0] = object;
            object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
            DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, DUEL_RESULT_RANK_Y, 0, 6, gDuel_pResultDisplayState->rank_tier,
                16, 8, gDisplay_abResourceBank0);
            DisplayObject_SelectOrderingTable1(object);
            DisplayObject_SetDepthOffset(object, -1);
            object->flags |= 0x20;
            gDuel_pResultDisplayState->children[1] = object;
        }
        Duel_ShowResultPage((s8)gDuel_pResultDisplayState->page_index);
        return;
    }
    if (gDuel_wSceneStateFlags & 0x4000) {
        if (!(gFade_State.flags & 0x80)) {
            if (!(gDuel_wSceneStateFlags & 0x2000)) {
                gDuel_wSceneStateFlags |= 0x2000;
                Fade_StartOut();
                gFade_State.level = 255;
                Fade_FillBandLevels(255);
            } else {
                /* Only the null test goes through `save`; the updates below
                   index gDuel_pSide0SaveData again each time, as retail reloads the
                   window pointer (106 differences through `save`). */
                SaveDataState *save = DUEL_SAVE_WINDOW(gDuel_bWinnerSide);
                gDuel_wOutcomeFlags |= 0x2000;
                if (save) {
                    if (gDuel_bSide0ControllerSelector[0] < 0 && gDuel_bOpponentID >= 0) {
                        DUEL_SAVE_WINDOW(0)->starchips +=
                            gDuel_pResultDisplayState->starchip_prize;
                        if (DUEL_SAVE_WINDOW(0)->starchips > 999999)
                            DUEL_SAVE_WINDOW(0)->starchips = 999999;
                        Duel_AwardCard(gDuel_pResultDisplayState->dropped_card_id);
                    } else {
                        value = DUEL_SAVE_WINDOW(gDuel_bWinnerSide)->duel_wins + 1;
                        DUEL_SAVE_WINDOW(gDuel_bWinnerSide)->duel_wins = value;
                        if (value >= 10000)
                            DUEL_SAVE_WINDOW(gDuel_bWinnerSide)->duel_wins = 9999;
                        value = DUEL_SAVE_WINDOW(gDuel_bWinnerSide ^ 1)
                                    ->duel_losses + 1;
                        DUEL_SAVE_WINDOW(gDuel_bWinnerSide ^ 1)->duel_losses = value;
                        if (value >= 10000)
                            DUEL_SAVE_WINDOW(gDuel_bWinnerSide ^ 1)
                                ->duel_losses = 9999;
                    }
                }
            }
        }
    } else if (gInput_awPadRepeat[0] & 0xA000) {
        gDuel_pResultDisplayState->page_index++;
        if (gInput_awPadRepeat[0] & 0x8000) {
            page = gDuel_pResultDisplayState->page_index - 2;
            gDuel_pResultDisplayState->page_index = page;
            if (page < 0)
                gDuel_pResultDisplayState->page_index = 2;
        }
        if ((s8)gDuel_pResultDisplayState->page_index >= 3)
            gDuel_pResultDisplayState->page_index = 0;
        SD_SEPlayFull(6);
        Duel_ShowResultPage((s8)gDuel_pResultDisplayState->page_index);
    } else if (gInput_awPadPressed[0] & DUEL_RESULT_REWARDS_CONFIRM_BUTTON) {
        gDuel_wSceneStateFlags |= 0x4000;
        Fade_SetTargetLevel(0, 6);
        SD_SEPlayFull(0x30);
    }
}

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "duel_card.h"
#include "file_transfer.h"
#include "input.h"
#include "display_object.h"

/* The developer effect-preview screen: a pad-driven camera and viewport
   nudge, the controller that builds one of four preview pages and spawns an
   effect on CROSS, and the HUD line that prints the two tuned values. The
   three call each other in that order and are the whole gcc_2_8_1_g8_split
   run between func_80021F80 and Duel_UpdateCardToSelectionView. */

/* The default window has axis at 0, coordinates at 2..3, and page at 4.
   The Spanish wrapper preserves its larger gap before the coordinates. */

/* Regional value: page 1's card object field_32 (0x16 -> 0x0E). The
   European build defines its own. */
#ifndef DEBUG_EFFECT_PAGE1_FIELD_32
#define DEBUG_EFFECT_PAGE1_FIELD_32 0x16
#endif

#ifndef DEBUG_EFFECT_PLACE_BUTTON
#define DEBUG_EFFECT_PLACE_BUTTON PAD_BUTTON_CROSS
#endif

/* The preview's edited-operand index, its two operands and its page. The
   operand pair is a byte array, so GCC word-aligns it: one pad byte follows
   the index where the index lands on an odd halfword (0x8009AF2A in the North
   American build) and three where it lands on a word, as in the four
   language builds. */
u8 gDebugEffect_bEditedOperand = 0;
u8 gDebugEffect_abOperands[2] = {0};
u8 gDebugEffect_bPage = 0;
/* "~c0702D EFFECT = %2d %2d\n" */
/* "               **\n~c777\0" */
/* "            **\n~c777\0" */

/* Held, the first speeds up the orbit and view steps and the second the
 * movement. The Japanese release swaps Cross and Circle, so a regional build
 * names its own. */
#ifndef DEBUG_EFFECT_FAST_STEP_BUTTON
#define DEBUG_EFFECT_FAST_STEP_BUTTON PAD_BUTTON_CROSS
#endif
#ifndef DEBUG_EFFECT_FAST_MOVE_BUTTON
#define DEBUG_EFFECT_FAST_MOVE_BUTTON PAD_BUTTON_CIRCLE
#endif

void DebugEffect_UpdateViewControls(void) {
    ViewState *b;
    ViewState *c;
    s32 a;
    s32 v;
    s32 x;
    s32 y;

    if ((gInput_awPadPressed[0] & PAD_BUTTON_SELECT) != 0) {
        gModel_dwFogDrawFlags = gModel_dwFogDrawFlags ^ 1;
    }

    b = &gViewState_Current;

    if ((gInput_awPadRepeat[0] & PAD_BUTTON_L1_R1_MASK) != 0) {
        a = 2;
        if ((gInput_awPadHeld[0] & DEBUG_EFFECT_FAST_STEP_BUTTON) != 0) {
            a = 0x10;
        }
        v = gViewState_Current.distance + a;
        if ((gInput_awPadRepeat[0] & PAD_BUTTON_L1) != 0) {
            v -= a * 2;
        }
        gViewState_Current.distance = v;
        ViewState_ApplyOrbit();
    }

    if ((gInput_awPadRepeat[0] & PAD_DIRECTION_MASK) != 0) {
        if ((gInput_awPadHeld[0] & PAD_BUTTON_TRIANGLE) != 0) {
            a = 2;
            if ((gInput_awPadHeld[0] & DEBUG_EFFECT_FAST_STEP_BUTTON) != 0) {
                a = 0x10;
            }
            if ((gInput_awPadRepeat[0] & PAD_DIRECTION_RIGHT) != 0) {
                b->view.vrx = b->view.vrx + a;
            }
            if ((gInput_awPadRepeat[0] & PAD_DIRECTION_DOWN) != 0) {
                b->view.vrz = b->view.vrz - a;
            }
            if ((gInput_awPadRepeat[0] & PAD_DIRECTION_LEFT) != 0) {
                b->view.vrx = b->view.vrx - a;
            }
            if ((gInput_awPadRepeat[0] & PAD_DIRECTION_UP) != 0) {
                b->view.vrz = b->view.vrz + a;
            }
        } else {
            v = 0x20;
            if ((gInput_awPadHeld[0] & DEBUG_EFFECT_FAST_MOVE_BUTTON) != 0) {
                v = 0x80;
            }
            y = b->angle;
            x = b->orbit_angle;
            if ((gInput_awPadRepeat[0] & PAD_DIRECTION_UP) != 0) {
                x += v;
            }
            if ((gInput_awPadRepeat[0] & PAD_DIRECTION_DOWN) != 0) {
                x -= v;
            }
            if ((gInput_awPadRepeat[0] & PAD_DIRECTION_RIGHT) != 0) {
                y -= v;
            }
            if ((gInput_awPadRepeat[0] & PAD_DIRECTION_LEFT) != 0) {
                y += v;
            }
            c = &gViewState_Current;
            c->angle = y;
            c->orbit_angle = x;
        }
        ViewState_ApplyOrbit();
    }
}

/* Debug display controller: START hands the pad to DebugEffect_UpdateViewControls; on the
   first call it initialises the gDuel_wSceneStateFlags mode flags and the cursor state.
   With the rebuild flag set it releases the two spawned objects and
   recreates the display for the current page (a message box, one or two
   card objects from gDebugEffect_aPreviewCardRecords). Otherwise SELECT cycles the page (0..3),
   left/right toggles which operand the up/down repeat adjusts, and CROSS
   places a new object through DuelEffect_AllocateRequest at a page-dependent position. */
void DebugEffect_UpdatePreview(void) {
    DisplayObject *obj;
    DuelEffectRequest *p;

    if (gInput_awPadHeld[0] & PAD_BUTTON_START) {
        DebugEffect_UpdateViewControls();
    }
    if ((gDuel_wSceneStateFlags & DUEL_SCENE_FLAG_INITIALIZED) == 0) {
        gDuel_wSceneStateFlags |= 0xC000;
        gDebugEffect_bPlacementCounter = 0;
        gDebugEffect_bPage = 0;
        gDebugEffect_bEditedOperand = 0;
        gDebugEffect_abOperands[1] = 0;
        gDebugEffect_pPreviewCard1 = 0;
        gDebugEffect_pPreviewCard0 = 0;
    }
    if (gDuel_wSceneStateFlags & 0x4000) {
        gDuel_wSceneStateFlags &= 0xBFFF;
        DuelEffect_ReleaseResourceObjects(0);
        DisplayObject_ReleaseIfPresent(gDebugEffect_pPreviewCard0);
        DisplayObject_ReleaseIfPresent(gDebugEffect_pPreviewCard1);
        gDebugEffect_pPreviewCard1 = 0;
        gDebugEffect_pPreviewCard0 = 0;
        switch (gDebugEffect_bPage) {
        case 0:
            break;
        case 1:
            CardPreview_RequestResourceTransfer(0, 1);
            File_WaitForTransfers();
            obj = (DisplayObject *)CardPreview_CreateResourceObjects(0, -1, -1);
            obj->xy.h.x = 0x5A;
            obj->xy.h.y = DEBUG_EFFECT_PAGE1_FIELD_32;
            Main_AdvanceFrames(4);
            DuelCard_CaptureRoundedTexture(obj);
            break;
        case 2:
            gDebugEffect_pPreviewCard0 = (u8 *)Duel_CreateCardDisplayObject(gDebugEffect_aPreviewCardRecords, 0x86, 0x52);
            break;
        case 3:
            gDebugEffect_pPreviewCard0 = (u8 *)Duel_CreateCardDisplayObject(gDebugEffect_aPreviewCardRecords, 0x5C, 0x52);
            gDebugEffect_pPreviewCard1 = (u8 *)Duel_CreateCardDisplayObject(&gDebugEffect_aPreviewCardRecords[1], 0xB0, 0x52);
            break;
        }
    } else if (gInput_awPadPressed[0] & PAD_BUTTON_SELECT) {
        gDuel_wSceneStateFlags |= 0x4000;
        gDebugEffect_bPage++;
        if (gDebugEffect_bPage >= 4) {
            gDebugEffect_bPage = 0;
        }
    } else if (gInput_awPadPressed[0] & (PAD_DIRECTION_LEFT | PAD_DIRECTION_RIGHT)) {
        gDebugEffect_bEditedOperand ^= 1;
    } else if (gInput_awPadRepeat[0] & (PAD_DIRECTION_UP | PAD_DIRECTION_DOWN)) {
        gDebugEffect_abOperands[gDebugEffect_bEditedOperand]++;
        if (gInput_awPadRepeat[0] & PAD_DIRECTION_DOWN) {
            gDebugEffect_abOperands[gDebugEffect_bEditedOperand] -= 2;
        }
    } else if (gInput_awPadPressed[0] & DEBUG_EFFECT_PLACE_BUTTON) {
        p = DuelEffect_AllocateRequest(
            gDebugEffect_abOperands[0]);
        gDebugEffect_bPlacementCounter = (gDebugEffect_bPlacementCounter + 1) & 7;
        p->phase = gDebugEffect_abOperands[1];
        switch (gDebugEffect_bPage) {
        case 0:
            p->field_00 = (u16)gDuel_aCardSlotGroundPositions[6].x;
            p->field_02 = -0x18;
            p->field_04 = (u16)gDuel_aCardSlotGroundPositions[6].y;
            break;
        case 1:
            p->field_00 = 0xA0;
            p->field_02 = 0x78;
            break;
        case 2:
            p->field_00 = 0xA0;
            p->field_02 = 0x70;
            break;
        case 3:
            p->field_00 = 0xA0;
            p->field_02 = 0x70;
            break;
        }
    }
}

/* Prints the "EFFECT = %2d %2d" debug line, then one of two divider strings
   depending on the edited operand. */
void DuelScene_UpdateEffectPreview(void) {
    u8 edited_operand;

    DebugEffect_UpdatePreview();
    FntPrint(
        gDebugEffect_szOperandsFormat,
        gDebugEffect_abOperands[0],
        gDebugEffect_abOperands[1]);
    edited_operand = gDebugEffect_bEditedOperand;
    if (edited_operand != 0) {
        FntPrint(gDebugEffect_szSecondOperandMarker);
    } else {
        FntPrint(gDebugEffect_szFirstOperandMarker);
    }
}

#include "display_object_layout.h"
#include "duel_card_layout.h"
#include "duel_card.h"
#include "card_presentation.h"

/* The mirror of Duel_UpdateCardToFieldView: the card turn-back animation step. Picks
 * the mode (+0x2E) from the card record's flags when DisplayObject_MarkInitialized says
 * the object is idle, then runs one of three mode arms -- a countdown that
 * sets the record's 0x400 flag (mode 0), and two rotate-then-settle arms
 * (modes 1 and 2) stepping the angle at +0x2A by +0x28 until the +0x60
 * counter runs out -- and resets through Duel_UpdateCardDisplayState. The
 * reset follows mode 0's countdown and tops a loop that modes 1 and 2 enter
 * through their case labels, so a finished settle continues back into it. */
void Duel_UpdateCardToSelectionView(DuelCardTurnObject *p) {
    DuelCardRecord *e;
    s32 m;
    s32 v;
    s16 c;
    s32 f;

    e = &gDuel_aCardRecords[p->record_index];
    if (DisplayObject_MarkInitialized((DisplayObjectLifecycle *)p) == 0) {
        m = e->flags;
        v = 0;
        if ((m & DUEL_CARD_FLAG_FACE_DOWN) == 0) {
            v = 1;
            if ((m & DUEL_CARD_FLAG_DEFENSE_POSITION) != 0) {
                v = 2;
            }
        }
        p->mode = v;
    }
    switch (p->mode) {
    case 0:
        f = p->phase;
        if ((f & 0x40) == 0) {
            p->phase = f | 0x40;
            p->counter = 8;
            e->flags = e->flags | 0x400;
            p->tint.packed = 0x8000;
            p->flags =
                p->flags | DISPLAY_OBJECT_FLAG_CLIP_TEST;
            if ((e->flags & DUEL_CARD_FLAG_DEFENSE_POSITION) != 0) {
                p->tint.channel[2] = 0xC0;
            }
        }
        c = p->counter - 1;
        p->counter = c;
        if (c == 0) {
            return;
        }
        /* The reset tops a loop that modes 1 and 2 enter through their case
           labels; a finished settle continues back into it. */
        for (;;) {
            p->phase = 0;
            p->field_24 = 0;
            Duel_UpdateCardDisplayState((DuelCardDisplayObject *)p);
            return;
        case 1:
            f = p->phase;
            if ((f & 0x40) == 0) {
                p->phase = f | 0x40;
                p->angle_step = 0x1000;
                p->angle.unsigned_value = 0x80;
                p->tint.packed = 0;
                p->counter = 4;
            }
            if ((p->phase & 0x20) == 0) {
                u16 flags;

                if (gDuel_bActiveSide != 0) {
                    p->angle.unsigned_value = p->angle.unsigned_value - p->angle_step;
                } else {
                    p->angle.unsigned_value = p->angle.unsigned_value + p->angle_step;
                }
                p->tint.channel[2] = p->angle.unsigned_value >> 8;
                c = p->counter - 1;
                p->counter = c;
                if (c != 0) {
                    return;
                }
                p->phase = p->phase | 0x20;
                e->flags = e->flags | 0x400;
                p->tint.packed = 0x4000;
                p->angle.signed_value = 0x4080;
                flags = p->flags;
                p->counter = 4;
                /* Inside the loop the constant 4 would outrank the flags
                   temp for $v0; the deeper store keeps retail's order. */
                do {
                    p->flags = flags | DISPLAY_OBJECT_FLAG_CLIP_TEST;
                } while (0);
                return;
            }
            p->angle.unsigned_value = p->angle.unsigned_value - p->angle_step;
            p->tint.channel[1] = p->angle.unsigned_value >> 8;
            c = p->counter - 1;
            p->counter = c;
            if (c != 0) {
                return;
            }
            p->tint.channel[1] = 0;
            continue;
        case 2:
            f = p->phase;
            if ((f & 0x40) == 0) {
                p->phase = f | 0x40;
                p->tint.packed = 0xC000;
                p->angle_step = 0x1000;
                p->angle.unsigned_value = 0x80;
                p->counter = 4;
            }
            if ((p->phase & 0x20) == 0) {
                u16 flags;

                if (gDuel_bActiveSide != 0) {
                    p->angle.unsigned_value = p->angle.unsigned_value - p->angle_step;
                } else {
                    p->angle.unsigned_value = p->angle.unsigned_value + p->angle_step;
                }
                p->tint.channel[2] = p->angle.unsigned_value >> 8;
                c = p->counter - 1;
                p->counter = c;
                if (c != 0) {
                    return;
                }
                p->phase = p->phase | 0x20;
                e->flags = e->flags | 0x400;
                p->angle.signed_value = -0x3F80;
                p->tint.packed = 0xC000C0;
                flags = p->flags;
                p->counter = 4;
                /* Same allocation steering as mode 1's settle setup. */
                do {
                    p->flags = flags | DISPLAY_OBJECT_FLAG_CLIP_TEST;
                } while (0);
                return;
            }
            p->angle.unsigned_value = p->angle.unsigned_value + p->angle_step;
            p->tint.channel[0] = p->angle.unsigned_value >> 8;
            c = p->counter - 1;
            p->counter = c;
            if (c != 0) {
                return;
            }
            p->tint.channel[0] = 0;
            continue;
        }
    }
}

/* Card flip/turn animation step on a duel card object: picks the animation
 * mode (+0x2E) from the card's record flags when DisplayObject_MarkInitialized says the
 * object is idle, then runs one of three mode arms -- a plain countdown
 * (mode 0), and two rotate-then-settle arms (modes 1 and 2) that step the
 * angle at +0x2A by +0x28 until the +0x60 counter runs out, mirror it
 * according to gDuel_bActiveSide, and reset through Duel_ApplyCardObjectFlags.
 * The reset follows mode 0's countdown and tops a loop that modes 1 and 2
 * enter through their case labels, so a finished settle continues back into
 * it. */
void Duel_UpdateCardToFieldView(DuelCardTurnObject *p) {
    DuelCardRecord *e;
    s32 m;
    s32 v;
    u32 s1;
    u32 s2;
    s32 d;
    s16 c;
    s32 f;

    e = &gDuel_aCardRecords[p->record_index];
    if (DisplayObject_MarkInitialized((DisplayObjectLifecycle *)p) == 0) {
        m = e->flags;
        v = 0;
        if ((m & DUEL_CARD_FLAG_FACE_DOWN) == 0) {
            v = 1;
            if ((m & DUEL_CARD_FLAG_DEFENSE_POSITION) != 0) {
                v = 2;
            }
        }
        p->mode = v;
    }
    switch (p->mode) {
    case 0:
        f = p->phase;
        if ((f & 0x40) == 0) {
            p->phase = f | 0x40;
            p->counter = 8;
        }
        c = p->counter - 1;
        p->counter = c;
        if (c > 0) {
            return;
        }
        e->flags = e->flags & 0xFBFF;
        p->tint.channel[2] = 0x80;
        p->tint.channel[0] = 0;
        p->tint.channel[1] = 0;
        p->tint.packed = 0x800000;
        /* Ends the test's skip outside a loop, so reorg does not treat it
           as a branch to the loop top below and fill its slot from there. */
        do {
            if ((e->flags & DUEL_CARD_FLAG_DEFENSE_POSITION) != 0) {
                p->tint.channel[1] = 0xC0;
            }
        } while (0);
        /* The reset tops a loop that modes 1 and 2 enter through their case
           labels; a finished settle continues back into it. */
        for (;;) {
            p->phase = 0;
            p->field_24 = 0;
            Duel_ApplyCardObjectFlags((DuelCardDisplayObject *)p);
            return;
        case 1:
            f = p->phase;
            if ((f & 0x40) == 0) {
                u16 flags;

                p->phase = f | 0x40;
                p->angle_step = 0x1000;
                p->angle.unsigned_value = 0x80;
                p->tint.packed = 0;
                flags = p->flags;
                p->counter = 4;
                /* Inside the loop the constant 4 would outrank the flags
                   temp for $v0; the deeper store keeps retail's order. */
                do {
                    p->flags = flags | DISPLAY_OBJECT_FLAG_CLIP_TEST;
                } while (0);
            }
            if ((p->phase & 0x20) == 0) {
                s1 = p->angle.unsigned_value + p->angle_step;
                p->angle.unsigned_value = s1;
                p->tint.channel[1] = s1 >> 8;
                c = p->counter - 1;
                p->counter = c;
                if (c != 0) {
                    return;
                }
                p->phase = p->phase | 0x20;
                d = gDuel_bActiveSide;
                e->flags = e->flags & 0xFBFF;
                if (d != 0) {
                    p->tint.packed = 0xC00000;
                    p->angle.signed_value = -0x3F80;
                } else {
                    p->tint.packed = 0x400000;
                    p->angle.signed_value = 0x4080;
                }
                p->counter = 4;
                return;
            }
            if (gDuel_bActiveSide != 0) {
                p->angle.unsigned_value = p->angle.unsigned_value + p->angle_step;
            } else {
                p->angle.unsigned_value = p->angle.unsigned_value - p->angle_step;
            }
            p->tint.channel[2] = p->angle.unsigned_value >> 8;
            c = p->counter - 1;
            p->counter = c;
            if (c != 0) {
                return;
            }
            p->tint.channel[2] = 0;
            continue;
        case 2:
            f = p->phase;
            if ((f & 0x40) == 0) {
                p->phase = f | 0x40;
                p->tint.packed = 0xC00000;
                p->angle_step = 0x1000;
                p->angle.unsigned_value = 0x80;
                p->counter = 4;
            }
            if ((p->phase & 0x20) == 0) {
                s2 = p->angle.unsigned_value - p->angle_step;
                p->angle.unsigned_value = s2;
                p->tint.channel[0] = s2 >> 8;
                c = p->counter - 1;
                p->counter = c;
                if (c != 0) {
                    return;
                }
                p->phase = p->phase | 0x20;
                d = gDuel_bActiveSide;
                e->flags = e->flags & 0xFBFF;
                if (d != 0) {
                    p->tint.packed = 0xC0C000;
                    p->angle.signed_value = -0x3F80;
                } else {
                    p->tint.packed = 0x40C000;
                    p->angle.signed_value = 0x4080;
                }
                p->counter = 4;
                return;
            }
            if (gDuel_bActiveSide != 0) {
                p->angle.unsigned_value = p->angle.unsigned_value + p->angle_step;
            } else {
                p->angle.unsigned_value = p->angle.unsigned_value - p->angle_step;
            }
            p->tint.channel[2] = p->angle.unsigned_value >> 8;
            c = p->counter - 1;
            p->counter = c;
            if (c != 0) {
                return;
            }
            p->tint.channel[0] = 0;
            continue;
        }
    }
}

void Duel_StartViewTween(
    s32 frames, s32 distance, s32 orbit_angle, s32 heading, s32 target_z)
{
    s16 current_distance = gViewState_Current.distance;
    s32 distance_step = ((distance - current_distance) << 16) / frames;
    s16 current_orbit_angle = gViewState_Current.orbit_angle;
    s32 orbit_step = ((orbit_angle - current_orbit_angle) << 16) / frames;
    s16 current_heading = gViewState_Current.angle;
    s32 heading_step = ((heading - current_heading) << 16) / frames;
    s32 current_target_z = gViewState_Current.view.vrz;
    s32 target_z_step = ((target_z - current_target_z) << 16) / frames;

    gDuel_wViewTweenFramesRemaining = frames;
    gDuel_wViewTweenDistanceTarget = distance;
    gDuel_wViewTweenOrbitAngleTarget = orbit_angle;
    gDuel_wViewTweenHeadingTarget = heading;
    gDuel_wViewTweenReferenceZTarget = target_z;
    gDuel_dwViewTweenDistanceAccumulator = (current_distance << 16) | 0x8000;
    gDuel_dwViewTweenOrbitAngleAccumulator = (current_orbit_angle << 16) | 0x8000;
    gDuel_dwViewTweenHeadingAccumulator = (current_heading << 16) | 0x8000;
    gDuel_dwViewTweenReferenceZAccumulator = (current_target_z << 16) | 0x8000;
    gDuel_dwViewTweenDistanceStep = distance_step;
    gDuel_dwViewTweenOrbitAngleStep = orbit_step;
    gDuel_dwViewTweenHeadingStep = heading_step;
    gDuel_dwViewTweenReferenceZStep = target_z_step;
}

void DuelSelection_UpdateLinkedObject(DisplayObject *object)
{
    u8 *base = gDuel_abSelectionSides;
    s32 offset =
        (s16)object->field_2C.h.field_2C * DUEL_SELECTION_RECORD_SIZE;
    DisplayObject *parent;

    offset += gDuel_bActiveSide * DUEL_SELECTION_SIDE_SIZE;
    parent = ((DuelSelectionRecord *)(base + offset))->info_panel;
    if (parent == 0) {
        DisplayObject_ReleaseIfPresent(object);
    } else {
        object->xy.h.x =
            parent->xy.h.x + object->position.h.field_28;
        object->xy.h.y =
            parent->xy.h.y + object->position.h.field_2A;
        if (parent->field_6C == 0) {
            object->field_6C = 0;
            object->update = 0;
        }
    }
}

#include "duel_grid.h"
#include "display_object.h"

void DuelSelection_LinkDisplayObject(
    DuelSelectionRecord *parent,
    volatile DisplayObject *object
)
{
    DisplayObject *base;
    int index;

    if (object != 0) {
        base = parent->info_panel;
        object->position.h.field_28 = object->xy.h.x - base->xy.h.x;
        object->position.h.field_2A = object->xy.h.y - base->xy.h.y;
        index = parent->record_index;
        object->field_6C = 1;
        object->update =
            (DisplayObjectCallback)DuelSelection_UpdateLinkedObject;
        object->field_2C.h.field_2C = index;
    }
}

/*
 * Derive paired cursors inside the loop: `DuelSelection_LinkDisplayObjects`
 *
 * The parent-link routine at `0x80022FF0` matches all 160 bytes with the existing
 * uniform `gcc_2_8_1_g8` profile, without its historical five register pins.
 * Passing `parent` and `clear` directly, rather than copying the arguments into
 * locals, first removes the saved-register displacement. Maintaining two
 * independent 12-byte cursors still exchanges `$s0` and `$s1` at nine instructions.
 * Deriving the second stored pointer from `entry->order_marker` inside each
 * iteration lets ordinary loop strength reduction allocate both cursors as
 * retail does.
 *
 * The remaining two-word discriminator is the increment order:
 * `entry++, slot++` gives retail's second-cursor increment before its slot
 * increment. Reversing those source increments swaps instructions at `+0x6C`
 * and `+0x70`. The source configures the base object first, then both objects in
 * each of five hand slots, through `DuelSelection_LinkDisplayObject`; it always
 * clears `parent->cursor_object` and only clears the two slot pointers when requested.
 * The canonical history and six terminal refinement rows remain unchanged;
 * one post-terminal resolution records the newly verified source structure.
 */
void DuelSelection_LinkDisplayObjects(DuelSelectionRecord *parent, s32 clear)
{
    DuelHandSlot *entry;
    DisplayObject **child;
    s32 slot;

    entry = parent->hand;
    if (entry != 0) {
        DuelSelection_LinkDisplayObject(parent, parent->cursor_object);
        parent->cursor_object = 0;
        for (slot = 0; slot < DUEL_FIELD_ROW_SIZE; entry++, slot++) {
            child = &entry->order_marker;
            DuelSelection_LinkDisplayObject(parent, entry->object);
            DuelSelection_LinkDisplayObject(parent, *child);
            if (clear) {
                entry->object = 0;
                *child = 0;
            }
        }
    }
}

#include "card_constants.h"
#include "duel_grid.h"
#include "ai.h"
#include "display_projection.h"

#include "duel_card.h"
#include "card_presentation.h"
#include "display_object.h"
#include "text_staging.h"
#include "text_message_staging.h"
#include "duel_card_staging.h"
#include "sound.h"

/* Regional value: the 0xF0 passed to Duel_CreateFieldCardPanel and stored in position
 * field_2A, the screen height (0xF0 -> 0x100). The European build
 * (src/game/european/) defines its own. */
#ifndef DUEL_FIELD_SCREEN_HEIGHT
#define DUEL_FIELD_SCREEN_HEIGHT 0xF0
#endif

s32 DuelField_GetGuardianStarDisplayCode(DuelSelectionRecord *cursor_a, DuelSelectionRecord *cursor_b)
{
    u8 *grid = gDuel_abFieldGridCardRecordIndices.flat;
    DuelCardRecord *records;
    s32 page;
    s32 offset_a;
    s32 row_b;
    s32 index_a;
    s32 index_b;
    s32 slot_a;
    s32 slot_b;
    s32 side;
    s32 order;

    row_b = cursor_b->row;
    index_a = cursor_a->row * DUEL_FIELD_ROW_SIZE + cursor_a->col;
    side = gDuel_bActiveSide;
    page = side * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
    slot_a = grid[index_a + page];
    /* Retail computes the typed byte offset before loading the record base. */
    offset_a = slot_a * sizeof(DuelCardRecord);
    records = gDuel_aCardRecords;
    index_b = row_b * DUEL_FIELD_ROW_SIZE + cursor_b->col;
    slot_b = grid[index_b + page];

    order = Duel_CalcGuardianStarBonus(
        (DuelCardRecord *)((u8 *)records + offset_a),
        &records[slot_b]
    );

    if (order == 0) {
        return 4;
    }
    if (order < 0) {
        return 1;
    }
    return 6;
}

void DuelField_CreateCardInfoTextBox(DuelSelectionRecord *source, s32 card_record_index)
{
    DuelCardRecord *record = &gDuel_aCardRecords[card_record_index];
    DuelEffectChannel *box;
    s32 *table;
    s32 style;
    s32 id;
    s32 value;
    s32 stats;

    gText_BranchSelectorA = 0;
    gText_BranchSelectorB = 0;
    style = 0x50;
    if (record->flags & 0x8000) {
        table = gDuel_adwCardStats;
        id = record->card_id;
        /* table[id - 1] folds the -1 into the load offset; retail subtracts first. */
        stats = *(s32 *)((s32)table + ((id - 1) << 2));
        gText_BranchSelectorA = 1;
        gDuel_wSelectedCardID = id;
        if (((stats >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK) <
            CARD_TYPE_MAGIC) {
            if (record->flags & 0x200) {
                gText_GuardianGlyphCode =
                    (stats >> CARD_STAT_GUARDIAN_STAR_2_SHIFT) &
                    CARD_STAT_GUARDIAN_STAR_MASK;
            } else {
                gText_GuardianGlyphCode =
                    (stats >> CARD_STAT_GUARDIAN_STAR_1_SHIFT) &
                    CARD_STAT_GUARDIAN_STAR_MASK;
            }
            gText_GuardianGlyphCode = gText_GuardianGlyphCode + 0x17;
            value = Duel_CalcCardStats(record);
            gText_StagedValues[0].card_stats.attack = (s16)value;
            gText_StagedValues[0].card_stats.defense = value >> 16;
        } else {
            style = 0x51;
        }
        if (card_record_index % DUEL_CARD_SIDE_RECORD_COUNT >= DUEL_FIELD_ROW_SIZE) {
            gText_BranchSelectorA = 2;
            if (record->flags & 0x1000) {
                gText_BranchSelectorA = 3;
            }
            value = card_record_index >= DUEL_CARD_SIDE_RECORD_COUNT;
            if (value == gDuel_bActiveSide &&
                gDuel_pActiveSideState->card_view_mode == 0) {
                gText_BranchSelectorA = 2;
            }
        } else {
            if (record->flags & 0x2000) {
                gText_BranchSelectorA = 0;
            }
        }
    }
    value = source->record_index;
    if (value == 3) {
        s32 rank;
        TextStagingValues *dst = gText_StagedValues;

        style += 4;
        rank = gDuel_aSideStates[gDuel_bActiveSide ^ 1].swords_turns_remaining;
        dst->card_stats.rank = rank;
        if (rank != 0) {
            if (rank < 0 || rank > 3) {
                dst->card_stats.rank = 3;
            }
            gText_BranchSelectorB = 1;
        }
        if (gText_BranchSelectorA != 0) {
            gText_BranchSelectorB = gText_BranchSelectorB | 2;
            gText_SelectedCardColorIndex = DuelField_GetGuardianStarDisplayCode(
                source,
                (DuelSelectionRecord *)
                    &(gDuel_abSelectionSides + 2 * DUEL_SELECTION_RECORD_SIZE)[gDuel_bActiveSide * DUEL_SELECTION_SIDE_SIZE]
            );
        }
    }
    if (source->record_index == 2 && source->field_actions_mode != 0) {
        s32 rank;
        TextStagingValues *dst;
        s32 side;

        side = gDuel_bActiveSide;
        if (source->row < 2) {
            side = side ^ 1;
        }
        dst = gText_StagedValues;
        rank = gDuel_aSideStates[side].swords_turns_remaining;
        dst->card_stats.rank = rank;
        if (rank != 0) {
            if (rank < 0 || rank > 3) {
                dst->card_stats.rank = 3;
            }
            gText_BranchSelectorB = 1;
        }
        style += 2;
    }
    box = TextBox_Create(
        source->text_channel, style,
        (s16)source->info_panel->xy.h.x + 0x10,
        (s16)source->info_panel->xy.h.y + source->text_y_offset,
        0x120, 0x40
    );
    box->depth_offset = (u8)source->info_panel->depth_offset + 1;
    TextBox_BuildBlocking((struct DuelEffectChannel *)box);
}

void DuelField_RefreshCursorCardInfo(DuelSelectionRecord *source)
{
    u8 *table = gDuel_abFieldGridCardRecordIndices.flat;
    s32 index =
        source->row * DUEL_FIELD_ROW_SIZE + source->col +
        gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;

    DuelField_CreateCardInfoTextBox(source, table[index]);
}

void Duel_CreateFieldCursorObject(DuelSelectionRecord *source)
{
    s32 index;
    DisplayObject *object;
    DuelFieldPosition *position;

    index = source->row * DUEL_FIELD_ROW_SIZE + source->col;
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteResource(
        object,
        4,
        3,
        source->record_index + gDuel_bActiveSide * 4,
        0x1F,
        0x100
    );

    {
        u8 *base = (u8 *)gDuel_aFieldGridGroundPositions;
        s32 offset = (u32)&((DuelFieldPosition *)0)[index] +
            gDuel_bActiveSide * DUEL_FIELD_SIDE_POSITION_BYTES;

        position = (DuelFieldPosition *)(base + offset);
    }
    object->position.h.field_28 = position->x;
    object->position.h.field_2A = position->y;
    object->flags = object->flags |
                    DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                    DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    object->update = (DisplayObjectCallback)DisplayObject_ProjectGroundPosition;
    source->cursor_object = object;
}

#define DUEL_FIELD_DISPLAY_CHANNEL(index) (&gText_aChannels[index])

/* The empty do-while loops are scheduling barriers. The first keeps the
 * gDuel_wDisplayTransitionFlags load above the entry test and `result = 0` below the view
 * address, so the zero lands in $v0 for the early exit. The second follows
 * the 0x8000 test, so `busy = 0` stays a literal zero rather than a copy of
 * that $v0. Each replay loop has its own gDuel_abCardStagingBuffer pointer, set once and
 * read once, as retail's loop-entry address loads require. */
s32 Duel_UpdateDisplayTransition(void)
{
    DuelSelectionRecord *source;
    ViewState *view;
    DisplayObject *o;
    s32 i;
    s32 moving;
    s32 busy;
    s32 x;
    s32 y;
    s32 z;
    s32 w;
    s32 t;
    s16 count;
    u8 *records;
    u8 *replay;
    s32 result;
    u16 flags;

    flags = gDuel_wDisplayTransitionFlags;
    source = gDuel_pActiveSelectionRecord;
    view = &gViewState_Current;
    do {
    } while (0);
    result = 0;
    if (flags != 0) {
        x = flags & 0x8000;
        do {
        } while (0);
        busy = 0;
        if (x == 0) {
            gDuel_wDisplayTransitionFlags = flags | 0x8000;
            if (gDuel_wDisplayTransitionFlags & 1) {
                Duel_StartViewTween(0x10, 0x14E, 0x3FE, gDuel_awSideViewHeadings[gDuel_bActiveSide],
                              gDuel_awFieldGridRowViewTargetZ[gDuel_bActiveSide][source->field_actions_mode][source->row]);
            }
            if (gDuel_wDisplayTransitionFlags & 4) {
                Duel_StartViewTween(0x10, 0x258, 0x100, gDuel_awSideViewHeadings[gDuel_bActiveSide], 0);
                DisplayObject_ReleaseIfPresent(source->cursor_object);
                source->cursor_object = 0;
                records = gDuel_abCardStagingBuffer;
                for (i = 5; i < 30; i++) {
                    if (i % DUEL_CARD_SIDE_RECORD_COUNT >= DUEL_FIELD_ROW_SIZE &&
                        (gDuel_aCardRecords[i].flags & 0x8000)) {
                        o = ((DuelCardReplayRecordBlock *)(
                            (u8 *)&((DuelCardRecord *)records)[i] +
                            DUEL_CARD_STAGING_REPLAY_BASE_OFFSET))->record.object;
                        o->field_6C = 1;
                        o->field_60 = 4;
                        o->update = (DisplayObjectCallback)Duel_UpdateCardToFieldView;
                    }
                }
                gDuel_wDisplayTransitionFlags |= 0x4000;
            }
            if (gDuel_wDisplayTransitionFlags & 2) {
                o = Duel_CreateFieldCardPanel(0, DUEL_FIELD_SCREEN_HEIGHT);
                source->text_y_offset = 0x24;
                if (source->panel_variant != 0) {
                    DisplayObject_SetResourceVariant(o, source->panel_variant);
                    source->text_y_offset = 7;
                }
                DisplayObject_SetDepthOffset(o, -(source->record_index * 2) - 2);
                o->position.h.field_28 = 0;
                o->position.h.field_2A = source->panel_target_y;
                DisplayObject_SavePosition((DisplayObjectSnapshot *)o);
                o->field_60 = -0x400;
                o->field_6C = 2;
                source->info_panel = o;
                if (source->hand != 0) {
                    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
                        if (source->hand[i].object != 0) {
                            source->hand[i].object->xy.h.x =
                                o->xy.h.x + (i * 0x3C + 0xE);
                            source->hand[i].object->xy.h.y =
                                o->xy.h.y - 0x1C;
                        }
                    }
                }
                DuelSelection_LinkDisplayObjects(source, 0);
            }
            if (gDuel_wDisplayTransitionFlags & 8) {
                o = source->info_panel;
                o->position.h.field_2A = DUEL_FIELD_SCREEN_HEIGHT;
                o->position.h.field_28 = o->xy.h.x;
                DisplayObject_SavePosition((DisplayObjectSnapshot *)o);
                o->field_60 = 0x400;
                o->field_6C = 2;
                DuelSelection_LinkDisplayObjects(source, 1);
            }
            busy = 0;
        }
        moving = busy;
        if (gDuel_wDisplayTransitionFlags & 0x40) {
            o = source->cursor_object;
            if (o->field_60 != 0) {
                t = ((s16)o->position.h.field_28 << 8) | o->pad_62[0];
                t += o->field_34.h.field_36;
                o->pad_62[0] = t;
                o->position.h.field_28 = t >> 8;
                t = ((s16)o->position.h.field_2A << 8) | o->field_64;
                t += o->field_38.h.field_3A;
                o->position.h.field_2A = t >> 8;
                o->field_64 = t;
                o->field_60--;
                busy = 1;
                if (o->field_60 <= 0) {
                    busy = 0;
                    o->field_60 = 0;
                    o->position.word = o->field_2C.word;
                }
            }
        }
        o = source->info_panel;
        if ((gDuel_wDisplayTransitionFlags & 2) && o->field_6C != 0) {
            busy = 1;
            moving = busy;
            Widget_SlideSine((DisplayObjectPosition *)o,
                             (s16)o->position.h.field_28,
                             (s16)o->position.h.field_2A, o->field_60);
            o->field_60 += 0x40;
            if (o->field_60 >= 0) {
                o->field_6C = 0;
                o->xy.word = o->position.word;
            }
        }
        if ((gDuel_wDisplayTransitionFlags & 8) && o->field_6C != 0) {
            busy = 1;
            Widget_SlideSine((DisplayObjectPosition *)o,
                             (s16)o->position.h.field_28,
                             (s16)o->position.h.field_2A, o->field_60);
            o->field_60 -= 0x40;
            moving = busy;
            if (o->field_60 < 0) {
                TextBox_Destroy(DUEL_FIELD_DISPLAY_CHANNEL(source->text_channel));
                DisplayObject_ReleaseIfPresent(o);
                source->info_panel = 0;
                if (source->hand != 0) {
                    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
                        DisplayObject_ReleaseIfPresent(source->hand[i].object);
                        source->hand[i].object = 0;
                        DisplayObject_ReleaseIfPresent(source->hand[i].order_marker);
                        source->hand[i].order_marker = 0;
                    }
                }
                o->field_6C = 0;
                moving = 0;
            }
        }
        if (moving != 0) {
            TextBox_SetPos(DUEL_FIELD_DISPLAY_CHANNEL(source->text_channel),
                           (s16)o->xy.h.x + 0x10,
                           (s16)o->xy.h.y + source->text_y_offset);
        }
        if (gDuel_wDisplayTransitionFlags & 0x4000) {
            busy = 1;
            if (DisplayObject_FindAllocatedByTag(1) != 0) {
                return busy;
            }
            gDuel_wDisplayTransitionFlags &= 0xBFFF;
        }
        if (gDuel_wViewTweenFramesRemaining != 0) {
            count = (u16)gDuel_wViewTweenFramesRemaining - 1;
            busy = 1;
            gDuel_wViewTweenFramesRemaining = count;
            x = gDuel_dwViewTweenDistanceAccumulator += gDuel_dwViewTweenDistanceStep;
            y = gDuel_dwViewTweenOrbitAngleAccumulator += gDuel_dwViewTweenOrbitAngleStep;
            z = gDuel_dwViewTweenHeadingAccumulator += gDuel_dwViewTweenHeadingStep;
            w = gDuel_dwViewTweenReferenceZAccumulator += gDuel_dwViewTweenReferenceZStep;
            view->distance = x >> 16;
            view->orbit_angle = y >> 16;
            view->angle = z >> 16;
            view->view.vrz = w >> 16;
            if (count == 0) {
                view->distance = gDuel_wViewTweenDistanceTarget;
                view->orbit_angle = gDuel_wViewTweenOrbitAngleTarget;
                view->angle = gDuel_wViewTweenHeadingTarget;
                view->view.vrz = gDuel_wViewTweenReferenceZTarget;
                if (gDuel_wDisplayTransitionFlags & 1) {
                    replay = gDuel_abCardStagingBuffer;
                    for (i = 5; i < 30; i++) {
                        if (i % DUEL_CARD_SIDE_RECORD_COUNT >= DUEL_FIELD_ROW_SIZE &&
                            (gDuel_aCardRecords[i].flags & 0x8000)) {
                            o = ((DuelCardReplayRecordBlock *)(
                                (u8 *)&((DuelCardRecord *)replay)[i] +
                                DUEL_CARD_STAGING_REPLAY_BASE_OFFSET))->record.object;
                            o->field_6C = 1;
                            o->field_60 = 4;
                            o->update = (DisplayObjectCallback)Duel_UpdateCardToSelectionView;
                        }
                    }
                    gDuel_wDisplayTransitionFlags |= 0x4000;
                }
            }
            ViewState_ApplyOrbit();
        }
        if (busy == 0) {
            if (gDuel_wDisplayTransitionFlags & 1) {
                Duel_CreateFieldCursorObject(source);
                DuelField_RefreshCursorCardInfo(source);
            }
            gDuel_wDisplayTransitionFlags = 0;
        }
        result = busy;
    }
    return result;
}

void DuelCursor_ProcessGridDirection(DuelSelectionRecord *o, s32 dir) {
    DisplayObject *d;
    s32 pos;
    s32 shift;
    s32 index;
    s32 q;

    d = o->cursor_object;
    if (o->status & 0x80) {
        if (gDuel_wDisplayTransitionFlags != 0) {
            return;
        }
        /* The record is deliberately left in $a0 rather than passed. */
        ((void (*)(void))DuelField_RefreshCursorCardInfo)();
        o->status &= 0x3F;
        SD_SEPlayFull(6);
        return;
    }
    if (dir < 0) {
        return;
    }
    if (dir & 1) {
        pos = o->row + 1;
        if (dir & 2) {
            pos = o->row - 1;
        }
        if (pos >= (s8)o->row_end) {
            return;
        }
        if (pos < (s8)o->row_min) {
            return;
        }
        shift = 0x10;
        if ((o->row ^ pos) != 3) {
            shift = 8;
        }
        Duel_StartViewTween(shift, 0x14E, 0x3FE, gDuel_awSideViewHeadings[gDuel_bActiveSide],
                      gDuel_awFieldGridRowViewTargetZ[gDuel_bActiveSide][o->field_actions_mode][pos]);
        o->row = pos;
        index = pos * DUEL_FIELD_ROW_SIZE + o->col;
        DisplayObject_ResetVelocity((DisplayObjectVelocity *)d);
        d->field_6C = 0;
        d->field_60 = shift;
        d->field_2C.h.field_2C = (s16)d->position.h.field_28;
        d->field_2C.h.field_2E = gDuel_aFieldGridGroundPositions[gDuel_bActiveSide][index].y;
        q = ((d->field_2C.h.field_2E - (s16)d->position.h.field_2A) << 8) / shift;
        gDuel_wDisplayTransitionFlags = 0x50;
        d->field_38.h.field_3A = q;
        o->status |= 0xC0;
    } else {
        pos = o->col + 1;
        if (dir & 2) {
            pos = o->col - 1;
        }
        if ((u32)pos >= DUEL_FIELD_ROW_SIZE) {
            return;
        }
        o->col = pos;
        index = o->row * DUEL_FIELD_ROW_SIZE + (s8)pos;
        DisplayObject_ResetVelocity((DisplayObjectVelocity *)d);
        shift = 8;
        d->field_60 = shift;
        d->field_6C = 0;
        d->field_2C.h.field_2C = gDuel_aFieldGridGroundPositions[gDuel_bActiveSide][index].x;
        gDuel_wDisplayTransitionFlags = 0x40;
        d->field_2C.h.field_2E = (s16)d->position.h.field_2A;
        d->field_34.h.field_36 =
            (((s16)d->field_2C.h.field_2C - (s16)d->position.h.field_28) << shift) / shift;
        o->status |= 0x80;
    }
}

#include "input.h"

/* Priority-encodes gInput_awPadHeld[0]'s direction bits into a small index (-1 if
   none are set; otherwise whichever bit, checked in RIGHT/DOWN/LEFT/UP
   order, was seen last wins). Reloads the volatile global on every check
   rather than caching it in a register, matching the ROM's per-check reload. */
void DuelCursor_ProcessHeldGridDirection(DuelSelectionRecord *cursor) {
    s32 direction = -1;

    if (gInput_awPadHeld[0] & PAD_DIRECTION_MASK) {
        if (gInput_awPadHeld[0] & PAD_DIRECTION_RIGHT) {
            direction = 0;
        }
        if (gInput_awPadHeld[0] & PAD_DIRECTION_DOWN) {
            direction = 1;
        }
        if (gInput_awPadHeld[0] & PAD_DIRECTION_LEFT) {
            direction = 2;
        }
        if (gInput_awPadHeld[0] & PAD_DIRECTION_UP) {
            direction = 3;
        }
    }

    DuelCursor_ProcessGridDirection(cursor, direction);
}

/* The two callers consume different duel_scene.h contracts, and both
   reasons are deliberate.

   duel_action_ready_marker.c calls DuelCursor_UpdateDirection through
   DuelCursorUpdateDirectionWithDir with two arguments while the definition takes
   one; the second reaches $a1 and the call site's argument setup is what the
   image contains, so the one-argument prototype would remove an instruction.

   duel_update_card_pick_cursor.c records that neither of its two call sites
   narrows the result -- in its listing each jal is followed by its delay slot
   and then a branch on $v0, with no andi or sll between -- so the widened
   value in $v0 is the callee's own. DuelCursor_UpdateFromInput ends
   `lbu $v0, 0x19($s0)` and its value really is a byte; the s32 return spelling
   here preserves it widened, which is what that call site reads.

   DuelCursor_UpdateDirection's one-argument call to DuelCursor_ProcessGridDirection is the harder case of
   the two, and it is now a call through the DuelCursorProcessGridDirectionAmbient type in
   duel_scene.h rather than a private declaration here. The definition
   reads dir four times, branching on its sign and on bits 0 and 1, while
   DuelCursor_UpdateDirection's prologue is subu/sw/sw/jal with the object copy in the
   delay slot and never assigns $a1: dir arrives as whatever
   DuelCursor_UpdateDirection's own caller left there. There is no expression
   this file could write for it, because the call site does not compute one.
   Compare
   DisplayObject_ResetVelocity, where the same shape IS fixable: its callee
   needs only $a0 and $a0 already holds the object, so naming the argument
   costs nothing. The test is whether every register the callee reads holds a
   value this file can name. */

s32 DuelCursor_UpdateFromInput(DuelSelectionRecord *object)
{
    DuelCursor_ProcessHeldGridDirection(object);
    return object->status;
}

s32 DuelCursor_UpdateDirection(DuelSelectionRecord *object)
{
    ((DuelCursorProcessGridDirectionAmbient)DuelCursor_ProcessGridDirection)(object);
    return object->status;
}

#include "duel_grid.h"
#include "duel_card.h"
#include "input.h"

/* Regional value: the record's panel_target_y (0x74 -> 0x84). The European build
 * (src/game/european/) defines its own. */
#ifndef DUEL_PICK_PANEL_TARGET_Y
#define DUEL_PICK_PANEL_TARGET_Y 0x74
#endif

/* The absolute spellings below are load-bearing (see the two stores in
 * Duel_UpdateCardPickCursor); the addresses are named so a regional build can
 * supply its own. */
#ifndef DUEL_VIEWER_CARD_ID_ADDRESS
#define DUEL_VIEWER_CARD_ID_ADDRESS 0x8009B246
#endif

#ifndef DUEL_VIEWER_Y_OFFSET_ADDRESS
#define DUEL_VIEWER_Y_OFFSET_ADDRESS 0x8009B24B
#endif

/* Per-frame step for the "pick a card off the field" cursor.
 *
 * gDuelCursor_PickFlags (0x2CC($gp)) is the mode byte for this cursor:
 *   0x80  the cursor has been initialised for this activation
 *   0x40  a pick has just been committed; hold until the display request clears
 *   0x20  the whole cursor mode should be torn down once 0x40 clears
 * gDuel_wDisplayTransitionFlags (0x25A($gp)) is the request/status word
 * that gates the 0x40 hold, not a separate countdown.
 *
 * First entry (0x80 clear) arms the cursor: mode becomes 0x80|0x40, the
 * object's own state fields are reset, and display request 0x3 is started.
 *
 * While 0x40 is set the cursor is frozen: nothing happens until the request
 * clears, then 0x40 is cleared and -- if 0x20 was also set -- the whole
 * mode byte is zeroed, ending the cursor.
 *
 * Otherwise the cursor is live. DuelCursor_UpdateFromInput refreshes the
 * object's status byte and returns nonzero while it is still busy; when it is
 * idle, the cell under the cursor is looked up in the field table (row *
 * DUEL_FIELD_ROW_SIZE + column, plus DUEL_FIELD_SIDE_GRID_SLOT_COUNT per
 * player side) and the record it names is offered to
 * Duel_GetCardViewerRequestId. A nonzero result is a successful pick: it is
 * published in gDuel_wViewerCardID along with the
 * event code 0x14 and state 2. A zero result only re-arms the hold (display
 * request 0xC, mode |= 0x60) when neither L2 nor R2 is held.
 */

/* gp-relative in the target (0x2CC/0x2CD/0x25A($gp)), so plain scalars. */

/* Absolute in the target, so array-typed to keep them out of small data. */

/* Neither call site narrows the result: each jal is followed by its delay
   slot and then a branch on $v0, with no andi or sll between, so the widened
   value is the callee's own. DuelCursor_UpdateFromInput's listing ends `lbu $v0,
   0x19($s0)` and really returns u8; Duel_GetCardViewerRequestId's non-zero exit is `lh $v0,
   0xC($a2)` and really returns s16. Both are declared s32, in
   duel_scene.h and duel_get_card_viewer_request_id.h, which this file includes. */
void Duel_UpdateCardPickCursor(DuelSelectionRecord *o) {
    u8 f;
    s32 picked;

    f = gDuelCursor_PickFlags;
    if (!(f & 0x80)) {
        gDuelCursor_PickFlags = f | 0xC0;
        o->row_end = 4;
        o->panel_target_y = DUEL_PICK_PANEL_TARGET_Y;
        o->field_actions_mode = 0;
        o->row_min = 0;
        o->status = 0;
        gDuel_wDisplayTransitionFlags = 3;
    }
    f = gDuelCursor_PickFlags;
    if (f & 0x40) {
        if (gDuel_wDisplayTransitionFlags == 0) {
            gDuelCursor_PickFlags = f & 0xBF;
            if (f & 0x20) {
                gDuelCursor_PickFlags = 0;
            }
        }
    } else if (DuelCursor_UpdateFromInput(o) == 0) {
        picked = Duel_GetCardViewerRequestId(
            &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices.flat[
                o->row * DUEL_FIELD_ROW_SIZE + o->col +
                gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT
            ]]);
        if (picked != 0) {
            /* Written as absolute literals, not through externs: these two
               are single incidental stores and the target addresses them
               through $at, which the symbolic form does not do. */
            *(u16 *) DUEL_VIEWER_CARD_ID_ADDRESS = picked;
            *(u8 *) DUEL_VIEWER_Y_OFFSET_ADDRESS = 0x14;
            gDuel_bEffectState = DUEL_EFFECT_STATE_CARD_VIEWER;
        } else if (!(gInput_awPadHeld[0] & PAD_BUTTON_TRIGGER_MASK)) {
            gDuel_wDisplayTransitionFlags = 0xC;
            gDuelCursor_PickFlags = gDuelCursor_PickFlags | 0x60;
        }
    }
}

#include "input.h"
#include "sound.h"
#include "rand_get_interval.h"
#include "../psyq/rand.h"
#include "card_constants.h"

/* Initialized data this unit owns at 0x80090998: the duel scene's phase callback table.
 *
 * DuelScene_Update (src/game/duel_scene_update.c) materializes this address
 * itself and
 * calls `gDuel_apfnSceneStateHandler[gDuel_wSceneStateFlags &
 * DUEL_SCENE_PHASE_MASK]()`, so the table is reached only through the
 * low four bits of that state word.
 *
 * It is written here rather than resolved out of the blob at 0x800908A0
 * because every entry is a function this tree already names. Matching
 * callbacks use their subsystem headers. The unmatched ones are
 * declared in unmatched.h rather than here: this file only takes their
 * addresses, but that is the same thing the entries already in that header
 * do, so a local declaration would just be a second place for them to be
 * spelled.
 *
 * The table is fifteen entries and the mask permits sixteen. Index 15 would
 * read the first word of duel_terrain_boost, which begins immediately after
 * it. That is recorded rather than corrected: nothing here establishes that
 * the game ever produces index 15, and widening the array would change the
 * bytes. */

void (*gDuel_apfnSceneStateHandler[])(void) = {
    DuelScene_UpdateEffectPreview,
    DuelScene_UpdateStartup,
    DuelScene_UpdateDrawPhase,
    DuelScene_UpdateDrawResolution,
    DuelScene_UpdateHandActions,
    DuelScene_UpdateFieldActions,
    DuelScene_UpdateCardUse,
    DuelScene_UpdateCardPlacement,
    DuelScene_UpdateCardMove,
    DuelScene_UpdateBattle,
    DuelScene_UpdateTurnSwitch,
    DuelScene_UpdateResume,
    DuelScene_UpdateResultOutro,
    DuelScene_UpdateResultRewards,
    DuelScene_UpdateExodiaResult,
};

/* One frame of the duel scene. It services the quit dialog when
 * gDuel_bQuitDialogState is live -- creating the box on the first frame and
 * tearing it down once its 0x2000 flag clears -- and otherwise dispatches the
 * current scene step through the scene callback table. */
/* The quit dialog's box. The Japanese and European builds draw it at another
   size, so a regional build supplies its own. */
#ifndef DUEL_QUIT_BOX_X
#define DUEL_QUIT_BOX_X 0x78
#define DUEL_QUIT_BOX_WIDTH 0x50
#define DUEL_QUIT_BOX_HEIGHT 0x24
#endif
/* Its flags: 0x40 in the European build. */
#ifndef DUEL_QUIT_BOX_FLAGS
#define DUEL_QUIT_BOX_FLAGS 0x20
#endif

void DuelScene_Update(void)
{
    u8 value;
    DuelEffectChannel *window;

    if (gDuel_wDisplayTransitionFlags != 0) {
        Duel_UpdateDisplayTransition();
    }
    DuelEffect_UpdateRequests();
    value = gDuel_bEffectRequestStatus;
    if (value & DUEL_EFFECT_REQUEST_STATUS_ACTIVE) {
        if (value & DUEL_EFFECT_REQUEST_STATUS_BLOCKING) {
            return;
        }
        gDuel_bEffectRequestStatus =
            value & ~DUEL_EFFECT_REQUEST_STATUS_ACTIVE;
    }
    if (DuelEffect_UpdateCardEffect() != 0 || DuelEffect_UpdateState() != 0) {
        return;
    }
    value = gDuel_bQuitDialogState;
    if (value != 0) {
        if (!(value & 0x80)) {
            gDuel_bQuitDialogState = value | 0x80;
            SD_SEPlayFull(0x30);
            window =
                TextBox_CreateFlagged(3, 0x22, DUEL_QUIT_BOX_X, 0x58,
                                      DUEL_QUIT_BOX_WIDTH,
                                      DUEL_QUIT_BOX_HEIGHT, DUEL_QUIT_BOX_FLAGS);
            do {
                TextBox_UpdateChannels();
            } while (window->prompt_object == 0);
        } else {
            DuelEffectChannel *cleanup;

            TextBox_UpdateChannels();
            cleanup = TEXT_CHANNEL(3);
            if (cleanup->flags_34 & 0x2000) {
                TextBox_Destroy(cleanup);
                gDuel_bQuitDialogState = 0;
                if (gDialog_bChoice != 0) {
                    gDuel_wOutcomeFlags |= 0x2000;
                }
            }
        }
    } else {
        void (**callbacks)(void);
        u16 index;

        callbacks = gDuel_apfnSceneStateHandler;
        index = gDuel_wSceneStateFlags;
        callbacks[index & DUEL_SCENE_PHASE_MASK]();
        if (!(gDuel_wSceneStateFlags & DUEL_SCENE_FLAG_INITIALIZED)) {
            gDuel_bSceneSubStep = 0;
        }
    }
}

void DuelScene_UpdateWithSideInput(void)
{
    int value = 0;

    if (gDuel_bOpponentID < 0) {
        value = gDuel_bActiveSide;
        if (gDuel_bOutroWinnerSide >= 0) {
            value = gDuel_bOutroWinnerSide;
        }
    }
    if (value != 0) {
        Input_BackupPad1AndUsePad2();
        DuelScene_Update();
        Input_RestorePad1FromBackup();
    } else {
        DuelScene_Update();
    }
}

void Duel_ShuffleDeck(const u16 *src, u16 *out16, u8 *out8) {
    u8 buf[CARD_COUNT];
    u16 *b16;
    u8 *b8;
    u16 *e;
    u16 *t;
    u8 *q;
    s32 i;
    s32 n;
    s32 acc;
    s32 lim;
    s32 x;
    s32 y;
    s32 t16;
    s32 t8;
    s32 u16b;
    s32 u8b;

    b16 = out16;
    b8 = out8;

    if (src == 0) {
        for (i = CARD_COUNT - 1; i >= 0; i--) {
            buf[i] = 0;
        }

        t = gDuel_awOpponentDeckPool;
        n = 0;
        while (n < DECK_SIZE) {
            lim = (rand() & (DUEL_DROP_WEIGHT_TOTAL - 1)) + 1;
            acc = 0;
            i = 0;
            e = t;
            while (1) {
                acc += *e;
                if (acc >= lim) {
                    q = buf + i;
                    if (*q < DECK_CARD_COPY_LIMIT) {
                        *out16 = i + 1;
                        *out8 = n;
                        n++;
                        out16 += 1;
                        out8 += 1;
                        /* `t16` is the block-copy temp forty lines down; its
                         * live range resumes there, and borrowing the name
                         * here is 10 differences against 4. A fresh local is
                         * not the same thing (the permuter found this). */
                        t16 = *q + 1;
                        *q = t16;
                    }
                    break;
                }
                i++;
                e += 1;
                if (i >= STARTER_DECK_WEIGHT_SCAN_COUNT) {
                    break;
                }
            }
        }
    } else {
        for (i = 0; i < DECK_SIZE; i++) {
            *out16 = *src;
            src += 1;
            *out8 = i;
            out8++;
            out16 += 1;
        }
    }

    for (i = 0; i < DUEL_DECK_SHUFFLE_SWAP_COUNT; i++) {
        x = Rand_GetInterval(DECK_SIZE);
        y = Rand_GetInterval(DECK_SIZE);
        t16 = t8 = b16[x];
        u16b = b16[y];
        b16[x] = u16b;
        b16[y] = t16;
        t8 = b8[x];
        u8b = b8[y];
        b8[x] = u8b;
        b8[y] = t8;
    }
}

void Duel_ShuffleBothDecks(const u16 *player_deck, const u16 *opponent_deck)
{
    u16 *base = gDuel_awPlayerShuffledDeck;

    Duel_ShuffleDeck(player_deck, base, (u8 *)base - 0x54);
    Duel_ShuffleDeck(opponent_deck, &base[DECK_SIZE],
                     (u8 *)base - 0x2C);
}

#include "card_constants.h"
#include "file_transfer.h"

/* Walks the sorted card-ID boundary list while advancing through the loaded
   card-data blocks. Mode 0 initializes the current range; mode 1 advances to
   the next unique card. */

void Duel_StepCardDataTransfer(FileTransferDescriptor *o, int mode) {
    s16 *p = o->callback_data;

    if (mode == 0) {
        o->field_30.h.counter = *p;
        o->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= ~0x230000;
        o->sector_destination = o->alternate_sector_destination = o->position;
        o->transfer_mode = 1;
        return;
    }
    if (mode == 1) {
        p++;
        o->callback_data = p;
        o->position += DUEL_CARD_DATA_BLOCK_SIZE;
    }
    o->field_30.h.counter++;
    if (o->field_30.h.counter == *p) {
        o->phase_size = FILE_SECTOR_SIZE;
        o->phase_index = 1;
        gFile_dwTransferState &= ~0x230000;
        o->sector_destination = o->alternate_sector_destination = o->position;
        o->transfer_mode = 1;
    } else {
        o->phase_size = FILE_SECTOR_SIZE;
        o->phase_index = 2;
        gFile_dwTransferState &= ~0x30000;
        gFile_dwTransferState |= FILE_TRANSFER_FLAG_SKIP_RAM_WRITE;
        o->transfer_mode = 1;
    }
}

int Util_CompareS16(const short *left, const short *right)
{
    if (*left == *right) {
        return 0;
    }
    if (*left > *right) {
        return 1;
    }
    return -1;
}

#include "../psyq/qsort.h"
#include "card_constants.h"
#include "duel_card_staging.h"
#include "file_transfer.h"
#include "util_memory.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "display_object_layout.h"
#include "duel_card.h"
#include "duel_card_layout.h"
#include "duel_grid.h"

/* Initialized data this unit owns at 0x800909D4: the terrain attack modifiers, in card-type
   order. */
s8 gDuel_aTerrainBoost[CARD_TYPE_MAGIC][DUEL_TERRAIN_COUNT] = {
    { 0, 0, DUEL_TERRAIN_BOOST_UNIT, 0, 0, 0 },
    { 0, 0, 0, 0, 0, DUEL_TERRAIN_BOOST_UNIT },
    { 0, DUEL_TERRAIN_BOOST_UNIT, 0, 0, 0, 0 },
    { 0, 0, 0, DUEL_TERRAIN_BOOST_UNIT, 0, 0 },
    { DUEL_TERRAIN_BOOST_UNIT, 0, 0, DUEL_TERRAIN_BOOST_UNIT, 0, 0 },
    { DUEL_TERRAIN_BOOST_UNIT, 0, 0, 0, 0, 0 },
    { 0, 0, DUEL_TERRAIN_BOOST_UNIT, 0, 0, 0 },
    { 0, 0, 0, 0, 0, DUEL_TERRAIN_BOOST_UNIT },
    { 0, 0, 0, 0, 0, DUEL_TERRAIN_PENALTY_UNIT },
    { DUEL_TERRAIN_BOOST_UNIT, 0, 0, 0, 0, 0 },
    { 0, DUEL_TERRAIN_BOOST_UNIT, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, DUEL_TERRAIN_BOOST_UNIT, 0 },
    { 0, 0, 0, 0, DUEL_TERRAIN_BOOST_UNIT, 0 },
    { 0, 0, 0, 0, DUEL_TERRAIN_PENALTY_UNIT, 0 },
    { 0, 0, DUEL_TERRAIN_BOOST_UNIT, 0, DUEL_TERRAIN_BOOST_UNIT, 0 },
    { 0, 0, 0, 0, DUEL_TERRAIN_BOOST_UNIT, 0 },
    { 0, 0, 0, 0, DUEL_TERRAIN_PENALTY_UNIT, 0 },
    { 0, DUEL_TERRAIN_BOOST_UNIT, 0, 0, 0, 0 },
    { DUEL_TERRAIN_BOOST_UNIT, 0, 0, 0, 0, 0 },
};

void Duel_RequestCombinedDeckData(void)
{
    u16 *source = gDuel_awCombinedDeckCardIds;
    u16 *output;
    FileTransferDescriptor *result;
    s32 i;
    s32 previous;
    s32 first_id;
    u8 *table;
    s32 value;

    Util_CopyWords(
        (u8 *)source,
        (u8 *)source - DUEL_DECK_ID_BUFFER_STRIDE,
        COMBINED_DECK_SIZE * sizeof(u16)
    );
    qsort(source, COMBINED_DECK_SIZE, sizeof(u16), (int (*)())Util_CompareS16);

    output = source + DUEL_DECK_ID_BUFFER_STRIDE / sizeof(u16);
    previous = 0;
    for (i = 0; i < COMBINED_DECK_SIZE; i++) {
        value = *source;
        if (value != previous) {
            *output = value;
            previous = value;
            output++;
        }
        source++;
    }

    *output = DUEL_CARD_ID_LIST_END;
    table = gDuel_abCardStagingBuffer;
    first_id =
        ((u16 *)table)[DUEL_UNIQUE_DECK_CARD_IDS_OFFSET / sizeof(u16)];
    result = File_TryRequestAsyncTransfer(
        0, (u8 *)0, first_id - 1, previous - first_id + 1,
        Duel_StepCardDataTransfer, 0, 0
    );
    result->callback_data = gDuel_awUniqueDeckCardIds;
    result->position = (u32)table;
    gFile_dwTransferState =
        result->status_flags | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
}

void Duel_PopulateCombinedDeckData(void)
{
    DuelCardThumbnailBlock *dst = gDuel_aCardThumbnailBlocks;
    DuelDeckCardRecord *rec = gDuel_aDeckCardRecords;
    DuelCombinedDeckInput *input;
    u8 *src;
    u16 *p;
    s32 i;
    s32 id;
    u16 v;

    for (i = 0; i < COMBINED_DECK_SIZE; i++) {
        /* Keep this binding in the loop so GCC reuses the staging base below. */
        input = (DuelCombinedDeckInput *)gDuel_abCardStagingBuffer;
        v = input->card_ids[i];
        rec->id = v;
        id = (s16)v;
        rec->pre_shuffle_index = input->pre_shuffle_indices[i];
        rec->deck_index = i;
        rec->data_block_index = i;

        src = gDuel_abCardStagingBuffer;
        p = gDuel_awUniqueDeckCardIds;
        for (;;) {
            s32 cur = *p;

            p++;
            if (cur == id) {
                break;
            }
            src += DUEL_CARD_DATA_BLOCK_SIZE;
        }
        Util_CopyWords((u8 *)dst, src, sizeof(*dst));
        dst++;
        rec++;
    }
}

void DuelCard_DeactivateRecord(DuelCardRecord *object)
{
    object->flags &= ~DUEL_CARD_FLAG_OCCUPIED;
    if (object->object != 0) {
        DisplayObject_ReleaseIfPresent(object->object);
        object->object = 0;
    }
}

void DuelCard_RemoveFromField(DuelCardRecord *object)
{
    DuelCard_DeactivateRecord(object);
    object->flags = 0;
}

s32 Duel_GetTerrainBoost(s32 cardType)
{
    if (gDuel_bTerrain == 0 || cardType >= CARD_TYPE_MAGIC) {
        return 0;
    }

    return gDuel_aTerrainBoost[cardType][gDuel_bTerrain - 1] * CARD_STAT_SCALE;
}

/* Two RECTs per live slot: the thumbnail at 2 * slot and the CLUT at
   2 * slot + 1. LoadImage consumes both, which is what types the table. */

DuelCardRecord *Duel_SetupCardRecord(s32 a, s32 b) {
    DuelCardRecord *p;
    RECT *q;
    RECT *r;
    u8 *tb;
    RECT *base;
    DuelStagedDeckRecordBlock *g;
    s32 idx;
    s32 m;
    s32 block_index;
    s32 n;
    s32 v;

    if ((a & 0x80) != 0) {
        idx = (a & 0x7F) + DUEL_CARD_SIDE_RECORD_COUNT;
    } else {
        idx = a;
    }

    p = &gDuel_aCardRecords[idx];
    p->flags = DUEL_CARD_FLAG_OCCUPIED;

    if (a >= DUEL_CARD_SIDE_RECORD_COUNT && b < DECK_SIZE) {
        b += DECK_SIZE;
    }
    if ((b & 0x80) != 0) {
        b = (b & 0x7F) + DECK_SIZE;
    }

    n = b * sizeof(DuelDeckCardRecord);
    tb = gDuel_abCardStagingBuffer;
    p->data = &gDuel_aDeckCardRecords[b];
    p->table_index = idx;

    g = (DuelStagedDeckRecordBlock *)(
        tb + n + DUEL_CARD_STAGING_REPLAY_BASE_OFFSET);
    v = (u16)g->record.id;
    p->card_id = v;
    p->attack =
        (gDuel_adwCardStats[(s16)v - 1] & CARD_STAT_VALUE_MASK) *
        CARD_STAT_SCALE;
    m = idx * 2 + 1;
    p->defense =
        ((gDuel_adwCardStats[p->card_id - 1] >>
          CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) *
        CARD_STAT_SCALE;
    p->stat_modifier = 0;
    p->terrain_modifier =
        Duel_GetTerrainBoost(
            (gDuel_adwCardStats[p->card_id - 1] >>
             CARD_STAT_TYPE_SHIFT) &
            CARD_STAT_TYPE_MASK);

    base = gDuel_aCardUploadRects;
    q = &base[idx * 2];
    block_index = ((DuelDeckCardRecord *)p->data)->data_block_index;
    q->w = 0x14;
    q->h = 0x20;
    q->x = (idx % DUEL_FIELD_ROW_SIZE) * 0x14 + 0x380;
    q->y = (idx / DUEL_FIELD_ROW_SIZE) * 0x20;
    LoadImage(q, (u32 *)gDuel_aCardThumbnailBlocks[block_index].pixels);

    r = &base[m];
    r->x = 0x380;
    r->y = idx + 0xE0;
    r->w = 0x40;
    r->h = 1;
    LoadImage(r, (u32 *)(gDuel_CardThumbnailClutBase +
                        block_index * DUEL_CARD_DATA_BLOCK_SIZE));

    return p;
}

/* Creates the world-space field-card object: x/-24/z translation,
   facing callback and type-selected front UV/CLUT for the 3D path. */
DuelCardDisplayObject *Duel_CreateFieldCardObject(s32 cardId, s32 x, s32 y) {
    DuelCardDisplayObject *obj;
    u32 desc;
    s32 type;

    obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 0);

    obj->out_y = -0x18;
    obj->out_x = x;
    obj->z = y;
    obj->deck_number_marker = 0;
    obj->is_back_facing = 0;
    obj->attribute = obj->attribute | DISPLAY_OBJECT_ATTRIBUTE_8BPP;

    desc = gDuel_adwCardStats[cardId - 1];
    obj->back_face_handler.callback = DisplayObject_SetResourceVariantFromSign;
    obj->icon_state = 0;
    obj->texture_v = 0xC0;

    type = (s32)desc >> CARD_STAT_TYPE_SHIFT;
    type &= CARD_STAT_TYPE_MASK;
    obj->card_type = (u8)type;
    obj->icon_variant = 0;

    if (type >= CARD_TYPE_MAGIC) {
        obj->icon_state = 0x38;

        switch (type) {
            case CARD_TYPE_EQUIP:
                obj->icon_variant = 1;
                break;
            case CARD_TYPE_MAGIC:
                obj->icon_variant = 1;
                break;
            case CARD_TYPE_TRAP:
                obj->icon_variant = 2;
                break;
            case CARD_TYPE_RITUAL:
                obj->icon_variant = 3;
                break;
        }
    }
    return obj;
}

void Duel_SetupFieldCardFromDeck(s32 a, s32 b)
{
    DuelCardRecord *slot;
    s32 idx;
    u8 *tb;
    DuelCardReplayRecordBlock *replay;
    DuelCardDisplayObject *obj;

    slot = Duel_SetupCardRecord(a, b);
    idx = a;
    if ((idx & 0x80) != 0) {
        idx = (idx & 0x7F) + DUEL_CARD_SIDE_RECORD_COUNT;
    }
    tb = gDuel_abCardStagingBuffer;
    replay = (DuelCardReplayRecordBlock *)(
        (u8 *)&((DuelCardRecord *)tb)[idx] +
        DUEL_CARD_STAGING_REPLAY_BASE_OFFSET
    );
    obj = Duel_CreateFieldCardObject(
        ((DuelDeckCardRecord *)replay->record.data)->id,
        gDuel_aCardSlotGroundPositions[idx].x,
        gDuel_aCardSlotGroundPositions[idx].y);
    slot->object = (DisplayObject *)obj;
    obj->card_index = idx;
}

#include "duel_session_controls.h"
#include "main_mode_state.h"

void Main_StartDuel(s32 side0_controller, s32 opponent_id, s32 win_scene, s32 loss_scene)
{
    gDuel_wBgmId = 0x7270;
    gDuel_wAnimatedBattleBgmCommand = 0x7280;
    gDuel_bSide0ControllerSelector[0] = side0_controller;
    gDuel_bOpponentID = opponent_id;
    gDuel_awCampaignSceneByOutcome[0] = win_scene;
    gDuel_awCampaignSceneByOutcome[1] = loss_scene;
    gDuel_bTerrain = 0;
    gDuel_bResumeFromAnimatedBattle = 0;
    gMain_bActiveModeState = 3;
}

s32 DuelEffect_MarkInitialized(void)
{
    u16 value = gDuel_wCardEffectFlags;

    if (!(value & 0x80)) {
        gDuel_wCardEffectFlags = value | 0x80;
        return 0;
    }
    return 1;
}

void DuelEffect_ClearCardEffect(void)
{
    gDuel_wCardEffectFlags = 0;
}

#include "duel_card.h"
#include "duel_card_layout.h"
#include "duel_package.h"
#include "file_transfer.h"
#include "sound.h"

/* The first sector of a terrain's effect data. A regional build whose WA.MRG
   lays the terrain packages out differently supplies its own. */
#ifndef DUEL_TERRAIN_EFFECT_SECTOR
#define DUEL_TERRAIN_EFFECT_SECTOR(terrain) \
    ((terrain) * DUEL_TERRAIN_PACKAGE_SECTOR_COUNT + \
     DUEL_TERRAIN_EFFECT_DATA_FIRST_SECTOR)
#endif

/* The last two differences of the original match were the `n = v & 0xFF`
 * that gcc sank into the jal's delay slot where retail keeps the andi before
 * the call and puts `n - 1` in the slot. The mask is not a mask: it is the
 * read-back of the byte global just stored, and the decrement belongs to the
 * same expression -- `n = gDuel_bTerrain - 1;` before the call. */
void DuelEffect_ApplyTerrain(void) {
    DuelSideState *r;
    DuelCardRecord *q;
    DuelEffectRequest *e;
    s32 i;
    s32 n;
    s32 f;
    s32 v;
    s32 b;
    DisplayObject *a;

    if (DuelEffect_MarkInitialized() == 0) {
        r = gDuel_pActiveSideState;
        r->rank.field_magic_used = r->rank.field_magic_used + 1;
        v = (u8)gDuel_wEffectCardID - 0x49;
        gDuel_bTerrain = v;
        n = gDuel_bTerrain - 1;
        e = DuelEffect_AllocateRequest(0xA);
        gDuelEffect_pTrackedRequest = e;
        e->phase = n;
        SD_SEPlayFull(0x13);
        return;
    }

    f = gDuel_wCardEffectFlags;

    if ((f & 0x40) == 0) {
        if (gDuelEffect_pTrackedRequest->progress != 0) {
            gDuel_wCardEffectFlags = f | 0x40;
            File_RequestAsyncTransfer(
                0, (u8 *)0,
                DUEL_TERRAIN_EFFECT_SECTOR(gDuel_bTerrain),
                DUEL_TERRAIN_EFFECT_DATA_SECTOR_COUNT,
                (FileTransferCallback)0, 0, 0x1000280);
        }
        return;
    }

    if ((f & 0x20) == 0) {
        if (((gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             gFile_dwCancelRequestState) == 0) {
            a = gDuel_pTerrainIcon;
            b = gDuel_bTerrain;
            gDuelEffect_pTrackedRequest->phase = -2;
            DisplayObject_SetResourceVariant(a, b);
            gDuel_wCardEffectFlags = gDuel_wCardEffectFlags | 0x20;
        }
        return;
    }

    if ((gDuelEffect_pTrackedRequest->flags &
         DUEL_EFFECT_REQUEST_FLAG_ACTIVE) != 0) {
        return;
    }

    q = gDuel_aCardRecords;
    i = 0;
    do {
        if ((q->flags & DUEL_CARD_FLAG_OCCUPIED) != 0) {
            q->terrain_modifier =
                Duel_GetTerrainBoost(((DisplayObject *)q->object)->resource_branch_index);
        }
        i++;
        q++;
    } while (i < DUEL_CARD_RECORD_COUNT);

    gDuel_wCardEffectFlags = 0;
}

#include "duel_grid.h"
#include "duel_card.h"
#include "display_object.h"

/* Searches the acting side's five mapped field slots for an occupied trap
   whose id equals the argument. A hit records the id in gDuel_wSelectedTrapCardId, copies
   the owning object's field_6A slot index to gDuel_bSelectedTrapSlot and returns the id; a
   miss leaves gDuel_wSelectedTrapCardId cleared and returns 0. */
s32 Duel_SelectTrapByCardId(s32 card_id)
{
    s32 i;
    s32 base;
    DuelCardRecord *record;
    DisplayObject *object;

    base = gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
    gDuel_wSelectedTrapCardId = 0;
    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
        record = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices.flat[i + base]];
        if (record->flags & DUEL_CARD_FLAG_OCCUPIED) {
            if ( record->card_id == card_id) {
                object = (DisplayObject *)record->object;
                gDuel_wSelectedTrapCardId = card_id;
                gDuel_bSelectedTrapSlot = object->field_6A;
                return card_id;
            }
        }
    }
    return 0;
}

#include "file_transfer.h"
#include "card_constants.h"
#include "sound.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "duel_card.h"
#include "card_presentation.h"

/* Initialized data this unit owns at 0x80090A4C. Each pair stores the card ID minus 600 and
   the monster-type selector; Crush Card instead stores its 1500-ATK threshold
   in CARD_STAT_SCALE units. */
u8 gDuel_abMonsterRemovalRules[] = {
    DUEL_WARRIOR_ELIMINATION_CARD_ID - DUEL_MONSTER_REMOVAL_CARD_ID_BASE,
    CARD_TYPE_WARRIOR,
    DUEL_ETERNAL_REST_CARD_ID - DUEL_MONSTER_REMOVAL_CARD_ID_BASE,
    CARD_TYPE_ZOMBIE,
    DUEL_STAIN_STORM_CARD_ID - DUEL_MONSTER_REMOVAL_CARD_ID_BASE,
    CARD_TYPE_MACHINE,
    DUEL_CRUSH_CARD_ID - DUEL_MONSTER_REMOVAL_CARD_ID_BASE,
    DUEL_CRUSH_CARD_ATTACK_THRESHOLD / CARD_STAT_SCALE,
    DUEL_ERADICATING_AEROSOL_CARD_ID - DUEL_MONSTER_REMOVAL_CARD_ID_BASE,
    CARD_TYPE_INSECT,
    DUEL_BREATH_OF_LIGHT_CARD_ID - DUEL_MONSTER_REMOVAL_CARD_ID_BASE,
    CARD_TYPE_ROCK,
    DUEL_ETERNAL_DRAUGHT_CARD_ID - DUEL_MONSTER_REMOVAL_CARD_ID_BASE,
    CARD_TYPE_FISH,
    0, 0, /* Retail alignment before the following effect-handler table. */
};
#define DUEL_FIELD_EFFECT_OBJECT_VIEW(object) \
    ((DuelFieldEffectObject *)(object))

/* Small data at 0x8009AF30, owned here: the recovery handler scales the
   first table by 100 and the direct-damage handler scales the second by 10,
   one entry per card in each family. */
u8 gDuel_abLifePointRecoveryUnits[DUEL_LIFE_POINT_EFFECT_COUNT] = {
    DUEL_MOOYAN_CURRY_RECOVERY / DUEL_LIFE_POINT_RECOVERY_SCALE,
    DUEL_RED_MEDICINE_RECOVERY / DUEL_LIFE_POINT_RECOVERY_SCALE,
    DUEL_GOBLINS_SECRET_REMEDY_RECOVERY /
        DUEL_LIFE_POINT_RECOVERY_SCALE,
    DUEL_SOUL_OF_THE_PURE_RECOVERY / DUEL_LIFE_POINT_RECOVERY_SCALE,
    DUEL_DIAN_KETO_RECOVERY / DUEL_LIFE_POINT_RECOVERY_SCALE,
};
u8 gDuel_abDirectDamageUnits[DUEL_LIFE_POINT_EFFECT_COUNT] = {
    DUEL_SPARKS_DAMAGE / DUEL_DIRECT_DAMAGE_SCALE,
    DUEL_HINOTAMA_DAMAGE / DUEL_DIRECT_DAMAGE_SCALE,
    DUEL_FINAL_FLAME_DAMAGE / DUEL_DIRECT_DAMAGE_SCALE,
    DUEL_OOKAZI_DAMAGE / DUEL_DIRECT_DAMAGE_SCALE,
    DUEL_TREMENDOUS_FIRE_DAMAGE / DUEL_DIRECT_DAMAGE_SCALE,
};

/* Runs the table-driven LP change phases. Recovery values are scaled by 100,
   added to the selected side's life points, and capped at its maximum; the
   alternate path subtracts the same values and floors the result at zero. */
void DuelEffect_ApplyLifePointRecovery(void) {
    s32 card_id;
    s32 effect_index;
    s32 flag;
    u16 life_points;
    DuelEffectRequest *obj;

    card_id = gDuel_wEffectCardID;
    effect_index = card_id - DUEL_LIFE_POINT_RECOVERY_FIRST_CARD_ID;
    if (DuelEffect_MarkInitialized() == 0) {
        if (Duel_SelectTrapByCardId(DUEL_BAD_REACTION_TO_SIMOCHI_CARD_ID) != 0) {
            effect_index = card_id - (DUEL_LIFE_POINT_RECOVERY_FIRST_CARD_ID -
                       DUEL_LIFE_POINT_EFFECT_COUNT);
        }
        obj = DuelEffect_CreateRequest(5);
        obj->field_00 = 0xA0;
        obj->field_02 = 0x78;
        obj->phase = effect_index;
        SD_SEPlayFull(0x14);
        return;
    }
    flag = gDuel_wCardEffectFlags;
    if (!(flag & 0x40)) {
        gDuel_wCardEffectFlags = flag | 0x60;
        if (gDuel_wSelectedTrapCardId == 0) {
            u8 *p = &gDuel_abLifePointRecoveryUnits[effect_index];
            life_points = gDuel_pActiveSideState->life_points.unsigned_value +
                 (*p) * DUEL_LIFE_POINT_RECOVERY_SCALE;
            gDuel_pActiveSideState->life_points.unsigned_value = life_points;
            if (gDuel_pActiveSideState->max_life_points < (s16) life_points) {
                gDuel_pActiveSideState->life_points.unsigned_value =
                    (u16) gDuel_pActiveSideState->max_life_points;
            }
            gDuel_wCardEffectFlags = 0;
            return;
        }
        gDuel_wTrapPresentationMode = 0;
    }
    if (gDuel_wCardEffectFlags & 0x20) {
        if (Duel_UpdateTrapPresentation() == 0) {
            gDuel_wCardEffectFlags &= 0xFFDF;
            obj = DuelEffect_CreateRequest(9);
            obj->field_00 = 0xA0;
            obj->field_02 = 0x78;
            obj->phase = effect_index;
        }
    } else {
        u8 *p = &gDuel_abLifePointRecoveryUnits[effect_index];
        life_points = gDuel_pActiveSideState->life_points.unsigned_value -
             (*p) * DUEL_LIFE_POINT_RECOVERY_SCALE;
        gDuel_pActiveSideState->life_points.unsigned_value = life_points;
        if ((s16) life_points < 0) {
            gDuel_pActiveSideState->life_points.unsigned_value = 0;
        }
        gDuel_wCardEffectFlags = 0;
    }
}

/* Applies one of the five direct-damage cards. The selected table value is
   scaled by 10 and taken off the target side's life points, clamped at zero. */
void DuelEffect_ApplyDirectDamage(void) {
    s32 unit;
    s32 flags;
    u16 remaining;
    DuelEffectRequest *obj;
    DuelSideState *p;

    unit = gDuel_wEffectCardID - DUEL_DIRECT_DAMAGE_FIRST_CARD_ID;
    if (DuelEffect_MarkInitialized() == 0) {
        if (Duel_SelectTrapByCardId(DUEL_GOBLIN_FAN_CARD_ID) != 0) {
            unit = DUEL_LIFE_POINT_EFFECT_COUNT;
        }
        obj = DuelEffect_CreateRequest(6);
        obj->field_00 = 0xA0;
        obj->field_02 = 0x78;
        obj->phase = unit;
        SD_SEPlayFull(0x1C);
        return;
    }
    flags = gDuel_wCardEffectFlags;
    do {
        if (!(flags & 0x40)) {
            gDuel_wCardEffectFlags = flags | 0x60;
            if (gDuel_wSelectedTrapCardId == 0) {
                p = &gDuel_aSideStates[gDuel_bActiveSide ^ 1];
                break;
            }
            gDuel_wTrapPresentationMode = 0;
        }
        if (gDuel_wCardEffectFlags & 0x20) {
            if (Duel_UpdateTrapPresentation() == 0) {
                gDuel_wCardEffectFlags &= 0xFFDF;
                obj = DuelEffect_CreateRequest(7);
                obj->field_00 = 0xA0;
                obj->field_02 = 0x78;
                obj->phase = unit;
                SD_SEPlayFull(0x1C);
            }
            return;
        }
        p = &gDuel_aSideStates[gDuel_bActiveSide];
    } while (0);
    remaining = p->life_points.unsigned_value -
                gDuel_abDirectDamageUnits[unit] *
                    DUEL_DIRECT_DAMAGE_SCALE;
    p->life_points.unsigned_value = remaining;
    if ((s16) remaining < 0) {
        p->life_points.unsigned_value = 0;
    }
    gDuel_wCardEffectFlags = 0;
}

void DuelEffect_ApplyMonsterRemoval(void) {
    DuelEffectRequest *p;
    DuelCardRecord *e;
    u8 *tb;
    u8 *cb;
    u8 *rb;
    s16 *sp;
    s32 i;
    s32 n;
    s16 m;
    s32 x;
    s32 ix;

    if (DuelEffect_MarkInitialized() == 0) {
        p = DuelEffect_CreateRequest(0xF);
        p->field_00 = 0;
        p->field_02 = 0;
        p->field_04 = 0;
        SD_SEPlayFull(0x22);
        i = 0;
        cb = gDuel_abMonsterRemovalRules;
        x = gDuel_wEffectCardID;
        while (1) {
            /* Integer address arithmetic keeps retail's addu operand order here
               and below; u8 pointer arithmetic swaps it. */
            if (*(u8 *)(i + (s32)cb) +
                    DUEL_MONSTER_REMOVAL_CARD_ID_BASE == x) {
                break;
            }
            i += DUEL_MONSTER_REMOVAL_RULE_SIZE;
        }
        p->phase = i / DUEL_MONSTER_REMOVAL_RULE_SIZE;
        m = gDuel_abMonsterRemovalRules[
            i + DUEL_MONSTER_REMOVAL_SELECTOR_OFFSET
        ];
        n = m;
        gDuel_wMonsterRemovalCriterion = m;
        if (n >= 0x15) {
            gDuel_wMonsterRemovalCriterion = n * CARD_STAT_SCALE;
            gDuel_wCardEffectFlags = gDuel_wCardEffectFlags | 1;
        }
        gDuel_wMonsterRemovalZone = 5;
        return;
    }

    tb = gDuel_abFieldGridCardRecordIndices.flat;
    sp = &gDuel_wMonsterRemovalZone;
    rb = (u8 *)gDuel_aCardRecords;
    for (;;) {
        ix = gDuel_wMonsterRemovalZone + gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
        e = (DuelCardRecord *)(*(u8 *)(ix + (s32)tb) * DUEL_CARD_RECORD_SIZE +
            (s32)rb);
        if ((e->flags & DUEL_CARD_FLAG_OCCUPIED) != 0) {
            if ((gDuel_wCardEffectFlags & 1) == 0) {
                if (e->object->resource_branch_index == sp[-1]) {
                    break;
                }
            } else if ((Duel_CalcCardStats(e) & 0xFFFF) >=
                       gDuel_wMonsterRemovalCriterion) {
                break;
            }
        }
        gDuel_wMonsterRemovalZone = gDuel_wMonsterRemovalZone + 1;
        if (gDuel_wMonsterRemovalZone >= DUEL_FIELD_SIDE_ZONE_COUNT) {
            gDuel_wCardEffectFlags = 0;
            return;
        }
    }

    e = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices.flat[
        gDuel_wMonsterRemovalZone + gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT
    ]];
    p = DuelEffect_CreateRequest(0xB);
    p->field_00 = e->object->xy.h.x;
    p->field_02 = e->object->xy.h.y;
    p->field_04 = (u16)e->object->field_34.h.z;
    p->phase = Duel_GetCardEffectVariant((CardObject *)e->object);
    DuelCard_RemoveFromField(e);
    SD_SEPlayFull(0x1F);
}

void DuelEffect_ApplyStopDefense(void) {
    DuelCardRecord *r;
    DuelEffectRequest *p;
    DuelFieldPosition *e;
    u8 *t;
    s32 f;
    s32 c;
    s32 v;
    s32 n;
    s32 w;

    if (DuelEffect_MarkInitialized() == 0) {
        gDuel_sEffectWorkCounter = -1;
    }

    f = gDuel_wCardEffectFlags;

    if ((f & 0x40) != 0) {
        if ((f & 0x20) == 0) {
            if (gDuelEffect_pTrackedRequest->progress != 0) {
                n = gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT +
                    DUEL_FIELD_ROW_SIZE;
                c = gDuel_abFieldGridCardRecordIndices.flat[gDuel_sEffectWorkCounter + n];
                r = &gDuel_aCardRecords[c];
                if ((*(s32 *)&r->terrain_modifier & 0x88000000) ==
                    0x88000000) {
                    DisplayObject_StartAlphaTransition(r->object, 0xC0, 0, 6);
                    r->flags &= ~DUEL_CARD_FLAG_DEFENSE_POSITION;
                }
                gDuel_wCardEffectFlags = gDuel_wCardEffectFlags | 0x20;
            }
        }

        if ((gDuelEffect_pTrackedRequest->flags &
             DUEL_EFFECT_REQUEST_FLAG_ACTIVE) != 0) {
            return;
        }
        gDuel_wCardEffectFlags = gDuel_wCardEffectFlags & 0xFF9F;
        return;
    }

    v = (u16)gDuel_sEffectWorkCounter + 1;
    gDuel_sEffectWorkCounter = v;
    if ((s16)v >= DUEL_FIELD_ROW_SIZE) {
        gDuel_wCardEffectFlags = 0;
        return;
    }

    p = DuelEffect_AllocateRequest(0xC);
    t = (u8 *)gDuel_aFieldGridGroundPositions;
    e = (DuelFieldPosition *)((
        (gDuel_sEffectWorkCounter + DUEL_FIELD_ROW_SIZE) *
            sizeof(DuelFieldPosition) +
        gDuel_bActiveSide * DUEL_FIELD_SIDE_POSITION_BYTES
    ) + t);
    w = (u16)e->x;
    gDuelEffect_pTrackedRequest = p;
    p->field_02 = 0;
    p->field_00 = w;
    p->field_04 = (u16)e->y;
    SD_SEPlayFull(0x20);

    gDuel_wCardEffectFlags = gDuel_wCardEffectFlags | 0x40;
}

void DuelEffect_ApplyBoardDestruction(void) {
    DuelEffectRequest *e;
    DuelCardRecord *p;
    DuelCardRecord *q;
    s32 i;
    s32 f;
    s32 g;

    if (DuelEffect_MarkInitialized() == 0) {
        SD_CdSoundPlay(0x8020);
        return;
    }

    f = gDuel_wCardEffectFlags;

    if ((f & 0x20) == 0) {
        if ((gFile_wTransferStepFlags & 0x4000) == 0) {
            return;
        }
        gDuel_wCardEffectFlags = f | 0x20;
        e = DuelEffect_CreateRequest(0x11);
        if (gDuel_wEffectCardID == DUEL_DRAGON_CAPTURE_JAR_CARD_ID) {
            g = gDuel_wCardEffectFlags;
            e->phase = 1;
            gDuel_wCardEffectFlags = g | 0x40;
        }
        e->field_00 = 0;
        e->field_02 = 0;
        e->field_04 = 0;
        return;
    }

    if ((gFile_wTransferStepFlags & 0x4000) != 0) {
        return;
    }

    if ((f & 0x40) != 0) {
        i = DUEL_FIELD_ROW_SIZE;
        while (1) {
            p = (DuelCardRecord *)(gDuel_abFieldGridCardRecordIndices.flat[
                i + gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT
            ] * DUEL_CARD_RECORD_SIZE + (u8 *)gDuel_aCardRecords);
            if ((p->flags & DUEL_CARD_FLAG_OCCUPIED) != 0 &&
                p->object->resource_branch_index == 0) {
                DuelCard_RemoveFromField(p);
            }
            i++;
            if (i >= DUEL_FIELD_SIDE_ZONE_COUNT) {
                break;
            }
        }
    } else {
        p = gDuel_aFieldCardRecords;
        q = p + DUEL_CARD_SIDE_RECORD_COUNT;
        i = 0;
        do {
            if ((p->flags & DUEL_CARD_FLAG_OCCUPIED) != 0) {
                DuelCard_RemoveFromField(p);
            }
            if ((q->flags & DUEL_CARD_FLAG_OCCUPIED) != 0) {
                DuelCard_RemoveFromField(q);
            }
            i++;
            p++;
            q++;
        } while (i < DUEL_FIELD_SIDE_ZONE_COUNT);
    }

    gDuel_wCardEffectFlags = 0;
}

void DuelEffect_ApplyRaigeki(void) {
    DuelEffectRequest *p;
    DuelFieldPosition *e;
    DuelCardRecord *r;
    DuelEffectRequest *q;
    s32 a;
    u8 *t;
    s32 v;
    s32 w;
    s32 n;

    if (DuelEffect_MarkInitialized() == 0) {
        gDuel_sEffectWorkCounter = 0;
        q = DuelEffect_AllocateRequest(0x10);
        t = (u8 *)gDuel_aFieldGridGroundPositions;
        e = (DuelFieldPosition *)((
            (gDuel_sEffectWorkCounter + DUEL_FIELD_ROW_SIZE) *
                sizeof(DuelFieldPosition) +
            gDuel_bActiveSide * DUEL_FIELD_SIDE_POSITION_BYTES
        ) + t);
        w = (u16)e->x;
        p = q;
        gDuelEffect_pTrackedRequest = p;
        p->field_02 = 0;
        p->field_00 = w;
        a = 0x15;
        p->field_04 = (u16)e->y;
        SD_SEPlayFull(a);
        return;
    }

    if ((gDuel_bEffectRequestStatus & 1) == 0) {
        gDuel_wCardEffectFlags = 0;
        return;
    }

    if (gDuelEffect_pTrackedRequest->progress ==
        gDuel_sEffectWorkCounter + 1) {
        SD_SEPlayFull(0x15);
        n = gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT +
            DUEL_FIELD_ROW_SIZE;
        r = (DuelCardRecord *)((u8 *)gDuel_aCardRecords +
            gDuel_abFieldGridCardRecordIndices.flat[gDuel_sEffectWorkCounter + n] * DUEL_CARD_RECORD_SIZE);
        v = r->flags & 0x8000;
        gDuel_sEffectWorkCounter = (u16)gDuel_sEffectWorkCounter + 1;
        if (v != 0) {
            q = DuelEffect_AllocateRequest(0xB);
            q->buffer = (u8 *)q->buffer + gDuel_sEffectWorkCounter * 0x3000;
            q->field_00 = r->object->xy.h.x;
            q->field_02 = r->object->xy.h.y;
            q->field_04 = (u16)r->object->field_34.h.z;
            q->phase = Duel_GetCardEffectVariant((CardObject *)r->object);
            DuelCard_RemoveFromField(r);
            a = 0x1F;
            SD_SEPlayFull(a);
        }
    }
}

#define DUEL_FIELD_EFFECT_TIMER_STEP 8
#define DUEL_FIELD_EFFECT_MARK_THRESHOLD 40
#define DUEL_FIELD_EFFECT_TIMER_LIMIT 64

/* The reveal path indexes by side and slot; the later status step walks the
   same storage from a flat base. */

void DuelEffect_UpdateRevealCard(DuelFieldEffectObject *o)
{
    o->timer += DUEL_FIELD_EFFECT_TIMER_STEP;
    if (!(o->active & 0x80) &&
        o->timer >= DUEL_FIELD_EFFECT_MARK_THRESHOLD) {
        o->active |= 0x80;
        gDuel_aCardRecords[o->index].flags &= ~DUEL_CARD_FLAG_DISPLAY_MARKER;
        o->mark = 0;
    }
    if (o->timer < DUEL_FIELD_EFFECT_TIMER_LIMIT) {
        o->timer = 0;
        o->active = 0;
        o->callback = 0;
        gDuel_aCardRecords[o->index].flags &= ~0x3400;
        Duel_ApplyCardObjectFlags((DuelCardDisplayObject *)o);
    }
}

/* Opens the duel-side effect prompt and, once acknowledged, hands every
   occupied slot to the shared reveal animation. */
void DuelEffect_ApplyDarkPiercingLight(void)
{
    DuelEffectRequest *object;
    DuelFieldEffectObject *target;
    DuelCardRecord *record;
    u16 flags;
    s32 i;

    if (DuelEffect_MarkInitialized() == 0) {
        object = DuelEffect_AllocateRequest(0x13);
        object->field_00 = 0xA0;
        gDuelEffect_pTrackedRequest = object;
        object->field_02 = 0x68;
        SD_SEPlayFull(0x13);
        return;
    }
    flags = gDuel_wCardEffectFlags;
    if ((flags & 0x40) == 0 &&
        gDuelEffect_pTrackedRequest->progress != 0) {
        gDuel_wCardEffectFlags = flags | 0x40;
        SD_SEPlayFull(0x1D);
        for (i = DUEL_FIELD_ROW_SIZE; i < DUEL_CARD_SIDE_RECORD_COUNT; i++) {
            record = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices.flat[
                i + gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT]];
            /* The retail code tests the two halfwords at +0x14 as one
               word; 0x90000000 selects bits 0x9000 of flags at +0x16. */
            if ((*(u32 *)&record->terrain_modifier & 0x90000000) ==
                0x90000000) {
                target = DUEL_FIELD_EFFECT_OBJECT_VIEW(record->object);
                target->callback = DuelEffect_UpdateRevealCard;
                target->active = 1;
            }
        }
    }
    if ((gDuel_bEffectRequestStatus & 1) == 0 && DisplayObject_FindAllocatedByTag(1) == 0) {
        gDuel_wCardEffectFlags = 0;
    }
}

/* Companion field-wide stat-penalty sweep. It advances one occupied slot of
 * the acting side's second row per countdown, spawns the effect at that card,
 * and waits on the same request-completion state as the transition above. */
void DuelEffect_ApplyStatPenalty(void) {
    DuelCardRecord *record;
    DuelEffectRequest *object;
    DisplayObject *card;
    s32 timer;
    s32 base_slot;

    if (DuelEffect_MarkInitialized() == 0) {
        gDuel_sEffectWorkCounter = 0;
        gDuel_wStepCounter = 0;
    }

    if ((gDuel_wCardEffectFlags & 0x40) != 0) {
        if ((gDuel_bEffectRequestStatus & 1) == 0) {
            gDuel_wCardEffectFlags = 0;
        }
        return;
    }

    timer = (u16)gDuel_sEffectWorkCounter - 1;
    gDuel_sEffectWorkCounter = timer;
    if ((s16)timer > 0) {
        return;
    }
    gDuel_sEffectWorkCounter = 0x10;

    base_slot = gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT +
                DUEL_FIELD_ROW_SIZE;
    record = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices.flat[(s16)gDuel_wStepCounter + base_slot]];
    if ((record->flags & DUEL_CARD_FLAG_OCCUPIED) != 0) {
        card = record->object;
        object = DuelEffect_AllocateRequest(0xD);
        object->field_00 = (s16)card->xy.h.x;
        object->field_02 = (s16)card->xy.h.y;
        object->field_04 = (s16)(u16)card->field_34.h.z;
        object->buffer =
            (void *)((s32)object->buffer + ((s16)gDuel_wStepCounter << 14));
        if (gDuel_wEffectCardID == DUEL_SPELLBINDING_CIRCLE_CARD_ID) {
            object->phase = 2;
            record->stat_modifier =
                record->stat_modifier - DUEL_STAT_PENALTY_PER_LEVEL;
            object->number = -DUEL_STAT_PENALTY_PER_LEVEL;
        } else {
            object->phase = 1;
            record->stat_modifier =
                record->stat_modifier - 2 * DUEL_STAT_PENALTY_PER_LEVEL;
            object->number = -2 * DUEL_STAT_PENALTY_PER_LEVEL;
        }
        SD_SEPlayFull(0x21);
    }

    gDuel_wStepCounter = gDuel_wStepCounter + 1;
    if ((s16)gDuel_wStepCounter >= DUEL_FIELD_ROW_SIZE) {
        gDuel_wCardEffectFlags = gDuel_wCardEffectFlags | 0x40;
    }
}

#include "ai.h"
#include "duel_grid.h"
#include "../psyq/stdio.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_effect_tables.h"
#include "sound.h"
#include "duel_card_staging.h"
#include "display_object_work_slots.h"
#include "input.h"
#include "../psyq/rand.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "duel_effect_dialog_state.h"

/* Initialized data this unit owns at 0x80090A5C: two handlers per effect group, in group
   order. */
DuelEffectHandler gDuelEffect_apfnGroupHandler
    [DUEL_EFFECT_GROUP_COUNT * DUEL_CARD_EFFECT_HANDLERS_PER_GROUP] = {
    DuelEffect_ClearCardEffect, DuelEffect_ClearCardEffect,
    DuelEffect_ClearCardEffect, DuelEffect_ApplyTerrain,
    DuelEffect_ClearCardEffect, DuelEffect_ApplyLifePointRecovery,
    DuelEffect_ClearCardEffect, DuelEffect_ApplyDirectDamage,
    DuelEffect_ClearCardEffect, DuelEffect_ApplyBoardDestruction,
    DuelEffect_ClearCardEffect, DuelEffect_ApplyMonsterRemoval,
    DuelEffect_ClearCardEffect, DuelEffect_ApplyStopDefense,
    DuelEffect_ClearCardEffect, DuelEffect_ApplyRaigeki,
    DuelEffect_ClearCardEffect, DuelEffect_ApplyDarkPiercingLight,
    DuelEffect_ClearCardEffect, DuelEffect_ApplyStatPenalty,
    DuelEffect_StartSwords, DuelEffect_ApplySwords,
    DuelEffect_ClearCardEffect, DuelEffect_ApplyCursebreaker,
    DuelEffect_StartRitual, DuelEffect_ApplyRitual,
    DuelEffect_ClearCardEffect, DuelEffect_ApplyHarpiesFeatherDuster,
    DuelEffect_ClearCardEffect, DuelEffect_ClearCardEffect,
};

/* The group each card effect id dispatches to. */
u8 gDuelEffect_abGroupByEffectId[DUEL_EFFECT_ID_COUNT] = {
    DUEL_EFFECT_GROUP_NONE, DUEL_EFFECT_GROUP_NONE,
    DUEL_EFFECT_GROUP_NONE, DUEL_EFFECT_GROUP_NONE,
    DUEL_EFFECT_GROUP_NONE, DUEL_EFFECT_GROUP_NONE,
    DUEL_EFFECT_GROUP_NONE, DUEL_EFFECT_GROUP_NONE,
    DUEL_EFFECT_GROUP_NONE, DUEL_EFFECT_GROUP_NONE,
    DUEL_EFFECT_GROUP_NONE, DUEL_EFFECT_GROUP_NONE,
    DUEL_EFFECT_GROUP_NONE, DUEL_EFFECT_GROUP_NONE,
    DUEL_EFFECT_GROUP_NONE, DUEL_EFFECT_GROUP_NONE,
    DUEL_EFFECT_GROUP_NONE, DUEL_EFFECT_GROUP_NONE,
    DUEL_EFFECT_GROUP_NONE, DUEL_EFFECT_GROUP_STOP_DEFENSE,
    DUEL_EFFECT_GROUP_NONE, DUEL_EFFECT_GROUP_NONE,
    DUEL_EFFECT_GROUP_NONE, DUEL_EFFECT_GROUP_NONE,
    DUEL_EFFECT_GROUP_NONE, DUEL_EFFECT_GROUP_NONE,
    DUEL_EFFECT_GROUP_NONE, DUEL_EFFECT_GROUP_NONE,
    DUEL_EFFECT_GROUP_BOARD_DESTRUCTION, DUEL_EFFECT_GROUP_TERRAIN,
    DUEL_EFFECT_GROUP_TERRAIN, DUEL_EFFECT_GROUP_TERRAIN,
    DUEL_EFFECT_GROUP_TERRAIN, DUEL_EFFECT_GROUP_TERRAIN,
    DUEL_EFFECT_GROUP_TERRAIN, DUEL_EFFECT_GROUP_BOARD_DESTRUCTION,
    DUEL_EFFECT_GROUP_RAIGEKI, DUEL_EFFECT_GROUP_LIFE_POINT_RECOVERY,
    DUEL_EFFECT_GROUP_LIFE_POINT_RECOVERY,
    DUEL_EFFECT_GROUP_LIFE_POINT_RECOVERY,
    DUEL_EFFECT_GROUP_LIFE_POINT_RECOVERY,
    DUEL_EFFECT_GROUP_LIFE_POINT_RECOVERY,
    DUEL_EFFECT_GROUP_DIRECT_DAMAGE, DUEL_EFFECT_GROUP_DIRECT_DAMAGE,
    DUEL_EFFECT_GROUP_DIRECT_DAMAGE, DUEL_EFFECT_GROUP_DIRECT_DAMAGE,
    DUEL_EFFECT_GROUP_DIRECT_DAMAGE, DUEL_EFFECT_GROUP_SWORDS,
    DUEL_EFFECT_GROUP_STAT_PENALTY,
    DUEL_EFFECT_GROUP_DARK_PIERCING_LIGHT,
    DUEL_EFFECT_GROUP_NONE, DUEL_EFFECT_GROUP_NONE,
    DUEL_EFFECT_GROUP_MONSTER_REMOVAL, DUEL_EFFECT_GROUP_NONE,
    DUEL_EFFECT_GROUP_CURSEBREAKER, DUEL_EFFECT_GROUP_MONSTER_REMOVAL,
    DUEL_EFFECT_GROUP_NONE, DUEL_EFFECT_GROUP_NONE,
    DUEL_EFFECT_GROUP_NONE, DUEL_EFFECT_GROUP_MONSTER_REMOVAL,
    DUEL_EFFECT_GROUP_MONSTER_REMOVAL,
    DUEL_EFFECT_GROUP_MONSTER_REMOVAL,
    DUEL_EFFECT_GROUP_MONSTER_REMOVAL,
    DUEL_EFFECT_GROUP_MONSTER_REMOVAL,
    DUEL_EFFECT_GROUP_RITUAL, DUEL_EFFECT_GROUP_RITUAL,
    DUEL_EFFECT_GROUP_RITUAL, DUEL_EFFECT_GROUP_NONE,
    DUEL_EFFECT_GROUP_STAT_PENALTY, DUEL_EFFECT_GROUP_RITUAL,
    DUEL_EFFECT_GROUP_RITUAL, DUEL_EFFECT_GROUP_HARPIES_FEATHER_DUSTER,
    DUEL_EFFECT_GROUP_RITUAL, DUEL_EFFECT_GROUP_RITUAL,
    DUEL_EFFECT_GROUP_RITUAL, DUEL_EFFECT_GROUP_RITUAL,
    DUEL_EFFECT_GROUP_RITUAL, DUEL_EFFECT_GROUP_RITUAL,
    DUEL_EFFECT_GROUP_RITUAL, DUEL_EFFECT_GROUP_RITUAL,
    DUEL_EFFECT_GROUP_NONE, DUEL_EFFECT_GROUP_NONE,
    DUEL_EFFECT_GROUP_NONE, DUEL_EFFECT_GROUP_NONE,
    DUEL_EFFECT_GROUP_NONE, DUEL_EFFECT_GROUP_NONE,
    DUEL_EFFECT_GROUP_NONE, DUEL_EFFECT_GROUP_NONE,
    DUEL_EFFECT_GROUP_NONE, DUEL_EFFECT_GROUP_NONE,
    DUEL_EFFECT_GROUP_RITUAL, DUEL_EFFECT_GROUP_RITUAL,
    DUEL_EFFECT_GROUP_RITUAL, DUEL_EFFECT_GROUP_RITUAL,
    DUEL_EFFECT_GROUP_RITUAL, DUEL_EFFECT_GROUP_RITUAL,
    DUEL_EFFECT_GROUP_RITUAL, DUEL_EFFECT_GROUP_RITUAL,
    DUEL_EFFECT_GROUP_RITUAL, DUEL_EFFECT_GROUP_RITUAL,
    DUEL_EFFECT_GROUP_RITUAL, DUEL_EFFECT_GROUP_NONE,
    DUEL_EFFECT_GROUP_NONE, DUEL_EFFECT_GROUP_NONE,
};

/* The five effect-state handlers DuelEffect_UpdateState runs. */
DuelEffectHandler
    gDuelEffect_apfnStateHandler[DUEL_EFFECT_STATE_HANDLER_COUNT] = {
    DuelEffect_UpdateDialogState,
    DuelEffect_UpdateDialogState,
    DuelEffect_UpdateCardViewerState,
    func_800289AC,
    DuelEffect_UpdateIdentitySave,
};

void DuelEffect_StartSwords(void)
{
    DuelEffectRequest *object;

    if (DuelEffect_MarkInitialized() == 0) {
        object = DuelEffect_CreateRequest(0x12);
        object->field_00 = 0xA0;
        object->field_02 = 0x78;
        object->phase = 1;
        SD_SEPlayFull(2);
    } else {
        gDuel_wCardEffectFlags = 0;
    }
}

/* Applies Swords of Revealing Light to the opposing side. The handler creates
 * its persistent effect object, stages the occupied field objects, then
 * installs the three-turn attack lock once the presentation completes. */

#define DUEL_FIELD_EFFECT_OBJECT_VIEW(object) \
    ((DuelFieldEffectObject *)(object))

void DuelEffect_ApplySwords(void)
{
    DuelEffectRequest *object;
    DuelCardRecord *entry;
    int slot;
    int side;
    int field_side;

    if (DuelEffect_MarkInitialized() == 0) {
        object = DuelEffect_AllocateRequest(0x15);
        side = DUEL_SIDE_SELECTOR_VOLATILE ^ 1;
        gDuelEffect_pTrackedRequest = object;
        gDuel_apSwordsEffectObjects[side] = object;
        object->flags |= DUEL_EFFECT_REQUEST_FLAG_NONBLOCKING;
        object->field_00 = 0xA0;
        object->field_02 = 0x78;
        field_side = DUEL_SIDE_SELECTOR_VOLATILE ^ 1;
        object->phase = field_side;
        SD_SEPlayFull(0x23);
    } else if (!(gDuel_wCardEffectFlags & 0x40)) {
        if (gDuelEffect_pTrackedRequest->progress != 0) {
            gDuel_wCardEffectFlags |= 0x40;
            for (
                slot = DUEL_FIELD_ROW_SIZE;
                slot < DUEL_FIELD_SIDE_ZONE_COUNT;
                slot++
            ) {
                entry = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices.rows[DUEL_SIDE_SELECTOR_VOLATILE][slot]];
                if ((*(u32 *)&entry->terrain_modifier & 0x90000000) ==
                    0x90000000) {
                    DuelFieldEffectObject *current;

                    current = DUEL_FIELD_EFFECT_OBJECT_VIEW(entry->object);
                    current->callback = DuelEffect_UpdateRevealCard;
                    current->active = 1;
                }
            }
        }
    } else if (DisplayObject_FindAllocatedByTag(1) == 0 &&
               gDuelEffect_pTrackedRequest->progress >= 2) {
        gDuel_aSideStates[DUEL_SIDE_SELECTOR_VOLATILE ^ 1].swords_turns_remaining =
            DUEL_SWORDS_INITIAL_COUNTER;
        gDuel_wCardEffectFlags = 0;
    }
}

/* One step of a field-wide effect sweep, driven once per 16 frames by the
 * gDuel_wStepCounter countdown. The first entry resets the step index in
 * gDuel_sEffectWorkCounter; every later entry advances it and spawns a type-8 effect
 * object over the next slot of the third grid row (slots 10..14 of the acting
 * side), offsetting the object's depth by the step so the objects stagger.
 * Steps past the fifth clear gDuel_wCardEffectFlags and end the sweep. A card sitting in
 * the swept slot with a negative stat modifier has it cleared and gets the
 * alternate object state 5. */
void DuelEffect_ApplyCursebreaker(void) {
    DuelCardRecord *record;
    DuelEffectRequest *object;
    DuelFieldPosition *position;
    u8 *positions;
    s32 card;
    s32 next;
    s32 base_slot;
    s32 x;
    s32 timer;
    s32 step;
    s32 position_index;
    u8 *grid;

    if (DuelEffect_MarkInitialized() == 0) {
        gDuel_sEffectWorkCounter = -1;
        gDuel_wStepCounter = 0;
    }

    timer = gDuel_wStepCounter - 1;
    gDuel_wStepCounter = timer;
    if ((s16)timer > 0) {
        return;
    }

    gDuel_wStepCounter = 0x10;
    next = (u16)gDuel_sEffectWorkCounter + 1;
    gDuel_sEffectWorkCounter = next;
    if ((s16)next < DUEL_FIELD_ROW_SIZE) {
        /* This step walks the grid flat, with the side folded into base_slot
           below, while sibling DuelEffect_ApplySwords indexes it [side][slot]. The single-pass loop preserves the
           retail grid and side register allocation under GCC 2.8.1. The
           selected record uses DuelFieldPosition, with u16 member views to
           retain retail's unsigned halfword loads. */
        do {
            grid = gDuel_abFieldGridCardRecordIndices.flat;
            base_slot = DUEL_SIDE_SELECTOR_VOLATILE * DUEL_FIELD_SIDE_GRID_SLOT_COUNT +
                        DUEL_FIELD_SIDE_ZONE_COUNT;
        } while ((s16)next == 0 && (s16)next != 0);
        card = grid[(s16)next + base_slot];
        record = &gDuel_aCardRecords[card];
        object = DuelEffect_AllocateRequest(8);
        positions = (u8 *)gDuel_aFieldGridGroundPositions;
        step = gDuel_sEffectWorkCounter;
        /* Keep the row bias in the typed index for retail scheduling. */
        position_index = step + DUEL_FIELD_SIDE_ZONE_COUNT;
        object->phase = 3;
        position = (DuelFieldPosition *)(
            (u32)&((DuelFieldPosition *)0)[position_index] +
            DUEL_SIDE_SELECTOR_VOLATILE * DUEL_FIELD_SIDE_POSITION_BYTES +
            positions
        );
        x = (u16)position->x;
        object->field_02 = 0;
        object->field_00 = (s16)x;
        object->field_04 = (s16)(u16)position->y;
        object->buffer = (void *)((s32)object->buffer + step * 0x3000);
        SD_SEPlayFull(0x14);

        if ((record->flags & DUEL_CARD_FLAG_OCCUPIED) != 0) {
            if (record->stat_modifier < 0) {
                record->stat_modifier = 0;
                object->phase = 5;
            }
        }
    } else {
        gDuel_wCardEffectFlags = 0;
    }
}

void DuelEffect_StartRitual(void)
{
    if (!DuelEffect_MarkInitialized()) {
        gDuel_RitualResultCardId = Duel_CheckRitual(0, gDuel_wEffectCardID);
        if (gDuel_RitualResultCardId) {
            DuelEffectRequest *request = DuelEffect_CreateRequest(0x12);

            request->field_00 = 0xA0;
            request->field_02 = 0x78;
            request->phase = 1;
            SD_SEPlayFull(2);
            CardPreview_RequestResourceTransfer(1, gDuel_RitualResultCardId);
        }
    } else if (!(
        (gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
        gFile_dwCancelRequestState
    )) {
        gDuel_wCardEffectFlags = 0;
    }
}

/* Regional values: the ritual card's slide-in start and rest y (-34/22 ->
   -42/14) and the screen height it slides from (240 -> 256). The European
   build (src/game/european/) defines its own. */
#ifndef DUEL_RITUAL_CARD_START_Y
#define DUEL_RITUAL_CARD_START_Y -34
#endif
#ifndef DUEL_RITUAL_CARD_REST_Y
#define DUEL_RITUAL_CARD_REST_Y 22
#endif
#ifndef DUEL_RITUAL_SCREEN_HEIGHT
#define DUEL_RITUAL_SCREEN_HEIGHT 240
#endif

#ifndef DUEL_EFFECT_RITUAL_CONFIRM_MASK
#define DUEL_EFFECT_RITUAL_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#endif

void DuelEffect_ApplyRitual(void)
{
    DisplayObject *object;
    DuelCardRecord *card;
    DuelEffectChannel *text;
    s32 value;
    s32 timer;
    u16 flags;

    if (!DuelEffect_MarkInitialized()) {
        gDuel_RitualResultCardId = Duel_CheckRitual(
            &gDisplayObject_apWorkSlots.ritual.result, gDuel_wEffectCardID);
        if (gDuel_RitualResultCardId) {
            Duel_MarkCreatedCardKnown((s32)gDuel_RitualResultCardId);
            gDuelEffect_pTrackedRequest = DuelEffect_AllocateRequest(22);
            gDuel_wTrapPresentationMode = 0;
            gDuelEffect_pTrackedRequest->phase = gDuel_wEffectCardID;
            SD_CdSoundPlay(0x8022);
        } else {
            gDuel_wCardEffectFlags = 0;
        }
        return;
    }
    switch (gDuel_wTrapPresentationMode & 0xF) {
    case 0:
        if (gDuelEffect_pTrackedRequest->progress) {
            gDuel_bCardMoveSlot = gDisplayObject_apWorkSlots.slots[3]->field_6A;
            DuelCard_DeactivateRecord(&gDuel_aCardRecords[gDisplayObject_apWorkSlots.slots[2]->field_6A]);
            DuelCard_DeactivateRecord(&gDuel_aCardRecords[gDisplayObject_apWorkSlots.slots[3]->field_6A]);
            DuelCard_DeactivateRecord(&gDuel_aCardRecords[gDisplayObject_apWorkSlots.slots[4]->field_6A]);
            gDuel_wTrapPresentationMode = 1;
        }
        break;
    case 1: {
        u16 scale;
        s32 countdown;
        object = gDuel_pCardPreviewObject;
        if (!(gDuel_wTrapPresentationMode & 0x80)) {
            s32 attribute;
            s16 object_flags;
            gDuel_wTrapPresentationMode |= 0x80;
            object = (DisplayObject *)CardPreview_CreateResourceObjects(1, -1, -1);
            object->xy.h.x = 90;
            /* Negative constants go into these u16 halves through an s16
               view here and below; stored as u16, GCC materialises the
               zero-extended value (li 0xffde) where retail has li -34. */
            *(s16 *)&object->xy.h.y = DUEL_RITUAL_CARD_START_Y;
            object->field_60 = 24;
            attribute = object->attribute;
            object_flags = object->flags;
            object->field_44.scale.y = 0;
            object->field_44.scale.x = 0;
            object->attribute = attribute & 0xF7FFFFFF;
            object->flags = object_flags | 4;
            DisplayObject_ResetVelocity((DisplayObjectVelocity *)object);
            object->field_38.h.field_38 = 597;
            gDuel_pCardPreviewObject = object;
            SD_SEPlayFull(0x26);
        }
        DisplayObject_StepPositionY((DisplayObjectVelocity *)object);
        scale = (u16)object->field_44.scale.x + 170;
        countdown = (u16)object->field_60 - 1;
        object->field_44.scale.x = scale;
        object->field_44.scale.y = scale;
        object->field_60 = countdown;
        if ((s32)((u32)countdown << 16) <= 0) {
            s32 attribute;
            s16 object_flags;
            object->field_44.scale.y = 4096;
            object->field_44.scale.x = 4096;
            object->xy.h.y = DUEL_RITUAL_CARD_REST_Y;
            gDuel_wTrapPresentationMode = 2;
            attribute = object->attribute | 0x08000000;
            object_flags = object->flags & 0xFFFB;
            object->attribute = attribute;
            object->flags = object_flags;
        }
        break;
    }
    case 2:
        object = gDuel_pCardPreviewObject;
        if (!(gDuel_wTrapPresentationMode & 0x80)) {
            gDuel_wTrapPresentationMode |= 0x80;
            gDuel_sEffectWorkCounter = 32;
        }
        if (!(gDuel_wTrapPresentationMode & 0x40)) {
            timer = (u16)gDuel_sEffectWorkCounter - 1;
            gDuel_sEffectWorkCounter = timer;
            if ((s32)((u32)timer << 16) <= 0) {
                gDuel_wTrapPresentationMode |= 0x40;
                DuelCard_CaptureRoundedTexture(object);
                gDisplayObject_apWorkSlots.slots[0] =
                    Duel_CreateCardEffectOverlay(object);
                gDisplayObject_apWorkSlots.slots[0]->attribute |= 0x50000000;
                gDisplayObject_apWorkSlots.slots[0]->attribute &= ~0x08000000;
                gDisplayObject_apWorkSlots.slots[1] =
                    Duel_CreateCardEffectOverlay(object);
                DisplayObject_SetDepthOffset(gDisplayObject_apWorkSlots.slots[1], -1);
                gDisplayObject_apWorkSlots.slots[1]->attribute |= 0x60000000;
                gDisplayObject_apWorkSlots.slots[1]->attribute &= ~0x08000000;
                DuelEffect_ReleaseResourceObjects(1);
            }
        } else {
            DisplayObject *first = gDisplayObject_apWorkSlots.slots[0];
            DisplayObject *second = gDisplayObject_apWorkSlots.slots[1];
            value = first->field_44.scale.x + 128;
            second->field_44.scale.y = value;
            second->field_44.scale.x = value;
            first->field_44.scale.y = value;
            first->field_44.scale.x = value;
            value = (u8)gDisplayObject_apWorkSlots.slots[0]->rgb;
            value -= 4;
            if (value < 0)
                value = 0;
            value = value | ((value << 16) | (value << 8));
            gDisplayObject_apWorkSlots.slots[0]->rgb = value;
            gDisplayObject_apWorkSlots.slots[1]->rgb = value;
            if (!value) {
                DisplayObject_ReleaseIfPresent(gDisplayObject_apWorkSlots.slots[0]);
                DisplayObject_ReleaseIfPresent(gDisplayObject_apWorkSlots.slots[1]);
                gDuel_wTrapPresentationMode = 3;
            }
        }
        break;
    case 3: {
        u16 state = gDuel_wTrapPresentationMode;
        DuelEffectResourceRecord *resource = &gCardPreview_Resources[1];
        u8 *data;
        card = &gDuel_aCardRecords[gDuel_bCardMoveSlot];
        if (!(state & 0x80)) {
            u16 ritual;
            u16 y;
            /* The data pointer is reloaded through its address as u8 **, a
               non-struct read; as a plain card->data read the function's
               schedule changes (17 differences). */
            ((DuelDeckCardRecord *)card->data)->id =
                (s16)(ritual = gDuel_RitualResultCardId);
            data = *(u8 **)&card->data;
            gDuel_wTrapPresentationMode = state | 0x80;
            card->card_id = ritual;
            value = ((DuelDeckCardRecord *)data)->data_block_index;
            y = resource->src_y;
            resource->rects[0].x = resource->src_x + 56;
            resource->rects[0].w = 8;
            resource->rects[0].h = 88;
            resource->rects[0].y = y;
            StoreImage(&resource->rects[0],
                (u32 *)&gDuel_aCardThumbnailBlocks[value]);
            break;
        }
        Duel_SetupFieldCardFromDeck(
            gDuel_bCardMoveSlot, ((DuelDeckCardRecord *)card->data)->deck_index);
        object = card->object;
        gDisplayObject_apWorkSlots.slots[0] = object;
        *(s16 *)&object->xy.h.y = -DUEL_RITUAL_SCREEN_HEIGHT;
        if (gDuel_bSide0ControllerSelector[gDuel_bActiveSide] >= 0) {
            card->flags &= ~0x200;
            if (rand() & 1)
                card->flags |= 0x200;
            gDuel_wTrapPresentationMode = 5;
            break;
        }
        gDisplayObject_apWorkSlots.slots[1] =
            (DisplayObject *)Duel_CreateCardDisplayObject(card, 134, DUEL_RITUAL_SCREEN_HEIGHT);
        gDuel_wTrapPresentationMode = 4;
    }
    case 4:
        object = gDisplayObject_apWorkSlots.slots[1];
        if (!(gDuel_wTrapPresentationMode & 0x80)) {
            gDuel_wTrapPresentationMode |= 0x80;
            object->position.h.field_28 = 134;
            object->position.h.field_2A = 42;
            object->field_2C.h.field_2C = 16;
            object->field_6C = 1;
            object->update = (DisplayObjectCallback)DisplayObject_UpdateCosineSlide;
        } else if (!(gDuel_wTrapPresentationMode & 0x40)) {
            if (!DisplayObject_FindAllocatedByTag(1)) {
                gDuel_wSelectedCardID =
                    ((DuelCardStagingDeckView *)gDuel_abCardStagingBuffer)->field_cards[object->field_6A].card_id;
#ifdef VERSION_JAPAN
                text = TextBox_CreateFlagged(0, 33, 80, 110, 160, 48, 32);
#elif defined(VERSION_EUROPE)
                /* The European box takes 64 in its last argument and keeps
                   the text size InitEntry sets. */
                text = TextBox_CreateFlagged(0, 33, 72, 110, 176, 48, 64);
#else
                text = TextBox_CreateFlagged(0, 33, 72, 110, 176, 48, 32);
                text->glyph_advance_x = 8;
                text->line_advance_y = 16;
#endif
                do {
                    TextBox_UpdateChannels();
                } while (!text->prompt_object);
                gDuel_wTrapPresentationMode |= 0x40;
            }
        } else if (!(gDuel_wTrapPresentationMode & 0x20)) {
            if (!Dialog_ReadChoiceInput(gText_aChannels) &&
                (gInput_awPadPressed[0] & DUEL_EFFECT_RITUAL_CONFIRM_MASK)) {
                SD_SEPlayFull(7);
                object->position.h.field_28 = 134;
                *(s16 *)&object->position.h.field_2A = -128;
                object->field_2C.h.field_2C = 16;
                object->field_6C = 1;
                object->update = (DisplayObjectCallback)DisplayObject_UpdateCosineSlide;
                card = &gDuel_aCardRecords[object->field_6A];
                flags = card->flags & ~0x200;
                card->flags = flags;
                if (gDialog_bChoice)
                    card->flags = flags | 0x200;
                TextBox_Destroy(gText_aChannels);
                gDuel_wTrapPresentationMode |= 0x20;
            }
        } else if (!DisplayObject_FindAllocatedByTag(1)) {
            DisplayObject_ReleaseIfPresent(object);
            gDuel_wTrapPresentationMode = 5;
        }
        break;
    case 5: {
        s16 phase;
        object = gDisplayObject_apWorkSlots.slots[0];
        if (!(gDuel_wTrapPresentationMode & 0x80)) {
            gDuel_wTrapPresentationMode |= 0x80;
            DisplayObject_SavePosition((DisplayObjectSnapshot *)object);
            object->field_60 = -1024;
        }
        Widget_SlideSine((DisplayObjectPosition *)object,
            (s16)object->xy.h.x, -24, object->field_60);
        phase = (u16)object->field_60 + 42;
        object->field_60 = phase;
        if (phase >= 0) {
            *(s16 *)&object->xy.h.y = -24;
            gDuel_wCardEffectFlags = 0;
        }
        break;
    }
    }
}

void DuelEffect_ApplyHarpiesFeatherDuster(void)
{
    int i;

    if (!DuelEffect_MarkInitialized()) {
        DuelEffectRequest *object = DuelEffect_CreateRequest(0x17);
        int side = gDuel_bActiveSide ^ 1;

        object->field_04 = gDuel_aFieldGridGroundPositions[side][0].y;
#if !defined(VERSION_EUROPE)
        printf("%d\n", (short)object->field_04);
#endif
    } else {
        for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
            int position =
                i + gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
            DuelCardRecord *entry =
                &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices.flat[position]];

            if (entry->flags & DUEL_CARD_FLAG_OCCUPIED)
                DuelCard_RemoveFromField(entry);
        }
        gDuel_wCardEffectFlags = 0;
    }
}

int DuelEffect_UpdateCardEffect(void)
{
    u16 flags = gDuel_wCardEffectFlags;

    if (flags & DUEL_CARD_EFFECT_FLAG_ACTIVE) {
        u8 *indices = gDuelEffect_abGroupByEffectId;
        DuelEffectHandler *callbacks;
        int index = indices[gDuel_sCardEffectIndex] * DUEL_CARD_EFFECT_HANDLERS_PER_GROUP;

        if (flags & DUEL_CARD_EFFECT_FLAG_SECOND_HANDLER)
            index++;
        callbacks = gDuelEffect_apfnGroupHandler;
        callbacks[index]();
    }
    return gDuel_wCardEffectFlags;
}

void DuelEffect_StartCardEffect(int value, int flag)
{
    int index;

    if (((unsigned)(value - DUEL_EFFECT_FIRST_BLOCK_CARD_ID) <
         DUEL_EFFECT_CARD_BLOCK_SIZE) ||
        ((unsigned)(value - DUEL_EFFECT_SECOND_BLOCK_CARD_ID) <
         DUEL_EFFECT_CARD_BLOCK_SIZE) ||
        value == DUEL_DARK_MAGIC_RITUAL_CARD_ID) {
        if (value < DUEL_EFFECT_SECOND_BLOCK_CARD_ID) {
            index = value - DUEL_EFFECT_FIRST_BLOCK_CARD_ID;
        } else {
            index = value -
                (DUEL_EFFECT_SECOND_BLOCK_CARD_ID -
                 DUEL_EFFECT_CARD_BLOCK_SIZE);
            if (value == DUEL_DARK_MAGIC_RITUAL_CARD_ID) {
                index = DUEL_DARK_MAGIC_RITUAL_EFFECT_INDEX;
            }
        }
        gDuel_sCardEffectIndex = index;
        gDuel_wEffectCardID = value;
        gDuel_wCardEffectFlags = DUEL_CARD_EFFECT_FLAG_ACTIVE;
        if (flag) {
            gDuel_wCardEffectFlags =
                DUEL_CARD_EFFECT_FLAG_ACTIVE |
                DUEL_CARD_EFFECT_FLAG_SECOND_HANDLER;
        }
    }
}

#define DUEL_CARD_RECORD_AT_OFFSET(records, offset) \
    ((DuelCardRecord *)((u8 *)(records) + (offset)))

s32 Duel_FindFreeFieldSlot(s32 start)
{
    s32 base;
    s32 off;
    DuelCardRecord *p;
    s32 i;

    do {
        base = gDuel_bActiveSide * DUEL_CARD_SIDE_RECORD_COUNT + start;
        off = base << 3;
        off -= base;
        off <<= 2;
        p = DUEL_CARD_RECORD_AT_OFFSET(gDuel_aCardRecords, off);
        for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
            off = p->flags & DUEL_CARD_FLAG_OCCUPIED;
            if (off == 0) {
                return gDuel_bActiveSide * DUEL_CARD_SIDE_RECORD_COUNT + start + i;
            }
            p++;
        }
    } while (0);
    return -1;
}

s32 Duel_CollectFieldCardsBelowType(DuelCardRecord **out, s32 start,
                                    s32 type_limit)
{
    s32 count = 0;
    s32 i = 0;
    s32 side = gDuel_bActiveSide;
    s32 *stats = gDuel_adwCardStats;
    DuelCardRecord *record =
        &gDuel_aCardRecords[side * DUEL_CARD_SIDE_RECORD_COUNT + start];
    s32 k;

    do {
        if (record->flags & DUEL_CARD_FLAG_OCCUPIED) {
            k = record->card_id;
            k--;
            if (((stats[k] >> CARD_STAT_TYPE_SHIFT) &
                 CARD_STAT_TYPE_MASK) < type_limit) {
                *out++ = record;
                count++;
            }
        }
        i++;
        record++;
    } while (i < DUEL_FIELD_ROW_SIZE);

    *out = 0;
    return count;
}

s32 Duel_CollectFieldCardsByType(DuelCardRecord **out, s32 start,
                                 s32 type) {
    s32 count = 0;
    s32 i = 0;
    s32 side = gDuel_bActiveSide;
    s32 *stats = gDuel_adwCardStats;
    DuelCardRecord *record =
        &gDuel_aCardRecords[side * DUEL_CARD_SIDE_RECORD_COUNT + start];
    s32 k;

    do {
        /* The `do { } while (0)` is load-bearing and NOT a macro leftover:
         * it is 2 differences without it and 6 with `{ }` or `if (1) { }`
         * instead, because gcc's loop pass sees the extra loop node and
         * numbers the blocks differently. Coupled with `count` being
         * initialised before `i` -- that alone is 6. Found by the permuter;
         * see docs/PARKED.txt's former entry. */
        do {
            if (record->flags & DUEL_CARD_FLAG_OCCUPIED) {
                if (type < 0 ||
                    (k = record->card_id, k--,
                     ((stats[k] >> CARD_STAT_TYPE_SHIFT) &
                      CARD_STAT_TYPE_MASK) == type)) {
                    *out++ = record;
                    count++;
                }
            }
        } while (0);
        i++;
        record++;
    } while (i < DUEL_FIELD_ROW_SIZE);

    *out = 0;

    return count;
}

s32 Duel_SelectEquipPair(void) {
    DuelCardRecord *targets[DUEL_FIELD_ROW_SIZE + 1];
    DuelCardRecord *equips[DUEL_FIELD_ROW_SIZE + 1];
    DuelCardRecord **target_entry;
    DuelCardRecord **equip_entry;
    DuelCardRecord *equip;
    DuelCardRecord *target;
    s32 free_slot;
    s32 target_index;
    s32 equip_index;

    if (Duel_CollectFieldCardsByType(equips, 0, CARD_TYPE_EQUIP) != 0) {
        Duel_CollectFieldCardsByType(targets, DUEL_FIELD_ROW_SIZE, -1);
        equip = equips[0];
        if (equip != 0) {
            equip_entry = equips;
            do {
                target = targets[0];
                if (target != 0) {
                    target_entry = targets;
                    do {
                        if (Duel_CheckEquip(equip->card_id, target->card_id) != 0) {
                            gAi_Selection.bytes[0] =
                                (s8)equip->table_index % DUEL_FIELD_ROW_SIZE + 0xB;
                            gAi_Selection.bytes[1] = 0;
                            gAi_Selection.bytes[6] =
                                (s8)target->table_index % DUEL_FIELD_ROW_SIZE + 1;
                            gAi_Selection.bytes[7] = 0;
                            gAi_Selection.bytes[8] = 0;
                            return 0;
                        }
                        target_entry++;
                        target = *target_entry;
                    } while (target != 0);
                }
                equip_entry++;
                equip = *equip_entry;
            } while (equip != 0);
        }

        free_slot = Duel_FindFreeFieldSlot(DUEL_FIELD_ROW_SIZE);
        if (free_slot < 0) {
            return 1;
        }
        Duel_CollectFieldCardsBelowType(targets, 0, CARD_TYPE_MAGIC);
        equip = equips[0];
        if (equip != 0) {
            equip_entry = equips;
            do {
                target = targets[0];
                if (target != 0) {
                    target_entry = targets;
                    do {
                        if (Duel_CheckEquip(equip->card_id, target->card_id) != 0) {
                            target_index = (s8)target->table_index;
                            gAi_Selection.bytes[0] =
                                target_index % DUEL_FIELD_ROW_SIZE + 0xB;
                            equip_index = (s8)equip->table_index;
                            gAi_Selection.bytes[2] = 0;
                            gAi_Selection.bytes[7] = 0;
                            gAi_Selection.bytes[8] = 0;
                            gAi_Selection.bytes[6] =
                                free_slot % DUEL_FIELD_ROW_SIZE + 1;
                            gAi_Selection.bytes[1] =
                                equip_index % DUEL_FIELD_ROW_SIZE + 0xB;
                            return 0;
                        }
                        target_entry++;
                        target = *target_entry;
                    } while (target != 0);
                }
                equip_entry++;
                equip = *equip_entry;
            } while (equip != 0);
        }
    }
    return 1;
}

s32 Duel_SelectTrapPlay(void) {
    DuelCardRecord *trap_cards[6];
    s32 slot;
    s8 v;

    if (Duel_CollectFieldCardsByType(trap_cards, 0, CARD_TYPE_TRAP) == 0) {
        return 1;
    }
    slot = Duel_FindFreeFieldSlot(DUEL_FIELD_SIDE_ZONE_COUNT);
    if (slot < 0) {
        return 1;
    }
    v = trap_cards[0]->table_index;
    gAi_Selection.bytes[8] = 1;
    gAi_Selection.bytes[1] = 0;
    gAi_Selection.bytes[7] = 0;
    gAi_Selection.bytes[6] = slot % DUEL_FIELD_ROW_SIZE + 6;
    gAi_Selection.bytes[0] = v % DUEL_FIELD_ROW_SIZE + 0xB;
    return 0;
}

#include "../psyq/rand.h"
#include "ai.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_grid.h"
#include "rand_get_interval.h"
#include "duel_card_staging.h"

/* The AI's turn-action run: spell and fusion searches feed the action pick
   through gAi_Selection, followed by the occupied and face-up field-target
   selectors and the final per-turn play decision that consumes them. All six
   functions are contiguous and share the gcc_2_8_1_g8_split profile. */

/* The pending selection lives in ai.h. Keep its matched field accesses and
   the independent interior-symbol aliases used by the script handlers. */

/* Signed locals preserve the spell search's table_index reads from the
   canonical records, whose stored byte is unsigned. */

int Ai_SelectMagicPlay(void)
{
 DuelCardRecord*list[6];int count=Duel_CollectFieldCardsByType(list,0,CARD_TYPE_MAGIC);
 if(count){s8 first=list[0]->table_index;int index;gAi_Selection.fields.result=first%DUEL_FIELD_ROW_SIZE+11;gAi_Selection.fields.field1=0;gAi_Selection.fields.zero=0;gAi_Selection.fields.face_down=0;gAi_Selection.fields.value=6;
  index=Duel_FindFreeFieldSlot(DUEL_FIELD_SIDE_ZONE_COUNT);if(index>=0){gAi_Selection.fields.face_down=rand()&1;gAi_Selection.fields.value+=index%DUEL_FIELD_ROW_SIZE;}return 0;}return 1;
}

/* AI fusion attempt. Collects hand monsters, rolls a coin, and looks for a
   fusable pair with the occupied monster row (first pass) or, when a field
   slot is free, between two distinct hand entries (second pass).
   A hit fills the gAi_Selection selection with the two slot
   bytes and the target slot; returns 0 on a selection, 1 otherwise. */
s32 Ai_SelectFusionPlay(void) {
    DuelCardRecord *list1[6];
    DuelCardRecord *list0[6];
    DuelCardRecord *card_a;
    DuelCardRecord *card_b;
    s32 count;
    s32 slot;
    s32 i;
    s32 j;

    count = Duel_CollectFieldCardsBelowType(list0, 0, CARD_TYPE_MAGIC);
    if (count == 0) {
        return 1;
    }
    count--;
    if (count != 0) {
        Rand_GetInterval(count + 1);
    }
    if ((rand() & 1) == 0) {
        return 1;
    }
    Duel_CollectFieldCardsByType(list1, DUEL_FIELD_ROW_SIZE, -1);
    /* loop.c moves each selection exit after the barrier of the first
       `return 1`, giving retail's order of both exits ahead of phase two. */
    for (i = 0; (card_a = list0[i]) != 0; i++) {
        for (j = 0; (card_b = list1[j]) != 0; j++) {
            if (Duel_CheckFusion(card_a->card_id, card_b->card_id)) {
                AiSelectionFields *st;
                s8 v;

                st = &gAi_Selection.fields;
                v = card_a->table_index;
                st->field1 = 0;
                st->result = v % DUEL_FIELD_ROW_SIZE + 0xB;
                v = card_b->table_index;
                st->value = v % DUEL_FIELD_ROW_SIZE + 1;
                st->zero = rand() & 1;
                st->face_down = 0;
                return 0;
            }
        }
    }
    slot = Duel_FindFreeFieldSlot(DUEL_FIELD_ROW_SIZE);
    if (slot < 0) {
        return 1;
    }
    Duel_CollectFieldCardsBelowType(list1, 0, CARD_TYPE_MAGIC);
    for (i = 0; (card_a = list0[i]) != 0; i++) {
        for (j = 0; (card_b = list1[j]) != 0; j++) {
            if ((s8)card_a->table_index != (s8)card_b->table_index &&
                Duel_CheckFusion(card_a->card_id, card_b->card_id)) {
                AiSelectionFields *st;
                s8 v;
                s8 w;

                st = &gAi_Selection.fields;
                v = card_a->table_index;
                st->result = v % DUEL_FIELD_ROW_SIZE + 0xB;
                w = card_b->table_index;
                st->field_02 = 0;
                st->value = slot % DUEL_FIELD_ROW_SIZE + 1;
                st->field1 = w % DUEL_FIELD_ROW_SIZE + 0xB;
                st->zero = rand() & 1;
                st->face_down = 0;
                return 0;
            }
        }
    }
    return 1;
}

/* AI turn action pick: tries the equip, trap and spell searches at random
   odds, then picks a random card from the side's hand records and fills
   the gAi_Selection selection with a field slot for it: a monster goes to the
   front row with a random flag, anything else to the back row, forcing the
   flag on for an equip. Always returns 0. */
s32 Ai_SelectHandPlay(void) {
    DuelCardRecord *card;
    s32 slot;

    if ((rand() & 1) == 0 && Duel_SelectEquipPair() == 0) {
        return 0;
    }
    if ((rand() & 1) == 0 && Duel_SelectTrapPlay() == 0) {
        return 0;
    }
    if ((rand() & 3) == 0 && Ai_SelectMagicPlay() == 0) {
        return 0;
    }
    if (Ai_SelectFusionPlay() == 0) {
        return 0;
    }
    card = &gDuel_aCardRecords[
        gDuel_bActiveSide * DUEL_CARD_SIDE_RECORD_COUNT +
        Rand_GetInterval(DUEL_FIELD_ROW_SIZE)
    ];
    if (((gDuel_adwCardStats[card->card_id - 1] >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK) < CARD_TYPE_MAGIC) {
        s8 v;

        slot = Duel_FindFreeFieldSlot(DUEL_FIELD_ROW_SIZE);
        if (slot < 0) {
            slot = Rand_GetInterval(DUEL_FIELD_ROW_SIZE);
        }
        v = card->table_index;
        gAi_Selection.fields.field1 = 0;
        gAi_Selection.fields.value = slot % DUEL_FIELD_ROW_SIZE + 1;
        gAi_Selection.fields.result = v % DUEL_FIELD_ROW_SIZE + 0xB;
        gAi_Selection.fields.zero = rand() & 1;
        gAi_Selection.fields.face_down = 1;
    } else {
        s8 v;

        slot = Duel_FindFreeFieldSlot(DUEL_FIELD_SIDE_ZONE_COUNT);
        if (slot < 0) {
            slot = Rand_GetInterval(DUEL_FIELD_ROW_SIZE);
        }
        v = card->table_index;
        gAi_Selection.fields.field1 = 0;
        gAi_Selection.fields.zero = 0;
        gAi_Selection.fields.value = slot % DUEL_FIELD_ROW_SIZE + 6;
        gAi_Selection.fields.result = v % DUEL_FIELD_ROW_SIZE + 0xB;
        gAi_Selection.fields.face_down = rand() & 1;
        if (((gDuel_adwCardStats[card->card_id - 1] >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK) == CARD_TYPE_EQUIP) {
            gAi_Selection.fields.face_down = 1;
        }
    }
    return 0;
}

int Ai_FindFavorableFieldTarget(DuelCardRecord *source)
{
    int count = 0;
    int slot = DUEL_FIELD_ROW_SIZE;

    do {
        int position =
            slot + gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
        DuelCardRecord *entry = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices.flat[position]];

        if (entry->flags & DUEL_CARD_FLAG_OCCUPIED) {
            count++;
            if (Duel_CompareBattleStrength(source->object,
                              entry->object) > 0)
                return entry->object->field_6A;
        }
        slot++;
    } while (slot < DUEL_FIELD_SIDE_ZONE_COUNT);

    if (count == 0)
        return gDuel_abFieldGridCardRecordIndices.flat[
            gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT + 7
        ];
    return -1;
}

s32 Ai_FindFavorableFaceUpFieldTarget(DuelCardRecord *source)
{
    s32 i;
    s32 found = 0;
    DuelCardRecord *entry = 0;

    for (i = DUEL_FIELD_ROW_SIZE; i < DUEL_FIELD_SIDE_ZONE_COUNT; i++) {
        u8 row = gDuel_abFieldGridCardRecordIndices.flat[
            i + gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT
        ];
        entry = &gDuel_aCardRecords[row];
        if (entry->flags & DUEL_CARD_FLAG_OCCUPIED) {
            found++;
            if (!(entry->flags & DUEL_CARD_FLAG_FACE_DOWN)) {
                s32 result = Duel_CompareBattleStrength(
                    source->object,
                    entry->object);
                if (result > 0) {
                    return entry->object->field_6A;
                }
            }
        }
    }

    if (found == 0) {
        return gDuel_abFieldGridCardRecordIndices.flat[
            gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT + 7
        ];
    }
    return -1;
}

/* The AI's per-turn play decision. Writes the chosen action into the three
 * trailing bytes of the gAi_Selection selection: the action code in source_code,
 * its argument in target_code, and a flag in field_0B. It does nothing while bit
 * 0x1000 of gDuel_wOutcomeFlags is set.
 *
 * One turn in four it first looks for an equip: it collects the field's
 * equips (type 0x17) and occupied monster-row cards, and takes the first pair
 * Duel_CheckEquip accepts. Failing that it collects the type-0x14 magic cards and
 * picks one at random. Otherwise, and when neither search finds anything, it
 * walks grid slots 0xA to 0xE for the first card whose top two
 * flag bits read 10, and decides what to do with it through Ai_FindFavorableFaceUpFieldTarget and
 * Ai_FindFavorableFieldTarget, falling back to scanning the opponent's five monster zones
 * for one with 0x90000000 set.
 *
 * The walk body keeps retail's block order inside a do-while(0): the play
 * block first, then the Ai_FindFavorableFieldTarget fallback with the give-up block inline,
 * then the zone scan. Its record, grid and give-up arithmetic are written
 * index-first through integer casts, which is the operand order retail's
 * addu instructions show. */
s32 Ai_SelectFieldPlay(void)
{
    DuelCardRecord *pool[6];
    DuelCardRecord *listb[6];
    DuelCardRecord *lista[6];
    DuelCardRecord **equip_entry;
    DuelCardRecord **target_entry;
    DuelCardRecord **pool_cursor;
    DuelCardRecord *equip;
    DuelCardRecord *target;
    DuelCardRecord *rec;
    DuelCardRecord *scan;
    s32 slot;
    s32 n;
    s32 i;
    s32 mask;
    s32 res;
    s32 count;
    s32 v;
    u8 *grid;
    AiSelectionFields *out;
    DuelCardRecord *recs;
    DuelCardRecord *scanbase;
    s32 table_index;

    gAi_Selection.fields.source_code = 0;
    if (gDuel_wOutcomeFlags & 0x1000) {
        return 0;
    }
    if ((rand() & 3) == 0) {
        Duel_CollectFieldCardsByType((DuelCardRecord **)lista, 0xA, 0x17);
        Duel_CollectFieldCardsByType((DuelCardRecord **)listb, 5, -1);
        equip = lista[0];
        if (equip != 0) {
            equip_entry = lista;
            do {
                target = listb[0];
                if (target != 0) {
                    target_entry = listb;
                    do {
                        if (Duel_CheckEquip(equip->card_id,
                                            target->card_id)) {
                            gAi_Selection.fields.source_code = (s8)equip->table_index % 5 + 6;
                            gAi_Selection.fields.target_code = (s8)target->table_index % 5 + 1;
                            gAi_Selection.fields.field_0B = 0;
                            return 0;
                        }
                        target_entry++;
                        target = *target_entry;
                    } while (target != 0);
                }
                equip_entry++;
                equip = *equip_entry;
            } while (equip != 0);
        }
        n = Duel_CollectFieldCardsByType((DuelCardRecord **)lista, 0xA, 0x14);
        if (n != 0) {
            n--;
            if (n != 0) {
                n = Rand_GetInterval(n + 1);
            }
            gAi_Selection.fields.source_code = (s8)lista[n]->table_index % 5 + 6;
            gAi_Selection.fields.target_code = 0;
            gAi_Selection.fields.field_0B = 0;
            return 0;
        }
    }
    slot = 0xA;
    grid = gDuel_abFieldGridCardRecordIndices.flat;
    out = &gAi_Selection.fields;
    recs = gDuel_aCardRecords;
    scanbase = &gDuel_aCardRecords[5];
loop:
    {
        do {
            n = gDuel_bActiveSide * 20;
            /* Integer address arithmetic keeps retail's addu operand order. */
            rec = (DuelCardRecord *)((s32)*(u8 *)(slot + n + (s32)grid) * 28 + (s32)recs);
            /* Retail tests terrain_modifier and flags as one packed word. */
            if ((*(u32 *)&rec->terrain_modifier & 0xC0000000) == 0x80000000) {
                v = Ai_FindFavorableFaceUpFieldTarget(rec);
                if (v >= 0) {
    have:
                    if (gDuel_pActiveSideState->swords_turns_remaining == 0) {
                        table_index = (s8)rec->table_index;
                        slot = table_index % 5 + 1;
                        out->field_0B = 0;
                        out->target_code = v % 5 + 0x38;
                        out->source_code = slot;
                        return 0;
                    }
                    goto none;
                }
                if ((rand() & 3) == 0) {
                    v = Ai_FindFavorableFieldTarget(rec);
                    if (v >= 0) {
                        goto have;
                    }
    none:
                    v = (s8)rec->table_index % 15;
                    out->target_code = 0;
                    res = (s8)v - 4;
                    out->field_0B = 1;
                    out->source_code = res;
                    return 0;
                }
                if ((rand() & 1) != 0) {
                    count = 0;
                    i = 0;
                    scan = (DuelCardRecord *)((s32)((gDuel_bActiveSide ^ 1) * 15) * 28 + (s32)scanbase);
                    mask = 0x90000000;
                    pool_cursor = pool;
                    do {
                        if ((*(u32 *)&scan->terrain_modifier & mask)
                            == mask) {
                            *pool_cursor = scan;
                            pool_cursor++;
                            count++;
                        }
                        i++;
                        scan++;
                    } while (i < 5);
                    if (count != 0) {
                        v = count - 1;
                        if (v != 0) {
                            v = Rand_GetInterval(count);
                        }
                        v = (s8)pool[v]->table_index;
                        goto have;
                    }
                }
                goto none;
            }
        } while (0);
        slot++;
    }
    if (slot < 0xF) {
        goto loop;
    }
    return 0;
}

void Ai_ExportSideSnapshot(AiActiveCard *out, s32 side) {
    DuelCardRecord *base;
    s32 i;

    base = &gDuel_aCardRecords[side * DUEL_CARD_SIDE_RECORD_COUNT];

    {
        DuelCardRecord *rec;
        s32 *tbl;

        rec = base + DUEL_FIELD_ROW_SIZE;
        i = 0;
        tbl = gDuel_adwCardStats;
        do {
            out->card_id = 0;
            if (rec->flags & DUEL_CARD_FLAG_OCCUPIED) {
                s32 stats;
                s32 star_bits;

                out->card_id = rec->card_id;
                stats = Duel_CalcCardStats(rec);
                out->attack = stats;
                out->defense = stats >> 16;
                out->flags = rec->flags;
                out->card_type =
                    ((DuelCardDisplayObject *)rec->object)->card_type;
                if (rec->flags & DUEL_CARD_FLAG_USE_GUARDIAN_STAR_2) {
                    /* tbl[card_id - 1] folds the -1 into the load offset; retail subtracts first. */
                    star_bits = *(s32 *) ((( rec->card_id - 1) << 2) + (u32) tbl) >>
                        CARD_STAT_GUARDIAN_STAR_2_SHIFT;
                } else {
                    star_bits = *(s32 *) ((( rec->card_id - 1) << 2) + (u32) tbl) >>
                        CARD_STAT_GUARDIAN_STAR_1_SHIFT;
                }
                out->guardian_star = star_bits & CARD_STAT_GUARDIAN_STAR_MASK;
            }
            i += 1;
            out += 1;
            rec += 1;
        } while (i < DUEL_FIELD_ROW_SIZE);
    }

    {
        DuelCardRecord *rec;
        s32 *tbl;

        rec = base + DUEL_FIELD_SIDE_ZONE_COUNT;
        i = 0;
        tbl = gDuel_adwCardStats;
        do {
            out->card_id = 0;
            if (rec->flags & DUEL_CARD_FLAG_OCCUPIED) {
                s32 stats;
                s32 star_bits;

                out->card_id = rec->card_id;
                stats = Duel_CalcCardStats(rec);
                out->attack = stats;
                out->defense = stats >> 16;
                out->flags = rec->flags;
                out->card_type =
                    ((DuelCardDisplayObject *)rec->object)->card_type;
                if (rec->flags & DUEL_CARD_FLAG_USE_GUARDIAN_STAR_2) {
                    star_bits = *(s32 *) ((( rec->card_id - 1) << 2) + (u32) tbl) >>
                        CARD_STAT_GUARDIAN_STAR_2_SHIFT;
                } else {
                    star_bits = *(s32 *) ((( rec->card_id - 1) << 2) + (u32) tbl) >>
                        CARD_STAT_GUARDIAN_STAR_1_SHIFT;
                }
                out->guardian_star = star_bits & CARD_STAT_GUARDIAN_STAR_MASK;
            }
            i += 1;
            out += 1;
            rec += 1;
        } while (i < DUEL_FIELD_ROW_SIZE);
    }

    {
        s32 *tbl;

        i = 0;
        do {
            s32 n;

            n = gDuel_aSideStates[side].hand[i];
            if (n >= 0) {
                s32 *p;
                s32 id;

                id = ((DuelCardStagingDeckView *)gDuel_abCardStagingBuffer)->cards[n].id;
                out->card_id = id;
                tbl = gDuel_adwCardStats;
                p = (s32 *) (((id - 1) << 2) + (u32) tbl);
                out->attack = (*p & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                out->defense = ((*p >> CARD_STAT_DEFENSE_SHIFT) &
                                CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                out->flags = 0;
                out->card_type =
                    (*p >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
                out->guardian_star =
                    (*p >> CARD_STAT_GUARDIAN_STAR_1_SHIFT) &
                    CARD_STAT_GUARDIAN_STAR_MASK;
                out->guardian_star_2 =
                    (*p >> CARD_STAT_GUARDIAN_STAR_2_SHIFT) &
                    CARD_STAT_GUARDIAN_STAR_MASK;
                out += 1;
            }
            i += 1;
        } while (i < HAND_SIZE);
    }

    {
        DuelDeckCardRecord *deck_card;

        i = gDuel_aSideStates[side].deck_draw_cursor;
        deck_card = &gDuel_aDeckCardRecords[i + side * DECK_SIZE];
        if (i < DECK_SIZE) {
            s32 *tbl;

            tbl = gDuel_adwCardStats;
            do {
                s32 *p;
                s32 id;

                id = deck_card->id;
                out->card_id = id;
                p = (s32 *) (((id - 1) << 2) + (u32) tbl);
                out->attack = (*p & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                out->defense = ((*p >> CARD_STAT_DEFENSE_SHIFT) &
                                CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                out->flags = 0;
                out->card_type =
                    (*p >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
                i += 1;
                out->guardian_star =
                    (*p >> CARD_STAT_GUARDIAN_STAR_1_SHIFT) &
                    CARD_STAT_GUARDIAN_STAR_MASK;
                out->guardian_star_2 =
                    (*p >> CARD_STAT_GUARDIAN_STAR_2_SHIFT) &
                    CARD_STAT_GUARDIAN_STAR_MASK;
                out->deck_index = deck_card->deck_index;
                deck_card += 1;
                out += 1;
            } while (i < DECK_SIZE);
        }
    }
    out->card_id = 0;
}

#include "duel_card_layout.h"

void Ai_RefreshBoardSnapshot(void)
{
    Ai_ExportSideSnapshot(gAiSnapshot_aOwnCards, gDuel_bActiveSide);
    Ai_ExportSideSnapshot(gAiSnapshot_aOwnCards + AI_ACTIVE_CARD_SIDE_SLOT_STRIDE,
                 gDuel_bActiveSide ^ 1);
}

int DuelCard_DecodeTaggedSlotIndex(int value)
{
    if (value & 0x80) {
        return (value & 0x7F) + DUEL_CARD_SIDE_RECORD_COUNT;
    }
    return value;
}

#include "card_constants.h"
#include "duel_card_layout.h"
#include "duel_card.h"

s32 DuelCard_GetGuardianStar(DuelCardRecord *object)
{
    if (object->flags & DUEL_CARD_FLAG_USE_GUARDIAN_STAR_2) {
        s32 *table = gDuel_adwCardStats;
        s32 index = object->card_id - 1;

        return (table[index] >> CARD_STAT_GUARDIAN_STAR_2_SHIFT) &
               CARD_STAT_GUARDIAN_STAR_MASK;
    } else {
        s32 *table = gDuel_adwCardStats;
        s32 index = object->card_id - 1;

        return (table[index] >> CARD_STAT_GUARDIAN_STAR_1_SHIFT) &
               CARD_STAT_GUARDIAN_STAR_MASK;
    }
}
