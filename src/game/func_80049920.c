#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"

/* Second pass over the 0x28-byte records at D_8009B458: for each record
 * whose +0x18D counter is set and whose channel reports SPU_ON_ENV_OFF, either
 * bumps the counter or, once it reaches 2, requests key-off through the
 * SpuSetKey/SpuGetKeyStatus pair, clears the counter and ORs the channel mask
 * into one final SpuSetKey call. */
void func_80049920(void) {
    u8 *base;
    s32 i;
    s32 mask;
    s32 off;
    s32 *tbl;
    s32 *q;
    s32 v;
    s32 key_off_env_on;
    u8 b;
    u8 *p;
    s32 z;
    u8 *b1;
    u8 *b2;

    mask = 0;
    base = (u8 *)D_8009B458;
    i = 0;
    mask = i;
    z = 0;
    if (*(s16 *)(base + 0x510) > z) {
        key_off_env_on = SPU_OFF_ENV_ON;
        off = 0;
        tbl = D_80011434;
        do {
            if (*(base + off + 0x18D) != 0) {
                if (SpuGetKeyStatus(*tbl) == SPU_ON_ENV_OFF) {
                    b1 = (u8 *)D_8009B458;
                    p = b1 + off;
                    b = p[0x18D];
                    if (b >= 2) {
                        mask |= *tbl;
                        q = tbl;
                        do {
                            SpuSetKey(SPU_OFF, *q);
                            v = SpuGetKeyStatus(*q);
                        } while (v != key_off_env_on && v != SPU_OFF);
                        b2 = (u8 *)D_8009B458;
                        *(b2 + off + 0x18D) = 0;
                    } else {
                        p[0x18D] = b + 1;
                    }
                }
            }
            off += 0x28;
            base = (u8 *)D_8009B458;
            tbl++;
        } while (++i < *(s16 *)(base + 0x510));
    }
    if (mask != 0) {
        SpuSetKey(SPU_OFF, mask);
    }
}
