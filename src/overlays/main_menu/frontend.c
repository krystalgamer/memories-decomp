/* The front-end menu: its initialiser, the per-frame update, the scrolling
 * background, the slide transition that parks and recentres the wheel's
 * eleven entries, the teardown, and the afterimage sprites the moving entries
 * leave behind. The initialiser builds the three singleton objects and the
 * wheel of eleven entries the module opens on; the update steps the wheel
 * from the pad and returns the chosen entry. The Japanese and European builds
 * compile each function on its own through their regional wrappers. */
/* Each input branch of the update samples the pressed-pad word
 * independently; no other function in this unit reads pad state. */
#define GINPUT_PAD1_PRESSED_IS_VOLATILE
#include "../../types.h"
#include "../../game/two_player_save_setup.h"
#include "../../game/save_data.h"
#include "../../game/save_data_update_trade_load.h"
#include "../../unmatched.h"
#include "../../game/input.h"
#include "../../game/display_object.h"
#include "../../game/display_object_core.h"
#include "../../game/display_object_layout.h"
#include "../../psyq/libgte.h"
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
#include "../../game/sound.h"
#include "../../overlays/main_menu/frontend.h"
#include "../../overlays/main_menu/ordering_tables.h"
#include "../../game/graphics_frame.h"

#if (!defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)) || \
    defined(VERSION_JAPAN_MAIN_MENU_INIT_FRONTEND_MENU) || \
    defined(VERSION_EUROPE_MAIN_MENU_INIT_FRONTEND_MENU)
#ifdef VERSION_EUROPE
#define MAIN_MENU_ENTRY_TOP_Y 58
#define MAIN_MENU_ENTRY_BOTTOM_Y 50
#else
#define MAIN_MENU_ENTRY_TOP_Y 50
#define MAIN_MENU_ENTRY_BOTTOM_Y 42
#endif

void MainMenu_InitFrontendMenu(s32 unused, s32 menu)
{
    DisplayObject *object;
    DisplayObject *entry;
    DisplayObject *third;
    DisplayObject *fourth;
    s32 i;
    s32 y;
    s32 value;
    u16 state;

    gMain_bMenuID = menu % 11;

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    D_80184558 = object;
    if (object != 0) {
        DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 5, 0, 0, 0x1A, 1, gMainMenu_DisplayResources);
        D_80184558->attribute |= 0x1000000;
        D_80184558->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                            DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_SetDepthOffset(D_80184558, 0);
    }

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    D_8018455C = object;
    if (object != 0) {
        DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 8, 5, 0, 2, 0x1A, 1, gMainMenu_DisplayResources);
        D_8018455C->attribute |= 0x1000000;
        D_8018455C->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                            DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_SetDepthOffset(D_8018455C, 1);
    }

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    gMainMenu_pStartPrompt = object;
    if (object != 0) {
        DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 8, 5, 0, 1, 0x1A, 1, gMainMenu_DisplayResources);
        gMainMenu_pStartPrompt->attribute |= 0x1000000;
        gMainMenu_pStartPrompt->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                            DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_SelectOrderingTable1(gMainMenu_pStartPrompt);
        third = gMainMenu_pStartPrompt;
        third->field_6C = 0x3C;
        fourth = gMainMenu_pStartPrompt;
        third->field_60 = -2;
        fourth->field_34.h.field_36 = 0;
    }

    for (i = 0; i < 11; i++) {
        entry = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        if (i < 5) {
            y = i * 32 + MAIN_MENU_ENTRY_TOP_Y;
        } else {
            y = (i - 5) * 32 + MAIN_MENU_ENTRY_BOTTOM_Y;
        }
        if (entry != 0) {
            DisplayObject_ConfigureSpriteAtPositionWithResource(entry, 0xA0, y, 0, 0, 0, 0x18, 0, gMainMenu_DisplayResources);
            value = i * 2 | (gMain_bMenuID != i);
            entry->attribute |= 0x1000000;
            entry->flags =
                (entry->flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE) &
                ~DISPLAY_OBJECT_FLAG_RENDERABLE;
            DisplayObject_SetResourceVariant((DisplayObjectConfig *)entry, value);
            DisplayObject_SelectOrderingTable1(entry);
            gMain_apMenuEntries[i] = (u8 *)entry;
        } else {
            gMain_apMenuEntries[i] = 0;
        }
    }

    gMainMenu_bBackRequested = 0;
    gMainMenu_bTransitionMode = 0;
    gMainMenu_bShadeLevel = 0;
    if (gMain_bMenuID != 0) {
        state = gMainMenu_pStartPrompt->flags;
        gMainMenu_bShadeLevel = 0x80;
        gMainMenu_pStartPrompt->flags = state & ~DISPLAY_OBJECT_FLAG_RENDERABLE;
    }
    gMainMenu_nFadeDirection = 0;
    gMainMenu_bTransitionActive = 0;
    gMainMenu_bSaveWritePending = 0;
    gMainMenu_bLoadPending = 0;
    gMainMenu_bTradeLoadPending = 0;
    gMainMenu_bDuelLoadPending = 0;
    MainMenu_StartFrontendEntryTransition(0);
    gMain_apFrameServiceCallbacks[0] = MainMenu_DrawFrontendBackground;
    SD_BGMPlayDefaultStep(0x7000);
}

