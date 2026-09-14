/*
 * Scene mode 19's intro controller. The control-module entry points and
 * eight-byte view offset are captured before the frame-step setter; the
 * signed phase, module data arguments and model fields remain live.
 *
 * The full reconstruction matches all 1,880 text bytes and the 17-entry
 * signed-phase table with the uniform GCC 2.8.1 G8 split-address profile.
 * The byte block-move view preserves the original unaligned eight-byte copy.
 * The timing callback is sequenced before the final live scale-byte read.
 */
#include "../types.h"
#include "../psyq/libgte.h"
#include "model.h"
#include "model_load_step.h"
#include "model_graphics_state.h"
#include "model_slot_properties.h"
#include "model_slot_state_updates.h"
#include "model_state_setters.h"
#include "model_transfer_state.h"
#include "model_effect_state.h"
#include "func_80058E1C.h"
#include "func_80057AF4.h"
#include "func_8005D994.h"
#include "file_transfer.h"
#include "high_memory_addresses.h"
#include "fade.h"
#include "sound.h"
#include "sound_output.h"
#include "../unmatched.h"

#define MODEL_HANDLER_OFFSET_ABSOLUTE
#include "model_handler_state.h"
#include "model_scene_states.h"

void func_8004FE2C(void)
{
    SVECTOR offset;
    ModelSlot *slot;
    s32 (*first)(s32, s32);
    s32 (*second)(s32, s32);

    *(ModelBytes8 *)&offset = *(ModelBytes8 *)D_8009AFFC;
    slot = D_800F2C40;
    first = (s32 (*)(s32, s32))(D_80010014 + 4);
    second = (s32 (*)(s32, s32))(D_80010018 + 4);
    func_80059AEC(1);
    if ((u32)((u8)D_8009AF9A - 8) < 2 && slot->field_E15 == 0) {
        D_8009AFA0 = 0;
        if (first(D_80010024, -1) == 2) {
            slot->field_E15++;
        }
    }
    if (D_8009AF9A >= 10 && D_800F2C40[1].field_E15 == 0) {
        D_8009AFA0 = 0;
        if (second(D_80010028, -1) == 2) {
            func_80059700(0, 0);
            func_800156DC();
            D_8009AF9A = 15;
        }
    }
    switch ((s8)(D_8009AF9A + 1)) {
    case 0: {
        ModelSlot *current;
        if (D_800F2C40[0].field_E14 == 0xFF) {
            if ((D_8009B0F4_abs & 0x2000030) | D_8009B134_abs) {
                goto check_ready;
            }
            Model_LoadMonsterMerge(0, 0x309, 0, 0, 0, 0, 4);
        } else {
            func_80056828(0);
        }
check_ready:
        current = D_800F2C40;
        if (!current->field_E1F) {
            break;
        }
        func_8005F3B8(0, 10000, 0xE00, 0, 0);
        func_800597C8(0, 1, 0);
        if (slot->field_E0F) {
            func_80057AF4(0, 0, 0);
        }
        func_80059700(0, 1);
        func_80059590(0, 5, 0, 0, 0);
        slot->field_BF6 = 1;
        slot->field_BF4 = 2;
        func_80047314(0x7310);
        *(s32 *)&current->field_CF8.field_0C[2] = -1;
        *(s32 *)&current->field_CF8.field_0C[4] = -1;
        *(s32 *)&current->field_CF8.field_0C[6] = -1;
        D_8009AFA0 = 0;
        first(D_80010024, 0);
        second(D_80010028, 0);
        current[0].field_E15 = 0;
        current[1].field_E15 = 0;
        D_8009AF9A++;
        break;
    }
    case 1:
        if (func_8004703C() & 0x80) {
            offset.vy = -2000;
            func_800597C8(0, 0, 10);
            func_8005D994(0, 2000, 0x100, 0x100, (u8 *)&offset, 110);
            D_8009AF9A++;
        }
        break;
    case 2: {
        ModelSlot *current = D_800F2C40;
        if ((s8)current->field_DC0[0] >= 0) {
            current->field_DC0[0] += 2;
            current->field_DC0[1] += 2;
            current->field_DC0[2] += 2;
        } else {
            func_80059590(0, 0, 0x80, 0x80, 0x80);
            D_8009AF9A++;
        }
        break;
    }
    case 3:
        if (func_8005FB08() && slot->field_E06 >= 0x780) {
            func_800597C8(0, 0, 0);
            D_8009AF9A++;
        }
        break;
    case 4:
        if (slot->field_E06 >= 0x780) {
            func_800597C8(0, 0, 0);
            D_8009AF9A++;
        }
        break;
    case 5:
        if (slot->field_E06 >= 0x1E0) {
            func_800597C8(0, 0, 0);
            offset.vx = -2500;
            offset.vy = -1500;
            offset.vz = 1000;
            func_8005F3B8(0, 3000, -0x100, -0x100, &offset);
            D_8009AF9A++;
        }
        break;
    case 6:
        if (slot->field_E06 >= 0x1E0) {
            func_800597C8(0, 0, 0);
            offset.vx = 2500;
            offset.vy = -1500;
            offset.vz = 1000;
            func_8005F3B8(0, 3000, 0x100, -0x100, &offset);
            D_8009AF9A++;
        }
        break;
    case 7:
        if (slot->field_E06 >= 0x1E0) {
            func_800597C8(0, 0, 0);
            offset.vx = -500;
            offset.vy = 1500;
            offset.vz = 1000;
            func_8005F3B8(0, 3000, -0x200, -0x100, &offset);
            D_8009AF9A++;
        }
        break;
    case 8:
        if (slot->field_E06 >= 0x1E0) {
            func_800597C8(0, 0, 0);
            offset.vx = 500;
            offset.vy = 1500;
            offset.vz = 1000;
            func_8005F3B8(0, 3000, 0x200, -0x100, &offset);
            D_8009AF9A++;
        }
        break;
    case 9:
        if (slot->field_E06 >= 0x1E0) {
            offset.vy = -2000;
            func_8005F3B8(0, 3500, 0x40, 0x200, &offset);
            D_8009AF9A++;
        }
        break;
    case 10:
        if (slot->field_BF5 == 2) {
            D_8009AF9A++;
        }
        break;
    case 11:
        if (slot->field_E06 >= 0x8C0) {
            offset.vy = -1500;
            func_8005D994(0, 3500, 0, -0x80, (u8 *)&offset, 40);
            D_8009AF9A++;
        }
        break;
    case 12:
        if (slot->field_E06 >= 0xE60) {
            offset.vz = -1000;
            func_8005F3B8(0, 5000, 0x200, -0x100, &offset);
            func_800597C8(0, 0, 190);
            D_8009AF9A++;
        }
        break;
    case 13:
        if (slot->field_E06 >= 0xE60) {
            offset.vz = -1000;
            func_8005F3B8(0, 5000, -0x200, -0x100, &offset);
            func_800597C8(0, 0, 190);
            D_8009AF9A++;
        }
        break;
    case 14:
        if (slot->field_E06 >= 0xE60) {
            offset.vy = -1000;
            func_8005F3B8(0, 3000, 0, -0x80, &offset);
            func_800597C8(0, 0, 190);
            D_8009AF9A++;
        }
        break;
    case 15: {
        s32 remaining = slot->field_750[slot->field_BF5].max << 4;
        s32 timing = func_80058E1C();
        remaining -= slot->field_E0D * timing;
        if (slot->field_E06 >= remaining) {
            func_80059700(0, 0);
        }
        break;
    }
    case 16:
        D_8009AF9A = -2;
        break;
    }
}
