#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

extern u32 *D_800FE240 __attribute__((section(".data")));

/* Copies one GPU primitive into the packet buffer at D_800FE240 and links it
 * into an ordering table. The packet is laid out as the primitive's own tag,
 * then a 0xE1 draw-mode word carrying the semi-transparency rate from the low
 * two bits of `flags`, then the primitive's words, so the stored length is one
 * more than the source primitive's. A non-negative `flags` also sets the
 * semi-transparent bit in the copied primitive's code byte. The buffer pointer
 * is advanced past the packet. */
void func_8005B260(u32 *src, GsOT *ot, s32 idx, s32 flags)
{
    /* $4 and $5 pinned: the sequence is exact without them, but retail leaves
       the source pointer in its incoming $a0 and puts the 0xE1000200 constant
       in $a1, where the allocator otherwise spends $a0 on the constant and
       copies the pointer out. */
    register u32 *s __asm__("$4");
    register u32 draw_mode __asm__("$5");
    GsOT *table;
    s32 len;
    s32 i;
    u32 *dst;

    table = ot;
    s = src;
    draw_mode = 0xE1000200;
    len = ((P_TAG *)s)->len;
    D_800FE240[0] = *s++;
    D_800FE240[1] = ((flags & 3) << 5) | draw_mode;
    dst = D_800FE240 + 2;
    for (i = len - 1; i != -1; i--) {
        *dst++ = *s++;
    }
    setlen(D_800FE240, len + 1);
    if (flags >= 0) {
        ((u8 *)D_800FE240)[0xB] |= 2;
    }
    addPrim(&table->org[idx & 0xFFFF], D_800FE240);
    D_800FE240 = D_800FE240 + (len + 2);
}
