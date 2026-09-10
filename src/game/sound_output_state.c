#include "../types.h"
#include "../psyq/libspu.h"

#include "sound.h"
#include "sound_output_state.h"

void func_8004503C(s16 value, u8 flag, s32 unused)
{
    g_SDValue->field_0512 = value;
    g_SDValue->field_0049 = flag;
}

#include "sound_buffer_init.h"

