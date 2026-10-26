#ifndef MEMORIES_DECOMP_DUEL_FIELD_DISPLAY_OBJECTS_H
#define MEMORIES_DECOMP_DUEL_FIELD_DISPLAY_OBJECTS_H

#include "../types.h"
#include "display_object.h"
#include "display_parent_links.h"
#include "duel_grid.h"
#include "duel_selection_layout.h"

/* Compares the guardian-star matchup between two selection records and returns
 * display code 4 for neutral, 1 when the first loses, or 6 when it wins.
 * The second record keeps its independently matched interior-table alias. */
s32 DuelField_GetGuardianStarDisplayCode(
    DuelSelectionRecord *cursor_a, DuelSelectionRecord *cursor_b);

/* Rebuilds the card information box for the record's grid cell. */
void DuelField_RefreshCursorCardInfo(DuelSelectionRecord *source);

/* Stages the selected live card and creates its information text box. */
void DuelField_CreateCardInfoTextBox(
    DuelSelectionRecord *source, s32 card_record_index);

/* Creates the field cursor at record +0x04, selecting its resource from
 * record_index and the active side, positioning it on the grid and installing
 * ground projection. Target-selection entry and transition completion call
 * it. The sliding information panel is the separate pointer at +0x00. */
void Duel_CreateFieldCursorObject(DuelSelectionRecord *source);

/* Advances the active duel display transition: request entry, linked panel
 * objects, cursor/field objects and view tween. Clears the request once the
 * pending work is finished. The coordinator ignores its nonzero-busy return
 * and retains its existing request/effect/scene gates. */
s32 Duel_UpdateDisplayTransition(void);

#endif
