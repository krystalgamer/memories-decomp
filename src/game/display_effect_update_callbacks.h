#ifndef MEMORIES_DECOMP_DISPLAY_EFFECT_UPDATE_CALLBACKS_H
#define MEMORIES_DECOMP_DISPLAY_EFFECT_UPDATE_CALLBACKS_H
#include "../types.h"
#include "menu_record.h"

/* Step 4 rebuilds/crossfades old and new resource rows, with complementary
 * color and position updates; completion retains only the new primary row.
 * Step 5 is the separate portrait transition using only the first two
 * primary-row slots and its original keep-versus-release endpoints.
 * Both retain the MenuRecord callback ABI and all existing row views. */
void DisplayEffect_CrossfadeResources(MenuRecord *record);
void DisplayEffect_UpdatePortraitTransition(MenuRecord *record);

/* Incoming effect step 7: initialize the blend row and advance a stored
 * halfword ramp from zero by gGraphics_nBaseFrameStep * 8. Completion keeps the primary
 * row, restores its existing row attributes, releases grid[1] and clears
 * the activity-inhibit bit.
 *
 * Before canonical names, the table privately declared address 0x8003AAE4
 * as `void DisplayEffect_FadeInRecord(u8 *);` while the definition used MenuRecord *.
 * This owned header retains the definition's actual callback ABI. */
void DisplayEffect_FadeInRecord(MenuRecord *record);

/* Outgoing effect step 6: initialize a secondary blend row and ramp 0x80,
 * then subtract gGraphics_nBaseFrameStep * 8 in the same call. A nonpositive signed result
 * clears the step, releases grid[1] and then releases the record.
 *
 * Keep the MenuRecord callback type and explicit narrow initialization view.
 * Primary objects do not survive this endpoint as they do for fade-in. */
void DisplayEffect_FadeOutRecord(MenuRecord *record);

/* gDisplayEffect_apfnSteps display effect step: the easing sweep. It runs the record's six
 * tail halfwords -- 0x40 and 0x42 the destination, 0x44 and 0x46 the distance
 * still to travel, 0x48 the eased angle and 0x4A its per-tick step -- and
 * hands the result to the position helpers as x and y.
 *
 * Every offset belongs to MenuRecord, the callback table's argument type. */
void DisplayEffect_UpdatePositionTween(MenuRecord *record);

/* The record pair display_effect_update_callbacks.c moves as a unit: a group
 * holding up to three child display objects, and the child view it writes into.
 *
 * Both are narrow views. Only the coordinate pair each one carries is named,
 * with the rest as padding to reach it, so neither describes the whole
 * display-object record. */

/* A positioned child. The coordinates sit at 0x30 and 0x32. */
typedef struct {
    u8 pad_00[0x30];
    s16 x;
    s16 y;
} DisplayPositionChild;

/* The group. Its own coordinates are at 0x34 and 0x36, and the three child
 * pointers occupy the first twelve bytes. DisplayPositionGroup_SetChildPositions walks them from index
 * 2 down to 0 and skips null slots, so fewer than three children is normal
 * rather than an error. */
typedef struct {
    DisplayPositionChild *children[3];
    u8 pad_0C[0x28];
    s16 x;
    s16 y;
} DisplayPositionGroup;

/* Writes one coordinate pair into every non-null child, leaving the group's
 * own pair alone. */
void DisplayPositionGroup_SetChildPositions(DisplayPositionGroup *group, s16 x, s16 y);

/* Sets the group's own pair and then pushes it to the children. */
void DisplayPositionGroup_SetPosition(
    DisplayPositionGroup *group,
    s32 x,
    s32 y
);

/* The callbacks sharing the implementation file retain their individually
 * measured argument views where old GCC's allocation depends on them. */

#endif
