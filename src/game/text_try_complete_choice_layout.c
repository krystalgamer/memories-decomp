#include "../types.h"
#include "../unmatched.h"
#include "dialog_choice.h"
#include "text_stream_commands.h"

void Text_TryCompleteChoiceLayout(volatile DuelEffectChannel *object)
{
    u32 flags;
    u8 control;

    if (object->field_56 >= gDialog_bChoiceCount) {
        object->state_51 = 1;
        flags = object->flags_34;
        object->field_56 = 0;
        control = D_8009B34C;
        D_8009B340 = 0;
        flags &= 0xEFFF;
        control &= 0x30;
        ((DuelEffectChannel *)object)->flags_34 = flags;
        if (control != 0)
            gDialog_bChoiceCount = 2;
    }
}
