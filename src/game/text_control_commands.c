#include "../types.h"
#include "func_80036D3C.h"
#include "campaign_flags.h"
#include "duel_effect.h"

void func_80038D14(u8 *value)
{
    value[0x51] = 4;
    D_8009B350 = 1;
}

void func_80038D2C(u8 *object)
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
            s32 *cursor = (s32 *)(object + *(s8 *)(object + 0x58) * 4);

            *cursor = (*cursor & 0xFFFF0000) | target;
        }
    }
}
