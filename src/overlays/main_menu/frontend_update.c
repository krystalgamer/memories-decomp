/* Each input branch samples the pressed-pad word independently. */
#define GINPUT_PAD1_PRESSED_IS_VOLATILE
#include "../../types.h"
#include "../../game/two_player_save_setup.h"
#include "../../game/save_data.h"
#include "../../game/save_data_update_trade_load.h"
#include "../../unmatched.h"
#include "../../game/input.h"
#include "../../game/display_object.h"
#include "../../game/display_object_layout.h"
#include "../../psyq/libgte.h"
#include "../../overlays/main_menu/frontend.h"
#include "../../game/display_object_helpers.h"
#include "../../game/main_services.h"
#include "../../game/display_object_config.h"
#include "../../game/sound_output.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_constants.h"
#include "../../game/data_transfer_request.h"
#include "../../game/mem_card.h"
#include "../../overlays/main_menu/ordering_tables.h"
#include "../../game/sound.h"
#include "../../game/graphics_frame.h"

s32 MainMenu_UpdateFrontendMenu(void)
{
    DisplayObject *ent3;
    DisplayObject *entry;
    DisplayObject *ent6;
    u8 **slot;
    u8 **slot2;
    s32 step;
    s32 level;
    s16 timer;
    s32 value;
    s32 frame;
    s32 first;
    s32 product;
    s32 delta;
    s32 moved;
    s32 i;
    s32 base;
    s32 count;
    s32 lvl;
    s32 vx3;
    s32 acc;
    s32 neg;
    s32 chr;
    DisplayObject *ent5;
    s32 poll;
    DisplayObject *ent2;
    DisplayObject *eloop;

    if (D_8018459B != 0) {
        poll = SaveData_PollLoad();
        if (poll != 0) {
            if (poll == 1) {
                Input_ResetPads();
                MainMenu_StartFrontendEntryTransition(1);
                D_8018459B = 0;
            } else {
                Input_ResetPads();
                D_8018459B = 0;
            }
        }
        goto ret_m1;
    }

    if (D_8018459C != 0) {
        poll = SaveData_UpdateTradeLoad();
        if (poll != 0) {
            if (poll == 1) {
                Input_ResetPads();
                MainMenu_StartFrontendEntryTransition(1);
                D_8018459C = 0;
            } else {
                Input_ResetPads();
                D_8018459C = 0;
            }
        }
        goto ret_m1;
    }

    if (D_8018459D != 0) {
        poll = SaveData_UpdateDuelLoad();
        if (poll != 0) {
            if (poll == 1) {
                Input_ResetPads();
                MainMenu_StartFrontendEntryTransition(1);
                D_8018459D = 0;
            } else {
                Input_ResetPads();
                D_8018459D = 0;
            }
        }
        goto ret_m1;
    }

    if (D_8018459A != 0) {
        if (MemCardDialog_Poll() == 0) {
            goto ret_m1;
        }
        Input_ResetPads();
        D_8018459A = 0;
        goto ret_m1;
    }

    step = D_80184598;
    if (step != 0) {
        level = D_80184597 + (step << 3);
        D_80184597 = level;
        if (step > 0) {
            if ((s8)level < 0) {
                goto fade_done;
            }
        }
        if (step >= 0) {
            goto ret_m1;
        }
        if ((u8)level != 0) {
            goto ret_m1;
        }
    fade_done:
        if (D_80184598 < 0) {
            ent2 = D_80184560;
            ((u8 *)&ent2->field_0C)[2] = 0x80;
            ((u8 *)&ent2->field_0C)[1] = 0x80;
            ((u8 *)&ent2->field_0C)[0] = 0x80;
            ent2->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
            D_80184560->field_6C = 0x3C;
            D_80184560->field_34.h.field_36 = 0;
        }
        D_80184598 = 0;
        goto ret_m1;
    }

    entry = D_80184560;
    if (entry != 0 &&
        (entry->flags & DISPLAY_OBJECT_FLAG_RENDERABLE) != 0) {
        if (entry->field_6C != 0) {
            entry->field_6C = entry->field_6C - 1;
        } else {
            lvl = ((u8 *)&entry->field_0C)[2] + (u8)entry->field_60;
            ((u8 *)&entry->field_0C)[2] = lvl;
            ((u8 *)&entry->field_0C)[1] = lvl;
            ((u8 *)&entry->field_0C)[0] = lvl;
            entry = D_80184560;
            chr = ((u8 *)&entry->field_0C)[0];
            if ((u32)(chr - 0x41) >= 0x3F) {
                if ((s8)chr < 0) {
                    entry->field_6C = 0x3C;
                }
                ent5 = D_80184560;
                neg = ent5->field_60;
                ent5->field_60 = -neg;
            }
        }
        if ((gInput_wPad1Pressed & PAD_BUTTON_START) != 0) {
            SD_SEPlay(7, 0xFF, 0);
            ent3 = D_80184560;
            ent3->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
            MainMenu_StartFrontendEntryTransition(0);
            D_80184598 = 1;
            goto ret_m1;
        }
        ent6 = D_80184560;
        acc = (u16)ent6->field_34.h.field_36 + (u16)D_8009B0D8;
        ent6->field_34.h.field_36 = acc;
        if ((s16)acc >= 0xBB8) {
            return -2;
        }
        return -1;
    }

    if (D_80184599 != 0) {
        moved = 0;
        i = 0;
        slot = gMain_apMenuEntries;
    entry_loop:
        eloop = (DisplayObject *)*slot;
        if (eloop == 0) {
            goto next_entry;
        }
        timer = eloop->field_60;
        step = (u16)eloop->field_60;
        if (timer <= 0) {
            goto next_entry;
        }
        timer = step - 1;
        eloop->field_60 = timer;
        first = (u32)gMain_bMenuID < 5;
        if (first) {
            if (i >= 5) {
                goto hide_entry;
            }
        } else {
            count = i;
            if (count < 5) {
                goto hide_entry;
            }
        }
        eloop = (DisplayObject *)*slot;
        first = eloop->field_38.h.field_38;
        value = eloop->field_34.h.field_36;
        delta = first - value;
        value = eloop->field_60;
        /* Keep the selector/endpoint scratch live as the phase limit. */
        first = 0x10;
        frame = first - value;
        value = (u16)eloop->field_38.h.field_38;
        if (frame != first) {
            product = rsin(frame << 6) * delta;
            eloop = (DisplayObject *)*slot;
            /* In-place truncation avoids the old compiler's quotient copy. */
            if (product < 0) {
                product += 0xFFF;
            }
            product >>= 12;
            value = (u16)eloop->field_34.h.field_36 + product;
        }
        *(volatile s16 *)&eloop->field_30.h.field_30 = value;
        if ((frame & 1) != 0) {
            MainMenu_SpawnFrontendEntryAfterimage(*slot);
        }
        ((DisplayObject *)*slot)->flags =
            ((DisplayObject *)*slot)->flags | DISPLAY_OBJECT_FLAG_RENDERABLE;
        goto tick_entry;
    hide_entry:
        ((DisplayObject *)*slot)->flags =
            ((DisplayObject *)*slot)->flags & ~DISPLAY_OBJECT_FLAG_RENDERABLE;
    tick_entry:
        moved++;
        func_80040410((DisplayObjectConfig *)*slot, (i << 1) | (gMain_bMenuID != i));
    next_entry:
        i++;
        slot++;
        if (i < 0xB) {
            goto entry_loop;
        }
        if (moved != 0) {
            return -1;
        }
        vx3 = D_80184596;
        D_80184599 = 0;
        if (vx3 == 0) {
            goto ret_m1;
        }
        if (D_80184595 != 0) {
            if ((u32)gMain_bMenuID < 5) {
                i = 0;
                slot2 = gMain_apMenuEntries;
            /* Sharing i and omitting structured-loop notes preserves its
               allocation without rotating the first loop's saved registers. */
            clear_loop:
                ent3 = (DisplayObject *)*slot2;
                if (ent3 != 0) {
                    ent3->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
                }
                slot2++;
                i++;
                if (i < 0xB) {
                    goto clear_loop;
                }
                D_80184598 = -1;
            } else {
                MainMenu_StartFrontendEntryTransition(0);
                gMain_bMenuID = 1;
            }
            D_80184595 = 0;
            return -1;
        }
        if (gMain_bMenuID != 1) {
            return gMain_bMenuID;
        }
        MainMenu_StartFrontendEntryTransition(0);
        gMain_bMenuID = 5;
        return -1;
    }

    if ((gInput_wPad1Repeat & PAD_DIRECTION_VERTICAL_MASK) != 0) {
        if ((u32)gMain_bMenuID >= 5) {
            base = 5;
        } else {
            base = 0;
        }
        if ((u32)gMain_bMenuID < 5) {
            count = 5;
        } else {
            count = 6;
        }
        func_80040410((DisplayObjectConfig *)gMain_apMenuEntries[gMain_bMenuID], (gMain_bMenuID << 1) | 1);
        /* Keep the store in each arm: the join controls high-half reuse. */
        if ((gInput_wPad1Repeat & PAD_DIRECTION_UP) != 0) {
            *(volatile u8 *)&gMain_bMenuID = (gMain_bMenuID - base + count - 1) % count + base;
        } else {
            *(volatile u8 *)&gMain_bMenuID = (gMain_bMenuID - base + count + 1) % count + base;
        }
        func_80040410((DisplayObjectConfig *)gMain_apMenuEntries[gMain_bMenuID], gMain_bMenuID << 1);
        SD_SEPlay(6, 0xFF, 0);
        goto ret_m1;
    }

    if ((gInput_wPad1Pressed & (PAD_BUTTON_START | PAD_BUTTON_CANCEL | PAD_BUTTON_CONFIRM_MASK)) == 0) {
        return -1;
    }
    if ((gInput_wPad1Pressed & PAD_BUTTON_CANCEL) != 0) {
        if ((u32)gMain_bMenuID < 5) {
            SD_SEPlay(9, 0xFF, 0);
            return -1;
        }
        SD_SEPlay(8, 0xFF, 0);
        D_80184595 = 1;
    } else {
        SD_SEPlay(7, 0xFF, 0);
        switch (gMain_bMenuID) {
        case 1:
            SaveData_RequestLoad();
            D_8018459B = D_8018459B + 1;
            return -1;
        case 3:
            D_8009B3ED = 0;
            D_8009B3EA = 0;
            D_8018459C = D_8018459C + 1;
            return -1;
        case 2:
            D_8009B3ED = 0;
            D_8009B3EA = 0;
            D_8018459D = D_8018459D + 1;
            return -1;
        case 0xA:
            SaveData_RequestWrite();
            D_8018459A = D_8018459A + 1;
            return -1;
        }
    }
    MainMenu_StartFrontendEntryTransition(1);
ret_m1:
    return -1;
}