#undef MAIN_MENU_ENTRY_TOP_Y
#undef MAIN_MENU_ENTRY_BOTTOM_Y
#endif

#if (!defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)) || \
    defined(VERSION_JAPAN_MAIN_MENU_UPDATE_FRONTEND_MENU) || \
    defined(VERSION_EUROPE_MAIN_MENU_UPDATE_FRONTEND_MENU)
#ifdef VERSION_JAPAN
#define MAIN_MENU_FRONTEND_CANCEL_BUTTON PAD_BUTTON_CROSS
#else
#define MAIN_MENU_FRONTEND_CANCEL_BUTTON PAD_BUTTON_CANCEL
#endif

s32 MainMenu_UpdateFrontendMenu(void)
{
    DisplayObject *ent3;
    DisplayObject *entry;
    DisplayObject *ent6;
    u8 *G32 *slot;
    u8 *G32 *slot2;
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

    if (gMainMenu_bLoadPending != 0) {
        poll = SaveData_PollLoad();
        if (poll != 0) {
            if (poll == 1) {
                Input_ResetPads();
                MainMenu_StartFrontendEntryTransition(1);
                gMainMenu_bLoadPending = 0;
            } else {
                Input_ResetPads();
                gMainMenu_bLoadPending = 0;
            }
        }
        goto ret_m1;
    }

    if (gMainMenu_bTradeLoadPending != 0) {
        poll = SaveData_UpdateTradeLoad();
        if (poll != 0) {
            if (poll == 1) {
                Input_ResetPads();
                MainMenu_StartFrontendEntryTransition(1);
                gMainMenu_bTradeLoadPending = 0;
            } else {
                Input_ResetPads();
                gMainMenu_bTradeLoadPending = 0;
            }
        }
        goto ret_m1;
    }

    if (gMainMenu_bDuelLoadPending != 0) {
        poll = SaveData_UpdateDuelLoad();
        if (poll != 0) {
            if (poll == 1) {
                Input_ResetPads();
                MainMenu_StartFrontendEntryTransition(1);
                gMainMenu_bDuelLoadPending = 0;
            } else {
                Input_ResetPads();
                gMainMenu_bDuelLoadPending = 0;
            }
        }
        goto ret_m1;
    }

    if (gMainMenu_bSaveWritePending != 0) {
        if (MemCardDialog_Poll() == 0) {
            goto ret_m1;
        }
        Input_ResetPads();
        gMainMenu_bSaveWritePending = 0;
        goto ret_m1;
    }

    step = gMainMenu_nFadeDirection;
    if (step != 0) {
        level = gMainMenu_bShadeLevel + (step << 3);
        gMainMenu_bShadeLevel = level;
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
        if (gMainMenu_nFadeDirection < 0) {
            ent2 = gMainMenu_pStartPrompt;
            ((u8 *)&ent2->rgb)[2] = 0x80;
            ((u8 *)&ent2->rgb)[1] = 0x80;
            ((u8 *)&ent2->rgb)[0] = 0x80;
            ent2->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
            gMainMenu_pStartPrompt->field_6C = 0x3C;
            gMainMenu_pStartPrompt->field_34.h.field_36 = 0;
        }
        gMainMenu_nFadeDirection = 0;
        goto ret_m1;
    }

    entry = gMainMenu_pStartPrompt;
    if (entry != 0 &&
        (entry->flags & DISPLAY_OBJECT_FLAG_RENDERABLE) != 0) {
        if (entry->field_6C != 0) {
            entry->field_6C = entry->field_6C - 1;
        } else {
            lvl = ((u8 *)&entry->rgb)[2] + (u8)entry->field_60;
            ((u8 *)&entry->rgb)[2] = lvl;
            ((u8 *)&entry->rgb)[1] = lvl;
            ((u8 *)&entry->rgb)[0] = lvl;
            entry = gMainMenu_pStartPrompt;
            chr = ((u8 *)&entry->rgb)[0];
            if ((u32)(chr - 0x41) >= 0x3F) {
                if ((s8)chr < 0) {
                    entry->field_6C = 0x3C;
                }
                ent5 = gMainMenu_pStartPrompt;
                neg = ent5->field_60;
                ent5->field_60 = -neg;
            }
        }
        if ((gInput_wPad1Pressed & PAD_BUTTON_START) != 0) {
            SD_SEPlay(7, 0xFF, 0);
            ent3 = gMainMenu_pStartPrompt;
            ent3->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
            MainMenu_StartFrontendEntryTransition(0);
            gMainMenu_nFadeDirection = 1;
            goto ret_m1;
        }
        ent6 = gMainMenu_pStartPrompt;
        acc = (u16)ent6->field_34.h.field_36 + (u16)D_8009B0D8;
        ent6->field_34.h.field_36 = acc;
        if ((s16)acc >= 0xBB8) {
            return -2;
        }
        return -1;
    }

    if (gMainMenu_bTransitionActive != 0) {
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
            MainMenu_SpawnFrontendEntryAfterimage((DisplayObject *)*slot);
        }
        ((DisplayObject *)*slot)->flags =
            ((DisplayObject *)*slot)->flags | DISPLAY_OBJECT_FLAG_RENDERABLE;
        goto tick_entry;
    hide_entry:
        ((DisplayObject *)*slot)->flags =
            ((DisplayObject *)*slot)->flags & ~DISPLAY_OBJECT_FLAG_RENDERABLE;
    tick_entry:
        moved++;
        DisplayObject_SetResourceVariant((DisplayObjectConfig *)*slot, (i << 1) | (gMain_bMenuID != i));
    next_entry:
        i++;
        slot++;
        if (i < 0xB) {
            goto entry_loop;
        }
        if (moved != 0) {
            return -1;
        }
        vx3 = gMainMenu_bTransitionMode;
        gMainMenu_bTransitionActive = 0;
        if (vx3 == 0) {
            goto ret_m1;
        }
        if (gMainMenu_bBackRequested != 0) {
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
                gMainMenu_nFadeDirection = -1;
            } else {
                MainMenu_StartFrontendEntryTransition(0);
                gMain_bMenuID = 1;
            }
            gMainMenu_bBackRequested = 0;
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
        DisplayObject_SetResourceVariant((DisplayObjectConfig *)gMain_apMenuEntries[gMain_bMenuID], (gMain_bMenuID << 1) | 1);
        /* Keep the store in each arm: the join controls high-half reuse. */
        if ((gInput_wPad1Repeat & PAD_DIRECTION_UP) != 0) {
            *(volatile u8 *)&gMain_bMenuID = (gMain_bMenuID - base + count - 1) % count + base;
        } else {
            *(volatile u8 *)&gMain_bMenuID = (gMain_bMenuID - base + count + 1) % count + base;
        }
        DisplayObject_SetResourceVariant((DisplayObjectConfig *)gMain_apMenuEntries[gMain_bMenuID], gMain_bMenuID << 1);
        SD_SEPlay(6, 0xFF, 0);
        goto ret_m1;
    }

    if ((gInput_wPad1Pressed & (PAD_BUTTON_START | PAD_BUTTON_CANCEL | PAD_BUTTON_CONFIRM_MASK)) == 0) {
        return -1;
    }
    if ((gInput_wPad1Pressed & MAIN_MENU_FRONTEND_CANCEL_BUTTON) != 0) {
        if ((u32)gMain_bMenuID < 5) {
            SD_SEPlay(9, 0xFF, 0);
            return -1;
        }
        SD_SEPlay(8, 0xFF, 0);
        gMainMenu_bBackRequested = 1;
    } else {
        SD_SEPlay(7, 0xFF, 0);
        switch (gMain_bMenuID) {
        case 1:
            SaveData_RequestLoad();
            gMainMenu_bLoadPending = gMainMenu_bLoadPending + 1;
            return -1;
        case 3:
            gSaveData_PairSetupState = 0;
            gSaveData_LoadPairState = 0;
            gMainMenu_bTradeLoadPending = gMainMenu_bTradeLoadPending + 1;
            return -1;
        case 2:
            gSaveData_PairSetupState = 0;
            gSaveData_LoadPairState = 0;
            gMainMenu_bDuelLoadPending = gMainMenu_bDuelLoadPending + 1;
            return -1;
        case 0xA:
            SaveData_RequestWrite();
            gMainMenu_bSaveWritePending = gMainMenu_bSaveWritePending + 1;
            return -1;
        }
    }
    MainMenu_StartFrontendEntryTransition(1);
