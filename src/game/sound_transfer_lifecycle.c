#include "../types.h"
#include "func_8004A6D8.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_event_runtime.h"
#include "sound_init.h"
#include "sound_transfer_lifecycle.h"

void func_80049640(void)
{
    s32 value;

    func_8004A6D8();
    value = D_8009B458->field_081C;
    if (value > 0) {
        if (value < 4)
            func_8004B910();
    }
    func_80049434();
}

void SD_Term(void)
{
    func_8004A6D8();
    func_80049434();
    SpuQuit();
}

