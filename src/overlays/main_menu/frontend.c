/* The front-end menu: the wheel of eleven entries the module opens on, its
 * scrolling background, the slide transition that parks and recentres the
 * entries, and the afterimage sprites the moving entries leave behind.
 *
 * `gInput_wPad1Pressed` is read volatile for this whole unit.
 * MainMenu_UpdateFrontendMenu is the only reader; the declaration has to
 * precede the include, so it sits at the top of the file. */
#define GINPUT_PAD1_PRESSED_IS_VOLATILE
#include "../../types.h"
#include "../../game/two_player_save_setup.h"
#include "../../game/save_data.h"
#include "../../game/func_8003FCD8.h"
#include "../../unmatched.h"
#include "../../game/input.h"
#include "../../game/display_object_api.h"
#include "../../game/display_object_layout.h"
#include "../../psyq/libgte.h"
#include "entrypoints.h"
#include "frontend.h"
#include "../../game/display_object_helpers.h"
#include "../../game/main_services.h"
#include "../../game/display_object_config.h"
#include "../../game/sound_output.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_constants.h"
#include "../../game/data_transfer_request.h"
#include "../../game/mem_card.h"
#include "ordering_tables.h"

extern u16 D_8009B0D8;

extern void func_8003F87C(void);
extern void SD_SEPlay(s32, s32, s32);

typedef struct { s16 h; } H16s;

void MainMenu_InitFrontendMenu(s32 unused, s32 menu)
{
    u8 *object;
    u8 *entry;
    u8 *third;
    u8 *fourth;
    s32 i;
    s32 y;
    s32 value;
    u16 state;

    gMain_bMenuID = menu % 11;

    object = func_800400AC(func_8004002C(), 2);
    D_80184558 = object;
    if (object != 0) {
        func_800428A8(object, 0, 0, 5, 0, 0, 0x1A, 1, D_801AF800);
        *(u32 *)(D_80184558 + 4) |= 0x1000000;
        *(u16 *)(D_80184558 + 8) |= 0x28;
        func_800428EC(D_80184558, 0);
    }

    object = func_800400AC(func_8004002C(), 2);
    D_8018455C = object;
    if (object != 0) {
        func_800428A8(object, 0, 8, 5, 0, 2, 0x1A, 1, D_801AF800);
        *(u32 *)(D_8018455C + 4) |= 0x1000000;
        *(u16 *)(D_8018455C + 8) |= 0x28;
        func_800428EC(D_8018455C, 1);
    }

    object = func_800400AC(func_8004002C(), 2);
    D_80184560 = object;
    if (object != 0) {
        func_800428A8(object, 0, 8, 5, 0, 1, 0x1A, 1, D_801AF800);
        *(u32 *)(D_80184560 + 4) |= 0x1000000;
        *(u16 *)(D_80184560 + 8) |= 0x28;
        func_80042918(D_80184560);
        third = D_80184560;
        third[0x6C] = 0x3C;
        fourth = D_80184560;
        *(s16 *)(third + 0x60) = -2;
        *(u16 *)(fourth + 0x36) = 0;
    }

    for (i = 0; i < 11; i++) {
        entry = func_800400AC(func_8004002C(), 2);
        if (i < 5) {
            y = i * 32 + 50;
        } else {
            y = (i - 5) * 32 + 42;
        }
        if (entry != 0) {
            func_800428A8(entry, 0xA0, y, 0, 0, 0, 0x18, 0, D_801AF800);
            value = i * 2 | (gMain_bMenuID != i);
            *(u32 *)(entry + 4) |= 0x1000000;
            *(u16 *)(entry + 8) =
                (*(u16 *)(entry + 8) | DISPLAY_OBJECT_FLAG_SCREEN_SPACE) &
                ~DISPLAY_OBJECT_FLAG_RENDERABLE;
            func_80040410(entry, value);
            func_80042918(entry);
            gMain_apMenuEntries[i] = entry;
        } else {
            gMain_apMenuEntries[i] = 0;
        }
    }

    D_80184595 = 0;
    D_80184596 = 0;
    D_80184597 = 0;
    if (gMain_bMenuID != 0) {
        state = *(u16 *)(D_80184560 + 8);
        D_80184597 = 0x80;
        *(u16 *)(D_80184560 + 8) = state & ~DISPLAY_OBJECT_FLAG_RENDERABLE;
    }
    D_80184598 = 0;
    D_80184599 = 0;
    D_8018459A = 0;
    D_8018459B = 0;
    D_8018459C = 0;
    D_8018459D = 0;
    MainMenu_StartFrontendEntryTransition(0);
    D_800E9DB0[0] = MainMenu_DrawFrontendBackground;
    func_80047314(0x7000);
}
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
void MainMenu_DrawFrontendBackground(void)
{
    POLY_F4 flat;
    POLY_FT4 sprite;
    POLY_G4 shade;
    s32 shadeLevel;
    s32 x;
    s32 right;
    s32 u;

    shadeLevel = D_80184597;
    if (shadeLevel != 0) {
        setPolyF4(&flat);
        flat.r0 = shadeLevel;
        flat.g0 = shadeLevel;
        flat.b0 = shadeLevel;
        flat.x0 = 0;
        flat.y0 = 0;
        flat.x1 = GRAPHICS_DEFAULT_WIDTH;
        flat.y1 = 0;
        flat.x2 = 0;
        flat.y2 = GRAPHICS_DEFAULT_HEIGHT;
        flat.x3 = GRAPHICS_DEFAULT_WIDTH;
        flat.y3 = GRAPHICS_DEFAULT_HEIGHT;
        func_8005B260((u32 *)&flat, (GsOT *)D_800E9D90[2], 0, 2);
    }
    setPolyFT4(&sprite);
    sprite.r0 = 128;
    sprite.g0 = 128;
    sprite.b0 = 128;
    sprite.tpage = 15;
    sprite.clut = getClut(0, 244);
    for (x = 0; x < GRAPHICS_DEFAULT_WIDTH; x = right) {
        u = x % 256;
        right = x + 64;
        sprite.x0 = x;
        sprite.y0 = 0;
        sprite.x1 = right;
        sprite.y1 = 0;
        sprite.x2 = x;
        sprite.y2 = GRAPHICS_DEFAULT_HEIGHT;
        sprite.x3 = right;
        sprite.y3 = GRAPHICS_DEFAULT_HEIGHT;
        sprite.u0 = u;
        sprite.v0 = 0;
        sprite.u1 = u + 63;
        sprite.v1 = 0;
        sprite.u2 = u;
        sprite.v2 = 239;
        sprite.u3 = u + 63;
        sprite.v3 = 239;
        GsSortPoly(&sprite, D_800E9D90[2], 4095);
    }
    setPolyG4(&shade);
    shade.r2 = 255;
    shade.g2 = 255;
    shade.b2 = 255;
    shade.r3 = 255;
    shade.g3 = 255;
    shade.b3 = 255;
    shade.r0 = 0;
    shade.g0 = 0;
    shade.b0 = 0;
    shade.r1 = 0;
    shade.g1 = 0;
    shade.b1 = 0;
    shade.x0 = 0;
    shade.y0 = 0;
    shade.x1 = GRAPHICS_DEFAULT_WIDTH;
    shade.y1 = 0;
    shade.x2 = 0;
    shade.y2 = GRAPHICS_DEFAULT_HEIGHT;
    shade.x3 = GRAPHICS_DEFAULT_WIDTH;
    shade.y3 = GRAPHICS_DEFAULT_HEIGHT;
    func_8005B260((u32 *)&shade, (GsOT *)D_800E9D90[2], 4094, 2);
}
void MainMenu_StartFrontendEntryTransition(s32 mode)
{
    s32 i;
    s32 offset;

    for (i = 0; i < 0xB; i++) {
        if (i & 1) {
            offset = 0x1E0;
        } else {
            offset = -0xA0;
        }
        if (gMain_apMenuEntries[i] != 0) {
            if (mode != 0) {
                *(s16 *)(gMain_apMenuEntries[i] + 0x36) = 0xA0;
                *(s16 *)(gMain_apMenuEntries[i] + 0x38) = offset;
            } else {
                *(s16 *)(gMain_apMenuEntries[i] + 0x36) = offset;
                *(s16 *)(gMain_apMenuEntries[i] + 0x38) = 0xA0;
            }
            *(s16 *)(gMain_apMenuEntries[i] + 0x30) = *(u16 *)(gMain_apMenuEntries[i] + 0x36);
            *(s16 *)(gMain_apMenuEntries[i] + 0x60) = 0x10;
        }
    }
    D_80184596 = mode;
    D_80184599 = 1;
}

