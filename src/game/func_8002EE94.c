#include "../types.h"
#include "sound.h"

#include "duel_effect.h"
#include "text_box_lifecycle.h"

extern u8 *D_8009B290;
extern u16 D_8009B27C;
extern u16 D_8009B2A4[2];
extern u8 D_8009B27A;
extern u8 D_8009B268 __attribute__((section(".data")));
extern u8 D_8009B269 __attribute__((section(".data")));
extern u8 D_8009B26C __attribute__((section(".data")));
extern u8 D_8009B26D __attribute__((section(".data")));
extern u8 D_8009B34C __attribute__((section(".data")));
extern s8 gDialog_bChoice __attribute__((section(".data")));
extern s8 gDialog_bChoiceCount __attribute__((section(".data")));
extern volatile u16 gInput_wPad1Pressed __attribute__((section(".data")));
extern s16 D_801D0000[];

extern s32 func_8002E3B4(void);
extern s32 func_8002EE5C(void);
extern s32 func_8003F70C(void);
extern s32 Dialog_ReadChoiceInput(DuelEffectChannel *);
extern s32 DuelEffect_HasActiveEntry(DuelEffectChannel *);
extern void DuelEffect_MarkObjectIfActive(void *);
extern void SaveData_RequestWrite(void);
extern void func_80033C90(void);
extern void func_80039794(void);
extern void func_8003B6AC(s32, s32);
extern void func_80043178(u8 *);
extern void func_80043230(u8 *, s32, s32, s32);
extern void TextBox_SetPos(DuelEffectChannel *, s32, s32);

