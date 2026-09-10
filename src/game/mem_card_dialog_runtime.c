#include "../types.h"
#include "display_object.h"
#include "func_80043178.h"
#include "display_object_interpolation.h"
#include "display_object_lifecycle.h"
#include "display_object_api.h"
#include "../psyq/libmcrd.h"
#include "duel_effect.h"
#include "mem_card.h"
#include "save_data.h"
#include "file_transfer.h"
#include "text_box_lifecycle.h"
#include "display_object_helpers.h"
#include "text_box_runtime.h"
#include "func_80039794.h"
#include "../unmatched.h"
#include "duel_effect_mark_object_if_active.h"
#include "mem_card_dialog_steps.h"
#include "mem_card_dialog_runtime.h"

/* The memory-card save operation followed by the modal runtime that drives
   the operation table. func_8003F454 dispatches func_8003EED0 through
   D_80090F9C, and both sides share the dialog flags, result words, active
   channel, and request outcome. */

extern u32 D_8009B3E0;

void func_8003EED0(void)
{
    s32 files;
    u8 *record;

    if ((D_8009B3C1 & 0x80) == 0) {
        D_8009B3C1 |= 0x80;
        func_8003E46C(0xC0, 0);
        D_8009B3EB = 0;
    }
    switch (D_8009B3EB & 0xF) {
    case 0:
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            D_801D5648[0] = (D_8009B3F9 >> 4) + 1;
            do {
            } while (MemCardAccept(D_8009B3F9) == 0);
            goto io_pending;
        }
        switch (D_8009B3F4) {
        case 1:
            func_8003E46C(0xC1, 0x18);
            break;
        case 0:
        case 3:
            D_8009B3EB = 1;
            D_8009B3F9 ^= 0x10;
            if (D_8009B3F9 != 0) {
                D_8009B3EB = 0;
            }
            break;
        case 2:
            func_8003E46C(0xDA, 0x18);
            break;
        case 4:
            func_8003E46C(0xC3, 0x18);
            break;
        }
        break;
    case 1:
        D_801D5648[0] = (D_8009B3F9 >> 4) + 1;
        if (MemCardGetDirentry(D_8009B3F9, (char *)D_800EFE18,
                               (struct DIRENTRY *)D_800EFBC0, (long *)&files, 0,
                               MEM_CARD_BLOCK_COUNT) != 0) {
            func_8003E46C(0xDA, 0x18);
            break;
        }
        if (files == 0) {
            func_8003E46C(0xC3, 0x18);
            break;
        }
        D_8009B3F9 ^= 0x10;
        if (D_8009B3F9 != 0) {
            D_8009B3EB = 1;
            break;
        }
        D_8009B3EB = 2;
        /* fallthrough */
    case 2:
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
            D_8009B3EC = 0;
            D_801D5648[0] = (D_8009B3F9 >> 4) + 1;
            MemCardReadFile(D_8009B3F9, (char *)D_800EFE18,
                            (unsigned long *)gLibrary_aCardArtRecord,
                            D_8009B3C4, 0x480);
            goto io_pending;
        }
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            if (D_8009B3F4 != 0) {
                func_8003E46C(0xDA, 0x18);
                break;
            }
        }
        record = (u8 *)D_8009B3D0;
        if (D_8009B3F9 != 0) {
            record = (u8 *)D_8009B3E0;
        }
        if (SaveData_HasSameDuelistCode(
                (SaveDataState *)record,
                (SaveDataState *)gLibrary_aCardArtRecord) == 0) {
            if (D_8009B3EC != 0) {
                func_8003E46C(0xC3, 0x18);
                break;
            }
            D_8009B3EC++;
            D_8009B3EB &= ~MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            MemCardReadFile(D_8009B3F9, (char *)D_800EFE18,
                            (unsigned long *)gLibrary_aCardArtRecord,
                            D_8009B3C4 + SAVE_DATA_STATE_SIZE, 0x480);
            goto io_pending;
        }
        D_8009B3F9 ^= 0x10;
        if (D_8009B3F9 != 0) {
            D_8009B3EB = 2;
            break;
        }
        D_8009B3EB = 3;
        /* fallthrough */
    case 3:
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_READY) == 0) {
            D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_READY
                          | MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
            func_8003E46C(0xC4, 0);
        }
        if ((D_8009B3EB & MEM_CARD_DIALOG_FLAG_RESULT_CREATED) == 0) {
            goto step;
        }
    write:
        D_8009B3EB &= ~MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
        if (D_8009B3F9 != 0) {
            MemCardWriteFile(D_8009B3F9, (char *)D_800EFE18,
                             (unsigned long *)D_8009B3E0, D_8009B3C4, 0x80);
        } else {
            MemCardWriteFile(D_8009B3F9, (char *)D_800EFE18,
                             (unsigned long *)D_8009B3D0, D_8009B3C4, 0x80);
        }
    io_pending:
        gMemCard_wDialogFlags |= MEM_CARD_DIALOG_FLAG_IO_PENDING;
        break;
    step:
        if (D_8009B3F4 != 0) {
            func_8003E46C(0xC5, 0x18);
            break;
        }
        D_8009B3EB |= MEM_CARD_DIALOG_FLAG_RESULT_CREATED;
        D_8009B3F9 ^= 0x10;
        if (D_8009B3F9 == 0) {
            D_8009B3D0 += 0x80;
            D_8009B3E0 += 0x80;
            D_8009B3C4 += 0x80;
            D_8009B3C2 -= 0x80;
            if (D_8009B3C2 == 0) {
                D_8009B3EF = 1;
                func_8003E46C(0xC6, 0x18);
                break;
            }
        }
        goto write;
    }
}