void MainMenu_DestroyFrontendMenu(void)
{
    s32 i;

    func_8004036C(D_80184558);
    D_80184558 = 0;
    func_8004036C(D_8018455C);
    D_8018455C = 0;
    func_8004036C(D_80184560);
    D_80184560 = 0;
    for (i = 0; i < 0xB; i++) {
        if (gMain_apMenuEntries[i] != 0) {
            func_8004036C(gMain_apMenuEntries[i]);
            gMain_apMenuEntries[i] = 0;
        }
    }
    D_800E9DB0[0] = 0;
}
void MainMenu_SpawnFrontendEntryAfterimage(u8 *source)
{
    u8 *object;

    object = func_800400AC(func_8004002C(), 2);
    if (object != 0) {
        func_800428A8(object, *(s16 *)(source + 0x30), *(s16 *)(source + 0x32), 0,
                      0, source[0x69], 0x18, 0, D_801AF800);
        *(s32 *)(object + 4) |= 0x51000000;
        *(u16 *)(object + 8) |=
            DISPLAY_OBJECT_FLAG_RENDERABLE | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        func_80042918(object);
        func_800428EC(object, (s8)(-source[0x60]));
        *(MainMenuEntryEffectUpdate *)(object + 0x24) =
            MainMenu_UpdateFrontendEntryAfterimage;
        object[0xC] = source[0xC];
        object[0xD] = source[0xD];
        object[0xE] = source[0xE];
    }
}

void MainMenu_UpdateFrontendEntryAfterimage(u8 *object)
{
    s32 r;
    s32 g;
    s32 b;

    if ((*(s32 *)(object + 0xC) & 0xFFFFFF) != 0) {
        r = object[0xC] - 8;
        if (r < 0) {
            r = 0;
        }
        object[0xC] = r;
        g = object[0xD] - 8;
        if (g < 0) {
            g = 0;
        }
        object[0xD] = g;
        b = object[0xE] - 8;
        if (b < 0) {
            b = 0;
        }
        object[0xE] = b;
    } else {
        func_8004036C(object);
    }
}