void func_8002EE94(void)
{
    DuelEffectChannel *box;
    register u8 *obj __asm__("$17");
    u8 *p;
    u8 *p2;
    u8 *slot;
    DuelEffectChannel *chan;
    DuelEffectChannel *prompt;
    s32 id;
    register s32 lo __asm__("$8");
    s32 choice;
    s32 step;
    u16 flags;
    u16 next;

    if (func_8002E3B4() == 0) {
        p = D_8009B290;
        p2 = p + 2;
        gDialog_bChoice = -1;
        D_8009B290 = p2;
        lo = p[0];
        id = lo | (p[1] << 8);
        D_8009B290 = p + 4;
        D_8009B2A4[1] = p[2] | (p2[1] << 8);
        func_8003B6AC(0, 2);
        DuelEffect_MarkObjectIfActive(
            TextBox_Create(0, id, 0x10, 0xB0, 0x120, 0x30));
        return;
    }

    if ((D_8009B27C & 0x80) != 0) {
        if (func_8003F70C() == 0) {
            return;
        }
        gDialog_bChoiceCount = 4;
        gDialog_bChoice = 0;
        D_8009B34C = 0;
        D_8009B27C &= 0xFF7F;
        return;
    }

    func_80039794();
    flags = D_8009B27C;
    if ((flags & 0x4000) == 0) {
        chan = D_800EB0F8;
        if ((chan->flags_34 & 0x2000) == 0) {
            return;
        }
        if (DuelEffect_HasActiveEntry(chan) != 0) {
            return;
        }
        D_8009B27C |= 0x4000;
        if (func_8002EE5C() == 0) {
            D_8009B27C |= 0x200;
            SD_SEPlayFull(0x2A);
            return;
        }
        func_8003B6AC(0, 2);
        box = TextBox_Create(3, 0x11, -0x90, 0x38, 0x78, 0x30);
        DuelEffect_MarkObjectIfActive(box);
        box->flags_34 |= 0x24;
        do {
            func_80039794();
        } while (box->field_30 == 0);
        func_80043178((u8 *)box->field_28);
        slot = (u8 *)box->field_28;
        next = D_8009B27C | 0x6000;
        *(s16 *)(slot + 0x60) = -0x400;
        D_8009B27C = next;
        return;
    }

    box = &D_800EB0F8[3];
    obj = (u8 *)box->field_28;

    if ((flags & 0x400) != 0) {
        if ((flags & 0x800) == 0) {
            D_8009B27C = flags | 0x800;
            func_8003B6AC(0, 2);
            box = TextBox_Create(2, 0x12, 0x90, 0x70, 0x18, 0x18);
            DuelEffect_MarkObjectIfActive(box);
            box->flags_34 |= 0x20;
            do {
                func_80039794();
            } while (box->field_30 == 0);
        }
        prompt = &D_800EB0F8[2];
        if ((prompt->flags_34 & 0x2000) == 0) {
            return;
        }
        D_8009B27C &= 0xF3FF;
        TextBox_Destroy(prompt);
        if (gDialog_bChoice != 0) {
            D_8009B268 = 1;
            D_8009B26D = 5;
            D_8009B26C = 8;
        }
        gDialog_bChoiceCount = 4;
        gDialog_bChoice = 2;
        return;
    }

    if ((flags & 0x200) != 0) {
        if ((flags & 0x800) == 0) {
            D_8009B27C = flags | 0x800;
            func_8003B6AC(0, 2);
            DuelEffect_MarkObjectIfActive(TextBox_CreateFlagged(
                0, 0x1C, 0x10, 0xB0, 0x120, 0x24, 0x1008));
            return;
        }
        if ((D_800EB0F8[0].flags_34 & 8) != 0) {
            return;
        }
        SD_SEPlayFull(8);
        D_8009B268 = 1;
        D_8009B26D = 5;
        D_8009B26C = 8;
        return;
    }

    if ((flags & 0x1000) != 0) {
        if ((flags & 0x800) == 0) {
            D_8009B27C = flags | 0x800;
            func_80043178(obj);
            *(s16 *)(box->field_28 + 0x60) = 0x400;
        }
        step = *(u16 *)(obj + 0x60) - 0x40;
        *(s16 *)(obj + 0x60) = step;
        if ((s16)step <= 0) {
            TextBox_Destroy(box);
            D_8009B27C = 0;
            return;
        }
        func_80043230(obj, -0x90, 0x38, (s16)step);
        TextBox_SetPos(box, *(s16 *)(obj + 0x30), *(s16 *)(obj + 0x32));
        return;
    }

    if ((flags & 0x2000) != 0) {
        step = *(u16 *)(obj + 0x60) + 0x40;
        *(s16 *)(obj + 0x60) = step;
        if ((s16)step >= 0) {
            ((s16 *)obj)[0x18] = 0x10;
            ((s16 *)obj)[0x19] = 0x38;
            D_8009B27C = flags & 0xDFFF;
            TextBox_SetPos(box, ((s16 *)obj)[0x18], ((s16 *)obj)[0x19]);
            return;
        }
        func_80043230(obj, 0x10, 0x38, (s16)step);
        TextBox_SetPos(box, *(s16 *)(obj + 0x30), *(s16 *)(obj + 0x32));
        return;
    }

    if (Dialog_ReadChoiceInput(box) != 0) {
        return;
    }
    if ((gInput_wPad1Pressed & 0xC0) == 0) {
        return;
    }
    D_801D0000[0x3EE] = D_8009B2A4[1];
    choice = gDialog_bChoice;
    switch (choice) {
    case 0:
        SD_SEPlayFull(7);
        D_801D0000[0x3EE] = D_8009B2A4[1];
        SaveData_RequestWrite();
        D_8009B27C |= 0x80;
        break;
    case 1:
        SD_SEPlayFull(7);
        func_80033C90();
        D_8009B269 = 2;
        D_8009B27A = (u8)D_8009B2A4[1];
        break;
    case 2:
        SD_SEPlayFull(7);
        D_8009B27C |= 0x400;
        break;
    case 3:
        SD_SEPlayFull(8);
        D_8009B27C |= 0x1000;
        break;
    }
}
