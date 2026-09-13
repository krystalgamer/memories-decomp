#include "../types.h"
#include "text_stream_commands.h"
#include "duel_effect.h"
#include "../unmatched.h"
#include "dialog_choice.h"

void Text_SetStateFromStream(DuelEffectChannel *object)
{
    u8 **stream = &((TextStreamOwner *)object)->streams[object->stream_58];
    u8 *current = *stream;
    u8 value = current[0];

    *stream = current + 1;
    object->state_51 = value;
    D_8009B350 = 1;
}
