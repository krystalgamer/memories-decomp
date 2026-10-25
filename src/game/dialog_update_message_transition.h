#ifndef MEMORIES_DECOMP_DIALOG_UPDATE_MESSAGE_TRANSITION_H
#define MEMORIES_DECOMP_DIALOG_UPDATE_MESSAGE_TRANSITION_H

#include "../types.h"
#include "menu_record.h"

/* Message-only variant: create the sprite and channel, advance the slide
 * by 0x20 in the same call, then pump text after reaching the endpoint.
 * Completion clears the shared D_8009B3C1 byte, not the record or objects. */
void Dialog_UpdateMessageTransition(MenuRecord *record);

#endif
