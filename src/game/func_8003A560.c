#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"

typedef struct {
    u8 image[0x18000];
    u8 clut[0x400];
    u8 extra[0x800];
    RECT imageRect;
    RECT clutRect;
} VramSlot;

/* The scalars carry section(".data") so -G8 keeps them off $gp and they take
   the assembler macro form, while the one-byte D_8009B326 stays gp-relative. */
extern s8 D_8009B326;
extern s8 D_8015C410[];
extern u8 D_801AF000[];
extern u8 D_801AF800[];
extern u8 *D_80010000 __attribute__((section(".data")));
extern s32 D_8009B0F4 __attribute__((section(".data")));
extern s32 D_8009B134 __attribute__((section(".data")));

extern s32 func_80039F1C(void);
extern void func_8003A01C(void);
extern s32 func_8003A1EC(u8 *, u8 **, s32);
extern void Util_CopyWords(void *, void *, s32);

void func_8003A560(u8 *a)
{
    VramSlot *slots;
    VramSlot *slot;
    FileTransferDescriptor *req;
    s32 i;

    if (func_80039F1C() == 0) {
        slots = (VramSlot *)D_80010000;
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
            if (D_8015C410[i] == *(s8 *)(a + 0x30)) {
                while (IsIdleGPU(10) != 0) {
                    ;
                }
                slot->imageRect.x = 832 - a[0x3C] * 192;
                slot->imageRect.y = 256;
                slot->imageRect.w = 192;
                slot->imageRect.h = 256;
                LoadImage(&slot->imageRect, (u32 *)slot->image);
                slot->clutRect.x = 512;
                slot->clutRect.y = a[0x3C] * 2 + 240;
                slot->clutRect.w = 256;
                slot->clutRect.h = 2;
                LoadImage(&slot->clutRect, (u32 *)slot->clut);
                if (a[0x3C] != 0) {
                    Util_CopyWords(D_801AF800, slot->extra, 0x800);
                } else {
                    Util_CopyWords(D_801AF000, slot->extra, 0x800);
                }
                a[0x33] |= 0x40;
                return;
            }
            slot--;
        }
        if (((D_8009B0F4 & FILE_TRANSFER_REQUEST_BLOCKED_MASK) | D_8009B134) !=
            0) {
            a[0x33] &= 0x7F;
            return;
        }
        req = File_TryRequestAsyncTransfer(
            0, 0, *(s8 *)(a + 0x30) * 50 + 15182, 50, func_8003A01C, 0, 0
        );
        req->callback_data = D_801AF000;
        req->position = a[0x3C];
        if (a[0x3C] != 0) {
            req->callback_data = D_801AF800;
        }
        D_8009B0F4 = req->status_flags | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
    } else if ((a[0x33] & 0x40) == 0) {
        if (((D_8009B0F4 & FILE_TRANSFER_REQUEST_BLOCKED_MASK) | D_8009B134) !=
            0) {
            return;
        }
        a[0x33] |= 0x40;
        D_8015C410[D_8009B326] = *(s8 *)(a + 0x30);
        while (IsIdleGPU(10) != 0) {
            ;
        }
        slot = &((VramSlot *)D_80010000)[D_8009B326];
        slot->imageRect.x = 832 - a[0x3C] * 192;
        slot->imageRect.y = 256;
        slot->imageRect.w = 192;
        slot->imageRect.h = 256;
        StoreImage(&slot->imageRect, (u32 *)slot->image);
        slot->clutRect.x = 512;
        slot->clutRect.y = a[0x3C] * 2 + 240;
        slot->clutRect.w = 256;
        slot->clutRect.h = 2;
        StoreImage(&slot->clutRect, (u32 *)slot->clut);
        if (a[0x3C] != 0) {
            Util_CopyWords(slot->extra, D_801AF800, 0x800);
        } else {
            Util_CopyWords(slot->extra, D_801AF000, 0x800);
        }
    } else {
        func_8003A1EC(a, (u8 **)a, a[0x31]);
        a[0x33] = 0;
        a[0x32] |= 0x40;
    }
}
