#ifndef MEMORIES_DECOMP_DIALOG_CHOICE_STATE_H
#define MEMORIES_DECOMP_DIALOG_CHOICE_STATE_H

#include "../types.h"
#include "duel_effect.h"

/* Dialog_OpenChoice builds the choice/page cursor for a dialog record and hands
 * it back -- the caller stores it at the record's +0x30 and releases it through
 * DisplayObject_ReleaseIfPresent when the state ends. The frame pump handles
 * choice opening/closing; Text_WaitForPageAdvance owns the page-wait path.
 *
 * Four files declared Dialog_OpenChoice under three spellings between them,
 * `u8 *(u8 *)`, `u8 *(void *)` and `void *(void *)`. Every caller passes the
 * same record pointer and stores the result at the same channel slot, so the
 * differences were views of one signature.
 *
 * The parameter is now the record's own type. The five fields this function
 * reads -- rect_x, rect_width, rect_y, rect_height and depth_offset -- are all
 * declared in DuelEffectChannel at those offsets and at these widths, and
 * every caller, duel_effect_dialog_state.c included, holds a DuelEffectChannel *.
 *
 * The returned display object is stored in the channel's prompt_object slot and
 * released through the display-object lifecycle helpers. */
struct DisplayObject *Dialog_OpenChoice(DuelEffectChannel *record);

/* Choice-input polling and entry repainting. The constructor only builds the
 * cursor; the independent page-wait callback handles page input.
 * text_box_state_callbacks.c installs this in two adjacent state slots. */
void Dialog_UpdateChoice(DuelEffectChannel *object);

#endif
