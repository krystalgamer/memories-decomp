#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "gpu_packets.h"

#define GPU_PACKET_CODE_OFFSET(prefix_words) \
    ((u32)&((u32 *)0)[prefix_words] + (u32)&((P_CODE *)0)->code)
#define GPU_PACKET_TAG(packet) ((P_TAG *)(packet))
#define GPU_PACKET_BYTES(packet) ((u8 *)(packet))

void Graphics_SubmitDrawModePacket(u32 *src, GsOT *ot, s32 idx, s32 flags)
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
    gGraphics_pPacketWorkCursor[0] = *s++;
    gGraphics_pPacketWorkCursor[1] = ((flags & 3) << 5) | draw;
    dst = gGraphics_pPacketWorkCursor;
    dst = dst + 2;
    for (i = len - 1; i != -1; i--) {
        *dst++ = *s++;
    }
    draw = (u8)(len + 1);
    i = draw;
    GPU_PACKET_TAG(gGraphics_pPacketWorkCursor)->len = i;
    if (flags >= 0) {
        GPU_PACKET_BYTES(gGraphics_pPacketWorkCursor)[GPU_PACKET_CODE_OFFSET(2)] |= 2;
    }
    addPrim(&ot->org[index & 0xFFFF], gGraphics_pPacketWorkCursor);
    gGraphics_pPacketWorkCursor = gGraphics_pPacketWorkCursor + (len + 2);
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
    gGraphics_pPacketWorkCursor[0] = first;
    src++;
    gGraphics_pPacketWorkCursor[1] = 0xE2000000
                  | ((((-maskx) & 0xFF) / 8) & 0x1F)
                  | (((((-masky) & 0xFF) / 8) & 0x1F) << 5)
                  | ((((offx & 0xFF) / 8) & 0x1F) << 10)
                  | ((((offy & 0xFF) / 8) & 0x1F) << 15);
    dst = gGraphics_pPacketWorkCursor + 2;
    from = src;
    for (i = len - 1; i != -1; i--) {
        *dst++ = *from++;
    }
    gGraphics_pPacketWorkCursor[len + 2] = 0xE2000000;
    setlen(gGraphics_pPacketWorkCursor, len + 2);
    addPrim(&ot->org[index & 0xFFFF], gGraphics_pPacketWorkCursor);
    gGraphics_pPacketWorkCursor = gGraphics_pPacketWorkCursor + (len + 3);
}

void Graphics_SubmitMaskWritePacket(u32 *src, GsOT *ot, s32 idx, s32 flags)
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
    gGraphics_pPacketWorkCursor[0] = *s++;
    gGraphics_pPacketWorkCursor[1] = ((flags & 3) << 5) | draw_mode;
    gGraphics_pPacketWorkCursor[2] = mask_on;
    dst = gGraphics_pPacketWorkCursor + 3;
    for (i = len - 1; i != -1; i--) {
        *dst++ = *s++;
    }
    *(gGraphics_pPacketWorkCursor + len + 3) = 0xE6000000;
    setlen(gGraphics_pPacketWorkCursor, len + 3);
    if (flags >= 0) {
        GPU_PACKET_BYTES(gGraphics_pPacketWorkCursor)[GPU_PACKET_CODE_OFFSET(3)] |= 2;
    }
    addPrim(&ot->org[index & 0xFFFF], gGraphics_pPacketWorkCursor);
    gGraphics_pPacketWorkCursor = gGraphics_pPacketWorkCursor + (len + 4);
}