s32 func_8003F2B0(DisplayObject *object, s32 arg1, s32 arg2, s32 index)
{
    s32 saved_index = index;
    s32 value;

    if (func_80042B98((DisplayObjectLifecycle *)object) == 0) {
        func_80043178((DisplayObjectSnapshot *)object);
    }

    value = object->field_60;
    if (value < 0) {
        value += 0x40;
        if (value >= 0) {
            object->field_6C = 0;
            value = 0;
        }
    } else {
        value -= 0x40;
        if (value <= 0) {
            object->field_6C = 0;
            value = 0;
        }
    }
    object->field_60 = value;

    func_80043230((DisplayObjectPosition *)object, arg1, arg2, value);

    if (saved_index >= 0) {
        TextBox_SetPos(
            (u8 *)D_800EB0F8 + saved_index * 100,
            (s16)object->field_30.h.field_30,
            (s16)object->field_30.h.field_32);
    }

    return object->field_6C;
}

extern u8 D_800EB0F8_raw[] asm("D_800EB0F8");

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
    func_80042918((DisplayObject *)o);
    func_800428EC(o, 0xF);
    D_8009B3D8 = o;
}

void func_8003F454(void)
{
    DuelEffectChannel *p;
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
        if (func_8003F2B0(
                (DisplayObject *)D_8009B3D8, 0x20, 0x100, D_8009B3EE
            ) == 0) {
            TextBox_Destroy(D_800EB0F8_raw + D_8009B3EE * 100);
            func_8004036C(D_8009B3D8);
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
            DuelEffect_MarkObjectIfActive((MenuRecord *)p);
            p->field_59 = 0x10;
            if ((gMemCard_wDialogFlags & 0x20) != 0) {
                do {
                    func_80039794();
                } while (p->field_30 == 0);
                return;
            }
            if ((gMemCard_wDialogFlags & 0x10) != 0) {
                p->flags_34 = p->flags_34 | 0x1008;
                return;
            }
            func_80039A14((u8 *)p);
            goto b14;
        }
        func_80039794();
        p = (DuelEffectChannel *)(D_800EB0F8_raw + D_8009B3EE * 100);
        if ((*(s32 *)&p->flags_34 & 0x2008) != 0x2000) {
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
        if (func_8003F2B0(
                (DisplayObject *)D_8009B3D8, 0x20, 0x50, -1
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
