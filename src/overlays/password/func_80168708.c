#include "../../types.h"

typedef struct Obj {
    u8 unk0[0x4];
    u32 flags;
    u8 unk8[0x4];
    s16 unkC;
    u8 unkE[0x36];
    s16 scaleY;
    s16 scaleX;
    u8 unk48[0x4];
    struct Obj *target;
    u8 unk50[0xA];
    s16 saved;
    u8 unk5C[0x4];
    s16 frame;
} Obj;

extern s32 func_80042B98(void);
extern void func_8004036C(Obj *);

void func_80168708(Obj *obj)
{
    Obj *target;
    s32 step;
    s32 value;
    s16 frame;

    target = obj->target;
    if (func_80042B98() == 0) {
        if (target != 0) {
            obj->saved = target->unkC;
            target->unkC = 0x400;
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
    obj->scaleX = 4096 - step * 341;
    obj->scaleY = 4096 - step * 341;
    frame = obj->frame + 1;
    obj->frame = frame;
    if (frame >= 12) {
        if (target != 0) {
            target->unkC = obj->saved;
        }
    }
}
