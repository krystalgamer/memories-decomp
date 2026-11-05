#include "../types.h"
#include "text_stream_commands.h"
#include "duel_effect.h"
#include "../unmatched.h"
#include "dialog_choice.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
volatile s32 gText_CommandDisposition;

void Text_SetStateFromStream(DuelEffectChannel *object)
{
    u8 **stream = &((TextStreamOwner *)object)->streams[object->stream_index];
    u8 *current = *stream;
    u8 value = current[0];

    *stream = current + 1;
    object->playback_state = value;
    gText_CommandDisposition = 1;
}
