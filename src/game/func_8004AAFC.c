#define FUNC_8004A7C0_CALL_WITH_UNUSED_ARG
#define D_8009B458_IN_DATA
#include "../types.h"
#include "../psyq/libspu.h"
#include "func_8004A7C0.h"
#include "sound_sequence_constants.h"
#include "sound.h"

/* Per-entry sweep over the 0x28-byte records at D_8009B458 + 0x180: sends the
 * under-0x10 ones through func_8004A43C, resets a record whose voice is SPU_OFF,
 * advances the +0xD counter for SPU_ON_ENV_OFF, requests key-off for
 * SPU_ON/SPU_OFF_ENV_ON through the SpuSetKey/SpuGetKeyStatus pair, and ORs the
 * touched masks into one final SpuSetKey call. */

void func_8004AAFC(void) {
    u8 sp10[0x18];
    u8 *p;
    u8 *e;
    u8 *q;
    u8 *r;
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
    p = (u8 *)D_8009B458;
    i = 0;
    m = i;

    if (*(s16 *)(p + 0x510) > 0) {
        key_off_env_on = SPU_OFF_ENV_ON;
        t = D_80011434;
        q = sp10;
        o = 0x180;
        do {
            e = p + o;
            if (e[3] < SD_SEQUENCE_CHANNEL_COUNT) {
                func_8004A43C((SDSecondaryObject *)e, 0);
            }
            if (*q == SPU_OFF) {
                if (e[0xD] == 0) {
                    goto next;
                }
                r = (u8 *)D_8009B458 + e[3] * SD_SEQUENCE_CHANNEL_RECORD_SIZE;
                a = r[6];
                if ((a & 0xF) != 0) {
                    r[6] = a - 1;
                }
                e[0xD] = 0;
                *(s16 *)(e + 0x1E) = 0;
                e[3] = SD_SECONDARY_RECORD_NONE;
            }

            b = e[0xD];
            c = b & 0xFF;
            if (c != 0 && *q == SPU_ON_ENV_OFF) {
                if (c >= 2) {
                    m = m | *t;
                    func_8004A7C0(i, b);
                } else {
                    e[0xD] = b + 1;
                }
            }

next:
            if (e[0xF] == 0 && (u32)(*q - SPU_ON) < 2) {
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
            p = (u8 *)D_8009B458;
            o += 0x28;
            i++;
        } while (i < *(s16 *)(p + 0x510));
    }

    if (m != 0) {
        SpuSetKey(SPU_OFF, m);
    }
}
