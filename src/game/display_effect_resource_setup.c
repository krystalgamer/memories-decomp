#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_effect_lifecycle.h"
#include "color_constants.h"
#include "display_object.h"
#include "display_object_api.h"
#include "display_object_layout.h"
#include "display_object_helpers.h"
#include "file_transfer.h"
#include "util_memory.h"
#include "func_8003A01C.h"
#include "func_8003A198.h"
#include "func_8003A1EC.h"
#include "func_8003A440.h"
#include "func_8003A560.h"
#include "../unmatched.h"

extern s8 D_8009B326;
#define HIGH_MEMORY_ADDRESSES_BASE_IN_DATA
#include "high_memory_addresses.h"

int func_8003A198(unsigned char*b,int x,int y,int z){u16 *p;p=(u16*)(b+x*2);if(*p){p=(u16*)(b+*p+y*2);if(*p){p=(u16*)(b+*p+z*2);if(*p)return 1;}}return 0;}

s32 func_8003A1EC(MenuRecord *a, u8 **out, s32 c) {
    DisplayObject *p;
    u8 *tb;
    s32 f;
    s32 g;
    s32 h;
    s32 m;

    if (a->field_3C != 0) {
        f = -0xA;
        tb = D_801AF800;
        g = 0x1A;
        h = 0x202;
    } else {
        f = -0xE;
        tb = D_801AF000;
        g = 0x1D;
        h = 0x200;
    }

    if (func_8003A198(tb, c, 0, 0) == 0) {
        return 0;
    }

    {
        m = 0x41000000;
        p = func_800400AC(func_8004002C(), 2);
        func_800428A8(p, *(s16 *)&a->field_34, *(s16 *)&a->field_36, c, 0, 0, g, h,
                      tb);
        func_80042918(p);
        func_800428EC((u8 *)p, f);
        p->attribute = p->attribute | m;
        p->flags = p->flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        out[0] = (u8 *)p;

        if (func_8003A198(tb, c, 1, 0) != 0) {
            p = func_800400AC(func_8004002C(), 2);
            func_800428A8(p, *(s16 *)&a->field_34, *(s16 *)&a->field_36, c, 1, 0, g,
                          h, tb);
            func_80042918(p);
            func_800428EC((u8 *)p, f | 1);
            p->attribute = p->attribute | m;
            p->flags = p->flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        } else {
            p = (DisplayObject *)0;
        }
        out[1] = (u8 *)p;

        if (func_8003A198(tb, c, 2, 0) != 0) {
            p = func_800400AC(func_8004002C(), 2);
            func_800428A8(p, *(s16 *)&a->field_34, *(s16 *)&a->field_36, c, 2, 0, g,
                          h, tb);
            func_80042918(p);
            func_800428EC((u8 *)p, f | 1);
            p->attribute = p->attribute | 0x41000000;
            p->flags = p->flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        } else {
            p = (DisplayObject *)0;
        }
        out[2] = (u8 *)p;
    }

    return 1;
}

void func_8003A440(u8 **arg0, u32 arg1, s32 arg2)
{
    u8 *e;
    s8 c;
    u32 *w;
    s32 i;

    if (arg1 == 0) {
        for (i = 2; i >= 0; i--) {
            c = (s8)arg2;
            e = arg0[i];
            if (e != 0) {
                *(u32 *)(e + 4) = *(u32 *)(e + 4) & ~(GsALON | GsATWO | GsAONE);
                *(u32 *)(e + 4) = *(u32 *)(e + 4) | GsALON;
                func_800428EC(e, c);
                *(u32 *)(e + 0xC) = COLOR_RGB24_NEUTRAL_GREY;
                *(u16 *)(e + 0x42) -= 1;
            }
        }
    } else {
        for (i = 2; i >= 0; i--) {
            e = arg0[i];
            if (e != 0) {
                w = (u32 *)(e + 4);
                *w = (*(u32 *)(e + 4) & ~(GsALON | GsATWO | GsAONE)) | arg1;
                func_800428EC(e, (s8)arg2);
                if (arg1 == (GsALON | GsATWO)) {
                    *(u16 *)(e + 0x42) = 0xFD;
                } else {
                    *(u16 *)(e + 0x42) += 1;
                }
            }
        }
    }
}

