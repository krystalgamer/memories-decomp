#include "../../types.h"
#include "../../game/display_object_api.h"
#include "name_entry_keyboard.h"

extern void func_800429D8(void);
extern s32 func_800358FC(s32);
extern s32 func_80042B08(s32, s32);
extern s32 func_80042AD8(s32, s32, s32);
extern void func_80042A78(void *);
extern void func_8004036C(void *);
extern s32 func_80042B98(void);
extern void func_80040510(void *, s32, s32, s32, s32, s32, s32, s32, s32, s32);

void NameEntry_UpdateGlyphFragment(u8 *object)
{
    u8 flags;

    flags = object[0x6C];
    if ((flags & 0x80) == 0) {
        object[0x6C] = flags | 0x80;
        func_800429D8();
        *(s16 *)(object + 0x36) = func_800358FC(0x200) - 0x100;
        *(s16 *)(object + 0x38) = -func_800358FC(0x180);
    }
    *(s16 *)(object + 0x36) = func_80042B08(*(s16 *)(object + 0x36), 8);
    *(s16 *)(object + 0x38) = func_80042AD8(*(s16 *)(object + 0x38), 0x800, 0x40);
    func_80042A78(object);
    if (*(s16 *)(object + 0x32) >= 0xF0) {
        func_8004036C(object);
    }
}

void NameEntry_UpdateGlyphShatter(u8 *object)
{
    u8 *piece;
    s32 dx;
    s32 dy;

    if (func_80042B98() == 0) {
        for (dy = 0; dy < 0x10; dy += 4) {
            for (dx = 0; dx < 0x10; dx += 4) {
                piece = func_800400AC(func_8004002C(), 1);
                if (piece != 0) {
                    func_80040510(piece,
                                  *(s16 *)(object + 0x30) + dx,
                                  *(s16 *)(object + 0x32) + dy,
                                  4, 4,
                                  object[0x5C] + dx,
                                  object[0x5D] + dy,
                                  object[0x66],
                                  *(u16 *)(object + 0x40),
                                  *(u16 *)(object + 0x42));
                    piece[0x6C] = 3;
                    *(NameEntryGlyphUpdate *)(piece + 0x24) =
                        NameEntry_UpdateGlyphFragment;
                }
            }
        }
    } else {
        func_8004036C(object);
    }
}
