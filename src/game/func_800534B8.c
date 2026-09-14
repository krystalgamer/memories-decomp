#include "../types.h"
#define MODEL_HANDLER_DIAGNOSTICS_AS_ARRAY
#include "model_handler_state.h"
#include "model_debug_controller.h"
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#define GINPUT_PAD2_PRESSED_IN_DATA
#define GINPUT_PAD1_HELD_IN_DATA
#define GINPUT_PAD1_REPEAT_IN_DATA_VOLATILE
#include "input.h"
#include "model.h"
#include "model_load_step.h"
#include "model_graphics_state.h"
#include "camera_view.h"
#include "model_scene_setup.h"
#include "model_state_setters.h"
#include "func_80059AA8.h"
#include "func_8005922C.h"
#include "file_transfer.h"
#include "../psyq/rand.h"

u32 D_8009B004 = 0;

s32 func_800534B8(void)
{
    s32 reload = 0;
    char *normal = D_8009B00C;
    char *selected = D_8009B014;
    ModelSlot *slots;

    if (D_8009AF9A == -2)
        return 1;
    if (D_8009AF9A >= 40)
        return 0;
    if ((gInput_wPad1Pressed & 0x20) || (gInput_wPad2Pressed & 0x20)) {
        D_8009AF9A = 40;
        return 0;
    }
    slots = D_800F2C40;
    if (!slots[2].field_E1F) {
        func_80059AEC(1);
        FntPrint(D_80011518, slots[2].field_DF8, slots[2].field_E14);
        if (slots[2].field_E14 == 255) {
            if (!((D_8009B0F4_abs & 0x2000030) | D_8009B134_abs))
                Model_LoadMonsterMerge(2, -1, -1, -1, -1, -1, 0);
        } else {
            func_80056828(2);
        }
        if (D_800F2C40[2].field_E1F) {
            ModelSlot *stage = D_800F2C40;
            s32 height;

            stage += 2;
            *(u16 *)((u8 *)&D_8009B004 + 2) = *(u16 *)(D_8009AF88 + 0xA4);
            height = *(s16 *)((u8 *)&D_8009B004 + 2);
            if (stage->field_D18) {
                stage->field_D18->rot.vx = 0;
                stage->field_D18->rot.vy = 0;
                stage->field_D18->rot.vz = 0;
                stage->field_D18->matrix.t[0] = 0;
                stage->field_D18->matrix.t[1] = height;
                stage->field_D18->matrix.t[2] = 0;
            }
            func_8005922C(stage->field_D18, 0);
            D_8009AF9A = 0;
        }
    } else if (!slots[0].field_E1F) {
        func_80059AEC(1);
        FntPrint(D_8001152C, slots[0].field_DF8, slots[0].field_E14);
        if (slots[0].field_E14 == 255) {
            if (!((D_8009B0F4_abs & 0x2000030) | D_8009B134_abs))
                Model_LoadMonsterMerge(0, -1, -1, -1, -1, -1, 0);
        } else {
            func_80056828(0);
        }
        if (D_800F2C40[0].field_E1F)
            func_80059AA8(0, ((D_8009AF88[0xA1] >> 1) ^ 1) & 1);
    } else if (!slots[1].field_E1F) {
        func_80059AEC(1);
        FntPrint(D_80011540, slots[1].field_DF8, slots[1].field_E14);
        if (slots[1].field_E14 == 255) {
            if (!((D_8009B0F4_abs & 0x2000030) | D_8009B134_abs))
                Model_LoadMonsterMerge(1, -1, -1, -1, -1, -1, 0);
        } else {
            func_80056828(1);
        }
        if (D_800F2C40[1].field_E1F)
            func_80059AA8(1, ((D_8009AF88[0xA1] >> 1) ^ 1) & 1);
    }
    if ((gInput_wPad1Held & 0x100) && (gInput_wPad2Pressed & 0x100))
        ((u8 *)&D_8009B008)[0] ^= 1;
    if (!((u8 *)&D_8009B008)[0])
        return 0;
    if (D_8009AF9A == -2) {
        s32 i = 0;
        s32 excluded;
        s32 value;

        while (1) {
            do {
                value = (rand() >> 8) % 722;
                excluded = 720;
            } while ((u32)value >= 722);
            if ((u32)(value - 300) >= 50 && (u32)(value - 650) >= 50 && value != excluded) {
                D_8009B488[i] = value;
                if ((u16)value != D_800F2C40[i].field_DF8) {
                    i++;
                    if (i >= 2)
                        break;
                }
            }
        }
        reload++;
        D_8009B488[2] = (rand() >> 8) % 7;
    }
    if (gInput_wPad1Pressed & 0x10)
        reload++;
    if (gInput_wPad1Repeat & 0x900) {
        if (gInput_wPad1Repeat & 0x100)
            D_8009B488[2] = ((u16)D_8009B488[2] + 6) % 7;
        else
            D_8009B488[2] = ((u16)D_8009B488[2] + 8) % 7;
    }
    if (gInput_wPad1Repeat & 0xA000) {
        if (gInput_wPad1Repeat & 0x8000)
            D_8009AFA1 = (D_8009AFA1 + 5) % 6;
        else
            D_8009AFA1 = (D_8009AFA1 + 7) % 6;
    }
    if (gInput_wPad1Held & 0x80) {
        if (gInput_wPad1Repeat & 0x5000) {
            if (gInput_wPad1Repeat & 0x1000)
                ((u16 *)&D_8009B004)[1] -= 10;
            else
                ((u16 *)&D_8009B004)[1] += 10;
            if (D_800F2C40[2].field_E1F) {
                ModelSlot *stage = D_800F2C40;
                s32 height;

                stage += 2;
                height = ((s16 *)&D_8009B004)[1];
                if (stage->field_D18) {
                    stage->field_D18->rot.vx = 0;
                    stage->field_D18->rot.vy = 0;
                    stage->field_D18->rot.vz = 0;
                    stage->field_D18->matrix.t[0] = 0;
                    stage->field_D18->matrix.t[1] = height;
                    stage->field_D18->matrix.t[2] = 0;
                }
                func_8005922C(stage->field_D18, 0);
            }
        }
    } else if (gInput_wPad1Repeat & 0x5000) {
        s32 side = D_8009AFA1 >= 3u;
        u16 *number = (u16 *)&D_8009B488[side];
        s32 digits[3] = {
            (u16)((u16)(*number / 100u) % 10u),
            (u16)((u16)(*number / 10u) % 10u),
            (u16)(*number % 10u)
        };

        if (gInput_wPad1Repeat & 0x1000) {
            digits[(u8)(D_8009AFA1 % 3u)] =
                (digits[(u8)(D_8009AFA1 % 3u)] + 11) % 10;
        } else {
            digits[(u8)(D_8009AFA1 % 3u)] =
                (digits[(u8)(D_8009AFA1 % 3u)] + 9) % 10;
        }
        D_8009B488[side] = 100 * digits[0] + 10 * digits[1] + digits[2];
    }
    if (gInput_wPad1Pressed & 4)
        D_8009B48E[0] ^= 1;
    if (gInput_wPad1Pressed & 1)
        D_8009B490[0] ^= 1;
    if (gInput_wPad1Pressed & 8)
        D_8009B48E[1] ^= 1;
    if (gInput_wPad1Pressed & 2)
        D_8009B490[1] ^= 1;
    FntPrint(D_8009B01C, D_8009AFA1 == 0 ?
        selected : normal, (u16)((u16)((u16)D_8009B488[0] / 100u) % 10u));
    FntPrint(D_8009B01C, D_8009AFA1 == 1 ?
        selected : normal, (u16)((u16)((u16)D_8009B488[0] / 10u) % 10u));
    FntPrint(D_8009B01C, D_8009AFA1 == 2 ?
        selected : normal, (u16)((u16)D_8009B488[0] % 10u));
    FntPrint(normal);
    FntPrint(D_80011554, D_8009B48E[0], D_8009B490[0]);
    FntPrint(D_8009B024);
    FntPrint(D_8009B01C, D_8009AFA1 == 3 ?
        selected : normal, (u16)((u16)((u16)D_8009B488[1] / 100u) % 10u));
    FntPrint(D_8009B01C, D_8009AFA1 == 4 ?
        selected : normal, (u16)((u16)((u16)D_8009B488[1] / 10u) % 10u));
    FntPrint(D_8009B01C, D_8009AFA1 == 5 ?
        selected : normal, (u16)((u16)D_8009B488[1] % 10u));
    FntPrint(normal);
    FntPrint(D_80011554, D_8009B48E[1], D_8009B490[1]);
    FntPrint(D_8009B02C);
    FntPrint(D_80011564, (u16)D_8009B488[2], ((s16 *)&D_8009B004)[1]);
    if (reload) {
        File_WaitForTransfers();
        func_800533D8();
        Model_SetSlotProperties(0, (u16)D_8009B488[0], -1, -1,
            D_8009B48E[0], D_8009B490[0]);
        Model_SetSlotProperties(1, (u16)D_8009B488[1], -1, -1,
            D_8009B48E[1], D_8009B490[1]);
        Model_SetSlotProperties(2, (u16)D_8009B488[2]);
    }
    return 0;
}
