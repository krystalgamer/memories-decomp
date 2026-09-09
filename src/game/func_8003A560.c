#include "../types.h"
#include "display_effect_lifecycle.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "util_memory.h"
#include "func_8003A1EC.h"
#include "func_8003A560.h"
#include "../unmatched.h"

/* The scalars carry section(".data") so -G8 keeps them off $gp and they take
   the assembler macro form, while the one-byte D_8009B326 stays gp-relative. */
extern s8 D_8009B326;
extern s8 D_8015C410[];
extern u8 *D_80010000 __attribute__((section(".data")));

extern void func_8003A01C(u8 *, s32);

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
        slot = &slots[4];
        for (i = 4; i >= 0; i--) {
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
        func_8003A1EC((u8 *)a, (u8 **)a, a->field_31);
        a->state = 0;
        a->field_32 |= 0x40;
    }
}