ret_m1:
    return -1;
}
#endif

#if (!defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)) || \
    defined(VERSION_JAPAN_MAIN_MENU_DRAW_FRONTEND_BACKGROUND) || \
    defined(VERSION_EUROPE_MAIN_MENU_DRAW_FRONTEND_BACKGROUND)
void MainMenu_DrawFrontendBackground(void)
{
    POLY_F4 flat;
    POLY_FT4 sprite;
    POLY_G4 shade;
    s32 shadeLevel;
    s32 x;
    s32 right;
    s32 u;

    shadeLevel = gMainMenu_bShadeLevel;
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
#ifdef VERSION_EUROPE
        flat.y2 = 256;
#else
        flat.y2 = GRAPHICS_DEFAULT_HEIGHT;
#endif
        flat.x3 = GRAPHICS_DEFAULT_WIDTH;
#ifdef VERSION_EUROPE
        flat.y3 = 256;
#else
        flat.y3 = GRAPHICS_DEFAULT_HEIGHT;
#endif
        func_8005B260((u32 *)&flat, (GsOT *)gGraphics_apOrderingTables[2], 0, 2);
    }
    setPolyFT4(&sprite);
    sprite.r0 = 128;
    sprite.g0 = 128;
    sprite.b0 = 128;
    sprite.tpage = 15;
