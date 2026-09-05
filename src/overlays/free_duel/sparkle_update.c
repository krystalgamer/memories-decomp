#include "../../types.h"

typedef struct {
    u8 unk0[0x4];
    u32 flags;
    u8 unk8[0x4];
    u8 r;
    u8 g;
    u8 b;
    u8 unkF;
    u8 unk10[0x50];
    s16 timer;
    u8 unk62[0xA];
    u8 state;
} FreeDuelSparkle;

extern void *gFreeDuel_apSparklePool[];
extern void func_8004036C(void);

void FreeDuel_UpdateSparkle(void)
{
    FreeDuelSparkle *obj;
    s32 level;
    s16 timer;
    s32 i;

    for (i = 15; i >= 0; i--) {
        obj = (FreeDuelSparkle *)gFreeDuel_apSparklePool[i];
        if (obj != 0 && (obj->state & 0xF) == 1) {
            if (!(obj->state & 0x80)) {
                obj->state |= 0x80;
                obj->timer = 16;
                *(u32 *)&obj->r = 0x404040;
                obj->flags |= 0x50000000;
            }
            level = obj->r - 4;
            obj->b = level;
            obj->g = level;
            obj->r = level;
            timer = obj->timer - 1;
            obj->timer = timer;
            if (timer == 0) {
                func_8004036C();
                gFreeDuel_apSparklePool[i] = 0;
            }
        }
    }
}
