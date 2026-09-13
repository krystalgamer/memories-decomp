#include "../types.h"
#include "../psyq/libcd.h"
#include "../psyq/libds.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libspu.h"
#include "file_transfer.h"

void func_80013C28(s32 arg)
{
    s32 event;
    s32 remaining;
    u8 *transfer;
    u8 *p;
    u8 *q;
    u8 *r;
    u8 *image;
    u8 *counter;
    u8 *t;
    u8 *dst;
    u32 *src;
    u32 word;
    s32 i;
    s32 n;
    s32 image_y;

    event = arg & 0xFF;
    D_8009B114++;
    if (event != 1) {
        return;
    }
    D_8009B138 = (s32)((u32)D_8009B138 + 1);
    p = (u8 *)D_8009AF18;
    switch (p[0x46]) {
    case 1:
        if ((D_8009B0F4 & 0x200000) == 0) {
            if ((D_8009B0F4 & 0x40000000) == 0) {
                CdGetSector(*(void **)(p + 8), 0x200);
            } else {
                i = 0;
                t = p;
                src = D_8009B0F8;
                do {
                    word = src[i];
                    *(u32 *)(*(u8 **)(t + 8) + i * 4) = word;
                    i++;
                } while (i < 0x200);
            }
            *(u32 *)((u8 *)D_8009AF18 + 8) += 0x800;
        }
        transfer = (u8 *)D_8009AF18;
        remaining = *(s32 *)(transfer + 0x10);
        D_8009B0F8 += 0x200;
        remaining = (s32)((u32)remaining - 0x800);
        *(s32 *)(transfer + 0x10) = remaining;
        if (remaining <= 0) {
            DsEndReadySystem();
            CdReadyCallback(0);
        }
        q = (u8 *)D_8009AF18;
        *(s32 *)(q + 0x28) = (s32)((u32)*(s32 *)(q + 0x28) - 0x800);
        if (*(s32 *)(q + 0x28) <= 0) {
            *(s32 *)(q + 0x1C) = 0;
            if (*(s32 *)(q + 0x20) != 0) {
                (*(void (**)(u8 *, s32))(q + 0x20))(q, (*(u32 *)(q + 0x40))++);
            }
            *(s32 *)(q + 0x28) = *(s32 *)(q + 0x1C);
        }
        if (*(s32 *)((u8 *)D_8009AF18 + 0x10) <= 0) {
            goto clear;
        }
        return;
    case 2:
        dst = *(u8 **)((u32)p + ((*(u16 *)(p + 0x44) & 1) << 2) + 8);
        if ((D_8009B0F4 & 0x40000000) == 0) {
            CdGetSector(dst, 0x200);
        } else {
            src = D_8009B0F8;
            for (i = 0; i < 512; ++i) {
                ((u32 *)dst)[i] = src[i];
            }
            D_8009B0F8 += 0x200;
        }
        *(s32 *)((u8 *)D_8009AF18 + 0x10) = (s32)((u32)*(s32 *)((u8 *)D_8009AF18 + 0x10) - 0x800);
        if (*(s32 *)((u8 *)D_8009AF18 + 0x10) <= 0) {
            DsEndReadySystem();
            CdReadyCallback(0);
        }
        image = (u8 *)D_8009AF18;
        *(u16 *)(image + 0) = *(u16 *)(image + 0x30);
        *(u16 *)(image + 2) = *(u16 *)(image + 0x32);
        while (LoadImage2((RECT *)(u8 *)D_8009AF18, (u32 *)dst)) {
        }
        if ((D_8009B0F4 & 0x20000) != 0) {
            *(u16 *)((u8 *)D_8009AF18 + 0x30) += 0x40;
        } else {
            r = (u8 *)D_8009AF18;
            image_y = *(u16 *)(r + 0x32) + 0x10;
            *(u16 *)(r + 0x32) = image_y;
            if ((image_y & 0xFF) == 0) {
                *(u16 *)(r + 0x32) = (image_y ^ 0x100) & 0x100;
                *(u16 *)(r + 0x30) += 0x40;
            }
        }
        q = (u8 *)D_8009AF18;
        *(s32 *)(q + 0x28) = (s32)((u32)*(s32 *)(q + 0x28) - 0x800);
        if (*(s32 *)(q + 0x28) > 0) {
            goto counter;
        }
        goto step;
    case 3:
        n = 0x800;
        dst = *(u8 **)(p + 8);
        if (*(s32 *)(p + 0x28) < n) {
            n = *(s32 *)(p + 0x28);
        }
        if ((D_8009B0F4 & 0x40000000) == 0) {
            CdGetSector(dst, n / 4);
        } else {
            src = D_8009B0F8;
            for (i = 0; i < n / 4; ++i) {
                ((u32 *)dst)[i] = src[i];
            }
            D_8009B0F8 = (u32 *)((u8 *)D_8009B0F8 + n);
        }
        *(s32 *)((u8 *)D_8009AF18 + 0x10) = (s32)((u32)*(s32 *)((u8 *)D_8009AF18 + 0x10) - 0x800);
        if (*(s32 *)((u8 *)D_8009AF18 + 0x10) <= 0) {
            DsEndReadySystem();
            CdReadyCallback(0);
        }
        SpuSetTransferStartAddr(*(u32 *)((u8 *)D_8009AF18 + 0x30));
        SpuWrite(dst, (u32)n);
        q = (u8 *)D_8009AF18;
        *(u32 *)(q + 0x30) += (u32)n;
        *(s32 *)(q + 0x28) = (s32)((u32)*(s32 *)(q + 0x28) - 0x800);
        if (*(s32 *)(q + 0x28) > 0) {
            goto counter;
        }
    step:
        *(s32 *)(q + 0x1C) = 0;
        if (*(s32 *)(q + 0x20) != 0) {
            (*(void (**)(u8 *, s32))(q + 0x20))(q, (*(u32 *)(q + 0x40))++);
        }
        *(s32 *)(q + 0x28) = *(s32 *)(q + 0x1C);
    counter:
        counter = (u8 *)D_8009AF18;
        *(u16 *)(counter + 0x44) = *(u16 *)(counter + 0x44) + 1;
        if (*(s32 *)(counter + 0x10) > 0) {
            return;
        }
    clear:
        D_8009B0F4 &= ~0x100;
        return;
    }
}
