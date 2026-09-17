/* The rest of the front-end menu: its scrolling background, the slide
 * transition that parks and recentres the wheel's eleven entries, the
 * teardown, and the afterimage sprites the moving entries leave behind. They
 * follow MainMenu_UpdateFrontendMenu, a build-integrated candidate since #3859
 * (src/candidates/main_menu/func_80180390.c); the initialiser is in
 * frontend.c. */
#include "../../types.h"
#include "../../game/two_player_save_setup.h"
#include "../../game/save_data.h"
#include "../../game/save_data_update_trade_load.h"
#include "../../unmatched.h"
#include "../../game/input.h"
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
    D_80184596 = mode;
    D_80184599 = 1;
}

void MainMenu_DestroyFrontendMenu(void)
{
    s32 i;

    DisplayObject_ReleaseIfPresent(D_80184558);
    D_80184558 = 0;
    DisplayObject_ReleaseIfPresent(D_8018455C);
    D_8018455C = 0;
    DisplayObject_ReleaseIfPresent(D_80184560);
    D_80184560 = 0;
    for (i = 0; i < 0xB; i++) {
        if (gMain_apMenuEntries[i] != 0) {
            DisplayObject_ReleaseIfPresent(gMain_apMenuEntries[i]);
            gMain_apMenuEntries[i] = 0;
        }
    }
    D_800E9DB0[0] = 0;
}
void MainMenu_SpawnFrontendEntryAfterimage(u8 *source)
{
    DisplayObject *entry = (DisplayObject *)source;
    DisplayObject *object;

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    if (object != 0) {
        func_800428A8(object, (s16)entry->field_30.h.field_30,
                      (s16)entry->field_30.h.field_32, 0, 0, entry->field_69,
                      0x18, 0, D_801AF800);
        object->attribute |= 0x51000000;
        object->flags |=
            DISPLAY_OBJECT_FLAG_RENDERABLE | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_SelectOrderingTable1(object);
        DisplayObject_SetDepthOffset((u8 *)object, (s8)(-(u8)entry->field_60));
        object->update = MainMenu_UpdateFrontendEntryAfterimage;
        ((u8 *)&object->field_0C)[0] = ((u8 *)&entry->field_0C)[0];
        ((u8 *)&object->field_0C)[1] = ((u8 *)&entry->field_0C)[1];
        ((u8 *)&object->field_0C)[2] = ((u8 *)&entry->field_0C)[2];
    }
}

void MainMenu_UpdateFrontendEntryAfterimage(u8 *object)
{
    DisplayObject *o = (DisplayObject *)object;
    s32 r;
    s32 g;
    s32 b;

    if ((o->field_0C & 0xFFFFFF) != 0) {
        r = ((u8 *)&o->field_0C)[0] - 8;
        if (r < 0) {
            r = 0;
        }
        ((u8 *)&o->field_0C)[0] = r;
        g = ((u8 *)&o->field_0C)[1] - 8;
        if (g < 0) {
            g = 0;
        }
        ((u8 *)&o->field_0C)[1] = g;
        b = ((u8 *)&o->field_0C)[2] - 8;
        if (b < 0) {
            b = 0;
        }
        ((u8 *)&o->field_0C)[2] = b;
    } else {
        /* The parameter, not o: passing o here makes the rebuilt module
           four bytes longer than the target. */
        DisplayObject_ReleaseIfPresent(object);
    }
}
