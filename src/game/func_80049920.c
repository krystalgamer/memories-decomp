#define D_8009B458_IN_DATA
#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"

#define SD_SECONDARY_STATE_AT_OFFSET(state, offset) \
    ((SDSecondaryState *)((u8 *)(state) + (offset)))

/* Second pass over the 0x28-byte records at gSD_pSecondaryState: for each record
 * whose +0x18D counter is set and whose channel reports SPU_ON_ENV_OFF, either
 * bumps the counter or, once it reaches 2, requests key-off through the
 * SpuSetKey/SpuGetKeyStatus pair, clears the counter and ORs the channel mask
 * into one final SpuSetKey call. */
void func_80049920(void) {
    SDSecondaryState *base;
    s32 i;
    s32 mask;
    s32 off;
    s32 *tbl;
    s32 *q;
    s32 v;
    s32 key_off_env_on;
    u8 b;
    SDSecondaryState *p;
    s32 z;
    SDSecondaryState *b1;
    SDSecondaryState *b2;

    mask = 0;
    base = gSD_pSecondaryState;
    i = 0;
    mask = i;
    z = 0;
    if (base->object_count > z) {
        key_off_env_on = SPU_OFF_ENV_ON;
        off = 0;
        tbl = D_80011434;
        do {
            if (SD_SECONDARY_STATE_AT_OFFSET(base, off)
                    ->objects[0].tent_release_stage != 0) {
                if (SpuGetKeyStatus(*tbl) == SPU_ON_ENV_OFF) {
                    b1 = gSD_pSecondaryState;
                    p = SD_SECONDARY_STATE_AT_OFFSET(b1, off);
                    b = p->objects[0].tent_release_stage;
                    if (b >= 2) {
                        mask |= *tbl;
                        q = tbl;
                        do {
                            SpuSetKey(SPU_OFF, *q);
                            v = SpuGetKeyStatus(*q);
                        } while (v != key_off_env_on && v != SPU_OFF);
                        b2 = gSD_pSecondaryState;
                        SD_SECONDARY_STATE_AT_OFFSET(b2, off)
                            ->objects[0].tent_release_stage = 0;
                    } else {
                        p->objects[0].tent_release_stage = b + 1;
                    }
                }
            }
            off += sizeof(SDSecondaryObject);
            base = gSD_pSecondaryState;
            tbl++;
        } while (++i < base->object_count);
    }
    if (mask != 0) {
        SpuSetKey(SPU_OFF, mask);
    }
}
