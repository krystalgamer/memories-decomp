#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "name_entry_keyboard.h"
#include "../../game/display_object_api.h"
#include "../../game/display_object_lifecycle.h"

typedef struct Obj {
    u8 unk0[0x4];
    u32 flags;
    u8 unk8[0x4];
    s16 unkC;
    u8 unkE[0x36];
    s16 scale44;
    s16 scale46;
    u8 unk48[0x4];
    DuelEffectEntry *sourceGlyph;
    u8 unk50[0xA];
    s16 savedSourceX;
    u8 unk5C[0x4];
    s16 frame;
} Obj;

void NameEntry_UpdateGlyphPulse(u8 *sprite)
{
    Obj *obj = (Obj *)sprite;
    DuelEffectEntry *source;
    s32 step;
    s32 value;
    s16 frame;

    source = obj->sourceGlyph;
    if (func_80042B98((DisplayObjectLifecycle *)sprite) == 0) {
        if (source != 0) {
            obj->savedSourceX = source->x_0C;
            source->x_0C = 0x400;
        }
        obj->frame = 0;
        obj->flags &= ~GsROTOFF;
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
            source->x_0C = obj->savedSourceX;
        }
    }
}
