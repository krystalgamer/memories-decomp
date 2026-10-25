#define FUNC_8004A7C0_CALL_WITH_UNUSED_ARG
#define D_8009B458_IN_DATA
#include "../types.h"
#include "../psyq/libspu.h"
#include "sd_key_off_secondary_object.h"
#include "sound_sequence_constants.h"
#include "sound.h"

#define SD_SECONDARY_OBJECT_OFFSET_VIEW(state, offset) \
    ((SDSecondaryObject *)((u8 *)(state) + (offset)))

/* Per-entry sweep over the SDSecondaryObject records at
 * gSD_pSecondaryState->objects: sends the under-0x10 ones through SD_UpdateSecondaryObjectPitch,
 * resets a record whose voice is SPU_OFF, advances the +0xD counter for
 * SPU_ON_ENV_OFF, requests key-off for SPU_ON/SPU_OFF_ENV_ON through the
 * SpuSetKey/SpuGetKeyStatus pair, and ORs the touched masks into one final
 * SpuSetKey call. */

void SD_UpdateSecondaryObjectVoices(void) {
    u8 voice_key_status[0x18];
    SDSecondaryState *p;
    SDSecondaryObject *e;
    u8 *q;
    SDSecondaryRecord *r;
    s32 *t;
    s32 *u;
    s32 i;
    s32 m;
    s32 o;
    s32 key_off_env_on;
    s32 a;
    s32 b;
    u32 c;
    s32 v;

    SpuGetAllKeysStatus((char *)voice_key_status);
    p = gSD_pSecondaryState;
    i = 0;
    m = i;

    if (p->object_count > 0) {
        key_off_env_on = SPU_OFF_ENV_ON;
        t = gSD_adwVoiceMasks;
        q = voice_key_status;
        o = SD_SECONDARY_OBJECT_ARRAY_OFFSET;
        do {
            e = SD_SECONDARY_OBJECT_OFFSET_VIEW(p, o);
            if (e->channel_index < SD_SEQUENCE_CHANNEL_COUNT) {
                SD_UpdateSecondaryObjectPitch(e, 0);
            }
            if (*q == SPU_OFF) {
                if (e->lifecycle_stage == 0) {
                    goto next;
                }
                r = &gSD_pSecondaryState->channels[e->channel_index];
                a = r->voice_usage;
                if ((a & 0xF) != 0) {
                    r->voice_usage = a - 1;
                }
                e->lifecycle_stage = 0;
                e->reuse_countdown = 0;
                e->channel_index = SD_SECONDARY_RECORD_NONE;
            }

            b = e->lifecycle_stage;
            c = b & 0xFF;
            if (c != 0 && *q == SPU_ON_ENV_OFF) {
                if (c >= 2) {
                    m = m | *t;
                    SD_KeyOffSecondaryObject(i, b);
                } else {
                    e->lifecycle_stage = b + 1;
                }
            }

next:
            if (e->key_held == 0 && (u32)(*q - SPU_ON) < 2) {
                u = t;
                while (1) {
                    SpuSetKey(SPU_OFF, *u);
                    v = SpuGetKeyStatus(*u);
                    if (v == key_off_env_on) {
                        break;
                    }
                    if (v == SPU_OFF) {
                        break;
                    }
                }
                m = m | *u;
            }

            t++;
            q++;
            p = gSD_pSecondaryState;
            o += sizeof(SDSecondaryObject);
            i++;
        } while (i < p->object_count);
    }

    if (m != 0) {
        SpuSetKey(SPU_OFF, m);
    }
}
