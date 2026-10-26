#ifndef MEMORIES_DECOMP_DUEL_GRID_H
#define MEMORIES_DECOMP_DUEL_GRID_H

#include "../types.h"

#define DUEL_SIDE_COUNT 2
#define DUEL_FIELD_ROW_SIZE 5
#define DUEL_FIELD_SIDE_ZONE_COUNT 10
#define DUEL_FIELD_SIDE_GRID_SLOT_COUNT 20

/* Reference-target Z lookup by side, selector page and grid row. The values
 * feed Duel_StartViewTween, not the cursor's position-step calculation. */
extern s16 gDuel_awFieldGridRowViewTargetZ[DUEL_SIDE_COUNT][2][4];

/* Signed layout pairs. The field-grid ground table uses x/y as X/Z before
 * projection; cursor movement must preserve negative coordinate deltas. */
typedef struct {
    s16 x;
    s16 y;
} DuelFieldPosition;

#define DUEL_FIELD_SIDE_POSITION_BYTES \
    (DUEL_FIELD_SIDE_GRID_SLOT_COUNT * sizeof(DuelFieldPosition))

/* Cursor coordinates are the signed col/row members of the actual
 * DuelSelectionRecord in duel_selection_layout.h, not a separate object. */

/* The field grid: DUEL_SIDE_COUNT blocks of DUEL_FIELD_SIDE_GRID_SLOT_COUNT
 * bytes, each byte a gDuel_aCardRecords record index. The shape is not a guess --
 * Duel_InitScene (src/game/duel_init_scene.c) and
 * DuelScene_UpdateTurnSwitch (src/game/duel_scene_battle.c) both
 * build a per-side cursor as
 * gDuel_abFieldGridCardRecordIndices + gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT.
 *
 * Left unsized on purpose: a declared size is a -G input for this toolchain,
 * and no consumer needs the bound. */
#ifdef VERSION_JAPAN
#define DUEL_FIELD_GRID_LABEL asm("gJapanese_DuelFieldGridCardRecordIndices")
#define DUEL_FIELD_GRID_ALIAS_LABEL DUEL_FIELD_GRID_LABEL
#else
#define DUEL_FIELD_GRID_LABEL
#define DUEL_FIELD_GRID_ALIAS_LABEL asm("gDuel_abFieldGridCardRecordIndices")
#endif
#ifdef DUEL_FIELD_GRID_ALIASES
extern u8 D_800907D8_2d
    [DUEL_SIDE_COUNT][DUEL_FIELD_SIDE_GRID_SLOT_COUNT] DUEL_FIELD_GRID_ALIAS_LABEL;
extern u8 D_800907D8_flat[] DUEL_FIELD_GRID_ALIAS_LABEL;
#define gDuel_abFieldGridCardRecordIndices D_800907D8_flat
#elif defined(DUEL_FIELD_GRID_2D)
/* duel_card_effects.c and DuelEffect_ApplySwords
 * (src/candidates/func_80025F3C.c)
 * index it [side][slot]. That is not a spelling preference: rewriting either
 * of them to the flat index the other sixteen files use builds to the right
 * size and differs from byte 0x80025CA2.
 *
 * DuelEffect_ApplySwords shares duel_field_effects.c with
 * DuelEffect_ApplyCursebreaker and DuelEffect_ApplyHarpiesFeatherDuster, which
 * walk the grid flat, so the unit defines DUEL_FIELD_GRID_2D for the
 * [side][slot] user and casts at the flat ones. */
extern u8 gDuel_abFieldGridCardRecordIndices[DUEL_SIDE_COUNT][DUEL_FIELD_SIDE_GRID_SLOT_COUNT]
    DUEL_FIELD_GRID_LABEL;
#else
extern u8 gDuel_abFieldGridCardRecordIndices[] DUEL_FIELD_GRID_LABEL;
#endif

/* The hand table defined immediately before that grid, in the same unit:
 * duel_field_layout.c has it as
 *
 *     u8 gDuel_abHandCardRecordIndices[DUEL_SIDE_COUNT * HAND_SIZE + 2] = {
 *         0, 1, 2, 3, 4, 15, 16, 17, 18, 19, 0, 0 };
 *
 * and its own comment calls it the table that "maps a side's five hand slots
 * to their card-record entries", the same gDuel_aCardRecords indices the grid above
 * holds. The trailing two bytes pad to the next table.
 *
 * Left unsized for the same reason as the grid, and because all three readers
 * want the flat view: duel_draw_resolution.c reads
 * gDuel_abHandCardRecordIndices[i + gDuel_bActiveSide * HAND_SIZE], the hand-action
 * controller indexes its selected hand slot, and Duel_ApplyAiHandSelection
 * (duel_scene_card_move.c) takes the base pointer. */
extern u8 gDuel_abHandCardRecordIndices[];

/* The ground-plane position of every field slot, defined with initialisers in
 * duel_field_layout.c as a two-dimensional table. Six sources declared it
 * locally and only two of them had the shape right: the other four spelled
 * it a flat DuelFieldPosition[] and then cast it to u8 * anyway, which is
 * why the wrong shape never showed. Declared here in its real form, the
 * cast still reaches the same address and the flat spelling is not needed.
 * The cursor callback projects these inputs as (x, 0, z), rather than
 * treating them as output pixels. The generic y member holds Z here.
 * 160 bytes, far above the -G threshold, so completing the type is free. */
extern DuelFieldPosition
    gDuel_aFieldGridGroundPositions[DUEL_SIDE_COUNT][DUEL_FIELD_SIDE_GRID_SLOT_COUNT];

#endif
