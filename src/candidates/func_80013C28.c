/*
 * DsStartReadySystem sector callback. Current best: 283 of 286 target
 * instructions, opcode multiset distance 3. GCC coalesces one mode-1
 * descriptor copy and two callback reloads; preserve the copy-loop and
 * callback-block shapes while refining it.
 */
#include "../types.h"
#include "../psyq/libcd.h"
#include "../psyq/libds.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libspu.h"
#include "../game/file_transfer.h"

extern u8 *D_8009AF18;
extern u32 *D_8009B0F8;
extern u8 D_8009B114;
extern s32 D_8009B138;

extern s32 func_80077150(void *, s32);

void func_80013C28(s32 arg)
{
    s32 event;
    u8 *p;
    u8 *q;
    u8 *r;
    u8 *t;
    u8 *dst;
    u8 *w;
    u32 *src;
    s32 i;
    s32 n;
    s32 seq;

    event = arg & 0xFF;
    D_8009B114++;
    if (event != 1) {
        return;
    }
    D_8009B138++;
    p = D_8009AF18;
    switch (p[0x46]) {
    case 1:
        if ((D_8009B0F4 & 0x200000) == 0) {
            if ((D_8009B0F4 & 0x40000000) == 0) {
                CdGetSector(*(void **)(p + 8), 0x200);
            } else {
                t = p;
                src = D_8009B0F8;
                i = 0;
                do {
                    *(u32 *)(*(u8 **)(t + 8) + i * 4) = *src;
                    i++;
                    src++;
                } while (i < 0x200);
            }
            *(s32 *)(D_8009AF18 + 8) += 0x800;
        }
        D_8009B0F8 += 0x200;
        *(s32 *)(D_8009AF18 + 0x10) -= 0x800;
        if (*(s32 *)(D_8009AF18 + 0x10) <= 0) {
            DsEndReadySystem();
            CdReadyCallback(0);
        }
        q = D_8009AF18;
        *(s32 *)(q + 0x28) -= 0x800;
        if (*(s32 *)(q + 0x28) <= 0) {
            *(s32 *)(q + 0x1C) = 0;
            if (*(s32 *)(q + 0x20) != 0) {
                seq = *(s32 *)(q + 0x40);
                *(s32 *)(q + 0x40) = seq + 1;
                (*(void (**)(u8 *, s32))(q + 0x20))(q, seq);
            }
            *(s32 *)(q + 0x28) = *(s32 *)(q + 0x1C);
        }
        if (*(s32 *)(D_8009AF18 + 0x10) <= 0) {
            goto clear;
        }
        return;
    case 2:
        dst = *(u8 **)(p + 8 + (*(u16 *)(p + 0x44) & 1) * 4);
        if ((D_8009B0F4 & 0x40000000) == 0) {
            CdGetSector(dst, 0x200);
        } else {
            w = dst;
            src = D_8009B0F8;
            i = 0;
            do {
                *(u32 *)w = *src;
                src++;
                i++;
                w += 4;
            } while (i < 0x200);
            D_8009B0F8 += 0x200;
        }
        *(s32 *)(D_8009AF18 + 0x10) -= 0x800;
        if (*(s32 *)(D_8009AF18 + 0x10) <= 0) {
            DsEndReadySystem();
            CdReadyCallback(0);
        }
        r = D_8009AF18;
        *(u16 *)(r + 0) = *(u16 *)(r + 0x30);
        *(u16 *)(r + 2) = *(u16 *)(r + 0x32);
        while (LoadImage2((RECT *)D_8009AF18, (u32 *)dst)) {
        }
        if ((D_8009B0F4 & 0x20000) != 0) {
            *(u16 *)(D_8009AF18 + 0x30) += 0x40;
        } else {
            r = D_8009AF18;
            n = *(u16 *)(r + 0x32) + 0x10;
            *(u16 *)(r + 0x32) = n;
            if ((n & 0xFF) == 0) {
                *(u16 *)(r + 0x32) = (n ^ 0x100) & 0x100;
                *(u16 *)(r + 0x30) += 0x40;
            }
        }
        q = D_8009AF18;
        *(s32 *)(q + 0x28) -= 0x800;
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
            w = dst;
            src = D_8009B0F8;
            i = 0;
            while (i < n / 4) {
                i++;
                *(u32 *)w = *src;
                src++;
                w += 4;
            }
            D_8009B0F8 = (u32 *)((u8 *)D_8009B0F8 + n);
        }
        *(s32 *)(D_8009AF18 + 0x10) -= 0x800;
        if (*(s32 *)(D_8009AF18 + 0x10) <= 0) {
            DsEndReadySystem();
            CdReadyCallback(0);
        }
        SpuSetTransferStartAddr(*(u32 *)(D_8009AF18 + 0x30));
        func_80077150(dst, n);
        q = D_8009AF18;
        *(s32 *)(q + 0x30) += n;
        *(s32 *)(q + 0x28) -= 0x800;
        if (*(s32 *)(q + 0x28) > 0) {
            goto counter;
        }
    step:
        *(s32 *)(q + 0x1C) = 0;
        if (*(s32 *)(q + 0x20) != 0) {
            seq = *(s32 *)(q + 0x40);
            *(s32 *)(q + 0x40) = seq + 1;
            (*(void (**)(u8 *, s32))(q + 0x20))(q, seq);
        }
        *(s32 *)(q + 0x28) = *(s32 *)(q + 0x1C);
    counter:
        r = D_8009AF18;
        *(u16 *)(r + 0x44) = *(u16 *)(r + 0x44) + 1;
        if (*(s32 *)(r + 0x10) > 0) {
            return;
        }
    clear:
        D_8009B0F4 &= ~0x100;
        return;
    }
}
