#include "../types.h"
#include "func_80058938.h"
#include "model.h"

void Model_QueueTintRequest(
    s32 a0, s32 a1, ModelTintColor a2, ModelTintColor a3, s32 a4,
    const u8 *bits)
{
    ModelSlot *slot;
    ModelTintRequest *r;
    ModelTintRequest *end;
    register u8 *base = (u8 *)D_800F2B50;
    s32 off;
    s32 fill;
    s32 f;
    s32 mode;
    s32 f1, f2, f3, f4, f5, f6;
    s32 sel;

    slot = &D_800F2C40[a0];
    mode = (a0 & 1) << 1;
    sel = (a1 & 1) << 2;
    fill = 0xFF;
    off = 0;
    r = D_800F2B50;
    end = r + MODEL_TINT_REQUEST_COUNT;
    do {
        if ((*(u8 *)r & 1) == 0) {
            f = r->flags;
            f1 = f | 1;
            f2 = f1 & 0xFFFD;
            f3 = f2 | mode;
            f4 = f3 & 0xFFFB;
            f5 = f4 | sel;
            do {
                mode = 0;
            } while (0);
            r->flags = f5;
            f6 = f5 & 0xFF07;
            sel = off;
            f = f6 | ((slot->field_BF5 & 0x1F) << 3);
            r->flags = f;
            r->field_0A = slot->field_E06;
            r->elapsed = 0;
            r->duration = (a4 *= 2);
            r->start = a2;
            r->end = a3;
            for (; mode < 8; mode++) {
                base[mode + sel + 1] = bits ? bits[mode] : fill;
            }
            break;
        }
        off += sizeof(ModelTintRequest);
        r++;
    } while ((s32)r < (s32)end);
}
