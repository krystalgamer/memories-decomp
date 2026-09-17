#define FUNC_8004A7C0_CALL_WITH_UNUSED_ARG
#define D_8009B458_IN_DATA
#include "../types.h"
#include "../psyq/libspu.h"
#include "func_8004A7C0.h"
#include "sound_sequence_constants.h"
#include "sound.h"

/* Per-entry sweep over the SDSecondaryObject records at
 * D_8009B458->objects: sends the under-0x10 ones through func_8004A43C,
 * resets a record whose voice is SPU_OFF, advances the +0xD counter for
 * SPU_ON_ENV_OFF, requests key-off for SPU_ON/SPU_OFF_ENV_ON through the
 * SpuSetKey/SpuGetKeyStatus pair, and ORs the touched masks into one final
 * SpuSetKey call. */

void func_8004AAFC(void) {
    u8 sp10[0x18];
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

    SpuGetAllKeysStatus((char *)sp10);
    p = D_8009B458;
    i = 0;
    m = i;

    if (p->object_count > 0) {
        key_off_env_on = SPU_OFF_ENV_ON;
        t = D_80011434;
        q = sp10;
        o = SD_SECONDARY_OBJECT_ARRAY_OFFSET;
        do {
            e = (SDSecondaryObject *)((u8 *)p + o);
            if (e->channel_index < SD_SEQUENCE_CHANNEL_COUNT) {
                func_8004A43C(e, 0);
            }
            if (*q == SPU_OFF) {
                if (e->field_000D == 0) {
                    goto next;
                }
                r = &D_8009B458->channels[e->channel_index];
                a = r->field_0006;
                if ((a & 0xF) != 0) {
                    r->field_0006 = a - 1;
                }
                e->field_000D = 0;
                e->field_001E = 0;
                e->channel_index = SD_SECONDARY_RECORD_NONE;
            }

            b = e->field_000D;
            c = b & 0xFF;
            if (c != 0 && *q == SPU_ON_ENV_OFF) {
                if (c >= 2) {
                    m = m | *t;
                    func_8004A7C0(i, b);
                } else {
                    e->field_000D = b + 1;
                }
            }

next:
            if (e->field_000F == 0 && (u32)(*q - SPU_ON) < 2) {
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
            p = D_8009B458;
            o += sizeof(SDSecondaryObject);
            i++;
        } while (i < p->object_count);
    }

    if (m != 0) {
        SpuSetKey(SPU_OFF, m);
    }
}