#ifdef VERSION_EUROPE
    sprite.clut = 0x3528;
#else
    sprite.clut = getClut(0, 244);
#endif
    for (x = 0; x < GRAPHICS_DEFAULT_WIDTH; x = right) {
        u = x % 256;
        right = x + 64;
        sprite.x0 = x;
        sprite.y0 = 0;
        sprite.x1 = right;
        sprite.y1 = 0;
        sprite.x2 = x;
#ifdef VERSION_EUROPE
        sprite.y2 = 256;
#else
        sprite.y2 = GRAPHICS_DEFAULT_HEIGHT;
#endif
        sprite.x3 = right;
#ifdef VERSION_EUROPE
        sprite.y3 = 256;
#else
        sprite.y3 = GRAPHICS_DEFAULT_HEIGHT;
#endif
        sprite.u0 = u;
        sprite.v0 = 0;
        sprite.u1 = u + 63;
        sprite.v1 = 0;
        sprite.u2 = u;
#ifdef VERSION_EUROPE
        sprite.v2 = 255;
#else
        sprite.v2 = 239;
#endif
        sprite.u3 = u + 63;
#ifdef VERSION_EUROPE
        sprite.v3 = 255;
#else
        sprite.v3 = 239;
#endif
        GsSortPoly(&sprite, gGraphics_apOrderingTables[2], 4095);
    }
    setPolyG4(&shade);
#ifdef VERSION_EUROPE
    shade.r0 = 255;
    shade.g0 = 255;
    shade.b0 = 255;
    shade.r1 = 255;
    shade.g1 = 255;
    shade.b1 = 255;
    shade.r2 = 0;
    shade.g2 = 0;
    shade.b2 = 0;
    shade.r3 = 0;
    shade.g3 = 0;
    shade.b3 = 0;
    shade.x0 = 0;
    shade.y0 = 0;
    shade.x1 = GRAPHICS_DEFAULT_WIDTH;
    shade.y1 = 0;
    shade.x2 = 0;
    shade.y2 = 64;
    shade.x3 = GRAPHICS_DEFAULT_WIDTH;
    shade.y3 = 64;
    func_8005B260((u32 *)&shade, (GsOT *)gGraphics_apOrderingTables[2], 4094, 2);

    shade.r0 = 0;
    shade.g0 = 0;
    shade.b0 = 0;
    shade.r1 = 0;
    shade.g1 = 0;
    shade.b1 = 0;
    shade.r2 = 255;
    shade.g2 = 255;
    shade.b2 = 255;
    shade.r3 = 255;
    shade.g3 = 255;
    shade.b3 = 255;
    shade.x0 = 0;
    shade.y0 = 64;
    shade.x1 = GRAPHICS_DEFAULT_WIDTH;
    shade.y1 = 64;
    shade.x2 = 0;
    shade.y2 = 256;
    shade.x3 = GRAPHICS_DEFAULT_WIDTH;
    shade.y3 = 256;
#else
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
#endif
    func_8005B260((u32 *)&shade, (GsOT *)gGraphics_apOrderingTables[2], 4094, 2);
}
#endif

#if (!defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)) || \
    defined(VERSION_JAPAN_MAIN_MENU_START_FRONTEND_ENTRY_TRANSITION) || \
    defined(VERSION_EUROPE_MAIN_MENU_START_FRONTEND_ENTRY_TRANSITION)
