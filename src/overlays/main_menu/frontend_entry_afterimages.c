#include "../../types.h"
#include "../../game/display_object_api.h"
#include "../../game/display_object_layout.h"
#include "frontend.h"

extern u8 D_801AF800[];
extern void func_800428A8(void *, s32, s32, s32, s32, s32, s32, s32, void *);
extern void func_80042918(void *);
extern void func_800428EC(void *, s32);

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
