#include "../types.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "file_transfer.h"
#include "sound.h"

typedef struct {
    u8 pad0000[0x19];
    s8 pending;
    u8 pad001A[6];
} DuelSideRank;

extern u16 D_8009B23A;
extern u8 D_8009B1D5;
extern s8 D_8009B1B9;
extern s8 D_8009B208[8];
extern u8 *D_8009B1C8;
extern u8 *D_8009B1F0[DUEL_SIDE_COUNT];
extern u32 D_8009B134 __attribute__((section(".data")));
extern u16 D_8009B36A __attribute__((section(".data")));
extern u8 D_800907D8[];
extern u8 D_800E9ECE[];
extern u8 D_800E9FF0[];
extern u8 D_8015C424[];
extern u8 D_801A7AD8[];
extern u8 D_801A7B64[];

extern void func_80024D34(s32, s32);
extern void Duel_ApplyCardObjectFlags(u8 *);
extern void func_8001352C(void);
extern u8 *func_8002C604(s32);
extern u8 *func_8002C68C(s32);
extern void func_80024954(u8 *);
extern void func_800157DC(void);
extern void SD_BGMPlay(u32);
extern s16 func_800181EC(u8 *);

void func_8001825C(void)
{
    u8 *rec;
    u8 *p;
    u8 *obj;
    u8 *q;
    register u8 *b asm("$3");
    u8 *card;
    s32 i;
    s32 keep;
    s32 y;
    u16 flags;
    s8 n;

    if ((D_8009B23A & 0x8000) == 0) {
        D_8009B23A |= 0x8000;
        rec = D_801A7B64;
        for (i = 5; i < DUEL_CARD_SIDE_RECORD_COUNT;
             i++, rec += DUEL_CARD_RECORD_SIZE) {
            p = rec + 0x12;
            flags = *(u16 *)(p + 4);
            if (flags & DUEL_CARD_FLAG_OCCUPIED) {
                keep = flags & 0x7A00;
                y = *(s16 *)p;
                func_80024D34(i, *(s8 *)(*(u8 **)(p - 0xE) + 2));
                *(u16 *)(p + 4) |= keep;
                *(s16 *)p = y;
                Duel_ApplyCardObjectFlags(*(u8 **)rec);
            }
        }
        rec = D_801A7B64 + 0x1A4;
        for (i = 20; i < DUEL_CARD_RECORD_COUNT;
             i++, rec += DUEL_CARD_RECORD_SIZE) {
            p = rec + 0x12;
            flags = *(u16 *)(p + 4);
            if (flags & DUEL_CARD_FLAG_OCCUPIED) {
                keep = flags & 0x7A00;
                y = *(s16 *)p;
                func_80024D34(i, *(s8 *)(*(u8 **)(p - 0xE) + 2));
                *(u16 *)(p + 4) |= keep;
                *(s16 *)p = y;
                Duel_ApplyCardObjectFlags(*(u8 **)rec);
            }
        }
        func_8001352C();
        for (i = 0; i < DUEL_SIDE_COUNT; i++) {
            if (((DuelSideRank *)D_800E9FF0)[i].pending != 0) {
                obj = func_8002C604(0x15);
                *(u16 *)(obj + 0x1A) = i + 2;
                obj[0x1C] |= 0x20;
                D_8009B1F0[i] = obj;
            }
        }
        if (*(s8 *)D_8009B1C8 == 0x28) {
            D_8009B23A |= 0x2000;
            for (i = 0; i < DUEL_FIELD_SIDE_ZONE_COUNT; i++) {
                rec = D_801A7AD8 +
                      D_800907D8[i + D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT] *
                          DUEL_CARD_RECORD_SIZE;
                if (*(u16 *)(rec + 0x16) & DUEL_CARD_FLAG_OCCUPIED) {
                    func_80024954(rec);
                }
            }
        } else {
            q = D_800E9FF0;
            if (*(s16 *)(q + 0x14) != 0 && *(s16 *)(q + 0x34) != 0) {
                SD_BGMPlay(D_8009B36A);
            }
        }
        func_800157DC();
        return;
    }

    if ((D_8009B23A & 0x4000) == 0) {
        if (((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             D_8009B134) != 0) {
            return;
        }
        if ((D_800E9ECE[0] & 0x80) != 0) {
            return;
        }
        D_8009B23A |= 0x4000;
        D_8009B1B9 = 2;
        if ((D_8009B23A & 0x2000) != 0) {
            D_8009B23A = 0xC;
        }
        return;
    }

    for (;;) {
        n = D_8009B1B9 - 1;
        D_8009B1B9 = n;
        if (n < 0) {
            D_8009B23A = 5;
            return;
        }
        if (D_8009B208[n] >= 0) {
            break;
        }
    }
    {
        register s32 replay_offset asm("$5") = 0x48000;
        b = D_8015C424;
        card = *(u8 **)(b + D_8009B208[n] * DUEL_CARD_RECORD_SIZE +
                        replay_offset + 0x36B4);
    }
    func_8001352C();
    obj = func_8002C68C(0xB);
    *(u16 *)obj = *(u16 *)(card + 0x30);
    *(u16 *)(obj + 2) = *(u16 *)(card + 0x32);
    *(u16 *)(obj + 4) = *(u16 *)(card + 0x34);
    *(u16 *)(obj + 0x1A) = func_800181EC(card);
    func_80024954(D_801A7AD8 + card[0x6A] * DUEL_CARD_RECORD_SIZE);
    SD_SEPlayFull(0x1F);
}
