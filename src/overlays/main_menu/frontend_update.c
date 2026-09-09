#define GINPUT_PAD1_PRESSED_IS_VOLATILE
#include "../../types.h"
#include "../../game/input.h"
#include "../../game/display_object_layout.h"
#include "../../psyq/libgte.h"
#include "entrypoints.h"
#include "frontend.h"
#include "../../game/display_object_config.h"

extern u8 *D_80184560;
extern u8 *gMain_apMenuEntries[];
extern u8 gMain_bMenuID;
extern u8 D_80184595;
extern u8 D_80184596;
extern u8 D_80184597;

extern s8 D_80184598;
extern u8 D_80184599;
extern u8 D_8018459A;
extern u8 D_8018459B;
extern u8 D_8018459C;
extern u8 D_8018459D;
extern u16 D_8009B0D8;
extern u8 D_8009B3EA;
extern u8 D_8009B3ED;

extern s32 SaveData_PollLoad(void);
extern void SaveData_RequestLoad(void);
extern s32 func_8003FCD8(void);
extern s32 func_8003FD14(void);
extern s32 func_8003F70C(void);
extern void func_8003F87C(void);
extern void SD_SEPlay(s32, s32, s32);

typedef struct { s16 h; } H16s;

s32 MainMenu_UpdateFrontendMenu(void)
{
    u8 *ent3;
    u8 *entry;
    u8 *ent6;
    u8 **slot;
    u8 **slot2;
    s32 step;
    s32 level;
    /* These disjoint lifetimes deliberately share the retail argument/result
       registers; ordinary temporaries invert the loads and multiply result. */
    register u32 countdown __asm__("$5");
    register s32 timer __asm__("$2");
    register s32 value __asm__("$2");
    s32 frame;
    register s32 first __asm__("$3");
    register s32 product __asm__("$3");
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
    u8 *ent5;
    s32 poll;
    u8 *ent2;
    u8 *eloop;

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
        poll = func_8003FCD8();
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
        poll = func_8003FD14();
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
        if (func_8003F70C() == 0) {
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
            ent2[0xE] = 0x80;
            ent2[0xD] = 0x80;
            ent2[0xC] = 0x80;
            *(u16 *)(ent2 + 8) |= DISPLAY_OBJECT_FLAG_RENDERABLE;
            D_80184560[0x6C] = 0x3C;
            *(s16 *)(D_80184560 + 0x36) = 0;
        }
        D_80184598 = 0;
        goto ret_m1;
    }

    entry = D_80184560;
    if (entry != 0 &&
        (*(u16 *)(entry + 8) & DISPLAY_OBJECT_FLAG_RENDERABLE) != 0) {
        if (entry[0x6C] != 0) {
            entry[0x6C] = entry[0x6C] - 1;
        } else {
            lvl = entry[0xE] + entry[0x60];
            entry[0xE] = lvl;
            entry[0xD] = lvl;
            entry[0xC] = lvl;
            entry = D_80184560;
            chr = entry[0xC];
            if ((u32)(chr - 0x41) >= 0x3F) {
                if ((s8)chr < 0) {
                    entry[0x6C] = 0x3C;
                }
                ent5 = D_80184560;
                neg = *(s16 *)(ent5 + 0x60);
                *(s16 *)(ent5 + 0x60) = -neg;
            }
        }
        if ((gInput_wPad1Pressed & PAD_BUTTON_START) != 0) {
            SD_SEPlay(7, 0xFF, 0);
            ent3 = D_80184560;
            *(u16 *)(ent3 + 8) &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
            MainMenu_StartFrontendEntryTransition(0);
            D_80184598 = 1;
            goto ret_m1;
        }
        ent6 = D_80184560;
        acc = *(u16 *)(ent6 + 0x36) + D_8009B0D8;
        *(s16 *)(ent6 + 0x36) = acc;
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
        eloop = *slot;
        if (eloop == 0) {
            goto next_entry;
        }
        timer = *(s16 *)(eloop + 0x60);
        countdown = *(u16 *)(eloop + 0x60);
        if (timer <= 0) {
            goto next_entry;
        }
        timer = countdown - 1;
        ((H16s *)(eloop + 0x60))->h = timer;
        if ((u32)gMain_bMenuID < 5) {
            if (i >= 5) {
                goto hide_entry;
            }
        } else {
            count = i;
            if (count < 5) {
                goto hide_entry;
            }
        }
        eloop = *slot;
        first = *(s16 *)(eloop + 0x38);
        value = *(s16 *)(eloop + 0x36);
        delta = first - value;
        value = *(s16 *)(eloop + 0x60);
        frame = 0x10 - value;
        value = *(u16 *)(eloop + 0x38);
        if (frame != 0x10) {
            product = rsin(frame << 6) * delta;
            eloop = *slot;
            product /= 0x1000;
            value = *(u16 *)(eloop + 0x36) + product;
        }
        *(volatile s16 *)(eloop + 0x30) = value;
        if ((frame & 1) != 0) {
            MainMenu_SpawnFrontendEntryAfterimage(*slot);
        }
        *(u16 *)(*slot + 8) =
            *(u16 *)(*slot + 8) | DISPLAY_OBJECT_FLAG_RENDERABLE;
        goto tick_entry;
    hide_entry:
        *(u16 *)(*slot + 8) =
            *(u16 *)(*slot + 8) & ~DISPLAY_OBJECT_FLAG_RENDERABLE;
    tick_entry:
        moved++;
        func_80040410(*slot, (i << 1) | (gMain_bMenuID != i));
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
                ent3 = *slot2;
                if (ent3 != 0) {
                    *(u16 *)(ent3 + 8) &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
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
        func_80040410(gMain_apMenuEntries[gMain_bMenuID], (gMain_bMenuID << 1) | 1);
        /* Keep the store in each arm: the join controls high-half reuse. */
        if ((gInput_wPad1Repeat & PAD_DIRECTION_UP) != 0) {
            *(volatile u8 *)&gMain_bMenuID = (gMain_bMenuID - base + count - 1) % count + base;
        } else {
            *(volatile u8 *)&gMain_bMenuID = (gMain_bMenuID - base + count + 1) % count + base;
        }
        func_80040410(gMain_apMenuEntries[gMain_bMenuID], gMain_bMenuID << 1);
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
            func_8003F87C();
            D_8018459A = D_8018459A + 1;
            return -1;
        }
    }
    MainMenu_StartFrontendEntryTransition(1);
ret_m1:
    return -1;
    return -1;
}
