#ifndef MEMORIES_DECOMP_TEXT_CONTROL_COMMANDS_H
#define MEMORIES_DECOMP_TEXT_CONTROL_COMMANDS_H

#include "../types.h"
#include "duel_effect.h"

/* D_80090EAC entry, and the dispatcher for the whole table: it reads one byte
 * from the object's current stream, advances the cursor past it, and calls
 * D_80090EAC[byte] with the same object -- so an entry can chain into another,
 * and the table's bound is a property of the script data rather than of any
 * call site. */
void Text_DispatchSecondaryCommand(u8 *object);

/* D_80090EAC entry: sets up a dialog choice. It reads a control byte, and a
 * second one when bit 3 is set, then either advances the object's stream cursor
 * by the already-made choice and re-runs Text_SetCursorOffset (bit 7), or
 * publishes a new choice -- count from the low three bits, style from the high
 * nibble, enabled mask from the second byte -- and puts the dialog input state
 * at 1 when that byte's bit 7 is set. Always raises D_8009B350. */
void Text_HandleChoiceCommand(u8 *object);

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

/* D_80090EAC entry: pushes a stream cursor. It takes a word through
 * func_80036D3C, writes it as the low half of the NEXT cursor slot -- keeping
 * that slot's high half -- and then increments the object's +0x58 cursor index,
 * so the following commands read from the new stream. */
void Text_PushStreamOffset(DuelEffectChannel *object);

void Text_NewLine(u8 *object);
void Text_EndStream(u8 *object);

/* D_80090EAC entry: the effect-script command. It reads a command id and a flag
 * byte from the object's current stream, finds the display effect record for
 * the id in D_800EB010 -- ids below 0x41 live in the first two records, others
 * in the third -- and depending on the flags stops it (bit 7), adjusts a
 * running one (bits 5 and 6), or starts it in the slot bit 0 names, putting the
 * object into the matching wait state.
 *
 * The parameter is this unit's own view of the object, moved here from the
 * source because a prototype needs its type. The table's entry type is
 * `void (*)(u8 *)`, so the entry casts. */
typedef struct {
    u8 *streams[20];
    u8 unk50;
    u8 state;
    u8 pad52[6];
    s8 depth;
} EffectObject;

void Text_HandleDisplayEffectCommand(EffectObject *object);

#endif
