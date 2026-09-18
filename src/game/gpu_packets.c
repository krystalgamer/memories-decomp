#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "gpu_packets.h"

#define GPU_PACKET_CODE_OFFSET(prefix_words) \
    ((u32)&((u32 *)0)[prefix_words] + (u32)&((P_CODE *)0)->code)
#define GPU_PACKET_TAG(packet) ((P_TAG *)(packet))

void func_8005B260(u32 *src, GsOT *ot, s32 idx, s32 flags)
{
    u32 *s;
    u32 draw;
    s32 len;
    s32 i;
    s32 index;
    u32 *dst;

    draw = 0xE1000200;
    s = src;
    index = idx;
    len = GPU_PACKET_TAG(s)->len;
    D_800FE240[0] = *s++;
    D_800FE240[1] = ((flags & 3) << 5) | draw;
    dst = D_800FE240;
    dst = dst + 2;
    for (i = len - 1; i != -1; i--) {
        *dst++ = *s++;
    }
    draw = (u8)(len + 1);
    i = draw;
    GPU_PACKET_TAG(D_800FE240)->len = i;
    if (flags >= 0) {
        ((u8 *)D_800FE240)[GPU_PACKET_CODE_OFFSET(2)] |= 2;
    }
    addPrim(&ot->org[index & 0xFFFF], D_800FE240);
    D_800FE240 = D_800FE240 + (len + 2);
}

void Graphics_SubmitTextureWindowPacket(
    u32 *src,
    GsOT *ot,
    s32 idx,
    s32 offx,
    s32 offy,
    s32 maskx,
    s32 masky
)
{
    u32 *from;
    s32 len;
    s32 i;
    u32 *dst;
    s32 index;
    u32 first;

    len = GPU_PACKET_TAG(src)->len;
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

void func_8005B4D8(u32 *src, GsOT *ot, s32 idx, s32 flags)
{
    u32 *s;
    u32 draw_mode;
    u32 mask_on;
    s32 len;
    s32 i;
    s32 index;
    u32 *dst;

    draw_mode = 0xE1000200;
    mask_on = 0xE6000001;
    s = src;
    index = idx;
    len = GPU_PACKET_TAG(s)->len;
    D_800FE240[0] = *s++;
    D_800FE240[1] = ((flags & 3) << 5) | draw_mode;
    D_800FE240[2] = mask_on;
    dst = D_800FE240 + 3;
    for (i = len - 1; i != -1; i--) {
        *dst++ = *s++;
    }
    *(D_800FE240 + len + 3) = 0xE6000000;
    setlen(D_800FE240, len + 3);
    if (flags >= 0) {
        ((u8 *)D_800FE240)[GPU_PACKET_CODE_OFFSET(3)] |= 2;
    }
    addPrim(&ot->org[index & 0xFFFF], D_800FE240);
    D_800FE240 = D_800FE240 + (len + 4);
}
