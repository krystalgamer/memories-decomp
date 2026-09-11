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

void func_80047314(u32 value)
{
    func_8004733C(value & SD_COMMAND_VALUE_MASK, g_SDValue->field_164B);
}

