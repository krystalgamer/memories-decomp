#ifndef MEMORIES_DECOMP_TEXT_CONTROL_COMMANDS_H
#define MEMORIES_DECOMP_TEXT_CONTROL_COMMANDS_H

#include "../types.h"
#include "../ygo_types.h"
#include "duel_effect.h"

/* gText_SecondaryCommandHandlers entry, and the dispatcher for the whole table: it reads one byte
 * from the object's current stream, advances the cursor past it, and calls
 * gText_SecondaryCommandHandlers[byte] with the same object -- so an entry can chain into another,
 * and the table's bound is a property of the script data rather than of any
 * call site. */
void Text_DispatchSecondaryCommand(DuelEffectChannel *object);

#define TEXT_CHOICE_COMMAND_COUNT_MASK 7
#define TEXT_CHOICE_COMMAND_HAS_OPTIONS 0x08
#define TEXT_CHOICE_COMMAND_BRANCH 0x80
#define TEXT_CHOICE_OPTIONS_ENABLED_MASK 0x0F
#define TEXT_CHOICE_OPTIONS_INPUT_FLAG 0x80

/* Primary FB handler in the NA table. Consumes the optional options byte
 * before either setup or selected-branch handling. Setup without options uses
 * enabled mask 0xF and input state zero. Setup publishes the unchecked
 * three-bit count, high control nibble and low enabled nibble; options bit 7
 * sets input state to 1. Branching skips choice * 2 bytes and calls
 * Text_SetCursorOffset. Both forms raise gText_CommandDisposition. */
void Text_HandleChoiceCommand(DuelEffectChannel *object);

/* Two gText_SecondaryCommandHandlers entries. Text_StartPageWait puts the object into wait state
 * 4 and raises gText_CommandDisposition, which is how a command hands the frame back.
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

/* gText_SecondaryCommandHandlers entry: pushes a stream cursor. It takes a word through
 * TextStream_ReadU16LE, writes it as the low half of the NEXT cursor slot -- keeping
 * that slot's high half -- and then increments the object's +0x58 cursor index,
 * so the following commands read from the new stream. */
void Text_PushStreamOffset(DuelEffectChannel *object);

void Text_NewLine(DuelEffectChannel *object);
void Text_EndStream(DuelEffectChannel *object);

/* gText_SecondaryCommandHandlers entry: the effect-script command. It reads a command id and a flag
 * byte from the object's current stream, finds the display effect record for
 * the id in D_800EB010 -- ids below 0x41 live in the first two records, others
 * in the third -- and depending on the flags stops it (bit 7), adjusts a
 * running one (bits 5 and 6), or starts it in the slot bit 0 names, putting the
 * object into the matching wait state.
 *
 * The parameter uses the narrow shared EffectObject view in ygo_types.h. The
 * table uses the wider DuelEffectChannel view, so this entry retains an
 * explicit cast. */

void Text_HandleDisplayEffectCommand(EffectObject *object);

#endif
