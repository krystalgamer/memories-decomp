#include "../../types.h"
#include "name_entry_keyboard.h"

typedef struct Obj {
    u8 unk0[0x4];
    u32 flags;
    u8 unk8[0x4];
    s16 unkC;
    u8 unkE[0x36];
    s16 scale44;
    s16 scale46;
    u8 unk48[0x4];
    NameEntryGlyphNodeView *sourceGlyph;
    u8 unk50[0xA];
    s16 savedSourceX;
    u8 unk5C[0x4];
    s16 frame;
} Obj;

extern s32 func_80042B98(void);
extern void func_8004036C(Obj *);

void NameEntry_UpdateGlyphPulse(u8 *sprite)
{
    Obj *obj = (Obj *)sprite;
    NameEntryGlyphNodeView *source;
    s32 step;
    s32 value;
    s16 frame;

    source = obj->sourceGlyph;
    if (func_80042B98() == 0) {
        if (source != 0) {
            obj->savedSourceX = source->x;
            source->x = 0x400;
        }
        obj->frame = 0;
        obj->flags &= ~0x08000000;
    }
    value = obj->frame;
    if (value >= 12) {
        func_8004036C(obj);
        return;
    }
    step = value;
    if (step >= 6) {
        step = 11 - step;
    }
    obj->scale46 = 4096 - step * 341;
    obj->scale44 = 4096 - step * 341;
    frame = obj->frame + 1;
    obj->frame = frame;
    if (frame >= 12) {
        if (source != 0) {
            source->x = obj->savedSourceX;
        }
    }
}
