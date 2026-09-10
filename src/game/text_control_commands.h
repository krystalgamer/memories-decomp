#ifndef MEMORIES_DECOMP_TEXT_CONTROL_COMMANDS_H
#define MEMORIES_DECOMP_TEXT_CONTROL_COMMANDS_H

#include "../types.h"
#include "duel_effect.h"

/* Two D_80090EAC entries. Text_StartPageWait puts the object into wait state
 * 4 and raises D_8009B350, which is how a command hands the frame back.
 *
 * Text_HandleCampaignFlagCommand is the campaign-flag command, and like its
 * scene-script
 * counterpart it is both a write and a branch: with
 * CAMPAIGN_FLAG_COMMAND_WRITE it records a card-used flag and returns,
 * otherwise it reads a second word as the target and, when
 * Campaign_TestStoryFlag agrees, rewrites the low half of the object's current
 * stream cursor -- a jump within the same 64K rather than a full pointer
 * store. */
void Text_StartPageWait(DuelEffectChannel *object);
void Text_HandleCampaignFlagCommand(DuelEffectChannel *object);

#endif
