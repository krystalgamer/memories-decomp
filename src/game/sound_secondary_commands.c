#include "../types.h"
#include "sound_spatialize_object.h"
#include "../psyq/libapi.h"

#include "sound.h"
#include "sound_event_runtime.h"
#include "sound_spatialization.h"
#include "func_8004ACE4.h"

#include "sound_secondary_commands.h"
#include "../unmatched.h"

void func_8004B49C(s32 arg0, s32 arg1, u8 arg2)
{
    SDSecondaryState *b;
    SDSecondaryState *c;
    SDSecondaryRecord *e;
    u8 ok;
    s32 id;
    s32 id2;
    s32 sel;
    s32 v;
    s32 i;
    s32 m;
    s32 off;

    ok = 0;
    id = arg0 & 0xFF;
    b = D_8009B458;
    e = (SDSecondaryRecord *)b + id;
    sel = arg1 & 0xFF;

    switch (sel) {
    case SD_SEQUENCE_CONTROL_DATA_ENTRY:
        v = e->control_mode;
        e->control_value = arg2;
        if (v != SD_SEQUENCE_LOOP_START) {
            if (v != SD_SEQUENCE_LOOP_END) {
                func_8004ACE4(e, id);
            }
        }
        break;
    case 7:
        e->volume = arg2;
        ok = 1;
        break;
    case 0xA:
        if (arg2 != 0) {
            e->pan = arg2;
        } else {
            e->pan = 1;
        }
        ok = 1;
        break;
    case 0xB:
        e->expression = arg2;
        ok = 1;
        break;
    case 0x1E:
        b->field_0512 = arg2;
        SD_UpdateSecondaryObjectVolumes();
        break;
    case 0x30:
        e->field_0006 =
            (e->field_0006 & 0xF) | ((arg2 & 0xF) << 4);
        break;
    case 0x5B:
        e->field_0010 = arg2 & 0x7F;
        break;
    case SD_SEQUENCE_CONTROL_PARAMETER_SELECTOR:
        e->parameter_selector = arg2;
        break;
    case SD_SEQUENCE_CONTROL_MODE:
        e->control_mode = arg2;
        break;

    case 0x7F:
        break;
    }

    if (ok != 0) {
        c = D_8009B458;
        i = 0;
        if (c->object_count > 0) {
            id2 = arg0 & 0xFF;
            off = id2 * SD_SEQUENCE_CHANNEL_RECORD_SIZE;
            m = 0x180;
            do {
                if (id2 == c->objects[i].channel_index) {
                    if (c->objects[i].field_000D != 0) {
                        SD_SpatializeSecondaryObject(
                            (SDSecondaryObject *)((u8 *)c + m),
                            (SDSecondaryRecord *)((u8 *)c + off));
                        SD_SetVoiceVolume(i, D_8009B458->objects[i].level_left,
                                          D_8009B458->objects[i].level_right);
                    }
                }
                m += SD_SECONDARY_OBJECT_SIZE;
                c = D_8009B458;
                i++;
            } while (i < c->object_count);
        }
    }
}

void func_8004B6E8(unsigned char index, int value)
{
    register SDSecondaryRecord *entries =
        (SDSecondaryRecord *)D_8009B458;

    entries += index;

    entries->program = value;
}

void func_8004B70C(unsigned char index, int unused, int value)
{
    register SDSecondaryRecord *entries =
        (SDSecondaryRecord *)D_8009B458;

    entries += index;

    entries->pitch_bend_msb = value & SD_SEQUENCE_PITCH_BEND_MSB_MASK;
}

