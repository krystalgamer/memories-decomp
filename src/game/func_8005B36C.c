#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

extern u32 *D_800FE240 __attribute__((section(".data")));

void func_8005B36C(u32 *src, GsOT *ot, s32 idx, s32 offx, s32 offy,
                   s32 maskx, s32 masky)
{
    u32 *from;
    s32 len;
    s32 i;
    u32 *dst;
    s32 index;
    u32 first;

    len = ((P_TAG *)src)->len;
    index = idx;
    first = src[0];
    D_800FE240[0] = first;
    src++;
    D_800FE240[1] = 0xE2000000
                  | ((((-maskx) & 0xFF) / 8) & 0x1F)
                  | (((((-masky) & 0xFF) / 8) & 0x1F) << 5)
                  | ((((offx & 0xFF) / 8) & 0x1F) << 10)
                  | ((((offy & 0xFF) / 8) & 0x1F) << 15);
    dst = D_800FE240 + 2;
    from = src;
    for (i = len - 1; i != -1; i--) {
        *dst++ = *from++;
    }
    D_800FE240[len + 2] = 0xE2000000;
    setlen(D_800FE240, len + 2);
    addPrim(&ot->org[index & 0xFFFF], D_800FE240);
    D_800FE240 = D_800FE240 + (len + 3);
}
