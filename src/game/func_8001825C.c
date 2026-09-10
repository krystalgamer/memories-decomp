#define D_8009B36A_IN_DATA
#include "../types.h"
#include "display_object.h"
#include "duel_card_object_helpers.h"
#include "duel_effect_request.h"
#include "func_8002C604.h"
#include "duel_card.h"
#include "duel_card_icon_setup.h"
#include "duel_card_object_cleanup.h"
#include "duel_side_state.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "fade.h"
#include "file_transfer.h"
#include "sound.h"
#include "view_state.h"
#include "../unmatched.h"
#include "duel_apply_card_object_flags.h"
#include "func_8001825C.h"

extern s8 D_8009B1B9;
extern s8 D_8009B208[8];
extern u8 D_8015C424[];

void func_8001825C(void)
{
    DuelCardRecord *rec;
    u8 *obj;
    u8 *q;
    register u8 *b asm("$3");
    DisplayObject *card;
    s32 i;
    s32 keep;
    s32 y;
    u16 flags;
    s8 n;

    if ((D_8009B23A & 0x8000) == 0) {
        D_8009B23A |= 0x8000;
        rec = D_801A7B64;
        for (i = 5; i < DUEL_CARD_SIDE_RECORD_COUNT; i++, rec++) {
            flags = rec->flags;
            if (flags & DUEL_CARD_FLAG_OCCUPIED) {
                keep = flags & 0x7A00;
                y = rec->stat_modifier;
                func_80024D34(i, ((s8 *)rec->data)[2]);
                rec->flags |= keep;
                rec->stat_modifier = y;
                Duel_ApplyCardObjectFlags(
                    (DuelCardDisplayObject *)rec->object);
            }
        }
        rec = D_801A7B64 + 15;
        for (i = 20; i < DUEL_CARD_RECORD_COUNT; i++, rec++) {
            flags = rec->flags;
            if (flags & DUEL_CARD_FLAG_OCCUPIED) {
                keep = flags & 0x7A00;
                y = rec->stat_modifier;
                func_80024D34(i, ((s8 *)rec->data)[2]);
                rec->flags |= keep;
                rec->stat_modifier = y;
                Duel_ApplyCardObjectFlags(
                    (DuelCardDisplayObject *)rec->object);
            }
        }
        func_8001352C();
        for (i = 0; i < DUEL_SIDE_COUNT; i++) {
            if (D_800E9FF0[i].field_19 != 0) {
                obj = func_8002C604(0x15);
                *(u16 *)(obj + 0x1A) = i + 2;
                obj[0x1C] |= 0x20;
                D_8009B1F0[i] = obj;
            }
        }
        if (D_8009B1C8->field_00 == 0x28) {
            D_8009B23A |= 0x2000;
            for (i = 0; i < DUEL_FIELD_SIDE_ZONE_COUNT; i++) {
                rec = &D_801A7AD8[D_800907D8[
                    i + D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT]];
                if (rec->flags & DUEL_CARD_FLAG_OCCUPIED) {
                    func_80024954(rec);
                }
            }
        } else {
            q = (u8 *)D_800E9FF0;
            if (*(s16 *)(q + 0x14) != 0 && *(s16 *)(q + 0x34) != 0) {
                SD_BGMPlay(D_8009B36A);
            }
        }
        Fade_StartIn();
        return;
    }

    if ((D_8009B23A & 0x4000) == 0) {
        if (((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             D_8009B134_abs) != 0) {
            return;
        }
        if ((gFade_State.flags & 0x80) != 0) {
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
        card = *(DisplayObject **)(b + D_8009B208[n] * DUEL_CARD_RECORD_SIZE +
                        replay_offset + 0x36B4);
    }
    func_8001352C();
    obj = (u8 *)func_8002C68C(0xB);
    *(u16 *)obj = card->field_30.h.field_30;
    *(u16 *)(obj + 2) = card->field_30.h.field_32;
    *(u16 *)(obj + 4) = *(u16 *)&card->field_34;
    *(u16 *)(obj + 0x1A) = func_800181EC((CardObject *)card);
    func_80024954(&D_801A7AD8[card->field_6A]);
    SD_SEPlayFull(0x1F);
}
