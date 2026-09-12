#ifndef MEMORIES_DECOMP_SCRIPT_FLAG_COMMANDS_H
#define MEMORIES_DECOMP_SCRIPT_FLAG_COMMANDS_H

#include "../types.h"

/* func_8002E918 is the campaign-flag command, and it is both a write and a
 * branch: with CAMPAIGN_FLAG_COMMAND_WRITE set it records a card-used flag and
 * consumes two stream bytes, and without it it consumes four -- the flag id and
 * a 16-bit offset -- and jumps D_8009B290 into D_801A8000 when
 * Campaign_TestStoryFlag agrees. The stream pointer is written twice on that
 * path, first past two bytes and then past four. The decoded command is an
 * unsigned halfword; the offset is assembled from the next two bytes.
 *
 * Script_OpViewportTween takes three 16-bit operands -- target x, target y and
 * duration -- and leaves D_8009B27C at 7, which is what hands the scene over to
 * Script_UpdateViewportTween for the following ticks. */
void func_8002E918(void);
void Script_OpViewportTween(void);
void Script_UpdateViewportTween(void);

#endif
