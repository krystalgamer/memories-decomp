#include "../types.h"
#include "display_object_api.h"
#include "../psyq/libmcrd.h"
#include "duel_effect.h"
#include "mem_card.h"
#include "text_box_lifecycle.h"
#include "display_object_helpers.h"
#include "text_box_runtime.h"

typedef struct {
    u8 pad0000[0x30];
    s16 field_0030;
    s16 field_0032;
    u8 pad0034[0x2C];
    s16 field_0060;
    u8 pad0062[0x0A];
    u8 field_006C;
} ObjectState;

extern s32 func_80042B98(ObjectState *, s32, s32, s32);
extern void func_80043178(ObjectState *);
extern void func_80043230(ObjectState *, s32, s32);

u8 func_8003F2B0(ObjectState *object, s32 arg1, s32 arg2, s32 index)
{
    s32 saved_index = index;
    register s32 value asm("$7");

    if (func_80042B98(object, arg1, arg2, index) == 0) {
        func_80043178(object);
    }

    value = object->field_0060;
    if (value < 0) {
        value += 0x40;
        if (value >= 0) {
            object->field_006C = 0;
            value = 0;
        }
    } else {
        value -= 0x40;
        if (value <= 0) {
            object->field_006C = 0;
            value = 0;
        }
    }
    object->field_0060 = value;

    func_80043230(object, arg1, arg2);

    if (saved_index >= 0) {
        TextBox_SetPos(
            (u8 *)D_800EB0F8 + saved_index * 100,
            object->field_0030,
            object->field_0032);
    }

    return object->field_006C;
}

extern u8 D_8009B3EE;
extern u8 *D_8009B3D8;
extern void (*D_80090F9C[])(void);
extern s32 D_8009B3BC;
extern u8 D_8009B3C6;
extern u8 D_8009B3DE;
extern u8 D_8009B3EF;
extern s32 D_8009B3F0;
extern s32 D_8009B3F4;
extern u16 gMemCard_wDialogFlags;
extern u8 D_800EB0F8_raw[] asm("D_800EB0F8");
extern void func_80039794(void);
extern s32 func_8003F2B0_int(ObjectState *, s32, s32, s32)
    asm("func_8003F2B0");

void func_8003F388(void)
{
    s32 i = 0;
    DuelEffectChannel *p = D_800EB0F8;
    u8 *o;

    D_8009B3EE = 0;

    for (; i < DUEL_EFFECT_CHANNEL_COUNT; i++) {
        if (!(p->flags_34 & 0x8000)) {
            D_8009B3EE = i;
            break;
        }
        p++;
    }

    o = func_800400AC(func_8004002C(), 2);
    func_800404CC(o, 0x20, -0x40, 3, 2, 1, 0xB, 0x20C);
    *(u16 *)(o + 8) |= 0x28;
    func_80042918(o);
    func_800428EC(o, 0xF);
    D_8009B3D8 = o;
}

void func_8003F454(void)
{
    u8 *p;
    s32 f;
    s32 t;
    s32 u;
    s32 c;

    f = gMemCard_wDialogFlags;
    if ((f & MEM_CARD_DIALOG_FLAG_CLOSING) != 0) {
        if (D_8009B3D8 == (u8 *)0) {
            gMemCard_wDialogFlags = 0;
            return;
        }
        if (func_8003F2B0_int(
                (ObjectState *)D_8009B3D8, 0x20, 0x100, D_8009B3EE
            ) == 0) {
            TextBox_Destroy(D_800EB0F8_raw + D_8009B3EE * 100);
            func_8004036C((s32)D_8009B3D8);
            D_8009B3D8 = (u8 *)0;
        }
        return;
    }
    if ((f & (MEM_CARD_DIALOG_FLAG_OPENED |
              MEM_CARD_DIALOG_FLAG_RESULT_READY)) ==
        (MEM_CARD_DIALOG_FLAG_OPENED | MEM_CARD_DIALOG_FLAG_RESULT_READY)) {
        if ((f & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            gMemCard_wDialogFlags =
                f | MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            p = TextBox_Create(
                D_8009B3EE, D_8009B3C6, 0x20, 0x50, 0x100, 0x30
            );
            DuelEffect_MarkObjectIfActive(p);
            p[0x59] = 0x10;
            if ((gMemCard_wDialogFlags & 0x20) != 0) {
                do {
                    func_80039794();
                } while (*(s32 *)(p + 0x30) == 0);
                return;
            }
            if ((gMemCard_wDialogFlags & 0x10) != 0) {
                *(u16 *)(p + 0x34) = *(u16 *)(p + 0x34) | 0x1008;
                return;
            }
            func_80039A14(p);
            goto b14;
        }
        func_80039794();
        p = D_800EB0F8_raw + D_8009B3EE * 100;
        if ((*(s32 *)(p + 0x34) & 0x2008) != 0x2000) {
            return;
        }
    b14:
        t = gMemCard_wDialogFlags;
        u = t & 8;
        gMemCard_wDialogFlags =
            t & ~MEM_CARD_DIALOG_FLAG_RESULT_READY;
        if (u == 0) {
            return;
        }
        gMemCard_wDialogFlags = 0;
        goto b25;
    }
    if ((f & MEM_CARD_DIALOG_FLAG_OPENED) == 0) {
        if ((f & MEM_CARD_DIALOG_FLAG_STARTED) == 0) {
            gMemCard_wDialogFlags = f | MEM_CARD_DIALOG_FLAG_STARTED;
            MemCardStart();
            D_8009B3EF = 2;
            func_8003F388();
            *(s16 *)(D_8009B3D8 + 0x60) = -0x400;
            return;
        }
        if (func_8003F2B0_int(
                (ObjectState *)D_8009B3D8, 0x20, 0x50, -1
            ) == 0) {
            gMemCard_wDialogFlags =
                gMemCard_wDialogFlags | MEM_CARD_DIALOG_FLAG_OPENED;
        }
        return;
    }
    if ((f & MEM_CARD_DIALOG_FLAG_IO_PENDING) != 0) {
        c = MemCardSync(
            1, (long *)&D_8009B3F0, (long *)&D_8009B3F4
        );
        D_8009B3BC = c;
        if (c != 1) {
            return;
        }
        gMemCard_wDialogFlags =
            gMemCard_wDialogFlags & ~MEM_CARD_DIALOG_FLAG_IO_PENDING;
    }
    D_80090F9C[D_8009B3DE]();
    if (gMemCard_wDialogFlags != 0) {
        return;
    }
b25:
    gMemCard_wDialogFlags =
        gMemCard_wDialogFlags | MEM_CARD_DIALOG_FLAG_CLOSING;
    *(s16 *)(D_8009B3D8 + 0x60) = 0x400;
    MemCardStop();
}
