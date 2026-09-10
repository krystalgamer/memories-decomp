#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_card_record_lifecycle.h"
#include "duel_side_state.h"
#include "display_object_layout.h"
#include "display_object_api.h"
#include "display_object_helpers.h"
#include "file_transfer.h"
#include "func_8001944C.h"
#include "func_80019564.h"
#include "duel_magic_effect_dispatch.h"
#include "func_800291E0.h"
#include "duel_effect_resource_setup.h"
#include "../unmatched.h"
#include "func_80019608.h"
#include "display_object_work_slots.h"

/* Defined rather than declared: the assembler only resolves a small global
   gp-relative when the translation unit defines it, and that is what gives the
   store below the retail load-delay nop. c_symbols.ld overrides the common
   symbol, so no storage is allocated here. */
u16 D_8009B150;

void func_80019608(void)
{
    DisplayObject *p;
    DuelCardRecord *slot;
    DisplayObject *q0;
    DisplayObject *q1;
    u8 state;
    u16 flags;
    u16 f2;
    u16 f3;
    u8 st;
    s32 v1;
    s32 v2;
    s32 v3;
    s32 arg;
    /* The resize branch below is the one place where no source shape reaches
       retail's allocation: unpinned, the field read and the offset value share
       a register and the two independent values in the fade-out do not. The
       matched siblings func_8001944C.c and func_80037DA4 use the same
       device.  v4 and v5 share $5 because their live ranges are disjoint,
       which is what retail does. */
    register s32 fld __asm__("$3");
    register s32 v4 __asm__("$5");
    register s32 v5 __asm__("$5");

    p = D_800E9EF0[0];
    flags = D_8009B23A;
    if ((flags & 0x8000) == 0) {
        D_8009B23A = flags | 0xC000;
        slot = &D_801A7AD8[p->field_6A];
        arg = slot->card_id;
        D_8009B150 = *(u16 *)&slot->card_id;
        func_80029164(0, arg);
        if (p->field_68 == CARD_TYPE_MAGIC) {
            D_8009B1C8->field_05 = D_8009B1C8->field_05 + 1;
        }
        func_80024914(slot);
        D_8009B174 = 1;
    }
    state = D_8009B174;
    switch (state & 0xF) {
    case 1:
        if ((state & 0x80) == 0) {
            if (((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                 D_8009B134_abs) != 0) {
                return;
            }
            f2 = p->flags;
            D_8009B174 = state | 0x80;
            p->flags = f2 | DISPLAY_OBJECT_FLAG_CLIP_TEST;
            p = (DisplayObject *)func_800291E0(0, -1, -1);
            p->field_30.h.field_30 = 0x5A;
            p->field_30.h.field_32 = 0x16;
            p->field_20.b.field_21 = 0xC0;
            func_800428EC((u8 *)p, -0xA);
            p->flags =
                (p->flags | DISPLAY_OBJECT_FLAG_CLIP_TEST) &
                ~DISPLAY_OBJECT_FLAG_RENDERABLE;
            D_800E9EF0[1] = p;
            return;
        }
        if ((state & 0x40) == 0) {
            v1 = p->field_20.b.field_21 + 6;
            p->field_20.b.field_21 = v1;
            if ((u8)v1 < 0x40) {
                return;
            }
            D_8009B174 = D_8009B174 | 0x40;
            func_8004036C(p);
            q1 = D_800E9EF0[1];
            D_800E9EF0[0] = 0;
            q1->flags = q1->flags | DISPLAY_OBJECT_FLAG_RENDERABLE;
            return;
        }
        p = D_800E9EF0[1];
        if ((state & 0x20) == 0) {
            v2 = p->field_20.b.field_21 + 6;
            p->field_20.b.field_21 = v2;
            if ((s8)v2 < 0) {
                return;
            }
            p->field_60 = 0x1E;
            p->field_20.b.field_21 = 0;
            f3 = p->flags;
            st = D_8009B174 | 0x20;
            p->flags = f3 & ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
            D_8009B174 = st;
            return;
        }
        v3 = *(u16 *)&p->field_60 - 1;
        p->field_60 = v3;
        if ((s16)v3 > 0) {
            return;
        }
        D_8009B174 = 2;
        return;
    case 2:
        func_80026BA4((s16)D_8009B150, 0);
        D_8009B174 = 3;
        return;
    case 3:
        if ((state & 0x80) == 0) {
            p = D_800E9EF0[1];
            D_8009B174 = state | 0x80;
            func_8001944C((DisplayObject *)p);
            D_800E9EF0[0] =
                func_80019564((DisplayObjectConfigView *)p);
            D_800E9EF0[0]->attribute = D_800E9EF0[0]->attribute | (GsALON | GsAONE);
            D_800E9EF0[0]->attribute = D_800E9EF0[0]->attribute & ~GsROTOFF;
            D_800E9EF0[1] =
                func_80019564((DisplayObjectConfigView *)p);
            func_800428EC((u8 *)D_800E9EF0[1], -1);
            D_800E9EF0[1]->attribute = D_800E9EF0[1]->attribute | (GsALON | GsATWO);
            D_800E9EF0[1]->attribute = D_800E9EF0[1]->attribute & ~GsROTOFF;
            func_80029528(0);
            return;
        }
        q0 = D_800E9EF0[0];
        fld = q0->field_44.h.field_44;
        v4 = fld + 0x80;
        q1 = D_800E9EF0[1];
        *(u16 *)&q1->field_44.h.field_46 = v4;
        *(u16 *)&q1->field_44.h.field_44 = v4;
        *(u16 *)&q0->field_44.h.field_46 = v4;
        *(u16 *)&q0->field_44.h.field_44 = v4;
        v5 = *(u8 *)&D_800E9EF0[0]->field_0C - 4;
        if (v5 < 0) {
            v5 = 0;
        }
        v5 = v5 | ((v5 << 16) | (v5 << 8));
        D_800E9EF0[0]->field_0C = v5;
        D_800E9EF0[1]->field_0C = v5;
        if (v5 != 0) {
            return;
        }
        func_8004036C(D_800E9EF0[0]);
        func_8004036C(D_800E9EF0[1]);
        D_8009B174 = 4;
        return;
    case 4:
        if ((state & 0x80) == 0) {
            D_8009B174 = state | 0x80;
            func_80026BA4((s16)D_8009B150, 1);
            return;
        }
        D_8009B23A = 5;
        return;
    }
}
