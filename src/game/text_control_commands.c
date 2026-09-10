#include "../types.h"
#include "func_80036D3C.h"
#include "campaign_flags.h"
#include "duel_effect.h"
#include "text_control_commands.h"

void Text_StartPageWait(DuelEffectChannel *value)
{
    value->state_51 = 4;
    D_8009B350 = 1;
}

void Text_HandleCampaignFlagCommand(DuelEffectChannel *object)
{
    s32 flag = func_80036D3C(object);

    flag &= CAMPAIGN_FLAG_COMMAND_WORD_MASK;
    if (flag & CAMPAIGN_FLAG_COMMAND_WRITE) {
        Library_UpdateCardUsedFlag(flag & CAMPAIGN_FLAG_COMMAND_PAYLOAD_MASK);
        return;
    }

    {
        s32 target = func_80036D3C(object);

        target &= 0xFFFF;
        if (Campaign_TestStoryFlag(flag) != 0) {
            s32 *cursor =
                (s32 *)((u8 *)object + object->stream_58 * 4);

            *cursor = (*cursor & 0xFFFF0000) | target;
        }
    }
}
