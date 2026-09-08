#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"

extern u8 *D_80010000;
extern s8 D_8015C410[];
extern s8 D_8009B326;
extern s32 D_8009B0F4;
extern s32 D_8009B134;
extern u8 D_801AF000[];
extern u8 D_801AF800[];

extern s32 func_80039F1C(u8 *);
extern s32 func_8003A1EC(u8 *, u8 **, s32);
extern void func_8003A01C(u8 *, s32);
extern void Util_CopyWords(void *, void *, s32);

/* Swaps the effect's VRAM page in and out around a memory-card style
 * transition. D_8015C410 is a five-entry table of resident page ids and
 * D_80010000 points at the backing store, whose pages are 0x18C10 bytes apart.
 *
 * On the first tick it walks the table from the top down, recording the last
 * free entry in D_8009B326 as it goes; if the effect's page (+0x30) is already
 * resident it uploads it from that page - two LoadImage rectangles built in
 * the page's own 0xC00 scratch area, plus the 0x800-byte block at +0x18400 -
 * and sets bit 6 of +0x33. Otherwise, provided no transfer is in flight, it
 * queues one through File_TryRequestAsyncTransfer with func_8003A01C as the
 * callback.
 *
 * On a later tick, once bit 6 of +0x33 is clear, it claims the free entry for
 * this page and does the reverse: two StoreImage rectangles and the block copy
 * back out. Once bit 6 is set it hands off to func_8003A1EC and moves the flag
 * to +0x32. */
void func_8003A560(u8 *p)
{
    FileTransferDescriptor *req;
    register u8 *slot asm("$18");
    register u8 *img asm("$16");
    u8 *vram;
    u8 *buf;
    s32 i;
    s8 *tbl;
    register u8 *base asm("$3");
    s32 flags;
    s32 w;
    s32 c;
    s32 y;
    register s32 o asm("$2");

    if (func_80039F1C(p) == 0) {
        o = 4 * 0x18C10;
        i = 4;
        tbl = D_8015C410;
        base = D_80010000;
        D_8009B326 = 0;
        slot = base + o;
        for (; i >= 0; slot -= 0x18C10, i--) {
            if (*(s8 *)(i + (s32)tbl) < 0) {
                D_8009B326 = i;
            }
            if (*(s8 *)(i + (s32)tbl) == *(s8 *)(p + 0x30)) {
                while (IsIdleGPU(10)) {
                }
                img = slot + 0x18000;
                w = 0x340 - p[0x3C] * 0xC0;
                *(s16 *)(img + 0xC04) = 0xC0;
                *(s16 *)(img + 0xC02) = 0x100;
                *(s16 *)(img + 0xC06) = 0x100;
                *(s16 *)(img + 0xC00) = w;
                LoadImage((RECT *)(slot + 0x18C00), (u32 *)slot);
                *(s16 *)(img + 0xC08) = 0x200;
                y = p[0x3C] * 2 + 0xF0;
                *(s16 *)(img + 0xC0C) = 0x100;
                *(s16 *)(img + 0xC0E) = 2;
                *(s16 *)(img + 0xC0A) = y;
                LoadImage((RECT *)(slot + 0x18C08), (u32 *)img);
                if (p[0x3C] != 0) {
                    Util_CopyWords(D_801AF800, slot + 0x18400, 0x800);
                } else {
                    Util_CopyWords(D_801AF000, slot + 0x18400, 0x800);
                }
                p[0x33] |= 0x40;
                return;
            }
        }
        if (((D_8009B0F4 & FILE_TRANSFER_REQUEST_BLOCKED_MASK) | D_8009B134)
            != 0) {
            p[0x33] &= 0x7F;
            return;
        }
        req = File_TryRequestAsyncTransfer(
            0,
            0,
            *(s8 *)(p + 0x30) * 50 + 0x3B4E,
            0x32,
            func_8003A01C,
            0,
            0
        );
        req->callback_data = D_801AF000;
        req->position = p[0x3C];
        if (p[0x3C] != 0) {
            req->callback_data = D_801AF800;
        }
        D_8009B0F4 = req->status_flags | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
        return;
    }

    flags = p[0x33];
    if ((flags & 0x40) == 0) {
        if (((D_8009B0F4 & FILE_TRANSFER_REQUEST_BLOCKED_MASK) | D_8009B134)
            != 0) {
            return;
        }
        p[0x33] = flags | 0x40;
        D_8015C410[D_8009B326] = p[0x30];
        while (IsIdleGPU(10)) {
        }
        vram = D_80010000 + D_8009B326 * 0x18C10;
        c = p[0x3C];
        buf = vram + 0x18000;
        *(s16 *)(buf + 0xC02) = 0x100;
        *(s16 *)(buf + 0xC06) = 0x100;
        *(s16 *)(buf + 0xC00) = 0x340 - c * 0xC0;
        *(s16 *)(buf + 0xC04) = 0xC0;
        StoreImage((RECT *)(vram + 0x18C00), (u32 *)vram);
        *(s16 *)(buf + 0xC08) = 0x200;
        y = p[0x3C] * 2 + 0xF0;
        *(s16 *)(buf + 0xC0C) = 0x100;
        *(s16 *)(buf + 0xC0E) = 2;
        *(s16 *)(buf + 0xC0A) = y;
        StoreImage((RECT *)(vram + 0x18C08), (u32 *)buf);
        if (p[0x3C] != 0) {
            Util_CopyWords(vram + 0x18400, D_801AF800, 0x800);
        } else {
            Util_CopyWords(vram + 0x18400, D_801AF000, 0x800);
        }
    } else {
        func_8003A1EC(p, (u8 **)p, p[0x31]);
        p[0x33] = 0;
        p[0x32] |= 0x40;
    }
}

