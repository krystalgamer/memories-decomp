#include "../types.h"
#include "func_80044DC0.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_sequence_state.h"
#include "sound_output_state.h"
#include "sound_transfer_lifecycle.h"
#include "sound_voice_selection.h"

#include "sound_init.h"
#include "sound_pending_entries.h"
#include "sound_output.h"

void func_800473CC(u32 value)
{
    func_800473F0(value & SD_COMMAND_VALUE_MASK, -32);
}

void func_800473F0(u16 flags, s32 value)
{
    if ((flags & 0x8000) != 0)
        func_80045114();
    else
        func_80049230_s16(-1, value);
}

void func_80047430(s32 value, s32 flag)
{
    func_80049108(value, flag);
}

void func_80047458(s32 value, s32 flag)
{
    func_800490F0(value, flag);
}