void MainMenu_StartFrontendEntryTransition(s32 mode)
{
    s32 i;
    s32 offset;
    /* The entries are display objects; gMain_apMenuEntries keeps its u8 *
       declaration for its other users. */
    DisplayObject **entries = (DisplayObject **)gMain_apMenuEntries;

    for (i = 0; i < 0xB; i++) {
        if (i & 1) {
            offset = 0x1E0;
        } else {
            offset = -0xA0;
        }
        if (entries[i] != 0) {
            if (mode != 0) {
                entries[i]->field_34.h.field_36 = 0xA0;
                entries[i]->field_38.h.field_38 = offset;
            } else {
                entries[i]->field_34.h.field_36 = offset;
                entries[i]->field_38.h.field_38 = 0xA0;
            }
            entries[i]->field_30.h.field_30 = entries[i]->field_34.h.field_36;
            entries[i]->field_60 = 0x10;
        }
    }
    gMainMenu_bTransitionMode = mode;
    gMainMenu_bTransitionActive = 1;
}
#endif

#if (!defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)) || \
    defined(VERSION_JAPAN_MAIN_MENU_DESTROY_FRONTEND_MENU) || \
    defined(VERSION_EUROPE_MAIN_MENU_DESTROY_FRONTEND_MENU)
void MainMenu_DestroyFrontendMenu(void)
{
    s32 i;

    DisplayObject_ReleaseIfPresent(D_80184558);
    D_80184558 = 0;
    DisplayObject_ReleaseIfPresent(D_8018455C);
    D_8018455C = 0;
    DisplayObject_ReleaseIfPresent(gMainMenu_pStartPrompt);
    gMainMenu_pStartPrompt = 0;
    for (i = 0; i < 0xB; i++) {
        if (gMain_apMenuEntries[i] != 0) {
            DisplayObject_ReleaseIfPresent(gMain_apMenuEntries[i]);
            gMain_apMenuEntries[i] = 0;
        }
    }
    gMain_apFrameServiceCallbacks[0] = 0;
}
#endif

#if (!defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)) || \
    defined(VERSION_JAPAN_MAIN_MENU_SPAWN_FRONTEND_ENTRY_AFTERIMAGE) || \
    defined(VERSION_EUROPE_MAIN_MENU_SPAWN_FRONTEND_ENTRY_AFTERIMAGE)
void MainMenu_SpawnFrontendEntryAfterimage(DisplayObject *entry)
{
    DisplayObject *object;

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    if (object != 0) {
        DisplayObject_ConfigureSpriteAtPositionWithResource(object, (s16)entry->field_30.h.field_30,
                      (s16)entry->field_30.h.field_32, 0, 0, entry->field_69,
                      0x18, 0, gMainMenu_DisplayResources);
        object->attribute |= 0x51000000;
        object->flags |=
            DISPLAY_OBJECT_FLAG_RENDERABLE | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_SelectOrderingTable1(object);
        DisplayObject_SetDepthOffset(object, (s8)(-(u8)entry->field_60));
        object->update =
            (DisplayObjectCallback)MainMenu_UpdateFrontendEntryAfterimage;
        ((u8 *)&object->rgb)[0] = ((u8 *)&entry->rgb)[0];
        ((u8 *)&object->rgb)[1] = ((u8 *)&entry->rgb)[1];
        ((u8 *)&object->rgb)[2] = ((u8 *)&entry->rgb)[2];
    }
}

#endif

#if (!defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)) || \
    defined(VERSION_JAPAN_MAIN_MENU_UPDATE_FRONTEND_ENTRY_AFTERIMAGE) || \
    defined(VERSION_EUROPE_MAIN_MENU_UPDATE_FRONTEND_ENTRY_AFTERIMAGE)
void MainMenu_UpdateFrontendEntryAfterimage(DisplayObject *object)
{
    s32 r;
    s32 g;
    s32 b;

    if ((object->rgb & 0xFFFFFF) != 0) {
        r = ((u8 *)&object->rgb)[0] - 8;
        if (r < 0) {
            r = 0;
        }
        ((u8 *)&object->rgb)[0] = r;
        g = ((u8 *)&object->rgb)[1] - 8;
        if (g < 0) {
            g = 0;
        }
        ((u8 *)&object->rgb)[1] = g;
        b = ((u8 *)&object->rgb)[2] - 8;
        if (b < 0) {
            b = 0;
        }
        ((u8 *)&object->rgb)[2] = b;
    } else {
        DisplayObject_ReleaseIfPresent(object);
    }
}
#endif
