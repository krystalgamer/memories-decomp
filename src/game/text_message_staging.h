#ifndef GAME_TEXT_MESSAGE_STAGING_H
#define GAME_TEXT_MESSAGE_STAGING_H

#include "../types.h"
#include "text_staging.h"

extern TextStagingValues gText_StagedValues[];

/* The result controller stages its sign-extended dropped-card id at this
 * separate word, not inside the 0x80-byte view above. Only element zero
 * is measured. The incomplete s32 array retains the retail split absolute
 * store; a scalar .data declaration instead selects an $at store.
 * The image backs at least four bytes at 0x801D56A8, before the colour
 * slots at 0x801D5708. No total allocation size is inferred from that gap. */
extern s32 gDuel_DroppedCardTextValue[];

#endif
