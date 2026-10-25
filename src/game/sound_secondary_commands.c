#include "../types.h"
#include "sound_spatialize_object.h"
#include "../psyq/libapi.h"

#include "sound.h"
#include "sound_event_runtime.h"
#include "sound_spatialization.h"
#include "sound_sequence_channel_events.h"

#include "sound_secondary_commands.h"
#include "../unmatched.h"

#define SD_SECONDARY_STATE_BYTES(state) ((u8 *)(state))

void SD_HandleSequenceControlChange(s32 arg0, s32 arg1, u8 arg2)
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
    b = gSD_pSecondaryState;
    e = (SDSecondaryRecord *)b + id;
    sel = arg1 & 0xFF;

    switch (sel) {
    case SD_SEQUENCE_CONTROL_DATA_ENTRY:
        v = e->control_mode;
        e->control_value = arg2;
        if (v != SD_SEQUENCE_LOOP_START) {
            if (v != SD_SEQUENCE_LOOP_END) {
                SD_ApplySequenceReverbParameter(e, id);
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
        e->voice_usage =
            (e->voice_usage & 0xF) | ((arg2 & 0xF) << 4);
        break;
    case 0x5B:
        e->reverb_mode = arg2 & 0x7F;
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
        c = gSD_pSecondaryState;
        i = 0;
        if (c->object_count > 0) {
            id2 = arg0 & 0xFF;
            off = id2 * SD_SEQUENCE_CHANNEL_RECORD_SIZE;
            m = 0x180;
            do {
                if (id2 == c->objects[i].channel_index) {
                    if (c->objects[i].lifecycle_stage != 0) {
                        SD_SpatializeSecondaryObject(
                            (SDSecondaryObject *)(SD_SECONDARY_STATE_BYTES(c) + m),
                            (SDSecondaryRecord *)(SD_SECONDARY_STATE_BYTES(c) + off));
                        SD_SetVoiceVolume(i, gSD_pSecondaryState->objects[i].level_left,
                                          gSD_pSecondaryState->objects[i].level_right);
                    }
                }
                m += SD_SECONDARY_OBJECT_SIZE;
                c = gSD_pSecondaryState;
                i++;
            } while (i < c->object_count);
        }
    }
}

void SD_SetSequenceChannelProgram(unsigned char index, int value)
{
    register SDSecondaryRecord *entries =
        (SDSecondaryRecord *)gSD_pSecondaryState;

    entries += index;

    entries->program = value;
}

void SD_SetSequenceChannelPitchBendMsb(unsigned char index, int unused, int value)
{
    register SDSecondaryRecord *entries =
        (SDSecondaryRecord *)gSD_pSecondaryState;

    entries += index;

    entries->pitch_bend_msb = value & SD_SEQUENCE_PITCH_BEND_MSB_MASK;
}

PSXLONG SD_SequenceTimerCallback(void)
{
    SDSecondaryState *state = gSD_pSecondaryState;
    s32 i;

    if (state->sequence_updates_enabled == 0)
        return 1;
    if (state->flag_0500 != 0)
        return 1;
    if (state->vsync_update_mode != 0)
        return 1;
    if (state->flag_0501 == 0) {
        GetRCnt(RCntCNT2);
        gSD_pSecondaryState->flag_0501 = 1;
        for (i = 0; i < 8; i++) {
            void (*callback)(void);

            SD_ProcessSequenceTracks();
            gSD_pSecondaryState->timer_maintenance_counter++;
            state = gSD_pSecondaryState;
            if (state->timer_maintenance_counter >= 11) {
                state->timer_maintenance_counter = 0;
                SD_UpdateSecondaryObjectReuseCountdowns();
                SD_UpdateSecondaryObjectVoices();
                callback = gSD_pSecondaryState->timer_maintenance_callback;
                if (callback != 0)
                    callback();
            }
        }
        gSD_pSecondaryState->flag_0501 = 0;
    }
    return 0;
}
