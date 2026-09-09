#ifndef MEMORIES_DECOMP_TEXT_CONTROL_COMMANDS_H
#define MEMORIES_DECOMP_TEXT_CONTROL_COMMANDS_H

#include "../types.h"

/* Two D_80090EAC entries. func_80038D14 puts the object into wait state 4 and
 * raises D_8009B350, which is how a command hands the frame back.
 *
 * func_80038D2C is the campaign-flag command, and like its scene-script
 * counterpart it is both a write and a branch: with
 * CAMPAIGN_FLAG_COMMAND_WRITE it records a card-used flag and returns,
 * otherwise it reads a second word as the target and, when
 * Campaign_TestStoryFlag agrees, rewrites the low half of the object's current
 * stream cursor -- a jump within the same 64K rather than a full pointer
 * store. */
void func_80038D14(u8 *object);
void func_80038D2C(u8 *object);

#endif