/* The scalars carry section(".data") so -G8 keeps them off $gp and they take
   the assembler macro form, while the one-byte D_8009B326 stays gp-relative. */
void func_8003A560(DisplayEffectVramState *a)
{
    DisplayEffectVramSlot *slots;
    DisplayEffectVramSlot *slot;
    FileTransferDescriptor *req;
    s32 i;

    if (func_80039F1C((DisplayEffectState *)a) == 0) {
        slots = (DisplayEffectVramSlot *)D_80010000;
        D_8009B326 = 0;
        /* The slot cursor has to be an explicit walking pointer decremented at
           the end of the body: spelling it &slots[i] leaves the strength-reduced
           induction value in a call-clobbered register and costs a per-iteration
           move plus the fifth callee-saved register. */
        slot = &slots[DISPLAY_EFFECT_VRAM_SLOT_COUNT - 1];
        for (i = DISPLAY_EFFECT_VRAM_SLOT_COUNT - 1; i >= 0; i--) {
            if (D_8015C410[i] < 0) {
                D_8009B326 = i;
            }
            if (D_8015C410[i] == a->field_30) {
                while (IsIdleGPU(10) != 0) {
                    ;
                }
                slot->image_rect.x = 832 - a->field_3C * 192;
                slot->image_rect.y = 256;
                slot->image_rect.w = 192;
                slot->image_rect.h = 256;
                LoadImage(&slot->image_rect, (u32 *)slot->image);
                slot->clut_rect.x = 512;
                slot->clut_rect.y = a->field_3C * 2 + 240;
                slot->clut_rect.w = 256;
                slot->clut_rect.h = 2;
                LoadImage(&slot->clut_rect, (u32 *)slot->clut);
                if (a->field_3C != 0) {
                    Util_CopyWords(D_801AF800, slot->extra, 0x800);
                } else {
                    Util_CopyWords(D_801AF000, slot->extra, 0x800);
                }
                a->state |= 0x40;
                return;
            }
            slot--;
        }
        if (((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             D_8009B134_abs) != 0) {
            a->state &= 0x7F;
            return;
        }
        req = File_TryRequestAsyncTransfer(
            0, 0, a->field_30 * 50 + 15182, 50, func_8003A01C, 0, 0
        );
        req->callback_data = D_801AF000;
        req->position = a->field_3C;
        if (a->field_3C != 0) {
            req->callback_data = D_801AF800;
        }
        D_8009B0F4_abs = req->status_flags | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
    } else if ((a->state & 0x40) == 0) {
        if (((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             D_8009B134_abs) != 0) {
            return;
        }
        a->state |= 0x40;
        D_8015C410[D_8009B326] = a->field_30;
        while (IsIdleGPU(10) != 0) {
            ;
        }
        slot = &((DisplayEffectVramSlot *)D_80010000)[D_8009B326];
        slot->image_rect.x = 832 - a->field_3C * 192;
        slot->image_rect.y = 256;
        slot->image_rect.w = 192;
        slot->image_rect.h = 256;
        StoreImage(&slot->image_rect, (u32 *)slot->image);
        slot->clut_rect.x = 512;
        slot->clut_rect.y = a->field_3C * 2 + 240;
        slot->clut_rect.w = 256;
        slot->clut_rect.h = 2;
        StoreImage(&slot->clut_rect, (u32 *)slot->clut);
        if (a->field_3C != 0) {
            Util_CopyWords(slot->extra, D_801AF800, 0x800);
        } else {
            Util_CopyWords(slot->extra, D_801AF000, 0x800);
        }
    } else {
        func_8003A1EC((MenuRecord *)a, (u8 **)a, a->field_31);
        a->state = 0;
        a->field_32 |= 0x40;
    }
}
